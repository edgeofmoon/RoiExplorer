#pragma once

#include "MyJoinTreeLayout.h"
#include "MySegmentNodeMatchCounter.h"
#include "MySharedPointer.h"

class MyBloomDrawingHelper
{
public:
	MyBloomDrawingHelper();
	~MyBloomDrawingHelper();

	void Update();
	void Render();

	void SetLayout(MyJoinTreeLayoutScPtr layout){
		mLayout = layout;
	}

	void SetMatchCounter(MySegmentNodeMatchCounterScPtr counter){
		mMatchCounter = counter;
	}

	void SetSegmentColor(MyMapScPtr<const MySegmentNode*, MyVec4f> segColor){
		mRoiColors = segColor;
	}

protected:
	MyJoinTreeLayoutScPtr mLayout;
	MySegmentNodeMatchCounterScPtr mMatchCounter;
	MyMapScPtr<const MySegmentNode*, MyVec4f> mRoiColors;

	void DrawBloomBox(const MyBox2f& bloomBox, const MyVec4f& color) const;
	MyBox2f CutBox(const MyBox2f& box, float startRatio, float endRatio) const;
};

typedef MySharedPointer<MyBloomDrawingHelper> MyBloomDrawingHelperSPtr;
typedef MySharedPointer<const MyBloomDrawingHelper> MyBloomDrawingHelperScPtr;