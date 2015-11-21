#pragma once

#include <cassert>
#include "MyVoxContainer_header.h"

#define SegmentLarge_Ratio 0.01
#define SegmentSmall_Ratio 0.001

template<typename valueType>
valueType MyVoxContainer<valueType>::NullVoxelValue = -1;

// specialization the bool case
//template<>
//bool MyVoxContainerb::NullVoxelValue = false;

template<typename valueType>
int MyVoxContainer<valueType>::CountOverlapping(const MyVoxContainer<valueType>* voxels0,
	const MyVoxContainer<valueType>* voxels1){
	// first check bounding box
	int count = 0;
	if (!voxels0->mBoundingBox.IsIntersected(voxels1->mBoundingBox)){
		return count;
	}
	// note smaller containers generally are easier to traverse,
	// but more difficult to check voxel containment.
	// so it's wiser to traverse smaller container while check larger container
	const MyVoxContainer<valueType>* smaller;
	const MyVoxContainer<valueType>* larger;
	if (voxels0->GetNumVoxes() > voxels1->GetNumVoxes()){
		smaller = voxels1;
		larger = voxels0;
	}
	else{
		smaller = voxels0;
		larger = voxels1;
	}
	MyVoxContainer<valueType>::const_Iterator itr = smaller->Begin();
	MyVoxContainer<valueType>::const_Iterator itrEnd = smaller->End();
	while (itr != itrEnd){
		int index = itr.GetVoxelIndex();
		if (larger->IsVoxelIn(index)){
			count++;
		}
		itr++;
	}
	return count;
}

template<typename valueType>
bool MyVoxContainer<valueType>::IsOverlapping(const MyVoxContainer<valueType>* voxels0,
	const MyVoxContainer<valueType>* voxels1){
	// first check bounding box
	if (!voxels0->mBoundingBox.IsIntersected(voxels1->mBoundingBox)){
		return false;
	}
	// note smaller containers generally are easier to traverse,
	// but more difficult to check voxel containment.
	// so it's wiser to traverse smaller container while check larger container
	const MyVoxContainer<valueType>* smaller;
	const MyVoxContainer<valueType>* larger;
	if (voxels0->GetNumVoxes() > voxels1->GetNumVoxes()){
		smaller = voxels1;
		larger = voxels0;
	}
	else{
		smaller = voxels0;
		larger = voxels1;
	}
	MyVoxContainer<valueType>::const_Iterator itr = smaller->Begin();
	MyVoxContainer<valueType>::const_Iterator itrEnd = smaller->End();
	while (itr != itrEnd){
		int index = itr.GetVoxelIndex();
		if (larger->IsVoxelIn(index)){
			return true;
		}
		itr++;
	}
	return false;
}

template<typename valueType>
MyVoxContainerSPtr<valueType> MyVoxContainer<valueType>::MakeVoxContainer(
	const MyArrayi* voxIndices, const valueType& value, const MyVec3i& volSize){
	MyVoxContainer <valueType>* ptr;
	float density = voxIndices->size() / (float)My3dArrayi::ComputeVolume(volSize);
	if (density > SegmentLarge_Ratio){
		ptr = new MyVoxContainer_Large<valueType>(volSize, NullVoxelValue);
		ptr->AddVoxes(voxIndices, value);
	}
	else if (density > SegmentSmall_Ratio) {
		ptr = new MyVoxContainer_Small<valueType>(volSize);
		ptr->AddVoxes(voxIndices, value);
	}
	else{
		ptr = new MyVoxContainer_Tiny<valueType>(volSize);
		ptr->AddVoxes(voxIndices, value);
	}
	return MyVoxContainerSPtr<valueType>(ptr);
}

template<typename valueType>
MyVoxContainerSPtr<valueType> MyVoxContainer<valueType>::MakeVoxContainer(
	const MyArrayi* voxIndices, const MyArray<valueType>* voxValues, const MyVec3i& volSize){
	MyVoxContainer <valueType>* ptr;
	float density = voxIndices->size() / (float)My3dArrayi::ComputeVolume(volSize);
	if (density > SegmentLarge_Ratio){
		ptr = new MyVoxContainer_Large<valueType>(volSize, NullVoxelValue);
		ptr->AddVoxes(voxIndices, voxValues);
	}
	else if (density > SegmentSmall_Ratio) {
		ptr = new MyVoxContainer_Small<valueType>(volSize);
		ptr->AddVoxes(voxIndices, voxValues);
	}
	else{
		ptr = new MyVoxContainer_Tiny<valueType>(volSize);
		ptr->AddVoxes(voxIndices, voxValues);
	}
	return MyVoxContainerSPtr<valueType>(ptr);
}

