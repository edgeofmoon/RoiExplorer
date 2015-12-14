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
	mSegmentEffectSize.clear();
	mStdevRanges.clear();
	mStdevRange = MyVec2f(100, 0);
	mMeanRange = MyVec2f(100, 0);
	mThreeSigmaRange = MyVec2f(100, 0);
	mTScoreRange = MyVec2f(FLT_MAX, 0);
	mEffectSizeRange = MyVec2f(FLT_MAX, -FLT_MAX);
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
		float effectSize = this->ComputeEffectSize(nodeInfo0->GetSegmentNodeMeans(),
			nodeInfo1->GetSegmentNodeMeans(), m0, m1);
		mSegmentTScores[nodeInfo0->GetSegmentNode().get()] = tScore;
		mSegmentEffectSize[nodeInfo0->GetSegmentNode().get()] = effectSize;
		mStdevRange[0] = std::min(mStdevRange[0], std::min(s0, s1));
		mStdevRange[1] = std::max(mStdevRange[1], std::max(s0, s1));
		mEffectSizeRange[0] = std::min(mEffectSizeRange[0], effectSize);
		mEffectSizeRange[1] = std::max(mEffectSizeRange[1], effectSize);
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
	cout << "effectSize range: " << mEffectSizeRange[0] << ", " << mEffectSizeRange[1] << endl;
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
	// 
	/*
	// use divergent color based on t score
	MyMap<const MySegmentNode*, float>::const_iterator itr = mSegmentTScores.begin();
	float tScoreRangeAbs = max(fabs(mTScoreRange[0]), fabs(mTScoreRange[1]));
	cout << "T ABS range: " << tScoreRangeAbs << endl;
	while (itr != mSegmentTScores.end()){
		float rgba[4];
		//ColorScaleTable::DiffValueToColor(itr->second, minTScore, maxTScore, rgba);
		ColorScaleTable::DivergingColor(itr->second, -tScoreRangeAbs, tScoreRangeAbs, rgba);
		MyVec4f color(rgba);
		mRoiColors->operator[](itr->first) = color;
		itr++;
	}*/
	// use divergent color based on effect size
	MyMap<const MySegmentNode*, float>::const_iterator itr = mSegmentEffectSize.begin();
	float effectSizeRangeAbs = max(fabs(mEffectSizeRange[0]), fabs(mEffectSizeRange[1]));
	while (itr != mSegmentEffectSize.end()){
		float rgba[4];
		//ColorScaleTable::DiffValueToColor(itr->second, minTScore, maxTScore, rgba);
		ColorScaleTable::DivergingColor(itr->second, -effectSizeRangeAbs, effectSizeRangeAbs, rgba);
		MyVec4f color(rgba);
		mRoiColors->operator[](itr->first) = color;
		itr++;
	}
}

float MySegmentAssembleGroup::ComputeEffectSize(const MyArrayf& v0, 
	const MyArrayf& v1, float m0, float m1){
	float mean = (v0.size()*m0 + v1.size()*m1) / (v0.size() + v1.size());
	float variation = 0;
	for (int i = 0; i < v0.size(); i++){
		float deviation = v0[i] - mean;
		variation += deviation*deviation;
	}
	for (int i = 0; i < v1.size(); i++){
		float deviation = v1[i] - mean;
		variation += deviation*deviation;
	}
	float popStdev = sqrtf(variation / (v0.size() + v1.size()));
	return (m0 - m1) / popStdev;
}