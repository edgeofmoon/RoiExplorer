#include "MySpatialView.h"
#include "MyGLHeader.h"
#include "MyGraphicsTool.h"

// debug
#include <iostream>
using namespace std;
MySpatialView::MySpatialView()
{
}


MySpatialView::~MySpatialView()
{
}

void MySpatialView::Render(){
	mCanvas.On();
	mCanvas.Clear();

	MyGraphicsTool::PushMatrix();
	MyGraphicsTool::PushProjectionMatrix();
	MyMatrixf projectionMatrix = MyMatrixf::PerspectiveMatrix(
		45, MyBox2i::GetSize(0)/(float)MyBox2i::GetSize(1), 0.01f, 1000);
	MyGraphicsTool::LoadProjectionMatrix(&projectionMatrix);
	MyGraphicsTool::LoadModelViewMatrix(&MyMatrixf::IdentityMatrix());
	gluLookAt(0, 0, 400, 0, 0, 0, 0, 1, 0);

	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	MyGraphicsTool::LoadTrackBall(&mTrackBall);
	glPushMatrix();
	glTranslatef(-182 / 2, -218 / 2, -182 / 2);
	//cout << "#Renderers: " << mSurfaceRenderers.size() << endl;
	MyMap<MySurfaceRendererSPtr, int>::iterator itr;
	for (itr = mSurfaceRenderers.begin(); itr != mSurfaceRenderers.end(); itr++){
		itr->first->Render();
	}
	glPopMatrix();

	mCanvas.Off();
	//mCanvas.Show(MyView::GetViewport());

	MyGraphicsTool::SetViewport(MyView::GetViewport());
	glPushMatrix();
	glTranslatef(-182 / 2, -218 / 2, -182 / 2);
	for (itr = mSurfaceRenderers.begin(); itr != mSurfaceRenderers.end(); itr++){
		itr->first->Render();
	}
	glPopMatrix();

	glPushMatrix();
	if (mMeshRenderer){
		// whats happening with the wrong direction???
		glScalef(1, -1, -1);
		mMeshRenderer->Render();
	}
	glPopMatrix();

	glPopAttrib();
	MyGraphicsTool::PopProjectionMatrix();
	MyGraphicsTool::PopMatrix();
}

void MySpatialView::AddSurfaceRenderer(MySurfaceRendererSPtr surfaceRenderer){
	mSurfaceRenderers[surfaceRenderer] = 0;
}

void MySpatialView::RemoveSurfaceRenderer(MySurfaceRendererSPtr surfaceRenderer){
	mSurfaceRenderers.erase(surfaceRenderer);
}

void MySpatialView::SetMeshRenderer(MySurfaceRendererSPtr meshRenderer){
	mMeshRenderer = meshRenderer;
}

MySurfaceRendererSPtr MySpatialView::GetSurfaceRendererByName(const MyVec4i& name){
	MyMap<MySurfaceRendererSPtr, int>::iterator itr;
	for (itr = mSurfaceRenderers.begin(); itr != mSurfaceRenderers.end(); itr++){
		if (itr->first->GetName() == name){
			return itr->first;
		}
	}
	return 0;
}

int MySpatialView::HandleMouseBottonEvent(int button, int state, int x, int y){
	if (state == GLUT_DOWN){
		mTrackBall.StartMotion(x, y);
	}
	else{
		mTrackBall.EndMotion(x, y);
	}
	return 1;
}

int MySpatialView::HandleMouseWheelEvent(int button, int dir, int x, int y){
	if (dir > 0){
		mTrackBall.ScaleMultiply(1.05f);
	}
	else{
		mTrackBall.ScaleMultiply(1 / 1.05f);
	}
	return 1;
}

int MySpatialView::HandleMouseMoveEvent(int x, int y){
	mTrackBall.RotateMotion(x, y);
	return 1;
}
int MySpatialView::HandleResizeEvent(){
	MyView::HandleResizeEvent();
	mTrackBall.Reshape(MyBox2i::GetSize(0), MyBox2i::GetSize(1));
	return 1;
}