#include "MySegmentNodeInfo.h"
#include "MyMathHelper.h"
#include <cassert>

MySegmentNodeInfo::MySegmentNodeInfo()
{
	mUseAllDescendents = false;
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
	float segmentSum = 0;
	int numVoxel = 0;
	if (mUseAllDescendents){
		MySegmentNode::const_VoxelIterator itr = mSegNode->VoxelBegin();
		MySegmentNode::const_VoxelIterator itrEnd = mSegNode->VoxelEnd();
		while (itr != itrEnd){
			float voxelValue = vol->At(itr.GetVoxelIndex());
			if (voxelValue != MyVoxContainerf::GetNullVoxelValue()
				&& voxelValue != 0){
				segmentSum += voxelValue;
				numVoxel++;
			}
			itr++;
		}
	}
	else{
		MyVoxContainerf::const_Iterator itr = mSegNode->GetUniqueVoxes()->Begin();
		MyVoxContainerf::const_Iterator itrEnd = mSegNode->GetUniqueVoxes()->End();
		while (itr != itrEnd){
			float voxelValue = vol->At(itr.GetVoxelIndex());
			if (voxelValue != MyVoxContainerf::GetNullVoxelValue()
				&& voxelValue != 0){
				segmentSum += voxelValue;
				numVoxel++;
			}
			itr++;
		}
	}
	//assert(numVoxel == mSegNode->GetNumTotalVoxes());
	return segmentSum / numVoxel;
}

MyVec3f MySegmentNodeInfo::ComputeSegmentMassCenter() const{
	MyVec3f centerSum(0, 0, 0);
	int numVoxels = 0;
	MySegmentNode::const_VoxelIterator itr = mSegNode->VoxelBegin();
	MySegmentNode::const_VoxelIterator itrEnd = mSegNode->VoxelEnd();
	if (mUseAllDescendents){
		while (itr != itrEnd){
			int index = itr.GetVoxelIndex();
			MyVec3i pos = My3dArrayf::ComputePosition(index, mSegNode->GetVolumeSize());
			centerSum[0] += (pos[0] + 0.5);
			centerSum[1] += (pos[1] + 0.5);
			centerSum[2] += (pos[2] + 0.5);
			itr++;
			numVoxels++;
		}
	}
	else{
		MyVoxContainerf::const_Iterator itr = mSegNode->GetUniqueVoxes()->Begin();
		MyVoxContainerf::const_Iterator itrEnd = mSegNode->GetUniqueVoxes()->End();
		while (itr != itrEnd){
			int index = itr.GetVoxelIndex();
			MyVec3i pos = My3dArrayf::ComputePosition(index, mSegNode->GetVolumeSize());
			centerSum[0] += (pos[0] + 0.5);
			centerSum[1] += (pos[1] + 0.5);
			centerSum[2] += (pos[2] + 0.5);
			itr++;
			numVoxels++;
		}
	}
	//return centerSum / mSegNode->GetNumTotalVoxes();
	return centerSum / numVoxels;
}