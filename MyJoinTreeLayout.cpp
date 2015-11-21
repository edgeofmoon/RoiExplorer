#include "MyJoinTreeLayout.h"
#include <utility>
#include <list>
#include <stack>
#include "ColorScaleTable.h"
using namespace std;

// for debug
#include <iostream>
#include <cassert>
using namespace std;

#define FLOAT_POSITIVE_LIMIT 1e36
#define FLOAT_NEGATIVE_LIMIT -1e36
MyJoinTreeLayout::MyJoinTreeLayout()
{
	mXBoarder = 0.05;
	mTreeBox.Set(MyVec2f(FLOAT_POSITIVE_LIMIT, FLOAT_POSITIVE_LIMIT),
		MyVec2f(FLOAT_NEGATIVE_LIMIT, FLOAT_NEGATIVE_LIMIT));
}


MyJoinTreeLayout::~MyJoinTreeLayout()
{
	int debug = 1;
}

void MyJoinTreeLayout::SetJoinTreeRoot(MySegmentNodeScPtr root){
	mJoinTreeRoot = root;
}

void MyJoinTreeLayout::Update(){
	mPositions.clear();
	mTreeBox.Set(MyVec2f(FLOAT_POSITIVE_LIMIT, FLOAT_POSITIVE_LIMIT),
		MyVec2f(FLOAT_NEGATIVE_LIMIT, FLOAT_NEGATIVE_LIMIT));
	
	/*
	MyNode::const_TreeIterator itr = mJoinTreeRoot->Begin();
	MyNode::const_TreeIterator itrEnd = mJoinTreeRoot->End();
	while (itr != itrEnd){
		const MySegmentNode* segmentNode 
			= static_cast<const MySegmentNode*>(*itr);
		mLayoutInfo[segmentNode] = this->ComputeLayoutInfo(segmentNode);
		itr++;
	}
	*/
	// must use post-order traversal
	stack<const MySegmentNode*> stackSegment;
	stackSegment.push(mJoinTreeRoot.get());
	const MySegmentNode* prev = 0;
	const MySegmentNode* curr = 0;
	while (!stackSegment.empty()) {
		const MySegmentNode* prev = stackSegment.top();
		if (prev->GetNumChildren() == 0){
			// process leaves
			mLayoutInfo[prev] = this->ComputeLayoutInfo(prev);
			curr = prev;
			stackSegment.pop();
		}
		else if (MyNode::IsConnected(prev, curr)){
			// process nodes if its children are processed
			mLayoutInfo[prev] = this->ComputeLayoutInfo(prev);
			curr = prev;
			stackSegment.pop();
		}
		else{
			// if children are not processed yet
			for (int i = 0; i < prev->GetNumChildren(); i++){
				curr = prev->GetChild(i).get();
				stackSegment.push(curr);
			}
		}
	}

	// lets do the layout thingy now!!!
	this->LayoutSubTree(mJoinTreeRoot.get());
	//mLayoutInfo.clear();
	NormalizePositions();
}

void MyJoinTreeLayout::NormalizePositions(){
	MyMap<const MySegmentNode*, MyBox2f>::iterator itr = mPositions.begin();
	float width = mTreeBox.GetSize(0);
	float height = mTreeBox.GetSize(1);
	MyVec2f scale(1 / width, 1 / height);
	MyVec2f offset = mTreeBox.GetLowPos();
	offset[1] = 0;
	mTreeBox -= offset;
	mTreeBox.Scale(scale);
	while (itr != mPositions.end()){
		itr->second -= offset;
		itr->second.Scale(scale);
		itr++;
	}
}

SegmentLayoutInfo MyJoinTreeLayout::ComputeLayoutInfo(const MySegmentNode* segment) const{
	SegmentLayoutInfo info;
	info.range = ComputeRange(segment);
	info.width = ComputeWidth(segment);
	info.xMassCenter = ComputeXCenter(segment);
	info.highest = segment;
	
	MyVec2f tmpHighestRange = info.range;
	for (int i = 0; i < segment->GetNumChildren(); i++){
		const MySegmentNode* child = segment->GetChild(i).get();
		const MySegmentNode* childHighest = mLayoutInfo.at(child).highest;
		MyVec2f childHighestRange = mLayoutInfo.at(childHighest).range;
		if (childHighestRange[1]>tmpHighestRange[1]){
			info.highest = childHighest;
			tmpHighestRange = childHighestRange;
		}
	}
	return info;
}

MyVec2f MyJoinTreeLayout::ComputeRange(const MySegmentNode* segment) const{
	MyVoxContainerfScPtr voxels = segment->GetUniqueVoxes();
	MyVoxContainerf::const_Iterator itr = voxels->Begin();
	MyVoxContainerf::const_Iterator itrEnd = voxels->End();
	MyVec2f range(1, 0);
	while (itr != itrEnd){
		float value = *itr;
		if (value < range[0]) range[0] = value;
		if (value > range[1]) range[1] = value;
		itr++;
	}
	return range;
}

