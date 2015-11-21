#pragma once

#include "MyGridMD_header.h"

template<typename valueType, typename sizeType, int numDim>
MyGridMD<valueType, sizeType, numDim>::MyGridMD(
	const MyVec<int, numDim>& dimSizes, const valueType& value, GridType type)
	:MyArrayMD<valueType, numDim>(dimSizes, value), mType(type){
}

template<typename valueType, typename sizeType, int numDim>
MyGridMD<valueType, sizeType, numDim>::MyGridMD(
	const MyVec<int, numDim>& dimSizes, GridType type)
	: MyArrayMD<valueType, numDim>(dimSizes), mType(type){
}

template<typename valueType, typename sizeType, int numDim>
MyGridMD<valueType, sizeType, numDim>::MyGridMD(
	const MyArrayMD<valueType, numDim>& otherArray, GridType type)
	: MyArrayMD<valueType, numDim>(otherArray), mType(type){
}

template<typename valueType, typename sizeType, int numDim>
MyVec<int, numDim> MyGridMD<valueType, sizeType, numDim>::GetDimSize() const{
	MyVec<int, numDim> size = MyArrayMD<valueType, numDim>::GetDimSizes();
	if (mType == GridType_Grid){
		return size;
	}
	else{
		for (int i = 0; i < numDim; i++){
			size[i]--;
		}
		return size;
	}
}

template<typename valueType, typename sizeType, int numDim>
int MyGridMD<valueType, sizeType, numDim>::GetDimSize(int iDim) const{
	if (mType == GridType_Grid){
		return MyArrayMD<valueType, numDim>::GetDimSize(iDim);;
	}
	else{
		return MyArrayMD<valueType, numDim>::GetDimSize(iDim)-1;
	}
}

template<typename valueType, typename sizeType, int numDim>
void MyGridMD<valueType, sizeType, numDim>::SetType(GridType type){
	if (mType != type){
		mType = type;
		if (mType == GridType_Grid){
			for (int i = 0; i < numDim; i++){
				mCellSize[i] = this->GetSize(i) / mDimSizes[i];
			}
		}
		else{
			for (int i = 0; i < numDim; i++){
				mCellSize[i] = this->GetSize(i) / (mDimSizes[i] - 1);
			}
		}
	}
}

template<typename valueType, typename sizeType, int numDim>
void MyGridMD<valueType, sizeType, numDim>::Set(
	const MyBox<sizeType, numDim>& box){
	MyBox<sizeType, numDim>::Set(box);
	if (mType == GridType_Grid){
		for (int i = 0; i < numDim; i++){
			mCellSize[i] = this->GetSize(i) / mDimSizes[i];
		}
	}
	else{
		for (int i = 0; i < numDim; i++){
			mCellSize[i] = this->GetSize(i) / (mDimSizes[i] - 1);
		}
	}
}

template<typename valueType, typename sizeType, int numDim>
void MyGridMD<valueType, sizeType, numDim>::Set(
	const MyVec<sizeType, numDim>& low, const MyVec<sizeType, numDim>& high){
	MyBox<sizeType, numDim>::Set(low, high);
	if (mType == GridType_Grid){
		for (int i = 0; i < numDim; i++){
			mCellSize[i] = this->GetSize(i) / mDimSizes[i];
		}
	}
	else{
		for (int i = 0; i < numDim; i++){
			mCellSize[i] = this->GetSize(i) / (mDimSizes[i] - 1);
		}
	}
}

template<typename valueType, typename sizeType, int numDim>
MyVec<int, numDim> MyGridMD<valueType, sizeType, numDim>::ComputeIndex(
	const MyVec<sizeType, numDim> pos) const{
	MyVec<int, numDim> index;
	if (mType == GridType_Grid){
		for (int i = 0; i < numDim; i++){
			index[i] = (pos[i] - mLow[i]) / mCellSize[i];
		}
	}
	else{
		for (int i = 0; i < numDim; i++){
			index[i] = (pos[i] - mLow[i]) / mCellSize[i] + 0.5;
		}
	}
	return index;
}

template<typename valueType, typename sizeType, int numDim>
MyVec<sizeType, numDim> MyGridMD<valueType, sizeType, numDim>::ComputePosition(
	const MyVec<int, numDim> index) const{
	MyVec<sizeType, numDim> pos;
	if (mType == GridType_Grid){
		for (int i = 0; i < numDim; i++){
			pos[i] = (index[i] + 0.5) * mCellSize[i] + mLow[i];
		}
	}
	else{
		for (int i = 0; i < numDim; i++){
			pos[i] = (index[i] + 0) * mCellSize[i] + mLow[i];
		}
	}
	return pos;
}

template<typename valueType, typename sizeType, int numDim>
MyBox<sizeType, numDim> MyGridMD<valueType, sizeType, numDim>::ComputeCell(
	const MyVec<int, numDim> index) const{
	MyVec<sizeType, numDim> lowPos, highPos;
	for (int i = 0; i < numDim; i++){
		lowPos[i] = index[i] * mCellSize[i] + mLow[i];
	}
	highPos = lowPos + mCellSize;
	return MyBox<sizeType, numDim>(lowPos, highPos);
}

template<typename valueType, typename sizeType, int numDim>
valueType MyGridMD<valueType, sizeType, numDim>::InterpolateLinear(
	const MyVec<sizeType, numDim>& pos) const{
	MyVec<sizeType, numDim> fIndex = this->NomalizePosition(pos);
	for (int i = 0; i < numDim; i++){
		fIndex[i] *= this->GetDimSizes(i) - 1;
	}
	return MyArrayMD<valueType, numDim>::InterpolateLinear(fIndex);
}

template<typename valueType, typename sizeType, int numDim>
MyVec<sizeType, numDim> MyGridMD<valueType, sizeType, numDim>::NomalizePosition(
	const MyVec<sizeType, numDim>& pos) const{
	MyVec<sizeType, numDim> nPos;
	for (int i = 0; i < numDim; i++){
		nPos[i] = (pos[i] - mLow[i]) / this->GetSize(i);
	}
	return nPos;
}