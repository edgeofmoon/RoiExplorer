#pragma once

#include "MyVec.h"
#include "MyString.h"
#include <vector>
#include <algorithm>
#include "MySharedPointer.h"

template <typename T>
class MyArray :
	public std::vector<typename T>
{
public:
	MyArray(void);
	MyArray(const MyArray<T>& otherArray);
	MyArray(int n);
	MyArray(int n, const T& defaultValue);

	~MyArray(void);
	
	T* MakeArray() const;
	//friend MyArray<T>& operator<<(MyArray<T>& arr, const T& ele);

	//MyArray<T>& operator<<(MyArray<T>& arr, const T& ele);

	MyArray<T>& operator<<(const T& ele);
	friend MyArray<T> operator+(const MyArray<T>& arr0,const MyArray<T>& arr1);
	void operator+=(const MyArray<T>& arr);

	bool EraseOne(const T& ele);
	bool EraseAll(const T& ele);
	bool EraseAt(int index);
	void EraseOne(const MyArray<T>* arr);
	void EraseAll(const MyArray<T>* arr);

	bool HasOne(const T& ele)const;

	bool PushBackUniqueOne(const T& ele);

	void Swap(int i, int j);
	void Copy(const MyArray<T>* arr);
	void PushBack(const MyArray<T>* arr);
	void PushBack(const T val);
	int IndexOf(const T& ele) const;

	MyArray<int>* MakeSortResultArray() const;

	MyArray<int>* MakeSortResultArray(bool (*comp)(const T&, const T&)) const;

	int GetMinimumIndex() const;
	int GetMaximumIndex() const;

	void Permute(unsigned int seed);
	void Permute(unsigned int seed, int start, int end);

	static MyArray<T>* MakeCommonElementArray(const MyArray<T>& array1, const MyArray<T>& array2);

	static MyArray<T> GetSequence(const T& start, const T& end, const T& inc = T(1));

	// array1 and array2 must be sorted from small to large
	static int CountCommonElementsSorted(const MyArray<T>* array1, const MyArray<T>* array2);
	static MyArray<T>* MakeUnionArraySorted(const MyArray<T>* array1, const MyArray<T>* array2);
	
private:
	static const MyArray<T>* tmpArray;
	static bool isSmallerByIndex(int a, int b);

	static bool (*compare)(const T&, const T&);
	static bool isSmallerByIndexComp(int a, int b);
};


template <typename T>
const MyArray<T>* MyArray<T>::tmpArray = 0;

template <typename T>
bool(*MyArray<T>::compare)(const T&, const T&) = 0;

template <typename T>
MyArray<T>::MyArray(void)
	:std::vector<T>(){
}

template <typename T>
MyArray<T>::MyArray(const MyArray<T>& otherArray)
	: std::vector<T>(otherArray){
}

template <typename T>
MyArray<T>::MyArray(int n)
	:std::vector<T>(n){
}

template <typename T>
MyArray<T>::MyArray(int n, const T& defaultValue)
	:std::vector<T>(n, defaultValue){
}

template <typename T>
MyArray<T>::~MyArray(void){
}

template <typename T>
T* MyArray<T>::MakeArray() const{
	T* rst = new T[this->size()];
	for(unsigned int i = 0;i<this->size();i++){
		rst[i] = this->at(i);
	}
	return rst;
}

/*
template <typename T>
MyArray<T>& operator<<(MyArray<T>& arr, const T& ele){
	arr.push_back(ele);
	return arr;
};
*/
template <typename T>
MyArray<T>& MyArray<T>::operator<<(const T& ele){
	this->push_back(ele);
	return *this;
}

template <typename T>
MyArray<T> operator+(const MyArray<T>& arr0,const MyArray<T>& arr1){
	MyArray<T> arr(arr0);
	arr += arr1;
	return arr;
}
	
template <typename T>
void MyArray<T>::operator+=(const MyArray<T>& arr){
	for(unsigned int i = 0;i<arr.size();i++){
		this->push_back(arr[i]);
	}
}

template <typename T>
bool MyArray<T>::EraseOne(const T& ele){
	for(MyArray<T>::iterator itr = this->begin();
		itr != this->end();
		itr ++){
		if (*itr == ele){
			this->erase(itr);
			return true;
		}
	}
	return false;
}

template <typename T>
bool MyArray<T>::EraseAll(const T& ele){
	bool hasOne = false;
	for (MyArray<T>::iterator itr = this->begin();
		itr != this->end();
		itr++){
		if (*itr == ele){
			this->erase(itr);
			hasOne = true;
		}
	}
	return hasOne;
}

