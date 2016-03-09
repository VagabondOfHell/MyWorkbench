#include "stdafx.h"
#include "CppUnitTest.h"

#include "Grid.h"
#include <cstringt.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			std::wstring ToString(unsigned __int16 const& value)
			{
				return std::to_wstring(value);
			}
		}
	}
}


namespace GridTesting
{		
	TEST_CLASS(GridTesting)
	{
	public:
		TEST_METHOD(CreateAndDestroyGrid)
		{
			typedef Grid<int> intGrid;

			intGrid::dimension_size rowCount = 2;
			intGrid::dimension_size columnCount = 5;

			int defaultValue = 7;
			
			intGrid grid = intGrid(columnCount, rowCount, defaultValue);
			
			Assert::AreEqual(rowCount, grid.GetRowCount());
			Assert::AreEqual(columnCount, grid.GetColumnCount());

			for (intGrid::dimension_size columnIndex = 0;
			columnIndex < columnCount; columnIndex++)
			{
				for (intGrid::dimension_size rowIndex = 0; rowIndex < rowCount; rowIndex++)
				{
					Assert::AreEqual(grid.GetCell(columnIndex, rowIndex), defaultValue);

					Assert::AreEqual(grid[columnIndex][rowIndex], defaultValue);
				}
			}
		}

		TEST_METHOD(CreateAndDestroyGridManual)
		{
			typedef Grid<int> intGrid;

			intGrid::dimension_size rowCount = 3;
			intGrid::dimension_size columnCount = 5;

			intGrid grid = intGrid(columnCount, rowCount);

			for (intGrid::dimension_size columnIndex = 0; 
				columnIndex < columnCount; columnIndex++)
			{
				for (intGrid::dimension_size rowIndex = 0; 
					rowIndex < rowCount; rowIndex++)
				{
					grid[columnIndex][rowIndex] = columnIndex + rowIndex * columnCount;
				}
			}

			Assert::AreEqual(rowCount, grid.GetRowCount());
			Assert::AreEqual(columnCount, grid.GetColumnCount());

			for (intGrid::dimension_size columnIndex = 0; 
				columnIndex < columnCount; columnIndex++)
			{
				for (intGrid::dimension_size rowIndex = 0; rowIndex < rowCount; rowIndex++)
				{
					Assert::AreEqual(grid.GetCell(columnIndex, rowIndex), 
						columnIndex + rowIndex * columnCount);

					Assert::AreEqual(grid[columnIndex][rowIndex], columnIndex + rowIndex * columnCount);
				}
			}
		}

	};
}