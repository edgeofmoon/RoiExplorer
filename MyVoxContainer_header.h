#pragma once

#include "MyVec.h"
#include "MyBox.h"
#include "MyArray.h"
#include "MyArrayMD.h"
#include "MyMap.h"
#include "MySharedPointer.h"

template<typename valueType>
class MyVoxContainer;
template<typename valueType>
using MyVoxContainerSPtr = MySharedPointer < MyVoxContainer<valueType> >;
template<typename valueType>
using MyVoxContainerScPtr = MySharedPointer < const MyVoxContainer<valueType> >;

template<typename valueType>
class MyVoxContainer
{
protected:
	static valueType NullVoxelValue;
public:
	MyVoxContainer(const MyVec3i& volSize);
	virtual ~MyVoxContainer();

	enum ContainerType{
		ContainerType_NONE = 0,
		ContainerType_Large = 1,
		ContainerType_Small = 2,
		ContainerType_Tiny = 3,
	};

	static void SetNullVoxelValue(const valueType& value){
		NullVoxelValue = value;
	}

	static valueType GetNullVoxelValue(){
		return NullVoxelValue;
	}

	static int CountOverlapping(const MyVoxContainer<valueType>* voxels0,
		const MyVoxContainer<valueType>* voxels1);

	static bool IsOverlapping(const MyVoxContainer<valueType>* voxels0,
		const MyVoxContainer<valueType>* voxels1);

	static MyVoxContainerSPtr<valueType> MakeVoxContainer(
		const MyArrayi* voxIndices, const valueType& value,
		const MyVec3i& volBox);

	static MyVoxContainerSPtr<valueType> MakeVoxContainer(
		const MyArrayi* voxIndices, const MyArray<valueType>* voxValues, 
		const MyVec3i& volBox);

	static MyVoxContainerSPtr<valueType> MakeVoxContainer(
		const MyVoxContainer<valueType>* container, ContainerType type = ContainerType_NONE);

	// add one to this one
	virtual void Add(const MyVoxContainer<valueType>* container);

	virtual MyArrayMDSPtr <valueType, 3> MakeVolume() const;
	virtual MyArrayiSPtr MakeIndexArray() const;

	virtual MyBox3i GetBoundingBox() const {
		return mBoundingBox;
	};

	virtual int ComputeVolume() const {
		return MyArrayMD <valueType, 3>::ComputeVolume(mVolumeSize); 
	};

	virtual MyVec3i GetVolumeSize() const {
		return mVolumeSize;
	};

	virtual void Clear() = 0;
	virtual int GetNumVoxes() const = 0;
	virtual bool IsVoxelIn(const MyVec3i& pos) const = 0;
	virtual bool IsVoxelIn(int index) const = 0;
	// must make sure voxIndices does not overlap
	// this implementation will not check overlapping
	// and might cause counting error if not checked
	virtual void AddVoxes(const MyArrayi* voxIndices, const valueType& value) = 0;
	virtual void AddVoxes(const MyArrayi* voxIndices, 
		const MyArray<valueType>* voxValues) = 0;
	virtual valueType& GetVoxel(int index) = 0;
	virtual const valueType& GetVoxel(int index) const = 0;

protected:
	MyBox3i mBoundingBox;
	MyVec3i mVolumeSize;

