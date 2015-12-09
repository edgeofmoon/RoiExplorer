#pragma once

#include "MyRoiView.h"
#include "MySharedPointer.h"

class MyRoiViewConnectorDrawer
{
public:
	MyRoiViewConnectorDrawer();
	~MyRoiViewConnectorDrawer();

	void Render();

	void SetRoiViews(MyRoiViewScPtr view0, MyRoiViewScPtr view1);
	void SetViewport(const MyBox2i& viewport);

protected:
	MyRoiViewScPtr mRoiViews[2];
	MyBox2i mViewport;

	MyVec2f ComputeViewPosition(MyRoiViewScPtr roiView, const MyVec2f& pos) const;
	void RenderBoxConnectorLine(const MyBox2f& roiBox0, const MyBox2f& roiBox1,
		const MyVec4f& color0, const MyVec4f& color1) const;
	bool IsConnectionVisible(const MySegmentNode* roi) const;
	bool IsConnectionHighlighted(const MySegmentNode* roi) const;
};


typedef MySharedPointer<MyRoiViewConnectorDrawer> MyRoiViewConnectorDrawerSPtr;
typedef MySharedPointer<const MyRoiViewConnectorDrawer> MyRoiViewConnectorDrawerScPtr;