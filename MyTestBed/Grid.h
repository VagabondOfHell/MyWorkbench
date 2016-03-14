#pragma once
#include <limits>
#include <iterator>

template<typename Data_Type>
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
	typedef unsigned __int16 dimension_type;

	typedef ptrdiff_t difference_type;

	template <typename Data_Type>
	class GridIterator : public std::iterator<std::random_access_iterator_tag, Data_Type>
	{
	public:
		GridIterator(Data_Type* val_ref)
			:valRef(val_ref)
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

		reference operator[](unsigned short index)
		{
			return this->valRef[index];
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

		GridIterator<Data_Type> operator + (const GridIterator& rhs) const
		{
			return GridIterator(this->valRef + rhs.valRef);
		}

		difference_type operator - (const GridIterator& rhs) const
		{
			return (this->valRef - rhs.valRef);
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

		bool operator<(const GridIterator& rhs)
		{
			return this->valRef < rhs.valRef;
		}

		bool operator<=(const GridIterator& rhs)
		{
			return this->valRef <= rhs.valRef;
		}

		bool operator>(const GridIterator& rhs)
		{
			return this->valRef > rhs.valRef;
		}

		bool operator>=(const GridIterator& rhs)
		{
			return this->valRef >= rhs.valRef;
		}

	protected:
		Data_Type* valRef;

		inline void Increment()
		{
			++this->valRef;
		}

		inline void Decrement()
		{
			--this->valRef;
		}
	};

	template <typename Grid_Data_Type>
	class _Indexer
	{
		dimension_type columnIndex;
		Grid<Grid_Data_Type>& data;

	public:
		_Indexer(dimension_type ColumnIndex, Grid<Grid_Data_Type>& Data)
			: columnIndex(ColumnIndex), data(Data)
		{

		}

		reference operator[](dimension_type RowIndex)
		{
			return data.grid_data[data.GetOneDimensionIndex(columnIndex, RowIndex)];
		}

		const reference operator[](dimension_type RowIndex) const
		{
			return data.grid_data[data.GetOneDimensionIndex(columnIndex, RowIndex)];
		}
	};

public:
	typedef GridIterator<value_type> iterator;
	typedef GridIterator<const value_type> const_iterator;

	typedef _Indexer<value_type> indexer;
	typedef _Indexer<const value_type> const_indexer;

	Grid()
		:columnCount(0), rowCount(0), grid_data(nullptr)
	{
		
	}

	Grid(dimension_type _columnCount, dimension_type _rowCount)
		: columnCount(_columnCount), rowCount(_rowCount), grid_data(nullptr)
	{
		ResizeGrid(this->columnCount, this->rowCount);
	}

	Grid(dimension_type _columnCount, dimension_type _rowCount, value_type initVal)
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
		PerformMove(source);
	}

	~Grid()
	{
		FreeGridData();
	}

	const_indexer operator [](dimension_type index)const
	{
		return const_indexer(index, *this);
	}

	indexer operator [](dimension_type index)
	{
		return indexer(index, *this);
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
		FreeGridData();

		PerformMove(source);

		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Grid<value_type>& grid)
	{
		// write obj to stream
		for (dimension_type row = 0; row < grid.GetRowCount(); row++)
		{
			for (dimension_type column = 0; column < grid.GetColumnCount(); column++)
			{
				std::cout << "[" << column << "," << row << "(" << grid.GetOneDimensionIndex(column, row) << ") = " <<
					grid.GetCell(column, row) << "]\t";
			}

			std::cout << std::endl;
		}

		std::cout << std::endl;

		return os;
	}

	inline iterator begin()
	{
		return iterator(grid_data);
	}

	inline const_iterator begin() const
	{
		return const_iterator(grid_data);
	}

	inline const_iterator cbegin() const
	{
		return const_iterator(grid_data);
	}

	inline const_iterator cend() const
	{
		return const_iterator(&grid_data[size()]);
	}

	inline iterator end()
	{
		return iterator(&grid_data[size()]);
	}

	inline const_iterator end() const
	{
		return const_iterator(&grid_data[size()]);
	}

	inline reference GetCell(dimension_type columnIndex, dimension_type rowIndex)
	{
		if (columnIndex > this->columnCount || rowIndex > this->rowCount)
			throw std::out_of_range("Grid-GetCell Arguments Out of Range");

		return this->grid_data[this->GetOneDimensionIndex(columnIndex, rowIndex)];
	}

	inline const reference GetCell(dimension_type columnIndex, dimension_type rowIndex) const
	{
		return this->grid_data[this->GetOneDimensionIndex(columnIndex, rowIndex)];
	}

	inline reference GetCell(size_t index)
	{
		return this->grid_data[index];
	}

	inline const reference GetCell(size_t index) const
	{
		return this->grid_data[index];
	}

	inline dimension_type GetColumnCount()const
	{
		return this->columnCount;
	}

	inline size_t GetOneDimensionIndex(dimension_type ColumnIndex, dimension_type RowIndex) const
	{
		return ColumnIndex + RowIndex * this->columnCount;
	}

	inline dimension_type GetRowCount()const
	{
		return this->rowCount;
	}

	inline bool isEmpty()const
	{
		return !(this->rowCount > 0 && this->columnCount > 0);
	}

	inline size_type maxSize()const
	{
		return std::numeric_limits<size_type>::max();
	}
	
	///Resize's the grid. If you need data passed to the new grid, use ResizeGridPreserveData instead
	void ResizeGrid(dimension_type newColumnCount, dimension_type newRowCount,
		value_type initVal = value_type())
	{
		if (newRowCount == 0 || newColumnCount == 0)
			throw std::invalid_argument("Dimension cannot be 0");

		FreeGridData();

		this->columnCount = newColumnCount;
		this->rowCount = newRowCount;

		this->grid_data = new value_type[size()];

		for (dimension_type columnIndex = 0; columnIndex < this->columnCount; columnIndex++)
		{
			for (dimension_type rowIndex = 0; rowIndex < this->rowCount; rowIndex++)
			{
				this->grid_data[this->GetOneDimensionIndex(columnIndex, rowIndex)] = initVal;
			}
		}
	}


	///Resize's the grid while maintaining any data that can fit in the new bounds
	///Uses the empty filler if the new size is larger than old size
	void ResizeGridPreserveData(dimension_type newColumnCount, dimension_type newRowCount,
		value_type emptyFiller = value_type())
	{
		if (newRowCount == 0 || newColumnCount == 0)
			throw std::invalid_argument("Dimension cannot be 0");

		//Used to make sure we only copy as many elements as is safe
		size_type lowestRowCount = newRowCount <= this->rowCount ? newRowCount : this->rowCount;
		size_type lowestColumnCount = newColumnCount <= this->columnCount ? newColumnCount : this->columnCount;

		value_type* temporary = new value_type[newColumnCount * newRowCount];

		for (size_t index = 0; index < newRowCount; index++)
		{
			size_t initializerStart = 0;

			if (index < lowestRowCount)
			{
				std::copy(&grid_data[index * lowestColumnCount], 
					&grid_data[index * lowestColumnCount + lowestColumnCount],
					&temporary[index * newColumnCount]);

				initializerStart = lowestColumnCount;
			}
			
			for (size_t rowIndex = initializerStart; rowIndex < newColumnCount; rowIndex++)
			{
				temporary[rowIndex + index * newColumnCount] = emptyFiller;
			}
			
		}

		FreeGridData();

		this->grid_data = temporary;

		this->rowCount = newRowCount;
		this->columnCount = newColumnCount;
	}

	inline size_type size()const
	{
		return this->columnCount * this->rowCount;
	}

	inline void swap(Grid<value_type>& inGrid)
	{
		std::swap(*this, inGrid);
	}

protected:
	dimension_type columnCount;
	dimension_type rowCount;

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

	void PerformCopy(const Grid<Data_Type>& source)
	{
		this->rowCount = source.rowCount;
		this->columnCount = source.columnCount;

		if (this->rowCount > 0 && this->columnCount > 0)
		{
			this->grid_data = new value_type[this->columnCount * this->rowCount];

			std::copy(&source.grid_data[0],
				&source.grid_data[this->columnCount * this->rowCount], &this->grid_data[0]);
		}
	}

	void PerformMove(Grid& source)
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
	
};

