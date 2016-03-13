#pragma once
#include <limits>
#include <iterator>

template<typename Data_Type>
class Grid
{

	template <typename Data_Type>
	class GridIterator : public std::iterator<std::random_access_iterator_tag, Data_Type>
	{
	public:

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

		GridIterator<Data_Type>& operator[](unsigned short index)
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

		void SetPosition(unsigned short rowIndex, unsigned short columnIndex)
		{

		}
	};

	template <typename Grid_Data_Type>
	class Indexer
	{
		typedef __int16 dimension_size;
		dimension_size columnIndex;
		Grid<Grid_Data_Type>& data;

	public:
		Indexer(dimension_size ColumnIndex, Grid<Grid_Data_Type>& Data)
			: columnIndex(ColumnIndex), data(Data)
		{

		}

		Grid_Data_Type& operator[](dimension_size RowIndex)
		{
			return data.grid_data[data.GetOneDimensionIndex(columnIndex, RowIndex)];
		}
	};

	template <typename Grid_Data_Type>
	class ConstIndexer
	{
		typedef __int16 dimension_size;
		dimension_size columnIndex;
		const Grid<Grid_Data_Type>& data;

	public:
		ConstIndexer(dimension_size ColumnIndex, const Grid<Grid_Data_Type>& Data)
			: columnIndex(ColumnIndex), data(Data)
		{

		}

		const Grid_Data_Type& operator[](dimension_size RowIndex) const
		{
			return data.grid_data[data.GetOneDimensionIndex(columnIndex, RowIndex)];
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
		PerformMove(source);
	}

	~Grid()
	{
		FreeGridData();
	}

	const ConstIndexer<value_type> operator [](dimension_size index)const
	{
		return ConstIndexer<value_type>(index, *this);
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
		FreeGridData();

		PerformMove(source);

		return *this;
	}

	GridIterator<value_type> Begin() const
	{
		return GridIterator<value_type>(this, &grid_data[0]);
	}

	value_type& GetCell(dimension_size columnIndex, dimension_size rowIndex)
	{
		if (columnIndex > this->columnCount || rowIndex > this->rowCount)
			throw std::out_of_range("Grid-GetCell Arguments Out of Range");

		return this->grid_data[this->GetOneDimensionIndex(columnIndex, rowIndex)];
	}

	const value_type& GetCell(dimension_size columnIndex, dimension_size rowIndex) const
	{
		return this->grid_data[this->GetOneDimensionIndex(columnIndex, rowIndex)];
	}

	dimension_size GetColumnCount()const
	{
		return this->columnCount;
	}

	inline size_t GetOneDimensionIndex(dimension_size ColumnIndex, dimension_size RowIndex) const
	{
		return ColumnIndex + RowIndex * this->columnCount;
	}

	dimension_size GetRowCount()const
	{
		return this->rowCount;
	}

	bool IsEmpty()const
	{
		return !(this->rowCount > 0 && this->columnCount > 0);
	}

	inline size_type MaxSize()const
	{
		return std::numeric_limits<size_type>::max();
	}
	
	///Resize's the grid. If you need data passed to the new grid, use ResizeGridPreserveData instead
	void ResizeGrid(dimension_size newColumnCount, dimension_size newRowCount,
		value_type initVal = value_type())
	{
		if (newRowCount == 0 || newColumnCount == 0)
			throw std::invalid_argument("Dimension cannot be 0");

		FreeGridData();

		this->columnCount = newColumnCount;
		this->rowCount = newRowCount;

		this->grid_data = new value_type[Size()];

		for (dimension_size columnIndex = 0; columnIndex < this->columnCount; columnIndex++)
		{
			for (dimension_size rowIndex = 0; rowIndex < this->rowCount; rowIndex++)
			{
				this->grid_data[this->GetOneDimensionIndex(columnIndex, rowIndex)] = initVal;
			}
		}
	}


	///Resize's the grid while maintaining any data that can fit in the new bounds
	///Uses the empty filler if the new size is larger than old size
	void ResizeGridPreserveData(dimension_size newColumnCount, dimension_size newRowCount,
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

	
	inline size_type Size()const
	{
		return this->columnCount * this->rowCount;
	}

	void Swap(Grid<value_type>& inGrid)
	{
		std::swap(*this, inGrid);
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

