#pragma once

#include "MySegmentNode.h"
#include "MyArray.h"
#include "MyMap.h"

typedef const MySegmentNode* SegmentPtr;
//typedef MySegmentNodeScPtr SegmentPtr;

class MySegmentNodeMatch
{
public:
	MySegmentNodeMatch();
	~MySegmentNodeMatch();

	void Update();
	// may update parts only
	void Update(const MyVec2i& groupID);
	// return the group index
	int AddGroupByTree(SegmentPtr root);
	int AddGroupByArray(const MyArray<SegmentPtr>* nodeArray);

	const MyArray<SegmentPtr>& GetNeighbors(SegmentPtr seg, const MyVec2i& groupID) const;

protected:
	MyArray<MyArray<SegmentPtr>> mNodeGroups;

	typedef MyMap<SegmentPtr, MyArray<SegmentPtr>> NodeGroupNeighbors;

	MyMap<MyVec2i, NodeGroupNeighbors> mNodeNeighbors;

	static bool IsMatched(SegmentPtr seg0, SegmentPtr seg1);
};