template<typename valueType>
MyVoxContainerSPtr<valueType> MyVoxContainer<valueType>::MakeVoxContainer(
	const MyVoxContainer<valueType>* container, ContainerType type){
	MyVoxContainer <valueType>* ptr;
	float density = container->GetNumVoxes() / (float)container->ComputeVolume();
	MyVec3i volSize = container->GetVolumeSize();
	if (type == ContainerType_NONE){
		if (density > SegmentLarge_Ratio){
			ptr = new MyVoxContainer_Large<valueType>(volSize, NullVoxelValue);
			ptr->Add(container);
		}
		else if (density > SegmentSmall_Ratio) {
			ptr = new MyVoxContainer_Small<valueType>(volSize);
			ptr->Add(container);
		}
		else{
			ptr = new MyVoxContainer_Tiny<valueType>(volSize);
			ptr->Add(container);
		}
	}
	else if (type == ContainerType_Large){
		ptr = new MyVoxContainer_Large<valueType>(volSize, NullVoxelValue);
		ptr->Add(container);
	}
	else if (type == ContainerType_Small){
		ptr = new MyVoxContainer_Small<valueType>(volSize);
		ptr->Add(container);
	}
	else{
		ptr = new MyVoxContainer_Tiny<valueType>(volSize);
		ptr->Add(container);
	}
	return MyVoxContainerSPtr<valueType>(ptr);
}

template<typename valueType>
MyVoxContainer<valueType>::MyVoxContainer(const MyVec3i& volBox)
	:mVolumeSize(volBox)
{
	mBoundingBox.SetHigh(MyVec3i(-INT_MAX, -INT_MAX, -INT_MAX));
	mBoundingBox.SetLow(MyVec3i(INT_MAX, INT_MAX, INT_MAX));
}

template<typename valueType>
MyVoxContainer<valueType>::~MyVoxContainer()
{
}

template<typename valueType>
void MyVoxContainer<valueType>::Add(const MyVoxContainer<valueType>* container){
	int numVoxes = container->GetNumVoxes();
	MyArrayi indices;
	MyArray<valueType> values;
	indices.reserve(numVoxes);
	values.reserve(numVoxes);
	MyVoxContainer<valueType>::const_Iterator itr = container->Begin();
	while (itr != container->End()){
		indices << itr.GetVoxelIndex();
		values << *itr;
		itr++;
	}
	this->AddVoxes(&indices, &values);
}

template<typename valueType>
MyArrayMDSPtr <valueType, 3> MyVoxContainer<valueType>::MakeVolume() const {
	MyArrayMD <valueType, 3>* vol = new MyArrayMD <valueType, 3>(mVolumeSize, NullVoxelValue);
	MyVoxContainer<valueType>::const_Iterator itr = this->Begin();
	while (itr != this->End()){
		vol->operator[](itr.GetVoxelIndex()) = *itr;
		itr++;
	}
	return MyArrayMDSPtr<valueType, 3>(vol);
}

template<typename valueType>
MyArrayiSPtr MyVoxContainer<valueType>::MakeIndexArray() const{
	MyArrayiSPtr rst = std::make_shared<MyArrayi>();
	MyVoxContainer<valueType>::const_Iterator itr = this->Begin();
	while (itr != this->End()){
		//vol->operator[](itr.GetVoxelIndex()) = *itr;
		rst->PushBack(itr.GetVoxelIndex());
		itr++;
	}
	return rst;
}

/************************ Start Large ***********************************/
template<typename valueType>
MyArrayMDSPtr <valueType, 3> MyVoxContainer_Large<valueType>::MakeVolume() const{
	return std::make_shared< MyArrayMD <valueType, 3>>(mVolumeMask);
}

template<typename valueType>
void MyVoxContainer_Large<valueType>::Clear(){
	mVolumeMask = NullVoxelValue;
	mNumVoxes = 0;
}

template<typename valueType>
int MyVoxContainer_Large<valueType>::GetNumVoxes() const{
	return mNumVoxes;
}

template<typename valueType>
bool MyVoxContainer_Large<valueType>::IsVoxelIn(const MyVec3i& pos) const{
	return mVolumeMask[pos] != NullVoxelValue;
}

