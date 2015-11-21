#include "MyVolumnSimilarity.h"
#include "MyMathHelper.h"
#include "MyArray.h"

#include <cassert>
MyVolumnSimilarity::MyVolumnSimilarity()
{
}


MyVolumnSimilarity::~MyVolumnSimilarity()
{
}

float MyVolumnSimilarity::ComputeSimilarity(const MyVoxContainerf* vol1,
	const MyVoxContainerf* vol2, int method){
	MyArrayi voxels1, voxels2;
	MyVoxContainerf::const_Iterator itr1 = vol1->Begin();
	while (itr1 != vol1->End()){
		int index = itr1.GetVoxelIndex();
		voxels1 << index;
		itr1++;
	}
	MyVoxContainerf::const_Iterator itr2 = vol2->Begin();
	while (itr2 != vol2->End()){
		int index = itr2.GetVoxelIndex();
		voxels2 << index;
		itr2++;
	}
	sort(voxels1.begin(), voxels1.end());
	sort(voxels2.begin(), voxels2.end());
	MyArrayi* unionVoxels = MyArrayi::MakeUnionArraySorted(&voxels1, &voxels2);
	if (unionVoxels->empty()){
		return 0;
	}
	MyArrayf v1, v2;
	v1.reserve(unionVoxels->size());
	v2.reserve(unionVoxels->size());
	for (int i = 0; i < unionVoxels->size(); i++){
		int index = unionVoxels->at(i);
		if (vol1->IsVoxelIn(index)){
			v1 << vol1->GetVoxel(index);
			if (vol2->IsVoxelIn(index)){
				v2 << vol2->GetVoxel(index);
			}
			else{
				v2 << MyVoxContainerf::GetNullVoxelValue();
			}

		}
		else{
			// if vol1 does not have this voxel
			// vol2 must have it
			v1 << MyVoxContainerf::GetNullVoxelValue();
			v2 << vol2->GetVoxel(index);
		}
	}
	delete unionVoxels;

	return ComputeSimilarity(&v1, &v2, method);
}

float MyVolumnSimilarity::ComputeSimilarity(const My3dArrayf* vol1,
	const My3dArrayf* vol2, int method){
	assert(vol1->GetDimSizes() == vol2->GetDimSizes());
	int nVoxels = vol1->GetVolume();
	MyArrayf array1, array2;
	for (int i = 0; i < nVoxels; i++){
		if (vol1->At(i) != MyVoxContainerf::GetNullVoxelValue()){
			array1 << vol1->At(i);
			array2 << vol2->At(i);
		}
		else if (vol2->At(i) != MyVoxContainerf::GetNullVoxelValue()){
			array1 << vol1->At(i);
			array2 << vol2->At(i);
		}
	}
	return ComputeSimilarity(&array1, &array2, method);
}

float MyVolumnSimilarity::ComputeSimilarity(const MyArrayf* array1,
	const MyArrayf* array2, int method){

	// SSIM
	// source1: https://en.wikipedia.org/wiki/Structural_similarity
	// source2: http://mehdi.rabah.free.fr/SSIM/SSIM.cpp
	assert(array1->size() == array2->size());
	int nVoxels = array1->size();
	float c1 = 0.01f*0.01f;
	float c2 = 0.03f*0.03f;
	float mean1 = MyMathHelper::ComputeMean(array1);
	float mean2 = MyMathHelper::ComputeMean(array2);
	float stdev1 = MyMathHelper::ComputeStandardDeviation(array1, mean1);
	float stdev2 = MyMathHelper::ComputeStandardDeviation(array2, mean2);
	float covariance = 0;
	for (int i = 0; i < nVoxels; i++){
		covariance += (array1->at(i) - mean1)*(array2->at(i) - mean2);
	}
	covariance /= nVoxels;
	return (2 * mean1*mean2 + c1)*(2 * covariance + c2) /
		((mean1*mean1 + mean2*mean2 + c1)*(stdev1*stdev1 + stdev2*stdev2 + c2));
}