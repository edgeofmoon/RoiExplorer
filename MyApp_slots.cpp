#include "MyApp.h"
#include "MyGLHeader.h"
#include "MyIsosurfaceTracker.h"

void MyApp::ConnectAll(){
	mRoiView->Signal_SegmentSelected.Connect(this, &MyApp::AddRoiSurface);
	mRoiView->Signal_SegmentUnselected.Connect(this, &MyApp::RemoveRoiSurface);
	MyUiPanel::Signal_SetViewAngle.Connect(this, &MyApp::SetViewAngle);
	MyUiPanel::Signal_EnableComponent.Connect(this, &MyApp::EnableComponent);
	MyUiPanel::Signal_DisableComponent.Connect(this, &MyApp::DisableComponent);
	MyUiPanel::Signal_SetValuef.Connect(this, &MyApp::SetValuef);
	MyUiPanel::Signal_Event.Connect(this, &MyApp::HandleEvent);
}

void MyApp::AddRoiSurface(MyVec4i name){
	const MySegmentNode* roi = this->GetSegmentByName(name);
	if (roi){
		MySurfaceRendererSPtr oldRenderer = mSpatialView->GetSurfaceRendererByName(name);
		if (oldRenderer) return;
		My3dArrayfScPtr joinTreeVol
			= mJoinTreeView->GetJoinTreeDrawer()->GetJoinTree()->GetVolumn();
		My3dArrayfSPtr vol = roi->GetUniqueVoxes()->MakeVolume();
		MyIsosurfaceTracker tracker;
		// as always, use min
		int currentLowestIndex = this->FindLowestIndex(roi->GetUniqueVoxes().get());
		tracker.SetStartIndex(currentLowestIndex);
		tracker.SetVolumn(vol);
		// any number slightly higher than 0 
		// and lower than min will do
		tracker.SetIsovalue(0.0001f);
		tracker.Update();
		MyVec4f color = this->GetRoiViewByName(name)->GetRoiDrawer()
			->GetSegmentAssembleGroup()->GetRoiColors()->at(roi);
		MySurfaceRendererSPtr renderer = std::make_shared<MySurfaceRenderer>();
		renderer->SetGeometry(tracker.GetVertices().get(),
			tracker.GetNormals().get(), tracker.GetTriangles().get());
		renderer->SetName(name);
		renderer->SetColor(color);
		renderer->Update();
		// update spatial view
		mSpatialView->AddSurfaceRenderer(renderer);
		// give it a mark
		if (name[0] == mRoiView->GetIndex()) mRoiView->SetSelect(roi);
		if (mSecondRoiView){
			if (name[0] == mSecondRoiView->GetIndex()) {
				mSecondRoiView->SetSelect(roi);
			}
		}
	}
}

void MyApp::RemoveRoiSurface(MyVec4i name){
	const MySegmentNode* roi = this->GetSegmentByName(name);
	if (roi){
		MySurfaceRendererSPtr renderer = mSpatialView->GetSurfaceRendererByName(name);
		mSpatialView->RemoveSurfaceRenderer(renderer);
		// remove the mark
		if (name[0] == mRoiView->GetIndex()) mRoiView->UnsetSelect(roi);
		if (mSecondRoiView){
			if (name[0] == mSecondRoiView->GetIndex()){
				mSecondRoiView->UnsetSelect(roi);
			}
		}
	}
}

void MyApp::SetViewAngle(int viewAngle){
	MyTrackBall& trackBall = mSpatialView->GetTrackBall();
	switch (viewAngle){
	case 1:
		// superior
		trackBall.SetRotationMatrix(MyMatrixf::RotateMatrix(0, 0, 1, 0));
		break;
	case 2:
		// inferior
		trackBall.SetRotationMatrix(MyMatrixf::RotateMatrix(180, 0, 1, 0));
		break;
	case 3:
		// left
		trackBall.SetRotationMatrix(MyMatrixf::RotateMatrix(90, 0, 0, 1)
			*MyMatrixf::RotateMatrix(90, 0, 1, 0));
		break;
	case 4:
		// right
		trackBall.SetRotationMatrix(MyMatrixf::RotateMatrix(-90, 0, 0, 1)
			*MyMatrixf::RotateMatrix(-90, 0, 1, 0));
		break;
	case 5:
		// anterior
		trackBall.SetRotationMatrix(MyMatrixf::RotateMatrix(90, 1, 0, 0));
		break;
	case 6:
		// posterior
		trackBall.SetRotationMatrix(MyMatrixf::RotateMatrix(180, 0, 0, 1)
			*MyMatrixf::RotateMatrix(-90, 1, 0, 0));
		break;
	case 7:
	default:
		// custom
		trackBall.SetRotationMatrix(MyMatrixf::RotateMatrix(45, -1, 1, 0)
			*MyMatrixf::RotateMatrix(90, 0, 0, 1)*MyMatrixf::RotateMatrix(90, 0, 1, 0));
		break;
	}
	// seems buttom event does not always redisplay
	glutPostRedisplay();
}

void MyApp::EnableComponent(int component){
	switch (component){
	case 0:
	case 1:
		mJoinTreeView->SetComponentVisible(component + 1);
		break;
	case 2:
		this->mComponentVisible[4] = true;
		break;
	case 3:
		this->mComponentVisible[5] = true;
		break;
	default:
		break;
	}
}

void MyApp::DisableComponent(int component){
	switch (component){
	case 0:
	case 1:
		mJoinTreeView->UnsetComponentVisible(component + 1);
		break;
	case 2:
		this->mComponentVisible[4] = false;
		break;
	case 3:
		this->mComponentVisible[5] = false;
		break;
	default:
		break;
	}
}

void MyApp::SetValuef(int component, float value){
	switch (component){
	case 0:
		mSpatialView->GetMeshRenderer()->SetTransparency(value);
		break;
	case 1:
		mRoiView->GetRoiDrawer()->SetLinkDrawThreshold(value);
		if (mSecondRoiView){
			mSecondRoiView->GetRoiDrawer()->SetLinkDrawThreshold(value);
		}
		break;
	default:
		break;
	}
}

void MyApp::HandleEvent(int eveIdx){
	switch (eveIdx){
	case 1:
		this->DisableRoi(mRoiView);
		this->DisableRoi(mSecondRoiView);
		break;
	case 2:
		this->EnableRoi(mRoiView);
		this->EnableRoi(mSecondRoiView);
		break;
	case 3:
		this->ClearJoinTreeSurfaces();
		break;
	case 4:
		this->AddAssembleGroup();
		break;
	case 5:
		this->UpdateJoinTree();
		break;
	case 6:
		this->CreateRoiFromJoinTree();
		break;
	}
	// seems buttom event does not always redisplay
	glutPostRedisplay();
}

const MySegmentNode* MyApp::GetSegmentByName(const MyVec4i& name) const{
	int index = name[0];
	switch (index)
	{
	case 1:
		return mJoinTreeView->GetSegmentNodeByName(name);
		break;
	case 2:
		return mRoiView->GetSegmentNodeByName(name);
		break;
	case 3:
		return mSecondRoiView->GetSegmentNodeByName(name);
		break;
	default:
		return 0;
		break;
	}
}

MyRoiViewSPtr MyApp::GetRoiViewByName(const MyVec4i& name) const{
	int index = name[0];
	switch (index)
	{
	case 2:
		return mRoiView;
		break;
	case 3:
		return mSecondRoiView;
		break;
	default:
		return 0;
		break;
	}
}