#include "MySegmentNodeMatchCounter.h"
#include <stack>

// debug
#include <iostream>
using namespace std;

MySegmentNodeMatchCounter::MySegmentNodeMatchCounter()
{
	mNodeGroups.resize(2);
}


MySegmentNodeMatchCounter::~MySegmentNodeMatchCounter()
{
}

void MySegmentNodeMatchCounter::Update(){
	mNodeMatchCount.clear();
	for (int i = 0; i < mNodeGroups[0].size(); i++){
		const MySegmentNode* iNode = mNodeGroups[0][i];
		bool atLeastOneMatch = false;
		for (int j = 0; j < mNodeGroups[1].size(); j++){
			const MySegmentNode* jNode = mNodeGroups[1][j];
			int count = this->CountMatch(iNode, jNode);
			if (count > 0){
				mNodeMatchCount[iNode][jNode] = count;
				mNodeMatchCount[jNode][iNode] = count;
				atLeastOneMatch = true;
			}
		}
		if (!atLeastOneMatch){
			// not match, put an empty count
			mNodeMatchCount[iNode] = MatchCount();
		}
	}
}

void MySegmentNodeMatchCounter::AddToGroupAndUpdate(
	const MySegmentNode* node, int groupIdx){
	if (mNodeGroups[groupIdx].HasOne(node)){
		cout << "Already in match list, ignore.\n";
		return;
	}
	mNodeGroups[groupIdx] << node;
	int otherGroup = 1 - groupIdx;
	bool atLeastOneMatch = false;
	for (int i = 0; i < mNodeGroups[otherGroup].size(); i++){
		const MySegmentNode* otherNode = mNodeGroups[otherGroup][i];
		int count = this->CountMatch(node, otherNode);
		if (count > 0){
			mNodeMatchCount[node][otherNode] = count;
			mNodeMatchCount[otherNode][node] = count;
			atLeastOneMatch = true;
		}
	}
	if (!atLeastOneMatch){
		// not match, put an empty count
		mNodeMatchCount[node] = MatchCount();
	}
}

void MySegmentNodeMatchCounter::RemoveSegment(const MySegmentNode* node, int groupIdx){
	mNodeGroups[groupIdx].EraseOne(node);
	mNodeMatchCount.erase(node);
	MyMap<const MySegmentNode*, MatchCount>::iterator itr;
	for (itr = mNodeMatchCount.begin(); itr != mNodeMatchCount.end(); itr++){
		itr->second.erase(node);
	}
}

void MySegmentNodeMatchCounter::SetGroupByTree(
	const MySegmentNode* root, int groupIdx){
	mNodeGroups[groupIdx].clear();
	stack<const MySegmentNode*> stackSegments;
	stackSegments.push(root);
	while (!stackSegments.empty()){
		const MySegmentNode* thisSegment = stackSegments.top();
		stackSegments.pop();
		for (int i = 0; i < thisSegment->GetNumChildren(); i++){
			const MySegmentNode* child = thisSegment->GetChild(i).get();
			stackSegments.push(child);
		}
		mNodeGroups[groupIdx] << thisSegment;
	}
}

void MySegmentNodeMatchCounter::SetGroupByArray(
	const MyArray<const MySegmentNode*>* nodeArray, int groupIdx){
	mNodeGroups[groupIdx].clear();
	mNodeGroups[groupIdx].PushBack(nodeArray);
}

void MySegmentNodeMatchCounter::SetGroupByArray(
	const MyArray<MySegmentNodeSPtr>* nodeArray, int groupIdx){
	mNodeGroups[groupIdx].clear();
	for (int i = 0; i < nodeArray->size(); i++){
		mNodeGroups[groupIdx].PushBack(nodeArray->at(i).get());
	}
}

int MySegmentNodeMatchCounter::CountMatch(
	const MySegmentNode* seg0, const MySegmentNode* seg1){
	MyVoxContainerfScPtr voxels0 = seg0->GetUniqueVoxes();
	MyVoxContainerfScPtr voxels1 = seg1->GetUniqueVoxes();
	return MyVoxContainerf::CountOverlapping(voxels0.get(), voxels1.get());
}