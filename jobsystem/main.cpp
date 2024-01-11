/*
 * Please add your names and UIDs in the form: Daniel Gerendas <gs20m016>, Lukas Tyburtzy <gs20m021>, Julia Pichler <gs20m002>
 */

#include <cstdio>
#include <cstdint>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <string>

// Use this to switch betweeen serial and parallel processing (for perf. comparison)
constexpr bool isRunningParallel = true;

/*
* ===============================================
* Optick is a free, light-weight C++ profiler
* We use it in this exercise to make it easier for
* you to profile your jobsystem and see what's
* happening (previousJobs, locking, comparison to
* serial, ...)
*
* It's not mandatory to use it, just advised. If
* you dislike it or have problems with it, feel
* free to remove it from the solution
* ===============================================
*/
#include "optick_src/optick.h"

#include "Job.h"


using namespace std;

std::vector<Job*> tasks;	// we used a vector because this allowed us to get ot only the last Job, but any job in the container
std::mutex tasksMutex;		// used to lock the queue
std::mutex waitMutex;		// used to lock the conditional variable
std::condition_variable newTasksCV;	// conditional variable to tell the workers there are new possible jobs in the queue

bool isBlocked = false;

// Don't change this macros (unlsess for removing Optick if you want) - if you need something
// for your local testing, create a new one for yourselves.

#define MAKE_UPDATE_FUNC(NAME, DURATION) \
	void Update##NAME() { \
		OPTICK_EVENT(); \
		auto start = chrono::high_resolution_clock::now(); \
		decltype(start) end; \
		do { \
			end = chrono::high_resolution_clock::now(); \
		} while (chrono::duration_cast<chrono::microseconds>(end - start).count() < (DURATION)); \
	} \

// You can create other functions for testing purposes but those here need to run in your job system
// The previousJobs are noted on the right side of the functions, the implementation should be able to set them up so they are not violated and run in that order!
MAKE_UPDATE_FUNC(Input, 200) // no previousJobs
MAKE_UPDATE_FUNC(Physics, 1000) // depends on Input
MAKE_UPDATE_FUNC(Collision, 1200) // depends on Physics
MAKE_UPDATE_FUNC(Animation, 600) // depends on Collision
MAKE_UPDATE_FUNC(Particles, 800) // depends on Collision
MAKE_UPDATE_FUNC(GameElements, 2400) // depends on Physics
MAKE_UPDATE_FUNC(Rendering, 2000) // depends on Animation, Particles, GameElements
MAKE_UPDATE_FUNC(Sound, 1000) // no previousJobs

void UpdateSerial()
{
	OPTICK_EVENT();

	UpdateInput();
	UpdatePhysics();
	UpdateCollision();
	UpdateAnimation();
	UpdateParticles();
	UpdateGameElements();
	UpdateRendering();
	UpdateSound();
}

// function called by workers to get a new job
bool GetAvailableJob(Job*& selectedJob) {
	OPTICK_EVENT();
	const std::lock_guard<std::mutex> lock(tasksMutex);
	// if the container is empty, just return, dont waste time
	if (!tasks.empty()) {
		// we go through all the Jobs in the container,
		// if one can be worked on we select it and store its index on the vector to erase it
		// this allows just one worker to select a job (as the container is locked)
		int selectedIndex = -1;
		for (int i = 0; i < tasks.size(); i++) {
			if (tasks[i]->canBeWorkedOn()) {
				selectedJob = tasks[i];
				selectedIndex = i;
				break;
			}
		}
		if (selectedIndex >= 0) {
			tasks.erase(tasks.begin() + selectedIndex);
			return true;
		}
		else {
			// if no job was found, set this flag telling the system there are no possible jobs currently
			// this will make the thread go to sleep
			isBlocked = true;
			return false;
		}
		
	}
	return false;
}

// function that runs the function pointed byt the Job's function pointer
// this is done lockless because only one worker should do that every time
void executeJob(const Job* job) {
	job->execute();
}

// this funciton tidies up the job after its execution
// this one needs a lock, because the finishJob() function access other Jobs
void finishJob(Job* job) {
	const std::lock_guard<std::mutex> lock(tasksMutex);
	job->finishJob();
	// we tell the system there could be new jobs on the container, ones that got released after finishind this job
	isBlocked = false;
	// we have to delete this job, because it was created on the heap
	delete job;
	// to make sure the other workers try looking for a new job we notify the conditional variable
	newTasksCV.notify_all();
}

// this is the predicate that is run while testing the conditional variable
// it should only return true if there are items in the container and not all jobs are blocked
bool isThereJobToDo(){
	return !tasks.empty() && !isBlocked;
}

// the function run by the worker threads
void WorkerMain() {
	OPTICK_THREAD("Worker");
	while (true) {
		Job* job = nullptr;
		// we look for a job and execute it if found any
		if (GetAvailableJob(job)) {
			executeJob(job);
			finishJob(job);
		}
		else {
			// if none is found go to sleep until notified of new ones
			unique_lock<std::mutex> lk(waitMutex);
			newTasksCV.wait(lk, isThereJobToDo);
		}
	}
}


