#pragma once
#include <limits>
#include <stdexcept>

template<typename Data_Type>
class Grid
{
	template <typename Grid_Data_type>
	class Indexer
	{
		typedef __int16 dimension_size;
		dimension_size offset;
		Grid<Grid_Data_type>& data;

	public:
		Indexer(dimension_size _offset, Grid<Grid_Data_type>& _data)
			: offset(_offset), data(_data)
		{

		}

		Grid_Data_type& operator[](dimension_size index)
		{
			return data.grid_data[offset + (data.columnCount * index)];
		}
	};

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
		ResizeGrid(this->columnCount, this->rowCount);
	}

	Grid(dimension_size _columnCount, dimension_size _rowCount, value_type initVal)
		: columnCount(_columnCount), rowCount(_rowCount), grid_data(nullptr)
	{
		if (this->rowCount == 0 || this->columnCount == 0)
			throw std::invalid_argument("Dimension cannot be 0");

		ResizeGrid(this->columnCount, this->rowCount, initVal);
	}

	///Copy Constructor
	Grid(const Grid& source)
	{
		PerformCopy(source);
	}

	///Move Constructor
	Grid(Grid&& source)
	{
		//Copy the source over
		this->rowCount = source.rowCount;
		this->columnCount = source.columnCount;
		this->grid_data = source.grid_data;

		//Reset the source, as per move semantics
		source.rowCount = 0;
		source.columnCount = 0;
		source.grid_data = nullptr;
	}

	~Grid()
	{
		FreeGridData();
	}

	const Indexer<value_type> operator [](dimension_size index)const
	{
		return Indexer<value_type>(index, *this);
	}

	Indexer<value_type> operator [](dimension_size index)
	{
		return Indexer<value_type>(index, *this);
	}

	//Assignment operator
	Grid& operator=(const Grid& source)
	{
		if (this == &source)
		{
			return *this;
		}

		//Free data if it exists
		FreeGridData();
		
		//Deep copy
		PerformCopy(source);
	}

	///Move Assignment Operator
	Grid& operator=(Grid&& source)
	{
		Swap(source);

		return *this;
	}

	value_type GetCell(dimension_size columnIndex, dimension_size rowIndex) const
	{
		return this->grid_data[columnIndex + (rowIndex * this->columnCount)];
	}

	dimension_size GetColumnCount()const
	{
		return this->columnCount;
	}

	dimension_size GetRowCount()const
	{
		return this->rowCount;
	}

	bool IsEmpty()const
	{
		return !(this->rowCount > 0 && this->columnCount > 0);
	}

	inline size_type Size()const
	{
		return this->columnCount * this->rowCount;
	}

	inline size_type MaxSize()const
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

		//Used to make sure we only copy as many elements as is safe
		size_type lowestRowCount = newRowCount <= this->rowCount ? newGridSize : thisGridSize;

		value_type* temporary = new value_type[newGridSize];

		for (dimension_size columnIndex = 0; columnIndex < columnCount; columnIndex++)
		{
			if (this->grid_data && (columnIndex < this->columnCount))
			{
				std::copy(this->grid_data[columnIndex * this->columnCount],
					this->grid_data[columnIndex + (this->columnCount * lowestRowCount)],
					temporary[columnIndex * newColumnCount]);
			}

			for (dimension_size rowIndex = lowestRowCount; rowIndex < newRowCount; rowIndex++)
			{
				temporary[columnIndex + (newColumnCount * rowIndex)] = emptyFiller;
			}
		}

		FreeGridData();

		this->grid_data = temporary;

		this->rowCount = newRowCount;
		this->columnCount = newColumnCount;
	}

	///Resize's the grid. If you need data passed to the new grid, use ResizeGridPreserveData instead
	void ResizeGrid(dimension_size newColumnCount, dimension_size newRowCount,
		value_type initVal = value_type())
	{
		FreeGridData();

		this->columnCount = newColumnCount;
		this->rowCount = newRowCount;

		if (columnCount > 0 && rowCount > 0)
		{
			this->grid_data = new value_type[Size()];

			for (dimension_size columnIndex = 0; columnIndex < this->columnCount; columnIndex++)
			{
				for (dimension_size rowIndex = 0; rowIndex < this->rowCount; rowIndex++)
				{
					this->grid_data[columnIndex + (rowIndex * this->columnCount)] = initVal;
				}
			}
		}
	}

protected:
	dimension_size columnCount;
	dimension_size rowCount;

	value_type* grid_data;

	void FreeGridData()
	{
		if (this->grid_data)
		{
			delete[] this->grid_data;
			this->grid_data = nullptr;

			this->rowCount = this->columnCount = 0;
		}
	}

	void PerformCopy(const Grid<Data_Type>& source)const
	{
		this->rowCount = source.rowCount;
		this->columnCount = source.columnCount;

		if (this->rowCount > 0 && this->columnCount > 0)
		{
			this->grid_data = new value_type[this->columnCount * this->rowCount];

			std::copy(source.grid_data[0],
				source.grid_data[this->columnCount * this->rowCount], this->grid_data[0]);
		}
	}
};

template <typename Data_Type>
class GridIterator : public std::iterator<std::random_access_iterator_tag, Data_Type>
{
	GridIterator()
		:gridRef(nullptr), valRef(nullptr)
	{
		
	}

	GridIterator(Grid<Data_Type>* gridToRef, Data_Type* val_ref)
		:gridRef(gridToRef), valRef(val_ref)
	{

	}

	~GridIterator()
	{
		//Nothing to delete. We are only referencing memory owned by others
	}

	Data_Type& operator*()const
	{
		return *this->valRef;
	}

	Data_Type* operator->()const
	{
		return this->valRef;
	}

	GridIterator<Data_Type>& operator[](Grid::dimension_size index)
	{
		
	}

	GridIterator<Data_Type>& operator++()
	{
		Increment();
		return *this;
	}

	const GridIterator<Data_Type> operator++(int)
	{
		Increment();
		return *this;
	}

	GridIterator<Data_Type>& operator--()
	{
		Decrement();
		return *this;
	}

	const GridIterator<Data_Type> operator--(int)
	{
		Decrement();
		return *this;
	}

	bool operator==(const GridIterator& rhs)
	{
		return this->valRef == rhs.valRef;
	}

	bool operator!=(const GridIterator& rhs)
	{
		return !(*this == rhs);
	}

protected:
	
	Grid<Data_Type>* gridRef;
	Data_Type* valRef;

	void Increment()
	{
		++this->valRef;
	}

	void Decrement()
	{
		--this->valRef;
	}

	void SetPosition(Grid::dimension_size rowIndex, Grid::dimension_size columnIndex)
	{

	}
};

