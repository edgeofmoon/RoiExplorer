#pragma once

#include "MyArray.h"
#include "MyVec.h"

#include "MySharedPointer.h"

template <typename T, int numDim>
class MyArrayMD
{
public:
	MyArrayMD(const MyVec<int, numDim>& dimSizes, const T& value);
	MyArrayMD(const MyVec<int, numDim>& dimSizes);
	MyArrayMD(const MyArrayMD<T, numDim>& otherArray);

	const MyArray<T>& GetDataArray() const { return mData; };
	const int GetVolume() const;
	const int ComputeIndex(const MyVec<int, numDim>& pos) const;
	const MyVec<int, numDim> ComputePosition(int index) const;
	const int GetDimSize(const int dim) const;
	const MyVec<int, numDim>& GetDimSizes() const;
	const T& At(int index) const;
	const T& At(const MyVec<int, numDim>& pos) const;
	const T& Nearest(const MyVec<float, numDim>& pos) const;
	const T& NearestUniform(const MyVec<float, numDim>& pos) const;
	const T& operator[](int index) const;
	const T& operator[](const MyVec<int, numDim>& pos) const;
	T& operator[](int index);
	T& operator[](const MyVec<int, numDim>& pos);

	T InterpolateLinear(const MyVec<float, numDim>& pos) const;

	static int ComputeVolume(const MyVec<int, numDim>& dimSizes);
	static int ComputeIndex(const MyVec<int, numDim>& pos, const MyVec<int, numDim>& dimSizes);
	static MyVec<int, numDim> ComputePosition(int index, const MyVec<int, numDim>& dimSizes);

	void operator+=(T value);
	void operator-=(T value);
	void operator*=(T value);
	void operator/=(T value);
	void operator=(T value);

protected:
	MyArray<T> mData;
	MyVec<int, numDim> mDimSizes;
	int mVolume;

	T InterpolateLinear(const MyVec<float, numDim>& pos, 
		const MyVec<int, numDim>& iPos, int iDim = 0) const;
};

template <typename T, int numDim>
MyArrayMD<T, numDim>::MyArrayMD(const MyVec<int, numDim>& dimSizes, const T& value)
	:mData(ComputeVolume(dimSizes), value), 
	mDimSizes(dimSizes), 
	mVolume(ComputeVolume(dimSizes)){

}

template <typename T, int numDim>
MyArrayMD<T, numDim>::MyArrayMD(const MyArrayMD<T, numDim>& otherArray)
	: mData(otherArray.mData),
	mDimSizes(otherArray.mDimSizes),
	mVolume(otherArray.mVolume){
}

template <typename T, int numDim>
MyArrayMD<T, numDim>::MyArrayMD(const MyVec<int, numDim>& dimSizes)
	: mData(ComputeVolume(dimSizes)),
	mDimSizes(dimSizes),
	mVolume(ComputeVolume(dimSizes)){
}

template <typename T, int numDim>
const int MyArrayMD<T, numDim>::GetDimSize(const int dim) const{
	return mDimSizes[dim];
}

template <typename T, int numDim>
const MyVec<int, numDim>& MyArrayMD<T, numDim>::GetDimSizes() const{
	return mDimSizes;
}

template <typename T, int numDim>
const int MyArrayMD<T, numDim>::GetVolume() const{
	return mVolume;
}

template <typename T, int numDim>
const int MyArrayMD<T, numDim>::ComputeIndex(const MyVec<int, numDim>& pos) const{
	return MyArrayMD<T, numDim>::ComputeIndex(pos, mDimSizes);
}

template <typename T, int numDim>
const MyVec<int, numDim> MyArrayMD<T, numDim>::ComputePosition(int index) const{
	return MyArrayMD<T, numDim>::ComputePosition(index, mDimSizes);
}

template <typename T, int numDim>
const T& MyArrayMD<T, numDim>::At(int index) const{
	return mData[index];
}

template <typename T, int numDim>
const T& MyArrayMD<T, numDim>::At(const MyVec<int, numDim>& pos) const{
	int index = this->ComputeIndex(pos);
	return mData[index];
}

template <typename T, int numDim>
const T& MyArrayMD<T, numDim>::Nearest(const MyVec<float, numDim>& pos) const{
	MyVec<int, numDim> iPos;
	for (int i = 0; i < numDim; i++){
		iPos[i] = pos[i] + 0.5;
	}
	return this->At(iPos);
}