/*
* ===============================================================
* In `UpdateParallel` you should use your jobsystem to distribute
* the tasks to the job system. You can add additional parameters
* as you see fit for your implementation (to avoid global state)
* ===============================================================
*/
void UpdateParallel()
{
	OPTICK_EVENT();

	// we populate the job container with new jobs each frame
	// therefore we obtain a lock so that no other threads change the containers contents
	const std::lock_guard<std::mutex> lock(tasksMutex);

	Job* inputJob =  new Job({}, {}, Job::Category::INPUT, UpdateInput);
	Job* physicsJob = new Job({}, {}, Job::Category::PHYSICS, UpdatePhysics);
	Job* collisionJob = new Job({}, {}, Job::Category::COLLISION, UpdateCollision);
	Job* animationJob = new Job({}, {}, Job::Category::ANIMATION, UpdateAnimation);
	Job* particlesJob = new Job({}, {}, Job::Category::PARTICLES, UpdateParticles);
	Job* gameElementsJob = new Job({}, {}, Job::Category::GAMEELEMENTS, UpdateGameElements);
	Job* renderingJob = new Job({}, {}, Job::Category::RENDERING, UpdateRendering);
	Job* soundJob = new Job({}, {}, Job::Category::SOUND, UpdateSound);

	// we just set the dependencies
	// next Jobs are the Jobs that depend on this job, when this job finishes being executed they all are enabled
	// previous Jobs are the Jobs that have to be finished so this job can be executed. When each of them finishes execution, it is removed from the list. 
	//		when the list is empty, the job is enabled
	inputJob->nextJobs = { std::vector<Job*>({physicsJob}) };
	inputJob->previousJobs = { std::vector<Job*>({}) };

	physicsJob->nextJobs = { std::vector<Job*>({collisionJob, gameElementsJob}) };
	physicsJob->previousJobs = { std::vector<Job*>({inputJob}) };

	collisionJob->nextJobs = { std::vector<Job*>({animationJob, particlesJob}) };
	collisionJob->previousJobs = { std::vector<Job*>({physicsJob}) };

	animationJob->nextJobs = { std::vector<Job*>({renderingJob}) };
	animationJob->previousJobs = { std::vector<Job*>({collisionJob}) };

	particlesJob->nextJobs = { std::vector<Job*>({renderingJob}) };
	particlesJob->previousJobs = { std::vector<Job*>({collisionJob}) };

	gameElementsJob->nextJobs = { std::vector<Job*>({renderingJob}) };
	gameElementsJob->previousJobs = { std::vector<Job*>({physicsJob}) };

	renderingJob->nextJobs = { std::vector<Job*>({}) };
	renderingJob->previousJobs = { std::vector<Job*>({animationJob, particlesJob, gameElementsJob}) };

	soundJob->nextJobs = { std::vector<Job*>({}) };
	soundJob->previousJobs = { std::vector<Job*>({}) };

	tasks.push_back(inputJob);
	tasks.push_back(physicsJob);
	tasks.push_back(collisionJob);
	tasks.push_back(animationJob);
	tasks.push_back(particlesJob);
	tasks.push_back(gameElementsJob);
	tasks.push_back(renderingJob);
	tasks.push_back(soundJob);

	// we notify the conditional variable so that all workers search for a new available job
	newTasksCV.notify_all();
}

int main(int argc, char* argv[])
{
	// to read the arguments
	int wantedThreads = 3;
	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "--threads") {
			if (i + 1 < argc) {
				if (std::stoi(argv[i + 1]) >= 0) {
					wantedThreads = std::stoi(argv[i + 1]);
				}
				else {
					return 1;
				}
			}
			else {
				return 1;
			}
		}
	}

	/*
	* =======================================
	* Setting memory allocator for Optick
	* In a real setting this could use a
	* specific debug-only allocator with
	* limits
	* =======================================
	*/
	OPTICK_SET_MEMORY_ALLOCATOR(
		[](size_t size) -> void* { return operator new(size); }, 
		[](void* p) { operator delete(p); }, 
		[]() { /* Do some TLS initialization here if needed */ }
	);


	OPTICK_THREAD("MainThread");
	atomic<bool> isRunning = true;
	// We spawn a "main" thread so we can have the actual main thread blocking to receive a potential quit
	thread main_runner([ &isRunning ]()
	{
		OPTICK_THREAD("Update");

		while ( isRunning ) 
		{
			OPTICK_FRAME("Frame");
			if ( isRunningParallel ) 
			{
				UpdateParallel();	
			} 
			else
			{
				UpdateSerial();
			}

			// for testing we made a sleep, it works with or without it
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}

	});

	// get the ammount of available threads and set it according to what the user wrote
	// if the user did not specify we use 3
	//		3 was found to be the best possibility after profiling, because due to the dependencies there is a point where three jobs could be run in parallel
	//		more than 3 was wasted threads, and less than three did waste parallelisation possibilities
	unsigned int processor_count = std::thread::hardware_concurrency();
	if (wantedThreads > processor_count) {
		printf("This system does not support that ammount of threads");
		printf("using %d threads (maximum)", processor_count);
		wantedThreads = processor_count;
	}

	vector<thread*> threads;
	for (int i = 0; i < wantedThreads; i++) {
		thread* workThread = new thread(WorkerMain);
		threads.push_back(workThread);
	}

	for (int i = 0; i < wantedThreads; i++) {
		threads[i]->join();
	}

	printf("Type anything to quit...\n");
	char c;
	scanf_s("%c", &c, 1);
	printf("Quitting...\n");
	isRunning = false;

	main_runner.join();

	OPTICK_SHUTDOWN();
}