	/******************** iterator definition *******************************/
public:
	class Iterator;
	class const_Iterator;
	class Iterator
		: public std::iterator < std::bidirectional_iterator_tag, valueType > {

	public:
		friend const_Iterator;
		Iterator() : mIndex(-1), mContainer(nullptr) {};
		Iterator(int idx, MyVoxContainer<valueType>* container) 
			: mIndex(idx), mContainer(container) {};

		friend MyVoxContainer < valueType > ;
		// Operators : misc
	public:
		inline Iterator& operator=(const Iterator &rhs) { 
			mIndex = rhs.mIndex;
			mContainer = rhs.mContainer;
			return *this;
		}
		inline valueType& operator*() { 
			return mContainer->GetVoxel(this);
		}
		inline const valueType& operator*() const {
			return mContainer->GetVoxel(this);
		}
		//inline valueType* operator->() const { 
		//	return mContainer->GetVoxel(this);
		//}
		inline int GetVoxelIndex() const {
			return mContainer->TranslateIndex(mIndex);
		}

		// Operators : arithmetic
	public:
		// pre-increment
		inline virtual Iterator& operator++() {
			mContainer->Next(this);
			return *this;
		};
		inline virtual Iterator& operator--() {
			mContainer->Prev(this);
			return *this;
		};

		// post-increment
		inline virtual Iterator operator++(int) { 
			Iterator itr(*this);
			mContainer->Next(this);
			return itr;
		};
		inline virtual Iterator operator--(int) {
			Iterator itr(*this);
			mContainer->Prev(this);;
			return itr;
		};

		// Operators : comparison
	public:
		inline bool operator==(const Iterator& rhs) const {
			return mIndex == rhs.mIndex;
		}
		inline bool operator!=(const Iterator& rhs) const {
			return mIndex != rhs.mIndex;
		}
		inline bool operator>(const Iterator& rhs) const {
			return mIndex > rhs.mIndex;
		}
		inline bool operator<(const Iterator& rhs) const {
			return mIndex < rhs.mIndex;
		}
		inline bool operator>=(const Iterator& rhs) const {
			return mIndex >= rhs.mIndex;
		}
		inline bool operator<=(const Iterator& rhs) const {
			return mIndex <= rhs.mIndex;
		}
		// repeat for const_iterator
		inline bool operator==(const const_Iterator& rhs) const {
			return mIndex == rhs.mIndex;
		}
		inline bool operator!=(const const_Iterator& rhs) const {
			return mIndex != rhs.mIndex;
		}
		inline bool operator>(const const_Iterator& rhs) const {
			return mIndex > rhs.mIndex;
		}
		inline bool operator<(const const_Iterator& rhs) const {
			return mIndex < rhs.mIndex;
		}
		inline bool operator>=(const const_Iterator& rhs) const {
			return mIndex >= rhs.mIndex;
		}
		inline bool operator<=(const const_Iterator& rhs) const {
			return mIndex <= rhs.mIndex;
		}

		// Data members
	protected:
		int mIndex;
		MyVoxContainer<valueType>* mContainer;
	};

public:
	class const_Iterator
		: public std::iterator < std::bidirectional_iterator_tag, valueType > {
	public:
		friend Iterator;
		const_Iterator() : mIndex(-1), mContainer(nullptr) {};
		const_Iterator(int idx, const MyVoxContainer<valueType>* container)
			: mIndex(idx), mContainer(container) {};
		const_Iterator(const Iterator& itr)
			: mIndex(itr.mIndex), mContainer(itr.mContainer) {};

		friend MyVoxContainer < valueType >;
		// Operators : misc
	public:
		inline const_Iterator& operator=(const const_Iterator &rhs) {
			mIndex = rhs.mIndex;
			mContainer = rhs.mContainer;
			return *this;
		}
		inline const valueType& operator*() const { 
			return mContainer->GetVoxel(this);
		}
		//inline const valueType* operator->() const {
		//	return mContainer->GetVoxel(this);
		//}
		inline int GetVoxelIndex() const {
			return mContainer->TranslateIndex(mIndex);
		}

		// Operators : arithmetic
	public:
		// pre increment
		inline virtual const_Iterator& operator++() { 
			mContainer->Next(this);
			return *this; 
		};
		inline virtual const_Iterator& operator--() { 
			mContainer->Prev(this);
			return *this; 
		};

		// post-increment
		inline virtual const_Iterator operator++(int) {
			const_Iterator itr(*this);
			mContainer->Next(this);
			return itr;
		};
		inline virtual const_Iterator operator--(int) {
			const_Iterator itr(*this);
			mContainer->Prev(this);;
			return itr;
		};
		// Operators : comparison
	public:
		inline bool operator==(const const_Iterator& rhs) const {
			return mIndex == rhs.mIndex;
		}
		inline bool operator!=(const const_Iterator& rhs) const {
			return mIndex != rhs.mIndex;
		}
		inline bool operator>(const const_Iterator& rhs) const {
			return mIndex > rhs.mIndex;
		}
		inline bool operator<(const const_Iterator& rhs) const {
			return mIndex < rhs.mIndex;
		}
		inline bool operator>=(const const_Iterator& rhs) const {
			return mIndex >= rhs.mIndex;
		}
		inline bool operator<=(const const_Iterator& rhs) const {
			return mIndex <= rhs.mIndex;
		}
		// repeat for Iterator
		inline bool operator==(const Iterator& rhs) const {
			return mIndex == rhs.mIndex;
		}
		inline bool operator!=(const Iterator& rhs) const {
			return mIndex != rhs.mIndex;
		}
		inline bool operator>(const Iterator& rhs) const {
			return mIndex > rhs.mIndex;
		}
		inline bool operator<(const Iterator& rhs) const {
			return mIndex < rhs.mIndex;
		}
		inline bool operator>=(const Iterator& rhs) const {
			return mIndex >= rhs.mIndex;
		}
		inline bool operator<=(const Iterator& rhs) const {
			return mIndex <= rhs.mIndex;
		}
		// Data members
	protected:
		int mIndex;
		const MyVoxContainer<valueType>* mContainer;
	};