float MyJoinTreeLayout::ComputeWidth(const MySegmentNode* segment) const{
	//MyVec2f range = mLayoutInfo.at(segment).range;
	//MyVec2f range = ComputeRange(segment);
	//float rangeDiff = range[1] - range[0];
	//if (rangeDiff == 0) rangeDiff = 0.1;
	//return log(segment->GetUniqueVoxes()->GetNumVoxes() + 1) / rangeDiff;
	return log(segment->GetUniqueVoxes()->GetNumVoxes() + 1);
}

float MyJoinTreeLayout::ComputeXCenter(const MySegmentNode* segment)const{
	MyVoxContainerfScPtr voxels = segment->GetUniqueVoxes();
	MyVoxContainerf::const_Iterator itr = voxels->Begin();
	MyVoxContainerf::const_Iterator itrEnd = voxels->End();
	float xCenter = 0;
	float xCenterWhole = voxels->GetVolumeSize()[0] / 2;
	int numVoxels = 0;
	while (itr != itrEnd){
		MyVec3i pos = My3dArrayf::ComputePosition(itr.GetVoxelIndex(), voxels->GetVolumeSize());
		xCenter += pos[0];
		itr++;
		numVoxels++;
	}
	return xCenter / numVoxels - xCenterWhole;
}

MyBox2f MyJoinTreeLayout::ComputePathBox(const MyArray<const MySegmentNode*>* path) const{
	float width = this->GetPathWidth(path);
	MyVec2f range0 = mLayoutInfo.at(path->front()).range;
	MyVec2f range1 = mLayoutInfo.at(path->back()).range;

	// must also consider parent range
	if (path->front()->GetNumParents() != 0){
		const MySegmentNode* pSeg = path->front()->GetParent().get();
		MyVec2f pRange = mLayoutInfo.at(pSeg).range;
		range0[0] = min(range0[0], pRange[1]);
	}
	MyVec2f range(min(range0[0], range1[0]), max(range0[1], range1[1]));
	return MyBox2f(MyVec2f(0, range[0]), MyVec2f(width, range[1]));
}

void MyJoinTreeLayout::LayoutSubTree(const MySegmentNode* root){
	MyMap<const MySegmentNode*, float> xCenter;

	if (root->GetNumChildren() == 0){
		xCenter[root] = 0;
		return;
	}
	MyArraySPtr<const MySegmentNode*> centerPath = this->FindCenterPath(root);
	float centerPathWidth = this->GetPathWidth(centerPath.get());
	//MyArray2f leftTetris(1, MyVec2f(centerPathWidth / 2, 0));
	//MyArray2f rightTetris(1, MyVec2f(centerPathWidth / 2, 0));
	MyOrderedList<MyVec2f, VecRighter> leftTetris;
	leftTetris.Insert(MyVec2f(centerPathWidth / 2, 0));
	MyOrderedList<MyVec2f, VecRighter> rightTetris;
	rightTetris.Insert(MyVec2f(centerPathWidth / 2, 0));
	MyArraySPtr<MyArraySPtr<const MySegmentNode*>> branchOrdered
		= this->FindAllBranchInOrder(centerPath);
	int centerPathIndex = branchOrdered->IndexOf(centerPath);
	// left path
	for (int i = centerPathIndex - 1; i >= 0; i--){
		MyArray<const MySegmentNode*>* path = branchOrdered->at(i).get();
		MyBox2f pathBox = this->ComputePathBox(path);
		MyBox2f pathBoxPos = this->PushUpperTetris(pathBox, leftTetris);
		// mirror pathBoxPos for left
		pathBoxPos.Set(MyVec2f(-pathBoxPos.GetHighPos()[0], pathBoxPos.GetLowPos()[1]),
			MyVec2f(-pathBoxPos.GetLowPos()[0], pathBoxPos.GetHighPos()[1]));
		this->PutPathInBox(path, pathBoxPos, 1);
	}
	// center path
	MyBox2f centerPathBox(MyVec2f(-centerPathWidth / 2, 0), MyVec2f(centerPathWidth / 2, 1));
	this->PutPathInBox(centerPath.get(), centerPathBox, 0);
	// right path
	for (int i = centerPathIndex + 1; i < branchOrdered->size(); i++){
		MyArray<const MySegmentNode*>* path = branchOrdered->at(i).get();
		MyBox2f pathBox = this->ComputePathBox(path);
		MyBox2f pathBoxPos = this->PushUpperTetris(pathBox, rightTetris);
		this->PutPathInBox(path, pathBoxPos, -1);
	}
}

