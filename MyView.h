#pragma once

#include "MyBox.h"
#include "MyCanvas.h"

class MyView :
	public MyBox2i
{
public:
	MyView();
	virtual ~MyView();

	virtual void Render();

	void SetIndex(int index) { mIndex = index; };
	int GetIndex() const{ return mIndex; };
	virtual MyVec4i GetViewport() const;
	virtual MyVec4i GetName(const MyVec2i& pos) const;
	virtual MyVec2i ComputePixelPosition(const MyVec2f& geoPos) const;
	virtual MyVec2i ComputePixelPosition(const MyVec3f& geoPos) const;
	virtual MyVec2f ComputeGeometryPosition(const MyVec2i& pixPos) const;
	virtual void SetDSR(const MyVec2f& dsr_factor);

	virtual int HandleKeyboardEvent(unsigned char key, int x, int y);
	virtual int HandleMouseBottonEvent(int button, int state, int x, int y);
	virtual int HandleMouseWheelEvent(int button, int dir, int x, int y);
	virtual int HandleMousePassiveMotionEvent(int x, int y);
	virtual int HandleMouseMoveEvent(int x, int y);
	// resize should be done through modifying the MyBox2i functions
	// then using this to sync the size update
	virtual int HandleResizeEvent();
	virtual int HandleIdleEvent();

protected:
	MyVec2f mDSR;
	MyCanvas mCanvas;
	int mIndex;
};

