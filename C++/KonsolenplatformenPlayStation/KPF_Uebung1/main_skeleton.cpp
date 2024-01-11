/**
* Exercise: "DoubleEndedStackAllocator with Canaries" OR "Growing DoubleEndedStackAllocator with Canaries (VMEM)"
* Group members: Lukasz Tyburczy (gs20m021), Daniel Gerendas (gs20m016), Julia Pichler (gs20m007)
* We used C++11 standard
**/

#include "stdio.h"
#include <stdlib.h>
#include <iostream>
#include <assert.h>

namespace Tests
{
	void Test_Case_Success(const char* name, bool passed)
	{
		if (passed)
		{
			printf("[%s] passed the test!\n", name);
		}
		else
		{
			printf("[%s] failed the test!\n", name);
		}
	}

	void Test_Case_Failure(const char* name, bool passed)
	{	
		if (!passed)
		{
			printf("[%s] passed the test!\n", name);
		}
		else
		{
			printf("[%s] failed the test!\n", name);
		}
	}

	/** 
	* Example of how a test case can look like. The test cases in the end will check for
	* allocation success, proper alignment, overlaps and similar situations. This is an
	* example so you can already try to cover all cases you judge as being important by
	* yourselves.
	**/
	template<class A>
	bool VerifyAllocationSuccess(A& allocator, size_t size, size_t alignment)
	{
		void* mem = allocator.Allocate(size, alignment);

		if (mem == nullptr)
		{
			printf("[Error]: Allocator returned nullptr!\n");
			return false;
		}

		return true;
	}
}

// Assignment functionality tests are going to be included here 

#define WITH_DEBUG_CANARIES 1

/**
* You work on your DoubleEndedStackAllocator. Stick to the provided interface, this is
* necessary for testing your assignment in the end. Don't remove or rename the public
* interface of the allocator. Also don't add any additional initialization code, the
* allocator needs to work after it was created and its constructor was called. You can
* add additional public functions but those should only be used for your own testing.
**/
class DoubleEndedStackAllocator
{
public:
	// disabling all alternate constructors and assignment operators to prevent data override and for performance reasons
	DoubleEndedStackAllocator(DoubleEndedStackAllocator const&) = delete;
	DoubleEndedStackAllocator(DoubleEndedStackAllocator const&&) = delete;
	DoubleEndedStackAllocator& operator=(DoubleEndedStackAllocator const&) = delete;
	DoubleEndedStackAllocator& operator=(DoubleEndedStackAllocator const&&) = delete;

	DoubleEndedStackAllocator(size_t max_size) {
		// we check if the canaries should be added
		// if they should, the size is not zero and taken into account
		if (WITH_DEBUG_CANARIES) {
			canarySize = sizeof(canary);
		}
		// we save two types of data in the metadata
		metaDataSize = sizeof(size_t) * 2;
		maxSize = max_size;
		// we save the pointer containing the starter address of the requested memory
		front = reinterpret_cast<uintptr_t>(malloc(max_size));
		// this is the next address where something new could be stored in the front part of the stack
		frontCurrent = front;
		// this is the next address where something new could be stored in the back part of the stack
		backCurrent = front + max_size;
	}

	void* Allocate(size_t size, size_t alignment) {
		// we calculate the next position where we can store data, 
		// taking into account the size of metadata and canary (those are stored before the returned address)
		uintptr_t alignedAddress = AlignUp(frontCurrent + metaDataSize + canarySize, alignment);

		// check if there is enough space in the stack
		if (alignedAddress + size + canarySize > backCurrent || alignedAddress + size + canarySize > front + maxSize)
		{
			assert(alignedAddress + size + canarySize <= backCurrent);
			assert(alignedAddress + size + canarySize <= front + maxSize);
			return nullptr;
		}
		// save old front address because we need it to calculate the padding
		uintptr_t oldFrontCurrent = frontCurrent;

		// we move the current front address to its new position taking into account the size of the data and if there should exist canaries
		frontCurrent = alignedAddress + size + canarySize;

		// we save the amount of bytes allocated as first metadata
		uintptr_t metaAddress = alignedAddress - metaDataSize / 2;
		size_t* metaPointer = reinterpret_cast<size_t*>(metaAddress);
		*metaPointer = size;	

		// we save the padding as second metadata (this is the distance from the new alligned address to the last byte of the previous allocation)
		metaAddress = metaAddress - metaDataSize / 2;
		metaPointer = reinterpret_cast<size_t*>(metaAddress);
		*metaPointer = alignedAddress - oldFrontCurrent;

		// we save the canaries if needed
		if (WITH_DEBUG_CANARIES) {
			// the first canary before metadata
			metaAddress -= canarySize;
			int* canaryPointer = reinterpret_cast<int*>(metaAddress);
			*canaryPointer = canary;

			// the end canary after block of allocated space
			metaAddress = frontCurrent - canarySize;
			canaryPointer = reinterpret_cast<int*>(metaAddress);
			*canaryPointer = canary;
		}

		return reinterpret_cast<void*>(alignedAddress);
	}

