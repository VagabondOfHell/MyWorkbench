// MyTestBed.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Grid.h"

int main()
{
	Grid<int> testGrid(5, 3, 7);
	//Grid<int> newTestGrid(testGrid);

	testGrid.ResizeGridPreserveData(8, 5, 9);

	testGrid.ResizeGrid(2, 2, 3);

    return 0;
}

