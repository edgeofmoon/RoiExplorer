#pragma once
#include "MyView.h"
#include "MySurfaceRenderer.h"
#include "MyTrackBall.h"
#include "MyMesh.h"
#include "MyMap.h"
#include "MySharedPointer.h"

class MySpatialView :
	public MyView
{
public:
	MySpatialView();
	~MySpatialView();

	void Render();
	MyTrackBall& GetTrackBall(){
		return mTrackBall;
	};
	void AddSurfaceRenderer(MySurfaceRendererSPtr surfaceRenderer);
	void RemoveSurfaceRenderer(MySurfaceRendererSPtr surfaceRenderer);
	void SetMeshRenderer(MySurfaceRendererSPtr meshRenderer);
	MySurfaceRendererSPtr GetSurfaceRendererByName(const MyVec4i& name);
	MySurfaceRendererSPtr GetMeshRenderer() const { return mMeshRenderer; };

	virtual int HandleMouseBottonEvent(int button, int state, int x, int y);
	virtual int HandleMouseWheelEvent(int button, int dir, int x, int y);
	virtual int HandleMouseMoveEvent(int x, int y);
	virtual int HandleResizeEvent();

protected:
	MyMap<MySurfaceRendererSPtr, int> mSurfaceRenderers;
	MySurfaceRendererSPtr mMeshRenderer;
	MyTrackBall mTrackBall;
};

typedef MySharedPointer<MySpatialView> MySpatialViewSPtr;
typedef MySharedPointer<const MySpatialView> MySpatialViewScPtr;