	virtual void Next(Iterator* iterator) = 0;
	virtual void Prev(Iterator* iterator) = 0;
	virtual Iterator Begin() = 0;
	virtual Iterator End() = 0;

	virtual void Next(const_Iterator* iterator) const = 0;
	virtual void Prev(const_Iterator* iterator) const = 0;
	virtual const_Iterator Begin() const = 0;
	virtual const_Iterator End() const = 0;

protected:

	// used for iterator
	static int& GetIndex(Iterator* iterator){ return iterator->mIndex; };
	static int& GetIndex(const_Iterator* iterator){ return iterator->mIndex; };
	static int GetIndex(const Iterator* iterator){ return iterator->mIndex; };
	static int GetIndex(const const_Iterator* iterator){ return iterator->mIndex; };
	
	// translate the iterator index to voxel index
	// sometimes they are the same, e.g for large and small
	virtual int TranslateIndex(int index) const{ 
		return index; 
	};
	virtual const valueType& GetVoxel(const Iterator* iterator) const { 
		return this->GetVoxel(iterator->GetVoxelIndex());
	}
	virtual valueType& GetVoxel(const Iterator* iterator){
		return this->GetVoxel(iterator->GetVoxelIndex());
	}
	virtual const valueType& GetVoxel(const const_Iterator* iterator) const {
		return this->GetVoxel(iterator->GetVoxelIndex());
	}
	virtual valueType& GetVoxel(const const_Iterator* iterator){
		return this->GetVoxel(iterator->GetVoxelIndex());
	}
};

template<typename valueType>
class MyVoxContainer_Large
	:public MyVoxContainer<typename valueType>{

public:
	MyVoxContainer_Large(const MyVec3i& volBox)
		:MyVoxContainer(volBox), 
		mVolumeMask(volBox, NullVoxelValue),
		mNumVoxes(0){
	};

	MyVoxContainer_Large(const MyVec3i& volBox, valueType value)
		:MyVoxContainer(volBox),
		mVolumeMask(volBox, value),
		mNumVoxes(value == NullVoxelValue ? 0 : My3dArrayf::ComputeVolume(volBox)){
	};
	virtual ~MyVoxContainer_Large();


	virtual MyArrayMDSPtr <valueType, 3> MakeVolume() const;

	virtual void Clear();
	virtual int GetNumVoxes() const;
	virtual bool IsVoxelIn(const MyVec3i& pos) const;
	virtual bool IsVoxelIn(int index) const;
	virtual void AddVoxes(const MyArrayi* voxIndices, const valueType& value);
	virtual void AddVoxes(const MyArrayi* voxIndices, 
		const MyArray<valueType>* voxValues);
	virtual valueType& GetVoxel(int index);
	virtual const valueType& GetVoxel(int index) const;

	virtual void Next(Iterator* iterator);
	virtual void Prev(Iterator* iterator);
	virtual Iterator Begin();
	virtual Iterator End();

	virtual void Next(const_Iterator* iterator) const;
	virtual void Prev(const_Iterator* iterator) const;
	virtual const_Iterator Begin() const;
	virtual const_Iterator End() const;

protected:
	MyArrayMD <valueType,3> mVolumeMask;
	int mNumVoxes;
};