template<typename valueType>
bool MyVoxContainer_Large<valueType>::IsVoxelIn(int index) const{
	return mVolumeMask[index] != NullVoxelValue;
}

template<typename valueType>
valueType& MyVoxContainer_Large<valueType>::GetVoxel(int index){
	return mVolumeMask[index];
}

template<typename valueType>
const valueType& MyVoxContainer_Large<valueType>::GetVoxel(int index) const{
	return mVolumeMask[index];
}

template<typename valueType>
void MyVoxContainer_Large<valueType>::AddVoxes(const MyArrayi* voxIndices, const valueType& value){
	for (int i = 0; i < voxIndices->size(); i++){
		mVolumeMask[voxIndices->at(i)] = value;
		MyVec3i pos = MyArrayMD<valueType, 3>::ComputePosition(voxIndices->at(i), mVolumeSize);
		mBoundingBox.Engulf(pos);
	}
	mNumVoxes += voxIndices->size();
}

template<typename valueType>
void MyVoxContainer_Large<valueType>::AddVoxes(const MyArrayi* voxIndices,
	const MyArray<valueType>* voxValues){
	for (int i = 0; i < voxIndices->size(); i++){
		mVolumeMask[voxIndices->at(i)] = voxValues->at(i);
		MyVec3i pos = MyArrayMD<valueType, 3>::ComputePosition(voxIndices->at(i), mVolumeSize);
		mBoundingBox.Engulf(pos);
	}
	mNumVoxes += voxIndices->size();
}

template<typename valueType>
void MyVoxContainer_Large<valueType>::Next(Iterator* iterator){
	int size = mVolumeMask.GetVolume();
	while (++MyVoxContainer::GetIndex(iterator) < size){
		if (mVolumeMask.At(MyVoxContainer::GetIndex(iterator)) != NullVoxelValue) break;
	};
}

template<typename valueType>
void MyVoxContainer_Large<valueType>::Prev(Iterator* iterator){
	while (--MyVoxContainer::GetIndex(iterator) > 0){
		if (mVolumeMask.At(MyVoxContainer::GetIndex(iterator)) != NullVoxelValue) break;
	}
}

template<typename valueType>
typename MyVoxContainer<valueType>::Iterator
MyVoxContainer_Large<valueType>::Begin(){
	int index = 0;
	int size = mVolumeMask.GetVolume();
	while (index < size){
		if (mVolumeMask.At(index) != NullVoxelValue) break;
		index++;
	};
	return MyVoxContainer<valueType>::Iterator(index, this);
}


template<typename valueType>
typename MyVoxContainer<valueType>::Iterator
MyVoxContainer_Large<valueType>::End(){
	return MyVoxContainer<valueType>::Iterator(mVolumeMask.GetVolume(), this);
}


template<typename valueType>
void MyVoxContainer_Large<valueType>::Next(const_Iterator* iterator) const{
	int size = mVolumeMask.GetVolume();
	while (++MyVoxContainer::GetIndex(iterator) < size){
		if (mVolumeMask.At(MyVoxContainer::GetIndex(iterator)) != NullVoxelValue) break;
	};
}

template<typename valueType>
void MyVoxContainer_Large<valueType>::Prev(const_Iterator* iterator) const{
	while (--MyVoxContainer::GetIndex(iterator) > 0){
		if (mVolumeMask.At(MyVoxContainer::GetIndex(iterator)) != NullVoxelValue) break;
	}
}

template<typename valueType>
typename MyVoxContainer<valueType>::const_Iterator
MyVoxContainer_Large<valueType>::Begin() const{
	int index = 0;
	int size = mVolumeMask.GetVolume();
	while (index < size){
		if (mVolumeMask.At(index) != NullVoxelValue) break;
		index++;
	};
	return MyVoxContainer<valueType>::const_Iterator(index, this);
}


template<typename valueType>
typename MyVoxContainer<valueType>::const_Iterator
MyVoxContainer_Large<valueType>::End() const{
	return MyVoxContainer<valueType>::const_Iterator(mVolumeMask.GetVolume(), this);
}

/************************ Start Small ***********************************/


template<typename valueType>
void MyVoxContainer_Small<valueType>::Clear(){
	mVoxes.clear();
}

template<typename valueType>
int MyVoxContainer_Small<valueType>::GetNumVoxes() const{
	return mVoxes.size();
}

template<typename valueType>
bool MyVoxContainer_Small<valueType>::IsVoxelIn(const MyVec3i& pos) const{
	return mVoxes.HasKey(My3dArrayi::ComputeIndex(pos, mVolumeSize));
}

