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

void MyLabelManager::Update(){
	mLabelBoxes->clear();
	MySpaceFillingSpiral spaceFill;

	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = mBoxes->begin();
	while (itr != mBoxes->end()){
		spaceFill.ForceAddBox(itr->second);
		itr++;
	}

	// label
	float cutSize = 0.005;
	float pixelWidth = 1.1f / mWidth;
	float pixelHeight = 1.1f / mHeight * 2; // why times 2?
	void * font = GLUT_BITMAP_HELVETICA_12;

	/*
	MyMap<int, MyString>::const_iterator itrLabel = mLabels->begin();
	while (itrLabel != mLabels->end()){
		int index = itrLabel->first;
		if (mBoxes->HasKey(index)){
			MyBox2f objBox = mBoxes->at(index);
			MyString label = itrLabel->second;
			float length = 0;
			for (int i = 0; i < label.size(); i++){
				length += glutBitmapWidth(font, label[i]);
			}

			MyVec2f lowPos = objBox.GetHighPos() + MyVec2f(-length / 2 * pixelWidth, cutSize);
			MyVec2f highPos = lowPos + MyVec2f(length * pixelWidth, glutBitmapHeight(font)*pixelHeight / 2);
			//MyBox2f box = spaceFill.PushBoxFromTop(MyBox2f(lowPos, highPos), 0.0001);
			MyBox2f box = spaceFill.PushBox(MyBox2f(lowPos, highPos), MyVec2f(0.005, 0));
			mLabelBoxes->operator[](index) = box;
		}
		itrLabel++;
	}
	*/

	itr = mBoxes->begin();
	while (itr != mBoxes->end()){
		MyString label = mLabels->at(itr->first->GetIndex());
		MyBox2f objBox = itr->second;
		float length = 0;
		for (int i = 0; i < label.size(); i++){
			length += glutBitmapWidth(font, label[i]);
		}

		MyVec2f lowPos = objBox.GetHighPos() + MyVec2f(-length / 2 * pixelWidth, cutSize);
		MyVec2f highPos = lowPos + MyVec2f(length * pixelWidth, glutBitmapHeight(font)*pixelHeight / 2);
		//MyBox2f box = spaceFill.PushBoxFromTop(MyBox2f(lowPos, highPos), 0.0001);
		MyBox2f box = spaceFill.PushBox(MyBox2f(lowPos, highPos), MyVec2f(0.005, 0));
		mLabelBoxes->operator[](itr->first) = box;
		itr++;
	}

}

void MyLabelManager::Render(int winWidth, int winHeight){
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = mLabelBoxes->begin();
	while (itr != mLabelBoxes->end()){
		int index = itr->first->GetIndex();
		if (mLabels->HasKey(index)){
			MyString label = mLabels->at(index);
			MyVec2f labelBoxPos = mLabelBoxes->at(itr->first).GetLowPos();
			glColor4f(0, 0, 0, 1);
			glRasterPos2f(labelBoxPos[0], labelBoxPos[1]);
			glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)label.c_str());

			MyBox2f objBox = mBoxes->at(itr->first);
			MyVec2f objBoxPos(objBox.GetLowPos()[0], objBox.GetHighPos()[1]);
			glBegin(GL_LINES);
			glVertex2f(labelBoxPos[0], labelBoxPos[1]);
			glVertex2f(objBoxPos[0], objBoxPos[1]);
			glEnd();
		}
		itr++;
	}

}