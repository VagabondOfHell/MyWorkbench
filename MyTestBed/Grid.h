#pragma once
#include <limits>
#include <stdexcept>

template<class Data_Type>
class Grid
{
public:
	///The following are required to be 'accepted' in the STL
	typedef Data_Type value_type;
	typedef Data_Type& reference;
	typedef const Data_Type& const_reference;

	///We specify int32 because it is just barely large enough to 
	///hold the product of row * column if both are int16. Size_t can be
	///different size on different architecture, so we specify here
	typedef unsigned __int32 size_type; 
	typedef unsigned __int16 dimension_size;
	
	typedef ptrdiff_t difference_type;

	Grid()
		:columnCount(0), rowCount(0), grid_data(nullptr)
	{
		
	}

	Grid(dimension_size _columnCount, dimension_size _rowCount)
		: columnCount(_columnCount), rowCount(_rowCount), grid_data(nullptr)
	{
		ResizeGrid(columnCount, rowCount);
	}

	Grid(dimension_size _columnCount, dimension_size _rowCount, value_type initVal)
		: columnCount(_columnCount), rowCount(_rowCount), grid_data(nullptr)
	{
		if (rowCount == 0 || columnCount == 0)
			throw std::invalid_argument("Dimension cannot be 0");

		ResizeGrid(columnCount, rowCount, initVal);
	}

	Grid(const Grid& source)
	{
		FreeGridData();

		if (source.rowCount > 0 && source.columnCount > 0)
		{
			rowCount = source.rowCount;
			columnCount = source.columnCount;

			grid_data = new value_type*[columnCount];

			for (dimension_size columnIndex = 0; columnIndex < columnCount; columnIndex++)
			{
				grid_data = new value_type[rowCount];

				std::copy(source.grid_data[columnIndex][0], 
					source.grid_data[columnIndex][rowCount], grid_data[columnIndex][0]);
			}
		}
	}

	~Grid()
	{
		FreeGridData();
	}

	const value_type* const operator [](dimension_size index)const
	{
		return grid_data[index];
	}

	value_type* const operator [](dimension_size index)
	{
		return grid_data[index];
	}

	value_type GetCell(dimension_size columnIndex, dimension_size rowIndex) const
	{
		return grid_data[columnIndex][rowIndex];
	}

	dimension_size GetColumnCount()const
	{
		return columnCount;
	}

	dimension_size GetRowCount()const
	{
		return rowCount;
	}

	bool IsEmpty()const
	{
		return rowCount > 0 && columnCount > 0;
	}

	size_type Size()const
	{
		return columnCount * rowCount;
	}

	size_type MaxSize()const
	{
		return std::numeric_limits<size_type>::max();
	}

	void Swap(Grid<value_type>& inGrid)
	{
		std::swap(*this, inGrid);
	}

	///Resize's the grid while maintaining any data that can fit in the new bounds
	///Uses the empty filler if the new size is larger than old size
	void ResizeGridPreserveData(dimension_size newColumnCount, dimension_size newRowCount,
		value_type emptyFiller = value_type())
	{
		if (newRowCount == 0 || newColumnCount == 0)
			throw std::invalid_argument("Dimension cannot be 0");

		//Used to make sure we only copy as far as the lowest row we have
		dimension_size lowestRowCount = newRowCount <= rowCount ? newRowCount : rowCount;

		value_type** temporary = new value_type*[newColumnCount];

		for (dimension_size columnIndex = 0; columnIndex < newColumnCount; columnIndex++)
		{
			temporary[columnIndex] = new value_type[newRowCount];

			//copy if we have the data
			if (grid_data)
			{
				//if we can copy from the current column, copy up until the last valid row
				if(columnIndex < columnCount)
					std::copy(grid_data[columnIndex][0], 
						grid_data[columnIndex][lowestRowCount], temporary[columnIndex][0]);

				//continue from last valid row and fill with default values
				for (dimension_size rowIndex = lowestRowCount; rowIndex < newRowCount; rowIndex++)
				{
					temporary[columnIndex][rowIndex] = emptyFiller;
				}
			}
		}

		FreeGridData();

		grid_data = temporary;

		rowCount = newRowCount;
		columnCount = newColumnCount;
	}

	///Resize's the grid. If you need data passed to the new grid, use ResizeGridPreserveData instead
	void ResizeGrid(dimension_size newColumnCount, dimension_size newRowCount,
		value_type initVal = value_type())
	{
		FreeGridData();

		columnCount = newColumnCount;
		rowCount = newRowCount;

		if (columnCount > 0 && rowCount > 0)
		{
			grid_data = new value_type*[columnCount];

			for (dimension_size columnIndex = 0; columnIndex < columnCount; columnIndex++)
			{
				grid_data[columnIndex] = new value_type[rowCount];

				for (dimension_size rowIndex = 0; rowIndex < rowCount; rowIndex++)
				{
					grid_data[columnIndex][rowIndex] = initVal;
				}
			}
		}
	}

protected:
	dimension_size columnCount;
	dimension_size rowCount;

	value_type** grid_data;

	void FreeGridData()
	{
		if (grid_data)
		{
			for (dimension_size index = 0; index < columnCount; index++)
			{
				//If there is a row associated with this column, delete it
				if(grid_data[index])
					delete[] grid_data[index];
			}

			delete[] grid_data;
			grid_data = nullptr;

			rowCount = columnCount = 0;
		}
	}
};

class GridRowIterator
{

};