template<typename valueType>
bool MyVoxContainer_Small<valueType>::IsVoxelIn(int index) const{
	return mVoxes.HasKey(index);
}

template<typename valueType>
valueType& MyVoxContainer_Small<valueType>::GetVoxel(int index){
	assert(index != INT_MAX);
	return mVoxes[index];
}

template<typename valueType>
const valueType& MyVoxContainer_Small<valueType>::GetVoxel(int index) const{
	assert(index != INT_MAX);
	return mVoxes.at(index);
}

template<typename valueType>
void MyVoxContainer_Small<valueType>::AddVoxes(const MyArrayi* voxIndices, const valueType& value){
	for (int i = 0; i < voxIndices->size(); i++){
		mVoxes[voxIndices->at(i)] = value;
		MyVec3i pos = MyArrayMD<valueType, 3>::ComputePosition(voxIndices->at(i), mVolumeSize);
		mBoundingBox.Engulf(pos);
	}
}

template<typename valueType>
void MyVoxContainer_Small<valueType>::AddVoxes(const MyArrayi* voxIndices,
	const MyArray<valueType>* voxValues){
	for (int i = 0; i < voxIndices->size(); i++){
		mVoxes[voxIndices->at(i)] = voxValues->at(i);
		MyVec3i pos = MyArrayMD<valueType, 3>::ComputePosition(voxIndices->at(i), mVolumeSize);
		mBoundingBox.Engulf(pos);
	}
}

template<typename valueType>
void MyVoxContainer_Small<valueType>::Next(Iterator* iterator){
	MyMap<int, valueType>::const_iterator itr = mVoxes.find(MyVoxContainer::GetIndex(iterator));
	itr++;
	if (itr == mVoxes.end()){
		MyVoxContainer::GetIndex(iterator) = INT_MAX;
	}
	else MyVoxContainer::GetIndex(iterator) = itr->first;
}

template<typename valueType>
void MyVoxContainer_Small<valueType>::Prev(Iterator* iterator){
	MyMap<int, valueType>::const_iterator itr = mVoxes.find(MyVoxContainer::GetIndex(iterator));
	itr--;
	MyVoxContainer::GetIndex(iterator) = itr->first;
}

template<typename valueType>
typename MyVoxContainer<valueType>::Iterator
MyVoxContainer_Small<valueType>::Begin(){
	if (mVoxes.empty()){
		return MyVoxContainer<valueType>::Iterator(-1, this);
	}
	MyMap<int, valueType>::const_iterator itr = mVoxes.begin();
	return MyVoxContainer<valueType>::Iterator(itr->first, this);
}


template<typename valueType>
typename MyVoxContainer<valueType>::Iterator
MyVoxContainer_Small<valueType>::End(){
	return MyVoxContainer<valueType>::Iterator(INT_MAX, this);
}


template<typename valueType>
void MyVoxContainer_Small<valueType>::Next(const_Iterator* iterator) const{
	MyMap<int, valueType>::const_iterator itr = mVoxes.find(MyVoxContainer::GetIndex(iterator));
	itr++;
	if (itr == mVoxes.end()){
		MyVoxContainer::GetIndex(iterator) = INT_MAX;
	}
	else MyVoxContainer::GetIndex(iterator) = itr->first;
}

template<typename valueType>
void MyVoxContainer_Small<valueType>::Prev(const_Iterator* iterator) const{
	MyMap<int, valueType>::const_iterator itr = mVoxes.find(MyVoxContainer::GetIndex(iterator));
	itr--;
	MyVoxContainer::GetIndex(iterator) = itr->first;
}

template<typename valueType>
typename MyVoxContainer<valueType>::const_Iterator
MyVoxContainer_Small<valueType>::Begin() const{
	if (mVoxes.empty()){
		return MyVoxContainer<valueType>::const_Iterator(-1, this);
	}
	MyMap<int, valueType>::const_iterator itr = mVoxes.begin();
	return MyVoxContainer<valueType>::const_Iterator(itr->first, this);
}


template<typename valueType>
typename MyVoxContainer<valueType>::const_Iterator
MyVoxContainer_Small<valueType>::End() const{
	return MyVoxContainer<valueType>::const_Iterator(INT_MAX, this);
}

/********************** Start Tiny ***************************/

template<typename valueType>
void MyVoxContainer_Tiny<valueType>::Clear(){
	mIndices.clear();
	mValues.clear();
}