float MyJoinTreeLayout::GetPathWidth(const MyArray<const MySegmentNode*>* path) const{
	float width = -1;
	for (int i = 0; i < path->size(); i++){
		float thisWidth = mLayoutInfo.at(path->at(i)).width;
		if (thisWidth>width) width = thisWidth;
	}
	return width;
}

int segIndex = 0;
// side: -1=left, 0=middle, 1=right
void MyJoinTreeLayout::PutPathInBox(const MyArray<const MySegmentNode*>* path, const MyBox2f& box, int side){
	for (int i = 0; i < path->size(); i++){
		const MySegmentNode* segment = path->at(i);
		SegmentLayoutInfo info = mLayoutInfo.at(segment);
		MyVec2f range = info.range;
		float width = info.width;
		MyBox2f boxPos(MyVec2f(0, range[0]), MyVec2f(width, range[1]));
		switch (side)
		{
		case -1:
			boxPos += MyVec2f(box.GetLowPos()[0], 0);
			break;
		case 1:
			boxPos += MyVec2f(box.GetHighPos()[0]-width, 0);
			break;
		case 0:
		default:
			boxPos += MyVec2f((box.GetHighPos()[0] + box.GetLowPos()[0] - width) / 2, 0);
			break;
		}
		mPositions[segment] = boxPos; 
		mLayoutInfo[segment].index = segIndex++;
	}
	mTreeBox.Engulf(box);
}


void MyJoinTreeLayout::FindBranchInOrder(MyArraySPtr<const MySegmentNode*> centerPath,
	MyArraySPtr<MyArraySPtr<const MySegmentNode*>> rst) const{
	MyArraySPtr<const MySegmentNode*> branchRoots = FindAllBranchRootInOrder(centerPath.get());
	MyArray<MyArraySPtr<const MySegmentNode*>> branches;
	for (int i = 0; i < branchRoots->size(); i++){
		const MySegmentNode* branchRoot = branchRoots->at(i);
		MyArraySPtr<const MySegmentNode*> branch = FindCenterPath(branchRoot);
		branches << branch;
	}

	for (int i = branches.size()-1; i >=0; i--){
		if (mLayoutInfo.at(branches[i]->front()).xMassCenter <= 0){
			FindBranchInOrder(branches[i], rst);
		}
	}
	rst->PushBack(centerPath);
	for (int i = 0; i < branches.size(); i++){
		if (mLayoutInfo.at(branches[i]->front()).xMassCenter > 0){
			FindBranchInOrder(branches[i], rst);
		}
	}
	int numBranches = rst->size();
	if (numBranches % 1000 == 0){
		cout << "Number branch found: " << numBranches << "\r";
	}
}

// go back to recursive for now
MyArraySPtr<MyArraySPtr<const MySegmentNode*>> MyJoinTreeLayout::FindAllBranchInOrder(
	MyArraySPtr<const MySegmentNode*> centerPath) const{
	MyArraySPtr<MyArraySPtr<const MySegmentNode*>> rst
		= make_shared<MyArray<MyArraySPtr<const MySegmentNode*>>>();
	FindBranchInOrder(centerPath, rst);
	cout << "Number branch found: " << rst->size() << "\n";
	return rst;
}

/*
// iterative code 
// not working as intended
MyArraySPtr<MyArraySPtr<const MySegmentNode*>> MyJoinTreeLayout::FindAllBranchInOrder(
	MyArraySPtr<const MySegmentNode*> centerPath) const{
	MyArraySPtr<MyArraySPtr<const MySegmentNode*>> rst
		= make_shared<MyArray<MyArraySPtr<const MySegmentNode*>>>();
	// must use in-order traversal
	stack<MyArraySPtr<const MySegmentNode*>> pathStack;
	stack<MyArraySPtr<const MySegmentNode*>> parentPathStack;
	MyArraySPtr<const MySegmentNode*> current = centerPath;
	while (!pathStack.empty() || current){
		if (current){
			pathStack.push(current);
			parentPathStack.push(current);
			// stack left branches
			MyArraySPtr<const MySegmentNode*> branchRoots = FindAllBranchRootInOrder(current.get());
			// set to 0 to detect if left children exhausted
			current = 0;
			for (int i = 0; i < branchRoots->size(); i++){
				const MySegmentNode* branchRoot = branchRoots->at(i);
				if (mLayoutInfo.at(branchRoot).xMassCenter <= 0){
					current = FindCenterPath(branchRoot);
					pathStack.push(current);
				}
			}
		}
		else{
			current = pathStack.top();
			pathStack.pop();
			// consume it
			rst->PushBack(current);
			if (!parentPathStack.empty()){
				// stack right branches only if all left branches and center branch are consumed
				if (current == parentPathStack.top()){
					parentPathStack.pop();
					MyArraySPtr<const MySegmentNode*> branchRoots = FindAllBranchRootInOrder(current.get());
					// set to 0 to detect if right children exhausted
					current = 0;
					for (int i = 0; i < branchRoots->size(); i++){
						const MySegmentNode* branchRoot = branchRoots->at(i);
						if (mLayoutInfo.at(branchRoot).xMassCenter > 0){
							current = FindCenterPath(branchRoot);
							pathStack.push(current);
						}
					}
				}
			}
		}

		int numBranches = rst->size();
		if (numBranches % 1000 == 0){
			cout << "Number branch found: " << numBranches << "\r";
		}
	}
	cout << "Number branch found: " << rst->size() << "\n";
	return rst;
}
*/

