#pragma once

#include "MyArrayMD.h"
#include "MyNode.h"
#include "MySegmentNode.h"
#include "MySharedPointer.h"
#include "MyComponent.h"
#include "MyComponentFilter.h"


class MyJoinTree
{
public:
	MyJoinTree();
	~MyJoinTree();

	void Update();
	void SetVolumn(My3dArrayfScPtr vol){ mVolumn = vol; };
	void SetComponentFilter(MyComponentFilterSPtr cFilter){
		mFilter = cFilter;
	}
	My3dArrayfScPtr GetVolumn() const { return mVolumn; };
	MySegmentNodeSPtr GetRoot(){ return mJoinRoot; };
	MySegmentNodeScPtr GetRoot() const { return mJoinRoot; };

protected:
	My3dArrayfScPtr mVolumn;
	MySegmentNodeSPtr mJoinRoot;
	MyComponentFilterSPtr mFilter;

	void QueueJoinNeighbors(const MyVec3i& pos, MyArray3i& joinNeighbors);
	MySegmentNodeSPtr MakeSegment(MyComponent* component);
	MySegmentNodeSPtr MakeJoinTree(MyComponent* joinRoot);

	void CheckComponentsStatus(const MyComponent* joinRoot) const;
	void CheckJoinTreeStatus(const MySegmentNode* joinRoot) const;

	// ultilities
	class VoxelSortLess{
	public:
		bool operator()(int index1, int index2);
		bool operator()(const MyVec3i& pos1, const MyVec3i& pos2);
		My3dArrayfScPtr mVolumn;
	};
};

typedef MySharedPointer<MyJoinTree> MyJoinTreeSPtr;
typedef MySharedPointer<const MyJoinTree> MyJoinTreeScPtr;