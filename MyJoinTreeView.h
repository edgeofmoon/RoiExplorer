#pragma once
#include "MyView.h"
#include "MyJoinTreeDrawer.h"
#include "MyBloomDrawingHelper.h"
#include "MyRoiStatisticsDrawer.h"

#include "MySharedPointer.h"

class MyJoinTreeView :
	public MyView
{
public:
	MyJoinTreeView();
	~MyJoinTreeView();

	virtual void Render();
	virtual MyVec4i GetName(const MyVec2i& pixelPos) const;

	virtual MyVec2i ComputePixelPosition(const MyVec2f& geoPos) const;
	virtual MyVec2f ComputeGeometryPosition(const MyVec2i& pixPos) const;
	const MySegmentNode* ComputeSegmentNodeAt(const MyVec2i& pixelPos) const;
	MyVec4i GetSegmentNodeName(const MySegmentNode* seg) const;
	const MySegmentNode* GetSegmentNodeByName(const MyVec4i& name) const;

	virtual int HandleMouseBottonEvent(int button, int state, int x, int y);
	virtual int HandleMouseWheelEvent(int button, int dir, int x, int y);
	virtual int HandleMouseMoveEvent(int x, int y);
	virtual int HandleResizeEvent();

	void SetJoinTreeDrawer(MyJoinTreeDrawerSPtr drawer){
		mJoinTreeDrawer = drawer;
	}
	MyJoinTreeDrawerSPtr GetJoinTreeDrawer(){
		return mJoinTreeDrawer;
	}
	void SetMyBloomDrawingHelper(MyBloomDrawingHelperSPtr drawer){
		mBloomDrawer = drawer;
	}
	MyBloomDrawingHelperSPtr GetBloomDrawingHelper(){
		return mBloomDrawer;
	}
	void SetStatisticsDrawer(MyRoiStatisticsDrawerSPtr drawer){
		mStatisticsDrawer = drawer;
	}
	MyRoiStatisticsDrawerSPtr GetStatisticsDrawer(){
		return mStatisticsDrawer;
	}
protected:
	MyJoinTreeDrawerSPtr mJoinTreeDrawer;
	MyBloomDrawingHelperSPtr mBloomDrawer;
	MyRoiStatisticsDrawerSPtr mStatisticsDrawer;

	// interaction components
	MyVec2f mMouseAnchor;
	MyBox2f mClipBox;

	// other ultilities
	void RenderFrame();

	// marks indicating user selecions
protected:
	MyMap<const MySegmentNode*, float> mIsoMarks;
	void RenderMarks() const;
public:
	void UpdateMarks(const MySegmentNode* seg, float isoValue){
		mIsoMarks[seg] = isoValue;
	}
	void RemoveMarks(const MySegmentNode* seg){
		mIsoMarks.erase(seg);
	}
	MyMapSPtr<const MySegmentNode*, float> ComputeSeparableMarks() const;
	const MyMap<const MySegmentNode*, float>& GetMarks(){ 
		return mIsoMarks; 
	};
};

typedef MySharedPointer<MyJoinTreeView> MyJoinTreeViewSPtr;
typedef MySharedPointer<const MyJoinTreeView> MyJoinTreeViewScPtr;