template<typename valueType>
class MyVoxContainer_Small
	:public MyVoxContainer<typename valueType>{
public:
	MyVoxContainer_Small(const MyVec3i& volBox)
		:MyVoxContainer(volBox){};
	virtual ~MyVoxContainer_Small();

	virtual void Clear();
	virtual int GetNumVoxes() const;
	virtual bool IsVoxelIn(const MyVec3i& pos) const;
	virtual bool IsVoxelIn(int index) const;
	virtual void AddVoxes(const MyArrayi* voxIndices, const valueType& value);
	virtual void AddVoxes(const MyArrayi* voxIndices,
		const MyArray<valueType>* voxValues);
	virtual valueType& GetVoxel(int index);
	virtual const valueType& GetVoxel(int index) const;

	virtual void Next(Iterator* iterator);
	virtual void Prev(Iterator* iterator);
	virtual Iterator Begin();
	virtual Iterator End();

	virtual void Next(const_Iterator* iterator) const;
	virtual void Prev(const_Iterator* iterator) const;
	virtual const_Iterator Begin() const;
	virtual const_Iterator End() const;

protected:
	MyMap<int, valueType> mVoxes;
};

template<typename valueType>
class MyVoxContainer_Tiny
	:public MyVoxContainer<typename valueType>{
public:
	MyVoxContainer_Tiny(const MyVec3i& volBox)
		:MyVoxContainer(volBox){};
	virtual ~MyVoxContainer_Tiny();

	virtual void Clear();
	virtual int GetNumVoxes() const;
	virtual bool IsVoxelIn(const MyVec3i& pos) const;
	virtual bool IsVoxelIn(int index) const;
	virtual void AddVoxes(const MyArrayi* voxIndices, const valueType& value);
	virtual void AddVoxes(const MyArrayi* voxIndices,
		const MyArray<valueType>* voxValues);
	virtual valueType& GetVoxel(int index);
	virtual const valueType& GetVoxel(int index) const;

	virtual void Next(Iterator* iterator);
	virtual void Prev(Iterator* iterator);
	virtual Iterator Begin();
	virtual Iterator End();

	virtual void Next(const_Iterator* iterator) const;
	virtual void Prev(const_Iterator* iterator) const;
	virtual const_Iterator Begin() const;
	virtual const_Iterator End() const;

protected:
	MyArrayi mIndices;
	MyArray<valueType> mValues;

	virtual int TranslateIndex(int index) const;
	virtual const valueType& GetVoxel(const Iterator* iterator) const;
	virtual valueType& GetVoxel(const Iterator* iterator);
	virtual const valueType& GetVoxel(const const_Iterator* iterator) const;
	virtual valueType& GetVoxel(const const_Iterator* iterator);
};


typedef MyVoxContainer<int> MyVoxContaineri;
typedef MyVoxContainer<float> MyVoxContainerf;
typedef MyVoxContainer<double> MyVoxContainerd;
typedef MyVoxContainer<bool> MyVoxContainerb;
typedef MySharedPointer<MyVoxContaineri> MyVoxContaineriSPtr;
typedef MySharedPointer<MyVoxContainerf> MyVoxContainerfSPtr;
typedef MySharedPointer<MyVoxContainerd> MyVoxContainerdSPtr;
typedef MySharedPointer<MyVoxContainerb> MyVoxContainerbSPtr;
typedef MySharedPointer<const MyVoxContaineri> MyVoxContaineriScPtr;
typedef MySharedPointer<const MyVoxContainerf> MyVoxContainerfScPtr;
typedef MySharedPointer<const MyVoxContainerd> MyVoxContainerdScPtr;
typedef MySharedPointer<const MyVoxContainerb> MyVoxContainerbScPtr;
