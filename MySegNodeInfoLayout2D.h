#pragma once

#include "MyArray.h"
#include "MyBox.h"
#include "MySharedPointer.h"
#include "MySegmentNodeInfo.h"
#include "MyMap.h"
#include "MySegmentAssembleGroup.h"
#include "MyObjectStatus.h"
class MySegNodeInfoLayout2D
{
public:
	MySegNodeInfoLayout2D();
	~MySegNodeInfoLayout2D();

	void SetBoxesIn(MyMapScPtr<const MySegmentNode*, MyBox2f> boxes){
		mBoxesIn = boxes;
	}
	void SetSegmentAssembleGroup(MySegmentAssembleGroupScPtr asmbGroup){
		mSegAsmGroup = asmbGroup;
	}
	void SetBoxStatus(MyMapScPtr<const MySegmentNode*, MyObjectStatus> status){
		mBoxStatus = status;
	}
	const MyBox2f& GetSegmentPosition(const MySegmentNode* segment) const{
		return mBoxesOut->at(segment);
	}

	MyMapScPtr<const MySegmentNode*, MyBox2f> GetBoxesOut() const{
		return mBoxesOut;
	}
	void SetBaseBoxVerticalRange(const MyVec2f& range){
		mBaseBoxVerticalRange = range;
	}
	const MyVec2f& GetBaseBoxVerticalRange() const{
		return mBaseBoxVerticalRange;
	}
	void SetTScoreBoxHeight(float h){
		mTScoreBoxHeight = h;
	}
	float GetTScoreBoxHeight() const{
		return mTScoreBoxHeight;
	}
	void SetBaseBoxWidth(float w){
		mBaseBoxWidth = w;
	}
	float GetBaseBoxWidth() const{
		return mBaseBoxWidth;
	}
	void SetSmallBoxWidth(float w){
		mSmallBoxWidth = w;
	}
	float GetSmallBoxWidth() const{
		return mSmallBoxWidth;
	}
	void Update();

protected:
	// input
	MyMapScPtr<const MySegmentNode*, MyBox2f> mBoxesIn;
	MyMapScPtr<const MySegmentNode*, MyObjectStatus> mBoxStatus;
	MySegmentAssembleGroupScPtr mSegAsmGroup;

	// parameter
	MyVec2f mBaseBoxVerticalRange;
	float mTScoreBoxHeight;
	float mBaseBoxWidth;
	float mSmallBoxWidth;

	// output layout
	MyMapSPtr<const MySegmentNode*, MyBox2f> mBoxesOut;

	// sort boxes
	static bool IsBoxLeft(const MyBox2f& box0, const MyBox2f& box1);

	// get size by status
	MyBox2f ComputeBox(const MySegmentNode* seg) const;
};

typedef MySharedPointer<MySegNodeInfoLayout2D> MySegNodeInfoLayout2DSPtr;

typedef MySharedPointer<const MySegNodeInfoLayout2D> MySegNodeInfoLayout2DScPtr;