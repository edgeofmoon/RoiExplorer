#pragma once

#include "MyJoinTree.h"
#include "MyMap.h"
#include "MyArray.h"
#include "MySharedPointer.h"
#include "MyOrderedList.h"

class SegmentLayoutInfo{
public:
	MyVec2f range;
	float width;
	float xMassCenter;
	// must be computed in post-traversal order
	const MySegmentNode* highest;
	// debug use
	int index;

};

class MyJoinTreeLayout
{
public:
	MyJoinTreeLayout();
	~MyJoinTreeLayout();

	void SetJoinTreeRoot(MySegmentNodeScPtr root);
	MySegmentNodeScPtr GetJoinTreeRoot() const{
		return mJoinTreeRoot;
	}
	void Update();
	const MyMap<const MySegmentNode*, MyBox2f>& GetPositions() const {
		return mPositions;
	}
	const MyBox2f& GetPosition(const MySegmentNode* segment) const {
		return mPositions.at(segment);
	};
	int GetIndex(const MySegmentNode* segment) const{
		return mLayoutInfo.at(segment).index;
	}
	const MyBox2f& GetTreeBox() const { return mTreeBox; };

	const MyMap<const MySegmentNode*, SegmentLayoutInfo>& GetLayoutInfo() const{
		return mLayoutInfo;
	}

protected:

	float mXBoarder;
	MyBox2f mTreeBox;
	MySegmentNodeScPtr mJoinTreeRoot;
	MyMap<const MySegmentNode*, MyBox2f> mPositions;
	void NormalizePositions();

	// layout info for each segment
	// tmp info
	MyMap<const MySegmentNode*, SegmentLayoutInfo> mLayoutInfo;

	SegmentLayoutInfo ComputeLayoutInfo(const MySegmentNode* segment) const;
	MyVec2f ComputeRange(const MySegmentNode* segment) const;
	float ComputeWidth(const MySegmentNode* segment) const;
	float ComputeXCenter(const MySegmentNode* segment) const;
	MyBox2f ComputePathBox(const MyArray<const MySegmentNode*>* path) const;

	void LayoutSubTree(const MySegmentNode* root);
	// side: -1=left, 0=middle, 1=right
	void PutPathInBox(const MyArray<const MySegmentNode*>* path, 
		const MyBox2f& box, int side);
	void FindBranchInOrder(MyArraySPtr<const MySegmentNode*> centerPath,
		MyArraySPtr<MyArraySPtr<const MySegmentNode*>> rst) const;
	MyArraySPtr<MyArraySPtr<const MySegmentNode*>> FindAllBranchInOrder(
		MyArraySPtr<const MySegmentNode*> centerPath) const;
	MyArraySPtr<const MySegmentNode*> FindAllBranchRootInOrder(
		const MyArray<const MySegmentNode*>* path) const;
	float GetPathWidth(const MyArray<const MySegmentNode*>* path) const;
	MyArraySPtr<const MySegmentNode*> FindCenterPath(
		const MySegmentNode* root) const;
	MyBox2f PushUpperTetris(const MyBox2f& box, MyArray2f& base);

	class SegmentLower{
	public:
		bool operator()(const MySegmentNode* seg1, const MySegmentNode* seg2) const;
		const MyMap<const MySegmentNode*, SegmentLayoutInfo>* mLayoutInfo;
	};

	class VecRighter{
	public:
		bool operator()(const MyVec2f& vec0, const MyVec2f& vec1) const;
	};
	MyBox2f PushUpperTetris(const MyBox2f& box, MyOrderedList<MyVec2f, VecRighter>& base);
};

typedef MySharedPointer<MyJoinTreeLayout> MyJoinTreeLayoutSPtr;
typedef MySharedPointer<const MyJoinTreeLayout> MyJoinTreeLayoutScPtr;