MyArraySPtr<const MySegmentNode*> MyJoinTreeLayout::FindAllBranchRootInOrder(
	const MyArray<const MySegmentNode*>* path) const{
	MyArraySPtr<const MySegmentNode*> rst = make_shared<MyArray<const MySegmentNode*>>();
	// since it the root at the begining is the lowest
	// we should search in reverse order
	const MySegmentNode* prev = 0;
	const MySegmentNode* curr = 0;
	SegmentLower segLower;
	segLower.mLayoutInfo = &mLayoutInfo;
	for (int i = path->size() - 2; i >= 0; i--){
		prev = path->at(i + 1);
		curr = path->at(i);
		MyArray<const MySegmentNode*> neighbors;
		for (int j = 0; j < curr->GetNumChildren(); j++){
			const MySegmentNode* neighbor = curr->GetChild(j).get();
			if (neighbor != prev){
				neighbors << neighbor;
			}
		}
		// highest segment has priority if more braches exists
		if (neighbors.size()>1){
			sort(neighbors.begin(), neighbors.end(), segLower);
		}
		for (int j = neighbors.size() - 1; j >= 0; j--){
			rst->PushBack(neighbors[j]);
		}
	}
	return rst;
}

MyArraySPtr<const MySegmentNode*> MyJoinTreeLayout::FindCenterPath(
	const MySegmentNode* root) const{
	const MySegmentNode* highest = mLayoutInfo.at(root).highest;
	MyArraySPtr<const MySegmentNode*> path = make_shared<MyArray<const MySegmentNode*>>();
	const MySegmentNode* current = highest;
	path->PushBack(current);
	while (path->back() != root){
		// only has 1 parent
		current = current->GetParent().get();
		path->PushBack(current);
	}
	reverse(path->begin(), path->end());
	return path;
}

MyBox2f MyJoinTreeLayout::PushUpperTetris(const MyBox2f& box, MyArray2f& base){
	// this function does not do what I expected, why???
	float width = box.GetSize(0);
	float bottom = box.GetLowPos()[1];
	float top = box.GetHighPos()[1];
	float xBoarder = mXBoarder*width;
	assert(base.size() != 0);
	float xPos = base.front()[0];
	for (int i = base.size() - 1; i >= 0; i--){
		if (base[i][0] <= xPos) continue;
		if (base[i][1] > top){
			base.push_back(MyVec2f(xPos + width + xBoarder, bottom));
			return MyBox2f(MyVec2f(xPos + xBoarder, bottom), MyVec2f(xPos + width + xBoarder, top));
		}
		xPos = max(xPos, base[i][0]);
	}
	base.push_back(MyVec2f(xPos + width + xBoarder, bottom));
	return MyBox2f(MyVec2f(xPos + xBoarder, bottom), MyVec2f(xPos + width + xBoarder, top));
}

bool MyJoinTreeLayout::SegmentLower::operator()(
	const MySegmentNode* seg1, const MySegmentNode* seg2) const{
	return mLayoutInfo->at(mLayoutInfo->at(seg1).highest).range[1]
		< mLayoutInfo->at(mLayoutInfo->at(seg2).highest).range[1];
}

bool MyJoinTreeLayout::VecRighter::operator()(const MyVec2f& vec0, const MyVec2f& vec1) const{
	return vec0[0] > vec1[0];
}

MyBox2f MyJoinTreeLayout::PushUpperTetris(const MyBox2f& box, 
	MyOrderedList<MyVec2f, VecRighter>& base){
	float width = box.GetSize(0);
	float bottom = box.GetLowPos()[1];
	float top = box.GetHighPos()[1];
	float xBoarder = mXBoarder*width;
	assert(base.size() != 0);
	float xPos = (*base.begin())[0];
	MyOrderedList<MyVec2f, VecRighter>::iterator itr = base.begin();
	while (itr != base.end()){
		MyVec2f block = *itr;
		xPos = block[0];
		if (block[1] < top) {
			break;
		}
		itr++;
	}
	base.Insert(MyVec2f(xPos + width + xBoarder, bottom));
	return MyBox2f(MyVec2f(xPos + xBoarder, bottom), MyVec2f(xPos + width + xBoarder, top));
}