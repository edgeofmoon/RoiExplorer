#include "MySegmentNodeMatch.h"
#include <stack>
using namespace std;

// debug use
#include <iostream>
MySegmentNodeMatch::MySegmentNodeMatch()
{
}


MySegmentNodeMatch::~MySegmentNodeMatch()
{
}

int MySegmentNodeMatch::AddGroupByTree(SegmentPtr root){
	mNodeGroups.push_back(MyArray<SegmentPtr>());
	stack<SegmentPtr> stackSegments;
	stackSegments.push(root);
	while (!stackSegments.empty()){
		SegmentPtr thisSegment = stackSegments.top();
		stackSegments.pop();
		for (int i = 0; i < thisSegment->GetNumChildren(); i++){
			SegmentPtr child = thisSegment->GetChild(i).get();
			stackSegments.push(child);
		}
		mNodeGroups.back() << thisSegment;
	}
	return mNodeGroups.size() - 1;
}

int MySegmentNodeMatch::AddGroupByArray(const MyArray<SegmentPtr>* nodeArray){
	mNodeGroups.push_back(MyArray<SegmentPtr>());
	mNodeGroups.back().PushBack(nodeArray);
	return mNodeGroups.size() - 1;
}

const MyArray<SegmentPtr>& MySegmentNodeMatch::GetNeighbors(
	SegmentPtr seg, const MyVec2i& groupID) const{
	return mNodeNeighbors.at(groupID).at(seg);
}

void MySegmentNodeMatch::Update(){
	for (int i = 0; i < mNodeGroups.size(); i++){
		for (int j = i + 1; j < mNodeGroups.size(); j++){
			Update(MyVec2i(i, j));
		}
	}
}

void MySegmentNodeMatch::Update(const MyVec2i& groupID){
	MyVec2i revGroupId(groupID[1], groupID[0]);
	mNodeNeighbors[groupID].clear();
	mNodeNeighbors[revGroupId].clear();
	const MyArray<SegmentPtr>& group0 = mNodeGroups[groupID[0]];
	const MyArray<SegmentPtr>& group1 = mNodeGroups[groupID[1]];
	for (int i = 0; i < group0.size(); i++){
		SegmentPtr iNode = group0[i];
		cout << i << "th ROI match: ";
		for (int j = 0; j < group1.size(); j++){
			SegmentPtr jNode = group1[j];
			if (MySegmentNodeMatch::IsMatched(iNode, jNode)){
				mNodeNeighbors[groupID][iNode] << jNode;
				mNodeNeighbors[revGroupId][jNode] << iNode;
				cout << j << ", ";
			}
		}
		cout << endl;
	}
}

bool MySegmentNodeMatch::IsMatched(SegmentPtr seg0, SegmentPtr seg1){
	MyVoxContainerfScPtr voxels0 = seg0->GetUniqueVoxes();
	MyVoxContainerfScPtr voxels1 = seg1->GetUniqueVoxes();
	return MyVoxContainerf::IsOverlapping(voxels0.get(), voxels1.get());
}