template <typename T, int numDim>
const T& MyArrayMD<T, numDim>::NearestUniform(const MyVec<float, numDim>& pos) const{
	MyVec<float, numDim> rPos;
	for (int i = 0; i < numDim; i++){
		rPos[i] = pos[i] * (mDimSizes[i]-1);
	}
	return this->Nearest(rPos);
}

template <typename T, int numDim>
const T& MyArrayMD<T, numDim>::operator[](int index) const{
	return mData[index];
}


template <typename T, int numDim>
const T& MyArrayMD<T, numDim>::operator[](const MyVec<int, numDim>& pos) const{
	int index = this->ComputeIndex(pos);
	return mData[index];
}

template <typename T, int numDim>
T& MyArrayMD<T, numDim>::operator[](int index){
	return mData[index];
}

template <typename T, int numDim>
T& MyArrayMD<T, numDim>::operator[](const MyVec<int, numDim>& pos){
	int index = this->ComputeIndex(pos);
	return mData[index];
}

template <typename T, int numDim>
T MyArrayMD<T, numDim>::InterpolateLinear(
	const MyVec<float, numDim>& pos) const{
	MyVec<int, numDim> iPos;
	for (int i = 0; i < numDim; i++){
		iPos[i] = (int)pos[i];
	}
	return InterpolateLinear(pos, iPos);
}

template <typename T, int numDim>
T MyArrayMD<T, numDim>::InterpolateLinear(const MyVec<float, numDim>& pos,
	const MyVec<int, numDim>& iPos, int iDim) const{
	if (iDim > numDim) return this->At(iPos);
	float frac = pos[iDim] - iPos[iDim];
	// in case it hits the edge
	if (frac == 0){
		return InterpolateLinear(pos, iPos, iDim + 1);
	}
	else{
		MyVec<int, numDim> jPos = iPos;
		jPos[iDim] ++;
		return (1 - frac)*InterpolateLinear(pos, iPos, iDim + 1)
			+ frac*InterpolateLinear(pos, jPos, iDim + 1);
	}
}

template <typename T, int numDim>
int MyArrayMD<T, numDim>::ComputeVolume(const MyVec<int, numDim>& dimSizes){
	int size = 1;
	for (int jDim = 0; jDim < numDim; jDim++){
		size *= dimSizes[jDim];
	}
	return size;
}

template <typename T, int numDim>
int MyArrayMD<T, numDim>::ComputeIndex(const MyVec<int, numDim>& pos, const MyVec<int, numDim>& dimSizes){
	int index = 0;
	for (int iDim = numDim - 1; iDim >= 0; iDim--){
		int lowerDimSize = 1;
		for (int jDim = 0; jDim < iDim; jDim++){
			lowerDimSize *= dimSizes[jDim];
		}
		index += pos[iDim] * lowerDimSize;
	}
	return index;
}

template <typename T, int numDim>
MyVec<int, numDim> MyArrayMD<T, numDim>::ComputePosition(int index, const MyVec<int, numDim>& dimSizes){
	MyVec<int, numDim> position;
	for (int iDim = numDim - 1; iDim >= 0; iDim--){
		int lowerDimSize = 1;
		for (int jDim = 0; jDim < iDim; jDim++){
			lowerDimSize *= dimSizes[jDim];
		}
		position[iDim] = index / lowerDimSize;
		index %= lowerDimSize;
	}
	return position;
}

template <typename T, int numDim>
void MyArrayMD<T, numDim>::operator+=(T value){
	for (int i = 0; i < mData.size(); i++){
		mData[i] += value;
	}
}

template <typename T, int numDim>
void MyArrayMD<T, numDim>::operator-=(T value){
	for (int i = 0; i < mData.size(); i++){
		mData[i] -= value;
	}
}
template <typename T, int numDim>
void MyArrayMD<T, numDim>::operator*=(T value){
	for (int i = 0; i < mData.size(); i++){
		mData[i] *= value;
	}
}
template <typename T, int numDim>
void MyArrayMD<T, numDim>::operator/=(T value){
	for (int i = 0; i < mData.size(); i++){
		mData[i] /= value;
	}
}
template <typename T, int numDim>
void MyArrayMD<T, numDim>::operator=(T value){
	for (int i = 0; i < mData.size(); i++){
		mData[i] = value;
	}
}


