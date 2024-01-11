#pragma once
#include <string>
#include "Timing.h"

class Grid {
private:
	bool** cellsArray; // array of the states represented (y,x) for performance optimization
	bool** workArray; // array used to save the result of each generation calculation
	int height;	// height of the matrix
	int width;	// width of the matrix
	int currentGeneration = 0; // the current generation of the cellsArray
	bool measure; // if the time should be measured
	bool isCellAlive(int y, int x);	// returns wether this cell hast to live or die in next generation
	Timing* timer = nullptr;

public:
	void openFile(const std::string &fileName, bool measure);	// loads the specified file to memory
	void writeResult(const std::string &path);	// saves the current cellsArray state in the same format
	void computeNextGeneration();	// updates the cellsArray in one step
	void computeNextGenerationOMP(int threads);	// updates the cellsArray in one step using OpenMP
	void computeUntilThisGenerationSeq(int generation); // updates the cellsArray until it reaches the speciefied generation sequentially
	void computeUntilThisGenerationOMP(int generation, int threads); // updates the cellsArray until it reaches the speciefied generation using openMP
};