template<typename valueType>
int MyVoxContainer_Tiny<valueType>::GetNumVoxes() const{
	return mIndices.size();
}

template<typename valueType>
bool MyVoxContainer_Tiny<valueType>::IsVoxelIn(const MyVec3i& pos) const{
	return mIndices.HasOne(My3dArrayi::ComputeIndex(pos, mVolumeSize));
}

template<typename valueType>
bool MyVoxContainer_Tiny<valueType>::IsVoxelIn(int index) const{
	return mIndices.HasOne(index);
}

template<typename valueType>
valueType& MyVoxContainer_Tiny<valueType>::GetVoxel(int index){
	return mValues[index];
}

template<typename valueType>
const valueType& MyVoxContainer_Tiny<valueType>::GetVoxel(int index) const{
	return mValues[index];
}

template<typename valueType>
void MyVoxContainer_Tiny<valueType>::AddVoxes(const MyArrayi* voxIndices, const valueType& value){
	mIndices.PushBack(voxIndices);
	mValues.resize(mValues.size() + voxIndices->size(), value);
	for (int i = 0; i < voxIndices->size(); i++){
		MyVec3i pos = MyArrayMD<valueType, 3>::ComputePosition(voxIndices->at(i), mVolumeSize);
		mBoundingBox.Engulf(pos);
	}
}

template<typename valueType>
void MyVoxContainer_Tiny<valueType>::AddVoxes(const MyArrayi* voxIndices,
	const MyArray<valueType>* voxValues){
	mIndices.PushBack(voxIndices);
	mValues.PushBack(voxValues);
	for (int i = 0; i < voxIndices->size(); i++){
		MyVec3i pos = MyArrayMD<valueType, 3>::ComputePosition(voxIndices->at(i), mVolumeSize);
		mBoundingBox.Engulf(pos);
	}
}

template<typename valueType>
void MyVoxContainer_Tiny<valueType>::Next(Iterator* iterator){
	if (GetIndex(iterator) < mIndices.size())
		++GetIndex(iterator);
	
}

template<typename valueType>
void MyVoxContainer_Tiny<valueType>::Prev(Iterator* iterator){
	if (GetIndex(iterator) > 0)
		--GetIndex(iterator);
}

template<typename valueType>
typename MyVoxContainer<valueType>::Iterator
MyVoxContainer_Tiny<valueType>::Begin(){
	return MyVoxContainer<valueType>::Iterator(0, this);
}


template<typename valueType>
typename MyVoxContainer<valueType>::Iterator
MyVoxContainer_Tiny<valueType>::End(){
	return MyVoxContainer<valueType>::Iterator(mIndices.size(), this);
}


template<typename valueType>
void MyVoxContainer_Tiny<valueType>::Next(const_Iterator* iterator) const{
	if (GetIndex(iterator) < mIndices.size())
		++GetIndex(iterator);
}

template<typename valueType>
void MyVoxContainer_Tiny<valueType>::Prev(const_Iterator* iterator) const{
	if (GetIndex(iterator) > 0)
		--GetIndex(iterator);
}

template<typename valueType>
typename MyVoxContainer<valueType>::const_Iterator
MyVoxContainer_Tiny<valueType>::Begin() const{
	return MyVoxContainer<valueType>::const_Iterator(0, this);
}


template<typename valueType>
typename MyVoxContainer<valueType>::const_Iterator
MyVoxContainer_Tiny<valueType>::End() const{
	return MyVoxContainer<valueType>::const_Iterator(mIndices.size(), this);
}

template<typename valueType>
int MyVoxContainer_Tiny<valueType>::TranslateIndex(int index) const{
	return mIndices[index];
}

template<typename valueType>
const valueType& MyVoxContainer_Tiny<valueType>::GetVoxel(const Iterator* iterator) const {
	return mValues[GetIndex(iterator)];
}

template<typename valueType>
valueType& MyVoxContainer_Tiny<valueType>::GetVoxel(const Iterator* iterator) {
	return mValues[GetIndex(iterator)];
}

template<typename valueType>
const valueType& MyVoxContainer_Tiny<valueType>::GetVoxel(const const_Iterator* iterator) const {
	return mValues[GetIndex(iterator)];
}

template<typename valueType>
valueType& MyVoxContainer_Tiny<valueType>::GetVoxel(const const_Iterator* iterator) {
	return mValues[GetIndex(iterator)];
}


#undef SegmentLarge_Ratio
#undef SegmentSmall_Ratio