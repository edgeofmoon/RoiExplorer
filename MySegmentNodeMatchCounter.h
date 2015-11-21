#pragma once

#include "MySegmentNode.h"
#include "MyArray.h"
#include "MyMap.h"
#include "MySharedPointer.h"

typedef MyMap<const MySegmentNode*, int> MatchCount;

class MySegmentNodeMatchCounter
{
public:
	MySegmentNodeMatchCounter();
	~MySegmentNodeMatchCounter();

	void Update();

	int AddGroupByTree(const MySegmentNode* root);
	int AddGroupByArray(const MyArray<const MySegmentNode*>* nodeArray);

	const MyArray<MyArray<const MySegmentNode*>>& GetNodeGroups() const {
		return mNodeGroups;
	}

	const MyMap<const MySegmentNode*, MatchCount>& GetNodeMatchCount() const{
		return mNodeMatchCount;
	}

	const MatchCount& GetMatchCount(const MySegmentNode* node) const { 
		return mNodeMatchCount.at(node);
	}

protected:
	MyArray<MyArray<const MySegmentNode*>> mNodeGroups;

	MyMap<const MySegmentNode*, MatchCount> mNodeMatchCount;

	static int CountMatch(const MySegmentNode* seg0, const MySegmentNode* seg1);
};

typedef MySharedPointer<MySegmentNodeMatchCounter> MySegmentNodeMatchCounterSPtr;
typedef MySharedPointer<const MySegmentNodeMatchCounter> MySegmentNodeMatchCounterScPtr;
