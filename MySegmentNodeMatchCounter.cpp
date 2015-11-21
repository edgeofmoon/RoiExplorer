#include "MySegmentNodeMatchCounter.h"
#include <stack>
using namespace std;

MySegmentNodeMatchCounter::MySegmentNodeMatchCounter()
{
}


MySegmentNodeMatchCounter::~MySegmentNodeMatchCounter()
{
}

void MySegmentNodeMatchCounter::Update(){
	mNodeMatchCount.clear();
	for (int i = 0; i < mNodeGroups[0].size(); i++){
		const MySegmentNode* iNode = mNodeGroups[0][i];
		for (int j = 0; j < mNodeGroups[1].size(); j++){
			const MySegmentNode* jNode = mNodeGroups[1][j];
			int count = this->CountMatch(iNode, jNode);
			if (count > 0){
				mNodeMatchCount[iNode][jNode] = count;
				mNodeMatchCount[jNode][iNode] = count;
			}
		}
	}
}

int MySegmentNodeMatchCounter::AddGroupByTree(const MySegmentNode* root){
	mNodeGroups.push_back(MyArray<const MySegmentNode*>());
	stack<const MySegmentNode*> stackSegments;
	stackSegments.push(root);
	while (!stackSegments.empty()){
		const MySegmentNode* thisSegment = stackSegments.top();
		stackSegments.pop();
		for (int i = 0; i < thisSegment->GetNumChildren(); i++){
			const MySegmentNode* child = thisSegment->GetChild(i).get();
			stackSegments.push(child);
		}
		mNodeGroups.back() << thisSegment;
	}
	return mNodeGroups.size() - 1;
}

int MySegmentNodeMatchCounter::AddGroupByArray(const MyArray<const MySegmentNode*>* nodeArray){
	mNodeGroups.push_back(MyArray<const MySegmentNode*>());
	mNodeGroups.back().PushBack(nodeArray);
	return mNodeGroups.size() - 1;
}

int MySegmentNodeMatchCounter::CountMatch(const MySegmentNode* seg0, const MySegmentNode* seg1){
	MyVoxContainerfScPtr voxels0 = seg0->GetUniqueVoxes();
	MyVoxContainerfScPtr voxels1 = seg1->GetUniqueVoxes();
	return MyVoxContainerf::CountOverlapping(voxels0.get(), voxels1.get());
}