template <typename T>
bool MyArray<T>::EraseAt(int idx){
	bool hasOne = false;
	if(idx < this->size()){
		MyArray<T>::iterator itr = this->begin();
		itr += idx;
		this->erase(itr);
		hasOne = true;
	}
	return hasOne;
}

template <typename T>
bool MyArray<T>::HasOne(const T& ele) const{
	for(MyArray<T>::const_iterator itr = this->begin();
		itr != this->end();
		itr ++){
		if (*itr == ele){
			return true;
		}
	}
	return false;
}

template <typename T>
bool MyArray<T>::PushBackUniqueOne(const T& ele){
	if(this->HasOne(ele)){
		return false;
	}
	this->push_back(ele);
	return true;
}

template <typename T>
void MyArray<T>::Swap(int i, int j){
	T tmp = this->at(j);
	this->at(j) = this->at(i);
	this->at(i) = tmp;
}

template <typename T>
void MyArray<T>::EraseOne(const MyArray<T>* arr){
	for (int i = 0; i < arr->size(); i++){
		this->EraseOne(arr->at(i));
	}
}

template <typename T>
void MyArray<T>::EraseAll(const MyArray<T>* arr){
	for (int i = 0; i < arr->size(); i++){
		this->EraseAll(arr->at(i));
	}
}

template <typename T>
void MyArray<T>::Copy(const MyArray<T>* arr){
	*(this) = *arr;
}

template <typename T>
void MyArray<T>::PushBack(const MyArray<T>* arr){
	for(int i = 0;i<arr->size();i++){
		this->push_back(arr->at(i));
	}
}

template <typename T>
void MyArray<T>::PushBack(const T val){
	this->push_back(val);
}

template <typename T>
int MyArray<T>::IndexOf(const T& ele) const{
	for(int i = 0; i < this->size(); i++){
		if (this->at(i) == ele){
			return i;
		}
	}
	return -1;
}


template <typename T>
MyArray<int>* MyArray<T>::MakeSortResultArray() const{
	MyArray<int>* rst = new MyArray<int>(this->size());
	for(int i = 0;i<this->size();i++){
		rst->at(i) = i;
	}
	tmpArray = this;
	std::sort(rst->begin(), rst->end(), MyArray<T>::isSmallerByIndex);
	return rst;
}

template <typename T>
MyArray<int>* MyArray<T>::MakeSortResultArray(
	bool(*comp)(const T&, const T&)) const {
	MyArray<int>* rst = new MyArray<int>(this->size());
	for (int i = 0; i<this->size(); i++){
		rst->at(i) = i;
	}
	compare = comp;
	tmpArray = this;
	std::sort(rst->begin(), rst->end(), MyArray<T>::isSmallerByIndexComp);
	return rst;
}

template <typename T>
int MyArray<T>::GetMinimumIndex() const{
	T tmp = this->at(0);
	int idx = 0;
	for(int i = 1;i<this->size();i++){
		if(this->at(i)<tmp){
			tmp = this->at(i);
			idx = i;
		}
	}
	return idx;
}
	
template <typename T>
int MyArray<T>::GetMaximumIndex() const{
	T tmp = this->at(0);
	int idx = 0;
	for(int i = 1;i<this->size();i++){
		if(this->at(i)>tmp){
			tmp = this->at(i);
			idx = i;
		}
	}
	return idx;
}

template <typename T>
void MyArray<T>::Permute(unsigned int seed){
	if(this->size()<=1) return;
	srand(seed);
	for(int i = 0;i<size()-1;i++){
		int range = size()-i;
		int toSwapIdx = rand()%range+i;
		if(i != toSwapIdx){
			this->Swap(i, toSwapIdx);
		}
	}
}

template <typename T>
void MyArray<T>::Permute(unsigned int seed, int start, int end){
	if(end<start) return;
	srand(seed);
	int _end = end;
	if(_end>size()) _end = size();
	for(int i = start;i<_end-1;i++){
		int range = _end-i+1;
		int toSwapIdx = rand()%range+i;
		if(i != toSwapIdx){
			this->Swap(i, toSwapIdx);
		}
	}
}

template <typename T>
MyArray<T>* MyArray<T>::MakeCommonElementArray(const MyArray<T>& array1, const MyArray<T>& array2){
	MyArray<T>* rst = new MyArray<T>;
	for(int i = 0;i<array1.size();i++){
		const T& ele = array1[i];
		for(int j = 0;j<array2.size();j++){
			if(ele == array2[j]){
				rst -> push_back(ele);
				break;
			}
		}
	}
	return rst;
}

