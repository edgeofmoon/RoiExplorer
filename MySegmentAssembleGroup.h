#pragma once
#include "MyArray.h"
#include "MySegNodeInfoAssemble.h"
#include "MyMap.h"
#include "MySharedPointer.h"

class MySegmentAssembleGroup :
	public MyArray <MySegNodeInfoAssembleSPtr>
{
public:
	MySegmentAssembleGroup();
	~MySegmentAssembleGroup();

	void Update();
	const MyMap<const MySegmentNode*, float>& GetTScores() const {
		return mSegmentTScores;
	}

	MyMapScPtr<const MySegmentNode*, MyVec4f> GetRoiColors() const{
		return mRoiColors;
	};

	const MyVec2f& GetStdevRange() const { return mStdevRange; };
	const MyMap<const MySegmentNode*, MyVec2f>& GetStdevRanges() const { 
		return mStdevRanges; 
	};
	const MyVec2f& GetMeanRange() const { return mMeanRange; };
	const MyVec2f& GetThreeSigmaRange() const { return mThreeSigmaRange; };
	const MyVec2f& GetTScoreRange() const { return mTScoreRange; };

	void AddSegmentNode(MySegmentNodeSPtr segNode);
	void RemoveSegmentNode(const MySegmentNode* segNode);

protected:

	MyMap<const MySegmentNode*, float> mSegmentTScores;
	MyMapSPtr<const MySegmentNode*, MyVec4f> mRoiColors;
	MyMap<const MySegmentNode*, MyVec2f> mStdevRanges;

	// temp solution
	MyVec2f mStdevRange;
	MyVec2f mMeanRange;
	MyVec2f mThreeSigmaRange;
	MyVec2f mTScoreRange;
	void UpdateRoiColors();
};

typedef MySharedPointer<MySegmentAssembleGroup> MySegmentAssembleGroupSPtr;
typedef MySharedPointer<const MySegmentAssembleGroup> MySegmentAssembleGroupScPtr;