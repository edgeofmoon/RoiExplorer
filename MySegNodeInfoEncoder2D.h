#pragma once

#include "MySegmentNodeInfo.h"
#include "MyBox.h"

class MySegNodeInfoEncoder2D
{
public:
	MySegNodeInfoEncoder2D();
	~MySegNodeInfoEncoder2D();

	void SetSegmentInfo(const MySegmentNodeInfo* segInfo){ mSegNodeInfo = segInfo; };
	const MySegmentNodeInfo* GetSegmentInfo() const { return mSegNodeInfo; };

	const MyBox2f& GetBox() const { return mBox; };
	const MyVec4f& GetColor() const { return mColor; };
	void Update();

protected:
	const MySegmentNodeInfo* mSegNodeInfo;
	MyBox2f mBox;
	MyVec4f mColor;
};

typedef MySharedPointer<MySegNodeInfoEncoder2D> MySegNodeInfoEncoder2DSPtr;
typedef MySharedPointer<const MySegNodeInfoEncoder2D> MySegNodeInfoEncoder2DScPtr;