#pragma once

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define __CL_ENABLE_EXCEPTIONS

#include <string>
#include "Timing.h"
#include <CL/cl.hpp>



class GridCL {
private:
	bool* cellsArray; // array of the states represented (y,x) for performance optimization
	bool* workArray; // array used to save the result of each generation calculation
	int height;	// height of the matrix
	int width;	// width of the matrix
	int currentGeneration = 0; // the current generation of the cellsArray
	bool measure; // if the time should be measured
	Timing* timer = nullptr; // pointer to Timing class to make the measurements

	const std::string KERNEL_FILE = "kernel.cl";	// name of the kernel file
	cl_int err = CL_SUCCESS;	// to save wether an error was found
	cl::Program program;	// the CL compiled program
	std::vector<cl::Device> devices;	// the list of cl devices available
	cl::Context context;	// the cl context to which we send the commands
	cl::CommandQueue queue;	// the queue with which we communicate with the devices

	cl::Buffer bufferIn;	// the cl buffer that contains the data in the beginning 
	cl::Buffer bufferOut;	// the cl buffer that is used to store each generations result

	bool initializeCL();	// to initialize all the cl dependent code
	bool loadKernelFile(const std::string &name) throw (cl::Error);	// to load a kernel from the given kernel name and compile the program
	int compute();	// computes one generation. Initialize and loadKernelFile have to be called before

public:
	bool openFile(const std::string &fileName, bool measure);	// loads the specified file to memory
	void writeResult(const std::string &path);	// saves the current cellsArray state in the same format
	void computeNextGeneration();	// updates the cellsArray in one step
	void computeUntilThisGeneration(int generation); // updates the cellsArray until it reaches the speciefied generation sequentially
	
};