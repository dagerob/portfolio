#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include "Grid.h"
#include "GridCL.h"
#include <vector>
#include <string>
#include <sstream>
#include "Timing.h"
#include <CL/cl.hpp>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

enum Mode
{
	NONE,
	SEQ,
	OMP,
	OCLCPU,
	OCLGPU
};



int main(int argc, char* argv[])
{
	std::vector<std::string> consoleParams;
	std::string loadFile = "NULL", saveFile = "result.gol", mode;
	int generations = 1;
	Mode selectedMode = Mode::NONE;
	bool measure = false;
	int ompThreads = 8;

	for (int i = 1; i < argc; i++) {
		consoleParams.push_back(argv[i]);
	}

	// find the load file
	for (auto it = consoleParams.begin(); it != consoleParams.end(); it++) {
		if ((*it).compare("--load") == 0) {
			loadFile = *(it + 1);
		}
	}

	// find the save file
	for (auto it = consoleParams.begin(); it != consoleParams.end(); it++) {
		if ((*it).compare("--save") == 0) {
			saveFile = *(it + 1);
		}
	}

	// get the generations to be processed
	for (auto it = consoleParams.begin(); it != consoleParams.end(); it++) {
		if ((*it).compare("--generations") == 0) {
			std::stringstream intValue(*(it + 1));
			intValue >> generations;
		}
	}

	// get wether it has to measure
	for (auto it = consoleParams.begin(); it != consoleParams.end(); it++) {
		if ((*it).compare("--measure") == 0) {
			measure = true;
		}
	}

	// get which mode we need to use
	bool modeFound = false;
	for (auto it = consoleParams.begin(); it != consoleParams.end(); it++) {
		if ((*it).compare("--mode") == 0) {
			mode = *(it + 1);
			// the openMP mode was selected
			if (mode.compare("omp") == 0) {
				selectedMode = Mode::OMP;
				if ((*(it + 2)).compare("--threads") == 0) {
					// save the number of threads
					std::stringstream intValue(*(it + 3));
					intValue >> ompThreads;
					std::cout << "using "<< ompThreads <<" threads" << std::endl;
				}
				else {
					std::cout << "using default 8 threads" << std::endl;
				}
			}
			// the sequential mode was selected
			else if (mode.compare("seq") == 0) {
				selectedMode = Mode::SEQ;
			}
			else if (mode.compare("ocl") == 0) {
				selectedMode = Mode::OCLGPU;
			}
			else {
				selectedMode = Mode::SEQ;
				std::cout << "using default mode sequential" << std::endl;
			}
		}
	}

	// find which mode was selected to create the appropiate Grid object
	if (selectedMode == Mode::NONE) {
		return 0;
	}
	else if (selectedMode == Mode::SEQ) {
		Grid myGrid;
		myGrid.openFile(loadFile, measure);
		myGrid.computeUntilThisGenerationSeq(generations);
		myGrid.writeResult(saveFile);
	}
	else if (selectedMode == Mode::OMP) {
		Grid myGrid;
		myGrid.openFile(loadFile, measure);
		myGrid.computeUntilThisGenerationOMP(generations, ompThreads);
		myGrid.writeResult(saveFile);
	}
	else if (selectedMode == Mode::OCLGPU) {
		GridCL grid;
		if (!grid.openFile(loadFile, measure))
			return 0;
		grid.computeUntilThisGeneration(generations);
		grid.writeResult(saveFile);
	}
	// print the measurments
	if (measure) {
		Timing* timer = Timing::getInstance();
		std::cout << timer->getResults() << std::endl;
	}
}

