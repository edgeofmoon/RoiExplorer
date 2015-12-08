#include "MySegmentAssembleGroup.h"
#include "ColorScaleTable.h"
#include <cassert>

// debug
#include <iostream>
using namespace std;

MySegmentAssembleGroup::MySegmentAssembleGroup()
{
	mRoiColors = std::make_shared<MyMap<const MySegmentNode*, MyVec4f>>();
}


MySegmentAssembleGroup::~MySegmentAssembleGroup()
{
}

void MySegmentAssembleGroup::Update(){
	for (int i = 0; i < this->size(); i++){
		this->at(i)->Update();
	}
	mSegmentTScores.clear();
	mStdevRanges.clear();
	mStdevRange = MyVec2f(100, 0);
	mMeanRange = MyVec2f(100, 0);
	mThreeSigmaRange = MyVec2f(100, 0);
	mTScoreRange = MyVec2f(FLT_MAX, 0);
	if (size() < 2) return;
	for (int i = 0; i < front()->GetSegmentNodeInfos()->size(); i++){
		const MySegmentNodeInfo* nodeInfo0 = at(0)->GetSegmentNodeInfos()->at(i).get();
		const MySegmentNodeInfo* nodeInfo1 = at(1)->GetSegmentNodeInfos()->at(i).get();
		assert(nodeInfo0->GetSegmentNode().get() == nodeInfo1->GetSegmentNode().get());
		float m0 = nodeInfo0->GetSegmentNodeMeanAverage();
		float m1 = nodeInfo1->GetSegmentNodeMeanAverage();
		float s0 = nodeInfo0->GetSegmentNodeMeanStdev();
		float s1 = nodeInfo1->GetSegmentNodeMeanStdev();
		float n0 = nodeInfo0->GetVolumes()->size();
		float n1 = nodeInfo0->GetVolumes()->size();
		float mDiff = m0 - m1;
		float div0 = (n0 - 1)*s0*s0 + (n1 - 1)*s1*s1;
		float div1 = n0 + n1 - 2;
		float div2 = 1 / n0 + 1 / n1;
		float tScore = (m0 - m1) / sqrtf(div0 / div1*div2);
		mSegmentTScores[nodeInfo0->GetSegmentNode().get()] = tScore;
		mStdevRange[0] = std::min(mStdevRange[0], std::min(s0, s1));
		mStdevRange[1] = std::max(mStdevRange[1], std::max(s0, s1));
		mMeanRange[0] = std::min(mMeanRange[0], std::min(m0, m1));
		mMeanRange[1] = std::max(mMeanRange[1], std::max(m0, m1));
		mThreeSigmaRange[0] = std::min(mThreeSigmaRange[0], 
			std::min(m0 - 3 * s0, m1 - 3 * s1));
		mThreeSigmaRange[1] = std::max(mThreeSigmaRange[1], 
			std::max(m0 + 3 * s0, m1 + 3 * s1));
		MyVec2f stdevRange(FLT_MAX, 0);
		stdevRange[0] = std::min(s0, s1);
		stdevRange[1] = std::max(s0, s1);
		mStdevRanges[nodeInfo0->GetSegmentNode().get()] = stdevRange;
		// update t-score range
		if (tScore > mTScoreRange[1]) mTScoreRange[1] = tScore;
		if (tScore < mTScoreRange[0]) mTScoreRange[0] = tScore;
	}
	cout << "stdev  range: " << mStdevRange[0] << ", " << mStdevRange[1] << endl;
	cout << "mean   range: " << mMeanRange[0] << ", " << mMeanRange[1] << endl;
	cout << "3sigma range: " << mThreeSigmaRange[0] << ", " << mThreeSigmaRange[1] << endl;
	this->UpdateRoiColors();
}

void MySegmentAssembleGroup::AddSegmentNode(MySegmentNodeSPtr segNode){
	for (int i = 0; i < this->size(); i++){
		MySegmentNodeInfoSPtr segNodeInfo = std::make_shared<MySegmentNodeInfo>();
		segNodeInfo->SetSegmentNode(segNode);
		segNodeInfo->SetVolumes(this->at(i)->GetSegmentNodeInfos()->front()->GetVolumes());
		segNodeInfo->Update();
		this->at(i)->AddSegmentNodeInfo(segNodeInfo);
	}
}

void MySegmentAssembleGroup::RemoveSegmentNode(const MySegmentNode* segNode){
	for (int i = 0; i < this->size(); i++){
		this->at(i)->RemoveSegmentNodeInfo(segNode);
	}
}

void MySegmentAssembleGroup::UpdateRoiColors() {
	mRoiColors->clear();
	MyMap<const MySegmentNode*, float>::const_iterator itr = mSegmentTScores.begin();
	/*
	// use sequential color
	while (itr != mSegmentTScores.end()){
		float tScoreAbs = fabs(itr->second);
		if (tScoreAbs > maxTScore) maxTScore = tScoreAbs;
		if (tScoreAbs < minTScore) minTScore = tScoreAbs;
		itr++;
	}
	itr = mSegmentTScores.begin();
	while (itr != mSegmentTScores.end()){
		float rgba[4];
		ColorScaleTable::SequentialColor(fabs(itr->second), minTScore, maxTScore, rgba);
		MyVec4f color(rgba);
		mRoiColors->operator[](itr->first) = color;
		itr++;
	}
	*/
	// use divergent color
	float tScoreRangeAbs = max(fabs(mTScoreRange[0]), fabs(mTScoreRange[1]));
	cout << "T ABS range: " << tScoreRangeAbs << endl;
	while (itr != mSegmentTScores.end()){
		float rgba[4];
		//ColorScaleTable::DiffValueToColor(itr->second, minTScore, maxTScore, rgba);
		ColorScaleTable::DivergingColor(itr->second, -tScoreRangeAbs, tScoreRangeAbs, rgba);
		MyVec4f color(rgba);
		mRoiColors->operator[](itr->first) = color;
		itr++;
	}
}