
/*
* a kernel that simply prints the (2-dimensional) thread id
* processes no data
*/
__kernel void hello(void)
{
	int tidX = get_global_id(0);
	int tidY = get_global_id(1);
	printf("hi from thread (%d,%d) \n", tidX, tidY);
}

/*
* a kernel that add the elements of two vectors pairwise
*/
__kernel void vector_add(
	__global const int *A,
	__global const int *B,
	__global int *C)
{
	int i = get_global_id(0);
	C[i] = A[i] + B[i];
}

/*
* a kernel that add the elements of two vectors pairwise
*/
__kernel void check_alive(__global bool *cellsIn, __global bool *cellsOut, int width, int height)
{
	int id = get_global_id(0);
	bool val = cellsIn[id];

	int x = id % width; 
    int y = id / width;

	int liveNeighbourCounter = 0;
	bool lastState = cellsIn[id];
	for (int i = -1; i < 2; i++) {
		int yValue = y + i;
		if (yValue < 0)
			yValue = height - 1;
		else if (yValue >= height)
			yValue = 0;
		for (int j = -1; j < 2; j++) {
			int xValue = x + j;
			if (xValue < 0)
				xValue = width - 1;
			else if (xValue >= width)
				xValue = 0;

			if (xValue == x && yValue == y)
				continue;

			if (cellsIn[yValue * width + xValue])
				liveNeighbourCounter++;
		}
	}
	
	if (!lastState && liveNeighbourCounter == 3){
		cellsOut[id] = true;
		return;
	}
		
	
	if (lastState) {
		if (liveNeighbourCounter == 2 || liveNeighbourCounter == 3){
			cellsOut[id] = true;
			return;
		}
	}

	cellsOut[id] = false;

}