template <typename T>
MyArray<T> MyArray<T>::GetSequence(const T& start, const T& end, const T& inc = int(1)){
	MyArray<T> rst;
	T value = start;
	int numEle = (end - start) / inc + 1;
	if (numEle <= 0) {
		return rst;
	}
	rst.reserve(numEle);
	if (end >= start){
		do{
			rst << value;
			value += inc;
		} while (value <= end);
	}
	else{
		do{
			rst << value;
			value += inc;
		} while (value >= end);
	}
	return rst;
}

template <typename T>
int MyArray<T>::CountCommonElementsSorted(const MyArray<T>* array1, const MyArray<T>* array2){
	MyArray<T>::const_iterator itr1 = array1->begin();
	MyArray<T>::const_iterator itr2 = array2->begin();
	int count = 0;
	while (itr1 != array1->end() && itr2 != array2->end()){
		if (*itr1 == *itr2){
			count++;
			itr1++;
			itr2++;
		}
		else if (*itr1 > *itr2){
			itr2++;
		}
		else{
			itr1++;
		}
	}
	return count;
}

template <typename T>
MyArray<T>* MyArray<T>::MakeUnionArraySorted(const MyArray<T>* array1, const MyArray<T>* array2){
	int i = 0, j = 0;
	int numCommon = CountCommonElementsSorted(array1, array2);
	MyArray<T>* rst = new MyArray < T >;
	rst->reserve(array1->size() + array2->size() - numCommon);
	while (i < array1->size() && j < array2->size()){
		const T& ele1 = array1->at(i);
		const T& ele2 = array2->at(j);
		if (ele1 == ele2){
			rst->push_back(ele1);
			i++;
			j++;
		}
		else if (ele1 < ele2){
			rst->push_back(ele1);
			i++;
		}
		else{
			rst->push_back(ele2);
			j++;
		}
	}
	return rst;
};
template <typename T>
bool MyArray<T>::isSmallerByIndex(int a, int b){
	return tmpArray->at(a)<tmpArray->at(b);
}

template <typename T>
bool MyArray<T>::isSmallerByIndexComp(int a, int b){
	return compare(tmpArray->at(a),tmpArray->at(b));
}

typedef MyArray<int> MyArrayi;
typedef MyArray<MyVec2i> MyArray2i;
typedef MyArray<MyVec3i> MyArray3i;
typedef MyArray<MyVec4i> MyArray4i;
typedef MyArray<float> MyArrayf;
typedef MyArray<MyVec2f> MyArray2f;
typedef MyArray<MyVec3f> MyArray3f;
typedef MyArray<MyVec4f> MyArray4f;
typedef MyArray<MyString> MyArrayStr;
typedef MyArray<bool> MyArrayb;

typedef MySharedPointer <MyArray<int>> MyArrayiSPtr;
typedef MySharedPointer <MyArray<MyVec2i>> MyArray2iSPtr;
typedef MySharedPointer <MyArray<MyVec3i>> MyArray3iSPtr;
typedef MySharedPointer <MyArray<MyVec4i>> MyArray4iSPtr;
typedef MySharedPointer <MyArray<float>> MyArrayfSPtr;
typedef MySharedPointer <MyArray<MyVec2f>> MyArray2fSPtr;
typedef MySharedPointer <MyArray<MyVec3f>> MyArray3fSPtr;
typedef MySharedPointer <MyArray<MyVec4f>> MyArray4fSPtr;
typedef MySharedPointer <MyArray<MyString>> MyArrayStrSPtr;
typedef MySharedPointer <MyArray<bool>> MyArraybSPtr;

typedef MySharedPointer <const MyArray<int>> MyArrayiScPtr;
typedef MySharedPointer <const MyArray<MyVec2i>> MyArray2iScPtr;
typedef MySharedPointer <const MyArray<MyVec3i>> MyArray3iScPtr;
typedef MySharedPointer <const MyArray<MyVec4i>> MyArray4iScPtr;
typedef MySharedPointer <const MyArray<float>> MyArrayfScPtr;
typedef MySharedPointer <const MyArray<MyVec2f>> MyArray2fScPtr;
typedef MySharedPointer <const MyArray<MyVec3f>> MyArray3fScPtr;
typedef MySharedPointer <const MyArray<MyVec4f>> MyArray4fScPtr;
typedef MySharedPointer <const MyArray<MyString>> MyArrayStrScPtr;
typedef MySharedPointer <const MyArray<bool>> MyArraybScPtr;

template<typename T>
using MyArraySPtr = MySharedPointer < MyArray<T> > ;

template<typename T>
using MyArrayScPtr = MySharedPointer < const MyArray<T> >;