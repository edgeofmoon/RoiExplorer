#include "MyConnectorView.h"
#include "MyGLHeader.h"

MyConnectorView::MyConnectorView()
{
}


MyConnectorView::~MyConnectorView()
{
}

void MyConnectorView::Render(){
	mCanvas.On();
	mCanvas.Clear();

	//glViewport(mLow[0], mLow[1], MyBox2i::GetSize(0), MyBox2i::GetSize(1));
	//glDepthFunc(GL_ALWAYS);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	mConnectorDrawer->Render();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	//glDepthFunc(GL_LESS);

	mCanvas.Off();
	glDepthFunc(GL_ALWAYS);
	mCanvas.Show(MyView::GetViewport());
	glDepthFunc(GL_LESS);
}


void MyConnectorView::SetViewports(const MyBox2i& joinTreeViewport, const MyBox2i& roiViewport){
	mJoinTreeViewport = joinTreeViewport;
	mRoiViewport = roiViewport;
	MyBox2i::Set(joinTreeViewport);
	MyBox2i::Engulf(roiViewport);
}

int MyConnectorView::HandleMouseBottonEvent(int button, int state, int x, int y){
	return 1;
}

int MyConnectorView::HandleMouseWheelEvent(int button, int dir, int x, int y){
	return 1;
}

int MyConnectorView::HandleMouseMoveEvent(int x, int y){
	return 1;
}

int MyConnectorView::HandleResizeEvent(){
	MyView::HandleResizeEvent();
	mConnectorDrawer->SetViewport(*this);
	return 1;
}