	void* AllocateBack(size_t size, size_t alignment) { 
		// we calculate the next position where we can store data, 
		// taking into account the size of allocated data and the canary after the allocated block
		uintptr_t alignedAddress = AlignDown(backCurrent - canarySize - size, alignment);

		// check if there is enough space in the stack
		if (alignedAddress - canarySize - metaDataSize < frontCurrent || alignedAddress - canarySize - metaDataSize < front)
		{
			assert(alignedAddress - canarySize - metaDataSize >= frontCurrent);
			assert(alignedAddress - canarySize - metaDataSize >= front);
			return nullptr;
		}
		// save old back address because we need it to calculate the padding
		uintptr_t oldBackCurrent = backCurrent;

		// we move the current back address to its new position taking into account the size of the metadata and if canaries should be added
		backCurrent = alignedAddress - canarySize - metaDataSize;

		// we save the amount of bytes allocated as first metadata
		uintptr_t metaAddress = alignedAddress - metaDataSize / 2;
		size_t* metaPointer = reinterpret_cast<size_t*>(metaAddress);
		*metaPointer = size;

		// we save the padding as second metadata
		metaAddress = metaAddress - metaDataSize / 2;
		metaPointer = reinterpret_cast<size_t*>(metaAddress);
		*metaPointer = oldBackCurrent - (alignedAddress + size);

		// we save the canaries if needed
		if (WITH_DEBUG_CANARIES) {
			// the first canary before the metadata
			metaAddress -= canarySize;
			int* canaryPointer = reinterpret_cast<int*>(metaAddress);
			*canaryPointer = canary;

			// the end canary after allocated block
			metaAddress = alignedAddress + size;
			canaryPointer = reinterpret_cast<int*>(metaAddress);
			*canaryPointer = canary;
		}

		return reinterpret_cast<void*>(alignedAddress);
	}

	void Free(void* memory) {
		// move the front pointer back to the start of this block
		frontCurrent = reinterpret_cast<uintptr_t>(memory);

		// read the padding and size for the calculation
		size_t padding = *reinterpret_cast<size_t*>(frontCurrent - sizeof(size_t) * 2);	// padding is the first data in the block
		size_t size = *reinterpret_cast<size_t*>(frontCurrent - sizeof(size_t));		// size is in the second part

		// if canaries are enabled we have to assert they have remained the same after the user manipulated data
		if (WITH_DEBUG_CANARIES) {
			uintptr_t firstCanaryAddress = frontCurrent - metaDataSize - canarySize;
			uintptr_t secondCanaryAddress = frontCurrent + size;
			int canary1 = *reinterpret_cast<int*>(firstCanaryAddress);
			int canary2 = *reinterpret_cast<int*>(secondCanaryAddress);
			assert(canary1 == canary2);
		}

		// move front address back based on the saved padding
		frontCurrent -= padding;
	}

	void FreeBack(void* memory) {
		// move the back pointer back to the start of this block
		backCurrent = reinterpret_cast<uintptr_t>(memory);

		// read the padding and size for the calculation
		size_t padding = *reinterpret_cast<size_t*>(backCurrent - sizeof(size_t) * 2);	// padding is the first data in the block
		size_t size = *reinterpret_cast<size_t*>(backCurrent - sizeof(size_t));			// size is in the second part

		// if canaries are enabled we have to assert they have remained the same after the user manipulated data
		if (WITH_DEBUG_CANARIES) {
			uintptr_t firstCanaryAddress = backCurrent - metaDataSize - canarySize;
			uintptr_t secondCanaryAddress = backCurrent + size;
			int canary1 = *reinterpret_cast<int*>(firstCanaryAddress);
			int canary2 = *reinterpret_cast<int*>(secondCanaryAddress);
			assert(canary1 == canary2);
		}

		// move back address based on the saved size and padding
		backCurrent += size + padding;
	}

	void Reset(void) {
		// only move the front and back addresses to their starting positions, everything will get eventually overwritten
		frontCurrent = front;
		backCurrent = front + maxSize;
	}

	~DoubleEndedStackAllocator(void) {
		// just free all the memory allocated at the start based on the saved first front address
		free(reinterpret_cast<void*>(front));
	}

	void printFrontCurrent() {
		std::cout << "front current: " << frontCurrent << std::endl;
	}

	void printBackCurrent() {
		std::cout << "back current: " << backCurrent << std::endl;
	}

private:
	size_t maxSize;				// the size of the stack in bytes
	uintptr_t front;			// the first address of the stack
	uintptr_t frontCurrent;		// the next address where something new could be stored in the front part of the stack
	uintptr_t backCurrent;		// the next address where something new could be stored in the back part of the stack
	int canary = 0xABCD;		// the canary number
	int canarySize = 0;			// variable that lets us know if we had to add the canary bytes to our computations
	int metaDataSize;			// the size of the metadata in bytes

	// calculate the next aligned address
	uintptr_t AlignUp(uintptr_t address, size_t alignment)
	{
		return (address & ~(alignment - 1)) + alignment;
	}
	
	// calculate the previous aligned address
	uintptr_t AlignDown(uintptr_t address, size_t alignment)
	{
		return (address & ~(alignment - 1));
	}
};


int main()
{
	// You can add your own tests here, I will call my tests at then end with a fresh instance of your allocator and a specific max_size
	{
		// We have tested everything manually and erased that code	
	}

	// You can do whatever you want here in the main function

	// Here the assignment tests will happen - it will test basic allocator functionality. 
	{

	}
}