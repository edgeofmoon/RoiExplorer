#pragma once

#include "MyJoinTreeLayout.h"
#include "MySegNodeInfoLayout2D.h"
#include "MySegmentNodeMatchCounter.h"

class MyLineConnectorDrawer
{
public:
	MyLineConnectorDrawer();
	~MyLineConnectorDrawer();

	void Update();
	void Render(int width, int height);

	void SetJoinTreeLayout(MyJoinTreeLayoutScPtr layout){
		mJoinTreeLayout = layout;
	}
	void SetJoinTreeLayoutOffset(const MyVec2f& offset){
		mJoinTreeLayoutOffset = offset;
	}
	void SetRoiLayout(MySegNodeInfoLayout2DScPtr layout){
		mRoiLayout = layout;
	}
	void SetRoiLayoutOffset(const MyVec2f& offset){
		mRoiLayoutOffset = offset;
	}
	void SetMatchCounter(MySegmentNodeMatchCounterScPtr counter){
		mMatchCounter = counter;
	}
	void SetRoiColors(MyMapSPtr<const MySegmentNode*, MyVec4f> colors){
		mRoiColors = colors;
	}
protected:
	MyJoinTreeLayoutScPtr mJoinTreeLayout;
	MyVec2f mJoinTreeLayoutOffset;
	MySegNodeInfoLayout2DScPtr mRoiLayout;
	MyVec2f mRoiLayoutOffset;
	MySegmentNodeMatchCounterScPtr mMatchCounter;
	MyMapSPtr<const MySegmentNode*, MyVec4f> mRoiColors;

	bool IsOnCenterBranch(const MySegmentNode* segment) const;
};

