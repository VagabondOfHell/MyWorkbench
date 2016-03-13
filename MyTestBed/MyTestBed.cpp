// MyTestBed.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Grid.h"
#include <iostream>
#include <random>

template<typename Grid_Data_Type>
void PrintGrid(const Grid<Grid_Data_Type>& grid)
{
	int index = 0;

	for (Grid<Grid_Data_Type>::Dimension_Size row = 0; row < grid.GetRowCount(); row++, index++)
	{
		for (Grid<Grid_Data_Type>::Dimension_Size column = 0; column < grid.GetColumnCount(); column++)
		{
		
			std::cout << index << ":" << grid[column][row] << std::endl;
		}
	}
}

template<typename Grid_Data_Type>
void PrintIndices(const Grid<Grid_Data_Type>& grid)
{
	for (Grid<Grid_Data_Type>::Dimension_Size row = 0; row < grid.GetRowCount(); row++)
	{
		for (Grid<Grid_Data_Type>::Dimension_Size column = 0; column < grid.GetColumnCount(); column++)
		{
		
			std::cout << "[" << column << "," << row << "(" << grid.GetOneDimensionIndex(column,row) << ") = " << 
					grid.GetCell(column, row) << "]" ;
		}

		printf("\n");
	}
}

int main()
{
	Grid<int> testGrid(5, 3, 7);

	for (size_t x = 0; x < testGrid.GetColumnCount(); x++)
	{
		for (size_t y = 0; y < testGrid.GetRowCount(); y++)
		{
			testGrid.GetCell(x, y) = std::rand() % 80 + 1;
		}
	}

	PrintIndices(testGrid);

	std::cout << "Check Value: " << testGrid[2][1] << std::endl;
	std::cout << "Check Value Method: " << testGrid.GetCell(2, 1) << std::endl;

	testGrid.ResizeGridPreserveData(8, 5, 9);
	
	printf("\n");

	PrintIndices(testGrid);

	getchar();

    return 0;
}

