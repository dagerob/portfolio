#pragma once
#include <vector>
#include <functional>
#include <cstdio>

// the Object that contains the job to be done
class Job {
public:
	// used to know to which category this job belongs
	enum Category {
		INPUT,
		PHYSICS,
		COLLISION,
		ANIMATION,
		PARTICLES,
		GAMEELEMENTS,
		RENDERING,
		SOUND,
		NONE
	};

	std::vector<Job*> previousJobs;	// which tasks should be completed before this one
	std::vector<Job*> nextJobs;		// which Jobs depned on this job being finished
	bool isDone;					// to know if the job is finished			
	Job::Category thisCategory;
	void (*pTask)();				// the code that is going to be run

	Job(std::vector<Job*> previousJobs, std::vector<Job*> nextJobs, Category cat, void(*pTask)()) {
		// when initializing copy the contents not just the reference
		this->previousJobs.assign(previousJobs.begin(), previousJobs.end());
		this->nextJobs.assign(nextJobs.begin(), nextJobs.end());
		this->pTask = pTask;
		isDone = false;
		thisCategory = cat;
	};

	Job() { 
		isDone = false;
		thisCategory = Category::NONE;
	};

	Job& operator=(const Job& other) {
		if (this == &other)
			return *this;
		// when initializing copy the contents not just the reference
		previousJobs.assign(other.previousJobs.begin(), other.previousJobs.end());
		nextJobs.assign(other.nextJobs.begin(), other.nextJobs.end());
		this->pTask = other.pTask;
		isDone = other.isDone;
		thisCategory = other.thisCategory;
		return *this;
	}

	Job(const Job& other) {
		// when initializing copy the contents not just the reference
		previousJobs.assign(other.previousJobs.begin(), other.previousJobs.end());
		nextJobs.assign(other.nextJobs.begin(), other.nextJobs.end());
		this->pTask = other.pTask;
		isDone = other.isDone;
		thisCategory = other.thisCategory;
	};

	// just to run the function being pointed at by the function pointer
	void execute() const {
		pTask();
	};

	// Here we just remove this Job from the previousJobs on all the jobs stored in nextJobs
	void finishJob() {
		for (int i = 0; i < nextJobs.size(); i++) {
			nextJobs.at(i)->removeDependency(thisCategory);
		}
		isDone = true;
	}

	// a job can be worked on if the previousJobs vector is empty
	bool canBeWorkedOn() {
		if (previousJobs.size() == 0) {
			return true;
		}
		return false;
	}

	// Here we look for and erase the job on previousJobs belonging to the given category
	void removeDependency(Job::Category cat) {
		int indexToRemove = -1;
		for (int i = 0; i < previousJobs.size(); i++) {
			if (cat == previousJobs[i]->thisCategory) {
				indexToRemove = i;
				break;
			}
		}
		previousJobs.erase(previousJobs.begin() + indexToRemove);
	}
};