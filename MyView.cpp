#include "MyView.h"
#include "MyGLHeader.h"

MyView::MyView()
{
	mDSR = MyVec2f(1.f, 1.f);
}


MyView::~MyView()
{
}

void MyView::Render(){
	
}

MyVec4i MyView::GetViewport() const{
	return MyVec4i(mLow[0], mLow[1], MyBox2i::GetSize(0), MyBox2i::GetSize(1));
}

MyVec4i MyView::GetName(const MyVec2i& pos) const{
	MyVec2i inViewPos = pos - MyBox2i::GetLowPos();
	MyVec2i dsrPos(inViewPos[0] * mDSR[0] + 0.5, inViewPos[1] * mDSR[1] + 0.5);
	MyVec4i name = mCanvas.GetName(dsrPos);
	return name;
}

MyVec2i MyView::ComputePixelPosition(const MyVec2f& geoPos) const{
	return MyVec2i(-1, -1);
}

MyVec2i MyView::ComputePixelPosition(const MyVec3f& geoPos) const{
	return MyVec2i(-1, -1);
}

MyVec2f MyView::ComputeGeometryPosition(const MyVec2i& pixPos) const{
	return MyVec2f(-1, -1);
}

void MyView::SetDSR(const MyVec2f& dsr_factor){
	mDSR = dsr_factor;
}

int MyView::HandleKeyboardEvent(unsigned char key, int x, int y){ 
	return 0; 
};
int MyView::HandleMouseBottonEvent(int button, int state, int x, int y){ 
	return 0; 
};
int MyView::HandleMouseWheelEvent(int button, int state, int x, int y){ 
	return 0; 
};
int MyView::HandleMousePassiveMotionEvent(int x, int y){ 
	return 0; 
};
int MyView::HandleMouseMoveEvent(int x, int y){ 
	return 0; 
};
int MyView::HandleResizeEvent(){
	mCanvas.Resize(MyBox2i::GetSize(0)*mDSR[0], MyBox2i::GetSize(1)*mDSR[1]);
	return 0; 
};
int MyView::HandleIdleEvent(){ 
	return 0; 
};