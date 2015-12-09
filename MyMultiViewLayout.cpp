#include "MyMultiViewLayout.h"


MyMultiViewLayout::MyMultiViewLayout()
{
}


MyMultiViewLayout::~MyMultiViewLayout()
{
}

void MyMultiViewLayout::Update(){
	if (mNumberViews == 3){
		mViewports.clear();
		MyBox2i box0 = this->CutBox(mGlobalViewport, 0, 0.02, 0.32);
		MyBox2i box1 = this->CutBox(mGlobalViewport, 0, 0.34, 0.98);
		MyBox2i box10 = this->CutBox(box1, 1, 0.51, 0.98);
		MyBox2i box11 = this->CutBox(box1, 1, 0.02, 0.49);
		mViewports[0] = box0;
		mViewports[1] = box10;
		mViewports[2] = box11;
	}
	else{
		mViewports.clear();
		MyBox2i box0 = this->CutBox(mGlobalViewport, 0, 0.02, 0.32);
		MyBox2i box1 = this->CutBox(mGlobalViewport, 0, 0.34, 0.98);
		MyBox2i box10 = this->CutBox(box1, 1, 0.61, 0.99);
		MyBox2i box11 = this->CutBox(box1, 1, 0.31, 0.59);
		MyBox2i box12 = this->CutBox(box1, 1, 0.01, 0.29);
		mViewports[0] = box0;
		mViewports[1] = box10;
		mViewports[2] = box11;
		mViewports[3] = box12;
	}
}

int MyMultiViewLayout::GetViewportIndex(const MyVec2i& pos) const{
	MyMap<int, MyBox2i>::const_iterator itr = mViewports.begin();
	while (itr != mViewports.end()){
		if (itr->second.IsIntersected(pos)){
			return itr->first;
		}
		itr++;
	}
	return -1;
}

MyBox2i MyMultiViewLayout::CutBox(
	const MyBox2i& box, int iDim, float startRatio, float endRatio) const{
	MyVec2i lowPos = box.GetLowPos();
	lowPos[iDim] += box.GetSize(iDim)*startRatio;
	MyVec2i highPos = box.GetHighPos();
	highPos[iDim] = box.GetLowPos()[iDim] + box.GetSize(iDim)*endRatio;
	return MyBox2i(lowPos, highPos);
}