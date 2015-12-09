#include "MyLabelManager.h"
#include "MySpaceFillingNaive.h"

#include "MyGLHeader.h"

MyLabelManager::MyLabelManager()
{
	mLabelBoxes = std::make_shared<MyMap<const MySegmentNode*, MyBox2f>>();
	mWidth = mHeight = 800;
}


MyLabelManager::~MyLabelManager()
{
}

void MyLabelManager::SetWindowSize(int w, int h){
	mWidth = w;
	mHeight = h;
}

void MyLabelManager::SetLabels(MyMapScPtr<int, MyString> labels){
	mLabels = labels;
}

void MyLabelManager::SetBoxes(MyMapScPtr<const MySegmentNode*, MyBox2f> boxes){
	mBoxes = boxes;
}

void MyLabelManager::SetFont(MyFontScPtr font){
	mFont = font;
}

void MyLabelManager::Update(){
	mLabelBoxes->clear();

}

void MyLabelManager::Render(){
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = mBoxes->begin();
	while (itr != mBoxes->end()){
		// a hack: here small width means the roi is disabled
		if (itr->second.GetSize(0) < 0.01) {
			itr++;
			continue;
		}
		int index = itr->first->GetIndex();
		if (mLabels->HasKey(index)){
			MyString label;
			if (mLabels->HasKey(index)) label = mLabels->at(index);
			else label = "R" + index;
			MyVec2i labelSize = mFont->ComputeSizeInPixel(label);
			float heightWidthRatio = labelSize[1] / (float)labelSize[0];
			MyVec2f bl(itr->second.GetLowPos()[0], itr->second.GetHighPos()[1] + 0.01);
			//MyVec2f tr(itr->second.GetHighPos()[0], itr->second.GetHighPos()[1]
			//	+ heightWidthRatio*itr->second.GetSize(0));
			MyVec2f tr(itr->second.GetHighPos()[0], itr->second.GetHighPos()[1]
				+ 0.05);
			glColor4f(0, 0, 0, 1);
			//mFont->Render(label, MyBox2f(bl, tr), 0.9);
			mFont->RenderFill(label, MyBox2f(bl, tr), 0.9);
		}
		itr++;
	}

}