#include "MySegNodeInfoEncoder.h"


MySegNodeInfoEncoder::MySegNodeInfoEncoder()
{
	mVoxMappedValues = 0;
}


MySegNodeInfoEncoder::~MySegNodeInfoEncoder()
{
}

void MySegNodeInfoEncoder::Update(){
	MyVec3i voxSize(-1, -1, -1);
	MyArrayi voxIndices;

	MyNode::const_TreeIterator treeItr = mSegNodeInfo->GetSegmentNode()->Begin();
	voxSize = static_cast<const MySegmentNode*>(*treeItr)->GetUniqueVoxes()->GetVolumeSize();

	MySegmentNode::const_VoxelIterator itr = mSegNodeInfo->GetSegmentNode()->VoxelBegin();
	MySegmentNode::const_VoxelIterator itrEnd = mSegNodeInfo->GetSegmentNode()->VoxelEnd();
	float segmentSum = 0;
	int numVoxel = 0;
	while (itr != itrEnd){
		voxIndices << itr.GetVoxelIndex();
		itr++;
	}

	float average = mSegNodeInfo->GetSegmentNodeMeanAverage();
	mVoxMappedValues = MyVoxContainerf::MakeVoxContainer(&voxIndices, average, voxSize);
}