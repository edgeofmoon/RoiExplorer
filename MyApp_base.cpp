#include "MyApp.h"
#include "MyGLHeader.h"

// for debug
#include <iostream>
using namespace std;

MyApp::MyApp()
{
	mFocusView = 0;
	mViewLayout = std::make_shared<MyMultiViewLayout>();
	for (int i = 0; i < 6; i++){
		mComponentVisible[i] = true;
	}
	mAsmbVolLimit = INT_MAX;
}


MyApp::~MyApp()
{
}

void MyApp::Render(){
	for (int i = 0; i < this->GetNumberViews(); i++){
		if (mComponentVisible[i]){
			if (this->GetView(i)){
				this->GetView(i)->Render();
			}
		}
	}
	if (mComponentVisible[5]){
		mRoiConnectorDrawer.Render();
	}
}

int MyApp::HandleKeyboardEvent(unsigned char key, int x, int y){
	switch (key)
	{
	case 27:
		exit(1);
		return 1;
		break;
	case 'c':
	case 'C':
		return this->CreateRoiFromJoinTree();
		break;
	case 'd':
	case 'D':
		//return this->RemoveRoi();
		this->DisableRoi(mRoiView);
		this->DisableRoi(mSecondRoiView);
		return 1;
		break;
	case 'e':
	case 'E':
		this->EnableRoi(mRoiView);
		this->EnableRoi(mSecondRoiView);
		return 1;
		break;
	case 'g':
	case 'G':
		cout << "Update cohorts...\n";
		return this->UpdateAssembleGroup();
		break;
	case 'a':
	case 'A':
		cout << "adding cohorts...\n";
		return this->AddAssembleGroup();
		break;
	case 'j':
	case 'J':
		cout << "Update join tree...\n";
		return this->UpdateJoinTree();
		break;
	default:
		break;
	}
	return mFocusView->HandleKeyboardEvent(key, x, y);
}

int MyApp::HandleMouseBottonEvent(int button, int state, int x, int y){
	if (state == GLUT_DOWN){
		if (button == GLUT_RIGHT_BUTTON){
			if (this->RemoveIsosurface(x, y)){
				cout << "Remove Join Tree Surface.\n";
			}
			//if (this->RemoveRoisurface(x, y)){
			//	cout << "Remove Roi Surface.\n";
			//}
			if (this->RemoveSurface(x, y)){
				cout << "Remove 3D Surface.\n";
			}
		}
		else{
			if (this->UpdateIsosurface(x, y)){
				cout << "Update Join Tree Surface.\n";
			}
			//if (this->UpdateRoiSurface(x, y)){
			//	cout << "Update Roi Surface.\n";
			//}
		}
		int viewIdx = mViewLayout->GetViewportIndex(MyVec2i(x, y));
		mFocusView = this->GetView(viewIdx);
		if (mFocusView) {
			MyVec4i name = mFocusView->GetName(MyVec2i(x, y));
			cout << "Name: " << name[0] << ", " << name[1] << ", "
				<< name[2] << ", " << name[3] << endl;
		}
	}
	if (mFocusView) {
		return mFocusView->HandleMouseBottonEvent(button, state, x, y);
	}
	return 0;
}

int MyApp::HandleMouseWheelEvent(int button, int dir, int x, int y){
	int viewIdx = mViewLayout->GetViewportIndex(MyVec2i(x, y));
	mFocusView = this->GetView(viewIdx);
	if (mFocusView) {
		return mFocusView->HandleMouseWheelEvent(button, dir, x, y);
	}
	return 0;
}

int MyApp::HandleMousePassiveMotionEvent(int x, int y){
	int viewIdx = mViewLayout->GetViewportIndex(MyVec2i(x, y));
	MyView* thisView = this->GetView(viewIdx);
	if (thisView) {
		return thisView->HandleMousePassiveMotionEvent(x, y);
	}
	return 0;
}

int MyApp::HandleMouseMoveEvent(int x, int y){
	if (mFocusView) {
		return mFocusView->HandleMouseMoveEvent(x, y);
	}
	return 0;
}

int MyApp::HandleResizeEvent(){
	mViewLayout->SetGlobalViewport(*this);
	this->UpdateLayout();
	return 1;
}

int MyApp::HandleIdleEvent(){
	for (int i = 0; i < this->GetNumberViews(); i++){
		this->GetView(i)->HandleIdleEvent();
	}
	return 1;
}

void MyApp::UpdateLayout(){
	int numViews = 3;
	if (mSecondRoiView) numViews++;
	mViewLayout->SetNumberOfViews(numViews);
	mViewLayout->Update();
	mSpatialView->Set(mViewLayout->GetViewport(0));
	mSpatialView->HandleResizeEvent();
	mJoinTreeView->Set(mViewLayout->GetViewport(1));
	mJoinTreeView->HandleResizeEvent();
	mRoiView->Set(mViewLayout->GetViewport(2));
	mRoiView->HandleResizeEvent();
	if (mSecondRoiView){
		mSecondRoiView->Set(mViewLayout->GetViewport(3));
		mSecondRoiView->HandleResizeEvent();
	}
	mConnectorView->SetViewports(mViewLayout->GetViewport(1),
		mViewLayout->GetViewport(2));
	mConnectorView->HandleResizeEvent();
}

MyView* MyApp::GetView(int idx){
	switch (idx)
	{
	case 0:
		return mSpatialView.get();
		break;
	case 1:
		return mJoinTreeView.get();
		break;
	case 2:
		return mRoiView.get();
		break;
	case 3:
		return mSecondRoiView.get();
		break;
	case 4:
		return mConnectorView.get();
		break;
	default:
		return 0;
		break;
	}
	return 0;
}
int MyApp::GetNumberViews() const{
	//return 3;
	return 5;
}