#pragma once
#include "MyView.h"

#include "MySpatialView.h"
#include "MyRoiView.h"
#include "MyJoinTreeView.h"
#include "MyMultiViewLayout.h"
#include "MyConnectorView.h"
#include "MyUiPanel.h"
#include "MyRoiViewConnectorDrawer.h"

class MyApp :
	public MyView
{
public:
	MyApp();
	~MyApp();

	// MyApp_base
	virtual void Render();

	virtual int HandleKeyboardEvent(unsigned char key, int x, int y);
	virtual int HandleMouseBottonEvent(int button, int state, int x, int y);
	virtual int HandleMouseWheelEvent(int button, int dir, int x, int y);
	virtual int HandleMousePassiveMotionEvent(int x, int y);
	virtual int HandleMouseMoveEvent(int x, int y);
	virtual int HandleResizeEvent();
	virtual int HandleIdleEvent();

	// MyApp_init
	void Init();

public:
	// slots, return type can only be void
	void ConnectAll();
	void AddRoiSurface(MyVec4i name);
	void RemoveRoiSurface(MyVec4i name);
	void SetViewAngle(int viewAngle);
	void EnableComponent(int component);
	void DisableComponent(int component);
	void SetValuef(int component, float value);
	void HandleEvent(int eveIdx);

	// MyApp_interactions
protected:
	int UpdateIsosurface(int x, int y);
	int RemoveIsosurface(int x, int y);
	//int UpdateRoiSurface(int x, int y);
	//int RemoveRoisurface(int x, int y);
	int RemoveSurface(int x, int y);
	int CreateRoiFromJoinTree();
	//int RemoveRoi();
	int DisableRoi(MyRoiViewSPtr roiView);
	int EnableRoi(MyRoiViewSPtr roiView);
	int ClearJoinTreeSurfaces();
	int UpdateJoinTree();
	int UpdateAssembleGroup();
	int AddAssembleGroup();

	// ultilities
	int FindLowestIndex( const MyVoxContainerf* voxels,
		const My3dArrayf* vol) const;
	int FindLowestIndex(const MyVoxContainerf* voxels) const;
	const MySegmentNode* GetSegmentByName(const MyVec4i& name) const;
	MyRoiViewSPtr GetRoiViewByName(const MyVec4i& name) const;

	// data members
protected:
	// views
	MySpatialViewSPtr mSpatialView;
	MyJoinTreeViewSPtr mJoinTreeView;
	MyRoiViewSPtr mRoiView;
	MyRoiViewSPtr mSecondRoiView;
	MyConnectorViewSPtr mConnectorView;

	bool mComponentVisible[6];
	int mAsmbVolLimit;

	MyRoiViewConnectorDrawer mRoiConnectorDrawer;

	MyMultiViewLayoutSPtr mViewLayout;
	void UpdateLayout();

	// data
	//MySegmentAssembleGroupSPtr mAsmbGroup;
	//MySegmentAssembleGroupSPtr mSecondAsmbGroup;

	// ui
	MyUiPanel mUiPanel;

	// temporal variables
	MyView* mFocusView;

	MyView* GetView(int idx);
	int GetNumberViews() const;
};

