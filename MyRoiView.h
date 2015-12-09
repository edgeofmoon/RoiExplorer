#pragma once
#include "MyView.h"
#include "MySegsPlanarDrawer.h"
#include "MySharedPointer.h"
#include "MyObjectStatus.h"
#include "MyPolyLine.h"
#include "SimpleSignal.h"

class MyRoiView :
	public MyView
{
public:
	MyRoiView();
	~MyRoiView();

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

	void SetMySegsPlanarDrawer(MySegsPlanarDrawerSPtr drawer){
		mRoiDrawer = drawer;
	}
	MySegsPlanarDrawerSPtr GetRoiDrawer() const {
		return mRoiDrawer;
	}

	Signal1< MyVec4i > Signal_SegmentSelected;
	Signal1< MyVec4i > Signal_SegmentUnselected;

protected:
	MySegsPlanarDrawerSPtr mRoiDrawer;

	enum MouseAction{
		Action_None = 0,
		Action_Drag = 1,
		Action_Brush = 2,
	} mMouseAction;

	// interaction components
	// mouse drag
	MyVec2f mMouseAnchor;
	MyBox2f mClipBox;
	// mouse brush
	MyPolyline2f mBrushTrajectory;
	void UpdateLastBrushedBoxes();
	void UnselectAll();
	void RenderBrushLine();

	void RenderFrame();

protected:
	// status
	MyMapSPtr<const MySegmentNode*, MyObjectStatus> mStatus;
	void RenderMarks() const;
public:
	MyMapScPtr<const MySegmentNode*, MyObjectStatus> GetStatus() const{
		return mStatus;
	}
	void ClearAllStatus(){
		mStatus->clear();
	}
	void SetSelect(const MySegmentNode* seg){
		mStatus->operator[](seg).SetStatusBit(MyObjectStatus::STATUS_SELECT_BIT);
	}
	void UnsetSelect(const MySegmentNode* seg){
		if (!mStatus->HasKey(seg)) return;
		mStatus->operator[](seg).UnsetStatusBit(MyObjectStatus::STATUS_SELECT_BIT);
		if (mStatus->at(seg) == MyObjectStatus::STATUS_DEFAULT){
			mStatus->erase(seg);
		}
	}
	void SetDisable(const MySegmentNode* seg){
		mStatus->operator[](seg).SetStatusBit(MyObjectStatus::STATUS_DISABLE_BIT);
	}
	void UnsetDisable(const MySegmentNode* seg){
		if (!mStatus->HasKey(seg)) return;
		mStatus->operator[](seg).UnsetStatusBit(MyObjectStatus::STATUS_DISABLE_BIT);
		if (mStatus->at(seg) == MyObjectStatus::STATUS_DEFAULT){
			mStatus->erase(seg);
		}
	}
	bool IsSelected(const MySegmentNode* seg) const{
		if (!mStatus->HasKey(seg)) return false;
		return mStatus->at(seg).IsBitSet(MyObjectStatus::STATUS_SELECT_BIT);
	}
	bool IsDisable(const MySegmentNode* seg) const{
		if (!mStatus->HasKey(seg)) return false;
		return mStatus->at(seg).IsBitSet(MyObjectStatus::STATUS_DISABLE_BIT);
	}
};

typedef MySharedPointer<MyRoiView> MyRoiViewSPtr;
typedef MySharedPointer<const MyRoiView> MyRoiViewScPtr;


