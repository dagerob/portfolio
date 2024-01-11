#pragma once
#include "GridCL.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Timing.h"
#include <CL/cl.hpp>

bool GridCL::openFile(const std::string &fileName, bool measure) {
	// just store if we have to record measurments of time or not
	measure = measure;
	if (measure)
		timer = Timing::getInstance();

	// start measuring the init time
	if (timer != nullptr)
		timer->startSetup();

	std::ifstream thisFile(fileName);
	std::string line;
	if (thisFile.is_open()) {
		// flag to only look for dimensions in first line
		bool firstLine = true;

		// go through the whole file and search load the data
		int lineCounter = 0;
		while (std::getline(thisFile, line)) {

			// if it is the first line initialize dimmensions variables
			if (firstLine)
			{
				// obtain the data for height and width fro the first line of the file
				firstLine = false;
				int commaIndex = line.find(",");
				std::string x = line.substr(0, commaIndex);
				std::string y = line.substr(commaIndex + 1);
				std::stringstream toIntWidth(x);
				toIntWidth >> width;
				std::stringstream toIntHeight(y);
				toIntHeight >> height;

				// now that we know the height and width, initialize arrays for this system
				cellsArray = new bool[width * height];
				workArray = new bool[width * height];
				continue;
			}

			// if it is not the first line, start saving the data to the array
			// we map the data in a 1D array
			for (int i = 0; i < width; i++) {
				char thisChar = line.at(i);
				if (thisChar == '.')
					cellsArray[lineCounter*width + i] = false;
				else
					cellsArray[lineCounter*width + i] = true;
			}
			lineCounter++;
		}
		thisFile.close();
	}
	else {
		std::cout << "could not open file" << std::endl;
		return false;
	}

	// initialize code for OpenCL
	bool res = initializeCL();
	if (!res)
		return false;
	res = loadKernelFile(KERNEL_FILE);
	if (!res)
		return false;

	// stop the setup time
	if (timer != nullptr)
		timer->stopSetup();

	return true;
}

void GridCL::writeResult(const std::string &path) {

	// start measuring the finishing time
	if (timer != nullptr)
		timer->startFinalization();

	// read the result of the operations
	queue.enqueueReadBuffer(bufferIn, CL_TRUE, 0, height * width * sizeof(bool), &cellsArray[0]);

	// save the data to the given file
	// we have to map the 1D array to the 2D arrangement in the output file
	std::ofstream thisFile(path);
	if (thisFile.is_open()) {
		thisFile << width << "," << height << std::endl;
		for (int i = 0; i < width * height; i++) {
			char thisChar;
			if (cellsArray[i])
				thisChar = 'x';
			else
				thisChar = '.';
			thisFile << thisChar;
			if (i % width == width - 1){
				// need new line
				thisFile << std::endl;
			}
		}
		thisFile.close();
	}
	else {
		std::cout << "could not save file" << std::endl;
	}

	// stop finalization time
	if (timer != nullptr) {
		timer->stopFinalization();
	}
}

void GridCL::computeUntilThisGeneration(int generation) {
	// here we compute until we reached the desired generation
	// we start measuring the compute time
	if (timer != nullptr)
		timer->startComputation();

	while (currentGeneration < generation) {
		computeNextGeneration();
		currentGeneration++;
	}

	// we wait for all the sent commands to finish executing
	queue.finish();
	// now we are sure the computation is over, so stop measuring the compute time
	if (timer != nullptr)
		timer->stopComputation();
}

void GridCL::computeNextGeneration() {
	// we call the compute function that uses the kernel to parallelize the working load
	compute();

	// we swap the arrays
	bool *holder = workArray;
	workArray = cellsArray;
	cellsArray = holder;
}

bool GridCL::loadKernelFile(const std::string &name) throw (cl::Error) {
	// load and build the kernel
	std::ifstream sourceFile(name);
	// if no file found, log error and quit
	if (!sourceFile)
	{
		std::cout << "kernel source file " << KERNEL_FILE << " not found!" << std::endl;
		return false;
	}
	// parse the file
	std::string sourceCode(
		std::istreambuf_iterator<char>(sourceFile),
		(std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
	program = cl::Program(context, source);

	// try compiling the file, if an error is found, log it and quit
	try {
		program.build(devices);
	}
	catch (cl::Error err) {
		// error handling
		// if the kernel has failed to compile, print the error log
		std::string s;
		program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, &s);
		std::cout << s << std::endl;
		program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_OPTIONS, &s);
		std::cout << s << std::endl;

		std::cerr
			<< "ERROR: "
			<< err.what()
			<< "("
			<< err.err()
			<< ")"
			<< std::endl;
		return false;
	}
	return true;
}

bool GridCL::initializeCL() {
	// get available platforms 
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if (platforms.size() == 0) {
		std::cout << "No OpenCL platforms available!\n";
		return false;
	}

	// create a context and get available devices
	cl::Platform platform = platforms[0];
	cl_context_properties properties[] =
	{ CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
	cl::Context newContext(CL_DEVICE_TYPE_GPU, properties);
	context = newContext;
	
	devices = context.getInfo<CL_CONTEXT_DEVICES>();

	// create the commandqueue
	cl::CommandQueue newQueue(context, devices[0], 0, &err);
	queue = newQueue;

	// create the buffers
	bufferIn = cl::Buffer(context, CL_MEM_READ_WRITE, height * width * sizeof(bool));
	bufferOut = cl::Buffer(context, CL_MEM_READ_WRITE, height * width * sizeof(bool));

	// write the initial data to the device
	queue.enqueueWriteBuffer(bufferIn, CL_TRUE, 0, height * width * sizeof(bool), &cellsArray[0]);
	// wait for the data to be copied before executing further
	queue.finish();
}

int GridCL::compute() {

	// we create the kernel using the check_alive function in the loaded program
	cl::Kernel checkKernel(program, "check_alive", &err);
	// we set the arguments of the function
	checkKernel.setArg(0, bufferIn);
	checkKernel.setArg(1, bufferOut);
	checkKernel.setArg(2, width);
	checkKernel.setArg(3, height);
	
	// we set the values needed to enqueue the kernel execution
	cl::NDRange global(height * width);
	cl::NDRange local(1);
	cl::NDRange offset(0);
	// we execute the kernel
	queue.enqueueNDRangeKernel(checkKernel, offset, global, local);

	// after having executed the command, we swap the buffers
	cl::Buffer temp = bufferIn;
	bufferIn = bufferOut;
	bufferOut = temp;

	return EXIT_SUCCESS;
}