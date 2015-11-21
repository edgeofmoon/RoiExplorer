#include "MySegmentNodeInfo.h"
#include "MyMathHelper.h"
#include <cassert>

MySegmentNodeInfo::MySegmentNodeInfo()
{
}


MySegmentNodeInfo::~MySegmentNodeInfo()
{
}

void MySegmentNodeInfo::Update(){
	mSegNodeMeans.clear();
	mSegNodeMeans.reserve(mVolumes->size());
	for (int i = 0; i < mVolumes->size(); i++){
		float thisMean = this->ComputeSegmentAverage(i);
		mSegNodeMeans << thisMean;
	}
	mSegNodeMeanAverage = MyMathHelper::ComputeMean(&mSegNodeMeans);
	mSegNodeMeanStdev = MyMathHelper::ComputeStandardDeviation(&mSegNodeMeans, mSegNodeMeanAverage);
	mSegmentMassCenter = this->ComputeSegmentMassCenter();
	mRange = MyMathHelper::ComputeRange(&mSegNodeMeans);
}

float MySegmentNodeInfo::ComputeSegmentAverage(int volIdx) const{
	My3dArrayfScPtr vol = mVolumes->at(volIdx);
	MySegmentNode::const_VoxelIterator itr = mSegNode->VoxelBegin();
	MySegmentNode::const_VoxelIterator itrEnd = mSegNode->VoxelEnd();
	float segmentSum = 0;
	int numVoxel = 0;
	while (itr != itrEnd){
		float voxelValue = vol->At(itr.GetVoxelIndex());
		if (voxelValue != MyVoxContainerf::GetNullVoxelValue()
			&& voxelValue != 0){
			segmentSum += voxelValue;
			numVoxel++;
		}
		itr++;
	}
	//assert(numVoxel == mSegNode->GetNumTotalVoxes());
	return segmentSum / numVoxel;
}

MyVec3f MySegmentNodeInfo::ComputeSegmentMassCenter() const{
	MySegmentNode::const_VoxelIterator itr = mSegNode->VoxelBegin();
	MySegmentNode::const_VoxelIterator itrEnd = mSegNode->VoxelEnd();
	MyVec3f centerSum(0, 0, 0);
	int numVoxels = 0;
	while (itr != itrEnd){
		int index = itr.GetVoxelIndex();
		MyVec3i pos = My3dArrayf::ComputePosition(index, mSegNode->GetVolumeSize());
		centerSum[0] += (pos[0]+0.5);
		centerSum[1] += (pos[1]+0.5);
		centerSum[2] += (pos[2]+0.5);
		itr++;
		numVoxels++;
	}
	//return centerSum / mSegNode->GetNumTotalVoxes();
	return centerSum / numVoxels;
}