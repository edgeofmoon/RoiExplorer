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

	void Update();

protected:
	// input
	MyMapScPtr<const MySegmentNode*, MyBox2f> mBoxesIn;
	MyMapScPtr<const MySegmentNode*, MyObjectStatus> mBoxStatus;
	MySegmentAssembleGroupScPtr mSegAsmGroup;

	// output layout
	MyMapSPtr<const MySegmentNode*, MyBox2f> mBoxesOut;

	// sort boxes
	static bool IsBoxLeft(const MyBox2f& box0, const MyBox2f& box1);

	// get size by status
	MyBox2f ComputeBox(const MySegmentNode* seg) const;
};

typedef MySharedPointer<MySegNodeInfoLayout2D> MySegNodeInfoLayout2DSPtr;

typedef MySharedPointer<const MySegNodeInfoLayout2D> MySegNodeInfoLayout2DScPtr;