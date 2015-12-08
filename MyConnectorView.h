#pragma once
#include "MyView.h"
#include "MyLineConnectorDrawer.h"
#include "MySharedPointer.h"

class MyConnectorView :
	public MyView
{
public:
	MyConnectorView();
	~MyConnectorView();

	virtual void Render();

	void SetViewports(const MyBox2i& joinTreeViewport, 
		const MyBox2i& roiViewport);
	void SetConnectorDrawer(MyLineConnectorDrawerSPtr drawer){
		mConnectorDrawer = drawer;
	}

	MyLineConnectorDrawerSPtr GetConnectorDrawer(){
		return mConnectorDrawer;
	}
	virtual int HandleMouseBottonEvent(int button, int state, int x, int y);
	virtual int HandleMouseWheelEvent(int button, int dir, int x, int y);
	virtual int HandleMouseMoveEvent(int x, int y);
	virtual int HandleResizeEvent();

protected:
	MyBox2i mJoinTreeViewport;
	MyBox2i mRoiViewport;
	MyLineConnectorDrawerSPtr mConnectorDrawer;
};

typedef MySharedPointer<MyConnectorView> MyConnectorViewSPtr;
typedef MySharedPointer<const MyConnectorView> MyConnectorViewScPtr;