typedef MyArrayMD<double, 4> My4dArrayd;
typedef MyArrayMD<double, 3> My3dArrayd;
typedef MyArrayMD<double, 2> My2dArrayd;
typedef MyArrayMD<float, 4> My4dArrayf;
typedef MyArrayMD<float, 3> My3dArrayf;
typedef MyArrayMD<float, 2> My2dArrayf;
typedef MyArrayMD<int, 4> My4dArrayi;
typedef MyArrayMD<int, 3> My3dArrayi;
typedef MyArrayMD<int, 2> My2dArrayi;
typedef MyArrayMD<char, 4> My4dArrayc;
typedef MyArrayMD<char, 3> My3dArrayc;
typedef MyArrayMD<char, 2> My2dArrayc;
typedef MyArrayMD<bool, 4> My4dArrayb;
typedef MyArrayMD<bool, 3> My3dArrayb;
typedef MyArrayMD<bool, 2> My2dArrayb;

typedef MySharedPointer<MyArrayMD<double, 4>> My4dArraydSPtr;
typedef MySharedPointer<MyArrayMD<double, 3>> My3dArraydSPtr;
typedef MySharedPointer<MyArrayMD<double, 2>> My2dArraydSPtr;
typedef MySharedPointer<MyArrayMD<float, 4>> My4dArrayfSPtr;
typedef MySharedPointer<MyArrayMD<float, 3>> My3dArrayfSPtr;
typedef MySharedPointer<MyArrayMD<float, 2>> My2dArrayfSPtr;
typedef MySharedPointer<MyArrayMD<int, 4>> My4dArrayiSPtr;
typedef MySharedPointer<MyArrayMD<int, 3>> My3dArrayiSPtr;
typedef MySharedPointer<MyArrayMD<int, 2>> My2dArrayiSPtr;
typedef MySharedPointer<MyArrayMD<char, 4>> My4dArraycSPtr;
typedef MySharedPointer<MyArrayMD<char, 3>> My3dArraycSPtr;
typedef MySharedPointer<MyArrayMD<char, 2>> My2dArraycSPtr;
typedef MySharedPointer<MyArrayMD<bool, 4>> My4dArraybSPtr;
typedef MySharedPointer<MyArrayMD<bool, 3>> My3dArraybSPtr;
typedef MySharedPointer<MyArrayMD<bool, 2>> My2dArraybSPtr;

typedef MySharedPointer<const MyArrayMD<double, 4>> My4dArraydScPtr;
typedef MySharedPointer<const MyArrayMD<double, 3>> My3dArraydScPtr;
typedef MySharedPointer<const MyArrayMD<double, 2>> My2dArraydScPtr;
typedef MySharedPointer<const MyArrayMD<float, 4>> My4dArrayfScPtr;
typedef MySharedPointer<const MyArrayMD<float, 3>> My3dArrayfScPtr;
typedef MySharedPointer<const MyArrayMD<float, 2>> My2dArrayfScPtr;
typedef MySharedPointer<const MyArrayMD<int, 4>> My4dArrayiScPtr;
typedef MySharedPointer<const MyArrayMD<int, 3>> My3dArrayiScPtr;
typedef MySharedPointer<const MyArrayMD<int, 2>> My2dArrayiScPtr;
typedef MySharedPointer<const MyArrayMD<char, 4>> My4dArraycScPtr;
typedef MySharedPointer<const MyArrayMD<char, 3>> My3dArraycScPtr;
typedef MySharedPointer<const MyArrayMD<char, 2>> My2dArraycScPtr;
typedef MySharedPointer<const MyArrayMD<bool, 4>> My4dArraybScPtr;
typedef MySharedPointer<const MyArrayMD<bool, 3>> My3dArraybScPtr;
typedef MySharedPointer<const MyArrayMD<bool, 2>> My2dArraybScPtr;

template<typename T, int numDim >
using MyArrayMDSPtr = MySharedPointer < MyArrayMD<T, numDim> >;

template<typename T, int numDim >
using MyArrayMDScPtr = MySharedPointer < const MyArrayMD<T, numDim> >;