#include "stdafx.h"
#include "CppUnitTest.h"

#include "Grid.h"
#include <cstringt.h>
#include <random>

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

			std::wstring ToString(Grid<int> const& value)
			{
				return std::to_wstring(value.grid_data[0]);
			}
		}
	}
}


namespace GridTesting
{		
	TEST_CLASS(GridTesting)
	{
	public:

		TEST_METHOD(GridCreationWithDefaultValue)
		{
			typedef Grid<int> intGrid;

			intGrid::dimension_type rowCount = 3;
			intGrid::dimension_type columnCount = 5;

			intGrid grid = intGrid(columnCount, rowCount);

			for (intGrid::dimension_type columnIndex = 0;
				columnIndex < columnCount; columnIndex++)
			{
				for (intGrid::dimension_type rowIndex = 0;
					rowIndex < rowCount; rowIndex++)
				{
					grid[columnIndex][rowIndex] = columnIndex + rowIndex * columnCount;
				}
			}

			Assert::AreEqual(rowCount, grid.GetRowCount());
			Assert::AreEqual(columnCount, grid.GetColumnCount());

			for (intGrid::dimension_type columnIndex = 0;
				columnIndex < columnCount; columnIndex++)
			{
				for (intGrid::dimension_type rowIndex = 0; rowIndex < rowCount; rowIndex++)
				{
					Assert::AreEqual(grid.GetCell(columnIndex, rowIndex), 
						columnIndex + rowIndex * columnCount);

					Assert::AreEqual(grid[columnIndex][rowIndex], columnIndex + rowIndex * columnCount);
				}
			}
		}

		TEST_METHOD(CopyConstructorTest)
		{
			Grid<double> originalGrid(2, 5, 0.5);
			Grid<double> clonedGrid(originalGrid);

			Assert::AreEqual(clonedGrid.columnCount, originalGrid.columnCount);
			Assert::AreEqual(clonedGrid.rowCount, originalGrid.rowCount);

			for (Grid<double>::dimension_type rowIndex = 0; rowIndex < originalGrid.GetRowCount(); rowIndex++)
			{
				for (Grid<double>::dimension_type columnIndex = 0; columnIndex < originalGrid.GetColumnCount(); columnIndex++)
				{
					double evaluatedValue = originalGrid.GetCell(columnIndex, rowIndex);
					Assert::AreEqual(clonedGrid.GetCell(columnIndex, rowIndex), evaluatedValue);
				}
			}
		}

		TEST_METHOD(MoveConstructorTest)
		{
			Grid<int> destination(GetGridForMoveTest());

			for (Grid<int>::dimension_type columnIndex = 0;
			columnIndex < destination.columnCount; columnIndex++)
			{
				for (Grid<int>::dimension_type rowIndex = 0; rowIndex < destination.rowCount; rowIndex++)
				{
					Assert::AreEqual(destination.GetCell(columnIndex, rowIndex), 5);
				}
			}
		}

		Grid<int> GetGridForMoveTest()
		{
			return Grid<int>(3, 3, 5);
		}

		TEST_METHOD(OperatorsTest)
		{
			Grid<int> testGrid = Grid<int>(4, 6, 21);
			
			///[] Write Operator
			testGrid[3][3] = 1024;
			Assert::AreEqual(testGrid.GetCell(3, 3), 1024);

			///[] Read Operator
			Assert::AreEqual(testGrid[3][3], 1024);

			auto f1 = [&testGrid] { return testGrid.GetCell(4,7); };

			Assert::ExpectException<std::out_of_range>(f1);
		}

		TEST_METHOD(ResizeGridTest)
		{
			Grid<int> testGrid(8, 3, 5);
			Grid<int> cloneGrid(testGrid);

			Assert::AreNotSame(testGrid, cloneGrid);

			testGrid.ResizeGrid(10, 6, 8);

			Assert::AreEqual(testGrid.GetColumnCount(), (unsigned short)10);
			Assert::AreEqual(testGrid.GetRowCount(), (unsigned short)6);

			for (Grid<int>::dimension_type row = 0; row < testGrid.GetRowCount(); row++)
			{
				for (Grid<int>::dimension_type column = 0; column < testGrid.GetColumnCount(); column++)
				{
					Assert::AreEqual(testGrid.GetCell(column, row), 8);
				}
			}

			testGrid.ResizeGrid(1, 1, 1);

			Assert::AreEqual(testGrid.GetColumnCount(), (unsigned short)1);
			Assert::AreEqual(testGrid.GetRowCount(), (unsigned short)1);

			for (Grid<int>::dimension_type row = 0; row < testGrid.GetRowCount(); row++)
			{
				for (Grid<int>::dimension_type column = 0; column < testGrid.GetColumnCount(); column++)
				{
					Assert::AreEqual(testGrid.GetCell(column, row), 1);
				}
			}

			auto f1 = [&testGrid] { return testGrid.ResizeGrid(0, 0, 0); };

			Assert::ExpectException<std::invalid_argument>(f1);
		}

		TEST_METHOD(ResizeGridKeepDataTest)
		{
			Grid<int> testGrid(8, 3, 5);
			Grid<int> cloneGrid(testGrid);

			Assert::AreNotSame(testGrid, cloneGrid);

			testGrid.ResizeGridPreserveData(10, 6, 8);

			Assert::AreEqual(testGrid.GetColumnCount(), (unsigned short)10);
			Assert::AreEqual(testGrid.GetRowCount(), (unsigned short)6);

			for (Grid<int>::dimension_type row = 0; row < testGrid.GetRowCount(); row++)
			{
				for (Grid<int>::dimension_type column = 0; column < testGrid.GetColumnCount(); column++)
				{
					if (row < cloneGrid.GetRowCount() && column < cloneGrid.GetColumnCount())
					{
						Assert::AreEqual(cloneGrid.GetCell(column, row), testGrid.GetCell(column, row));
					}
					else
					{
						Assert::AreEqual(testGrid.GetCell(column, row), 8);
					}
					
				}
			}

			testGrid.ResizeGridPreserveData(2, 1, 7);

			Assert::AreEqual(testGrid.GetColumnCount(), (unsigned short)2);
			Assert::AreEqual(testGrid.GetRowCount(), (unsigned short)1);

			for (Grid<int>::dimension_type row = 0; row < testGrid.GetRowCount(); row++)
			{
				for (Grid<int>::dimension_type column = 0; column < testGrid.GetColumnCount(); column++)
				{
					if (row < cloneGrid.GetRowCount() && column < cloneGrid.GetColumnCount())
					{
						Assert::AreEqual(testGrid.GetCell(column, row), 5);
					}
					else
					{
						Assert::AreEqual(testGrid.GetCell(column, row), 7);
					}
				}
			}

			auto f1 = [&testGrid] { return testGrid.ResizeGrid(0, 0, 0); };

			Assert::ExpectException<std::invalid_argument>(f1);
		}

		TEST_METHOD(TestSwap)
		{
			Grid<int> a(3, 5, 6);
			Grid<int> b(5, 3, 2);

			a.swap(b);

			for (auto i = a.begin(); i < a.end(); i++)
			{
				Assert::AreEqual(*i, 2);
			}

			for (auto i = b.begin(); i < b.end(); i++)
			{
				Assert::AreEqual(*i, 6);
			}
		}

		TEST_METHOD(GridIteratorTesting)
		{
			Grid<int> testGrid(3, 7, 2);

			for (Grid<int>::dimension_type row = 0; row < testGrid.GetRowCount(); row++)
			{
				for (Grid<int>::dimension_type column = 0; column < testGrid.GetColumnCount(); column++)
				{
					testGrid[column][row] = std::rand() % 100 + 1;
				}
			}

			auto iter = testGrid.begin();

			Assert::AreEqual(*iter, testGrid.grid_data[0]);

			++iter;

			Assert::AreEqual(*iter, testGrid.grid_data[1]);

			Assert::AreEqual(iter[6], testGrid.grid_data[7]);

			--iter;

			Assert::AreEqual(*iter, testGrid.grid_data[0]);

			iter = testGrid.end();

			iter--;

			Assert::AreEqual(*iter, testGrid.grid_data[testGrid.size() - 1]);

			Assert::IsTrue(testGrid.begin() < testGrid.end());

			unsigned int index = 0;

			for (auto i = testGrid.begin(); i < testGrid.end(); i++)
			{
				Assert::AreEqual(*i, testGrid.grid_data[index]);
				index++;
			}

			Assert::AreEqual(index, testGrid.size());

		}
	};

}