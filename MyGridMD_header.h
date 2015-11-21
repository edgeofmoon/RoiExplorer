#pragma once

#include "MyArrayMD.h"
#include "MyBox.h"
#include "MySharedPointer.h"

template<typename valueType, typename sizeType, int numDim>
class MyGridMD :
	public MyArrayMD<valueType, numDim>,
	public MyBox<sizeType, numDim>
{
public:
	enum GridType{
		GridType_Grid = 0,
		GridType_Lattice = 1,
	};

	MyGridMD(const MyVec<int, numDim>& dimSizes, 
		const valueType& value, GridType type = GridType_Grid);
	MyGridMD(const MyVec<int, numDim>& dimSizes,
		GridType type = GridType_Grid);
	MyGridMD(const MyArrayMD<valueType, numDim>& otherArray,
		GridType type = GridType_Grid);

	MyVec<int, numDim> GetDimSize() const;
	int GetDimSize(int iDim) const;
	void SetType(GridType type);
	void Set(const MyBox<sizeType, numDim>& box);
	void Set(const MyVec<sizeType, numDim>& low, 
		const MyVec<sizeType, numDim>& high);

	// Grid: return nearest cell index
	// Lattice: return nearest lattice index
	MyVec<int, numDim> ComputeIndex(const MyVec<sizeType, numDim> pos) const;
	// Grid: return cell center
	// Lattice: return lattice position
	MyVec<sizeType, numDim> ComputePosition(const MyVec<int, numDim> index) const;
	// Grid&Lattice: return cell
	// Lattice: has one less row&column
	MyBox<sizeType, numDim> ComputeCell(const MyVec<int, numDim> index) const;
	// return linear interpolated value
	valueType InterpolateLinear(const MyVec<sizeType, numDim>& pos) const;
	// map position to [0,1] space
	MyVec<sizeType, numDim> NomalizePosition(
		const MyVec<sizeType, numDim>& pos) const;

protected:
	MyVec<sizeType, numDim> mCellSize;
	GridType mType;
};

typedef MyGridMD<float, float, 2> My2dfGridf;
typedef MySharedPointer<My2dfGridf> My2dfGridfSPtr;
typedef MySharedPointer<const My2dfGridf> My2dfGridfScPtr;

typedef MyGridMD<float, int, 2> My2diGridf;
typedef MySharedPointer<My2diGridf> My2diGridfSPtr;
typedef MySharedPointer<const My2diGridf> My2diGridfScPtr;