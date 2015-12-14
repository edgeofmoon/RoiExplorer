#pragma once

#include "MyJoinTreeLayout.h"
#include "MySegNodeInfoLayout2D.h"
#include "MySegmentNodeMatchCounter.h"
#include "MyJoinTreeView.h"
#include "MyRoiView.h"
#include "MySharedPointer.h"
#include "MyFont.h"

class MyLineConnectorDrawer
{
public:
	MyLineConnectorDrawer();
	~MyLineConnectorDrawer();

	void Update();
	void Render();

	void SetJoinTreeLayout(MyJoinTreeLayoutScPtr layout){
		mJoinTreeLayout = layout;
	}
	//void SetJoinTreeLayoutOffset(const MyVec2f& offset){
	//	mJoinTreeLayoutOffset = offset;
	//}
	void SetJoinTreeView(MyJoinTreeViewScPtr joinTreeView){
		mJoinTreeView = joinTreeView;
	}
	void SetRoiLayout(MySegNodeInfoLayout2DScPtr layout){
		mRoiLayout = layout;
	}
	//void SetRoiLayoutOffset(const MyVec2f& offset){
	//	mRoiLayoutOffset = offset;
	//}
	void SetRoiView(MyRoiViewScPtr roiView){
		mRoiView = roiView;
	}
	void SetViewport(const MyBox2i& viewport){
		mViewport = viewport;
	}
	void SetMatchCounter(MySegmentNodeMatchCounterSPtr counter){
		mMatchCounter = counter;
	}
	void SetRoiColors(MyMapScPtr<const MySegmentNode*, MyVec4f> colors){
		mRoiColors = colors;
	}

	MySegmentNodeMatchCounterSPtr GetMatchCounter(){
		return mMatchCounter;
	}

	void SetFont(MyFontScPtr font){ mFont = font; };
	MyFontScPtr GetFont() const{ return mFont; };
protected:
	float mMatchDrawThreshold;
	MyFontScPtr mFont;
	MyJoinTreeLayoutScPtr mJoinTreeLayout;
	//MyVec2f mJoinTreeLayoutOffset;
	MyJoinTreeViewScPtr mJoinTreeView;
	MySegNodeInfoLayout2DScPtr mRoiLayout;
	//MyVec2f mRoiLayoutOffset;
	MyRoiViewScPtr mRoiView;
	MyBox2i mViewport;
	MySegmentNodeMatchCounterSPtr mMatchCounter;
	MyMapScPtr<const MySegmentNode*, MyVec4f> mRoiColors;

	bool IsOnCenterBranch(const MySegmentNode* segment) const;

	// transform position from local normalized space
	// to combined normalized space
	MyVec2f ComputeJoinTreeViewPosition(const MyVec2f& pos) const;
	MyVec2f ComputeRoiViewPosition(const MyVec2f& pos) const;
	MyBox2f ComputeJoinTreeViewBox(const MyBox2f& box) const;
	MyBox2f ComputeRoiViewBox(const MyBox2f& box) const;

	// drawing helper
	void RenderConnectorLine(const MySegmentNode* roi, const MySegmentNode* seg);
	void RenderLabel(const MyMap<const MySegmentNode*, const MySegmentNode*>& matchToDraw);
	void RenderBoxConnectingLine(const MyBox2f& box0, const MyBox2f& box1);

	// helper to located best suited arc
	float ComputeMatchIndex(const MySegmentNode* node0, 
		const MySegmentNode* node1, int numVoxelMatch) const;
	const MySegmentNode* FindBestNode(const MySegmentNode* node,
		const MySegmentNode* localMatch) const;
};

typedef MySharedPointer<MyLineConnectorDrawer> MyLineConnectorDrawerSPtr;
typedef MySharedPointer<const MyLineConnectorDrawer> MyLineConnectorDrawerScPtr;