#pragma once

#include "MyArray.h"
#include "MyBox.h"
#include "MySharedPointer.h"
#include "MySegmentNodeInfo.h"
#include "MyMap.h"
#include "MySegmentAssembleGroup.h"

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

	const MyBox2f& GetSegmentPosition(const MySegmentNode* segment) const{
		return mBoxesOut->at(segment);
	}

	MyMapScPtr<const MySegmentNode*, MyBox2f> GetBoxesOut() const{
		return mBoxesOut;
	}

	void Update();

protected:
	MyMapScPtr<const MySegmentNode*, MyBox2f> mBoxesIn;
	MySegmentAssembleGroupScPtr mSegAsmGroup;

	// output layout
	MyMapSPtr<const MySegmentNode*, MyBox2f> mBoxesOut;

	// sort boxes
	static bool IsBoxLeft(const MyBox2f& box0, const MyBox2f& box1);
};

typedef MySharedPointer<MySegNodeInfoLayout2D> MySegNodeInfoLayout2DSPtr;

typedef MySharedPointer<const MySegNodeInfoLayout2D> MySegNodeInfoLayout2DScPtr;