#include "MyJoinTreeDrawer.h"
#include "MyGLHeader.h"
#include "MyString.h"

MyJoinTreeDrawer::MyJoinTreeDrawer()
{
}


MyJoinTreeDrawer::~MyJoinTreeDrawer()
{
}

void MyJoinTreeDrawer::SetJoinTree(MyJoinTreeScPtr joinTree){
	mJoinTree = joinTree;
}
void MyJoinTreeDrawer::SetJoinTreeLayout(MyJoinTreeLayoutSPtr layout){
	mLayout = layout;
}
void MyJoinTreeDrawer::Update(){
	mLayout->SetJoinTreeRoot(mJoinTree->GetRoot());
	mLayout->Update();
}
void MyJoinTreeDrawer::Render(int width, int height){
	MyNode::const_TreeIterator itr = mJoinTree->GetRoot()->Begin();
	MyNode::const_TreeIterator itrEnd = mJoinTree->GetRoot()->End();
	glPushAttrib(GL_DEPTH_BITS | GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	while (itr != itrEnd){
		this->RenderSegment(static_cast<const MySegmentNode*>(*itr));
		itr++;
	}
	glPopAttrib();
}

void MyJoinTreeDrawer::RenderSegment(const MySegmentNode* segment){
	MyBox2f box = mLayout->GetPosition(segment);
	if (box.GetSize(1) == 0){
		box.Engulf(box + MyVec2f(0, 0.001));
	};
	int index = mLayout->GetIndex(segment);
	//glColor4fv(&color[0]);
	//glColor4f(0.3, 0.3, 0.8, 0.5);
	/*
	glColor4f(0.3, 0.3, 0.3, 0.5);
	glBegin(GL_QUADS);
	glVertex3f(box.GetLowPos()[0], box.GetLowPos()[1], 0.4);
	glVertex3f(box.GetHighPos()[0], box.GetLowPos()[1], 0.4);
	glVertex3f(box.GetHighPos()[0], box.GetHighPos()[1], 0.4);
	glVertex3f(box.GetLowPos()[0], box.GetHighPos()[1], 0.4);
	glEnd();
	*/
	glColor4f(0, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex3f(box.GetLowPos()[0], box.GetLowPos()[1], 0.5);
	glVertex3f(box.GetHighPos()[0], box.GetLowPos()[1], 0.5);
	glVertex3f(box.GetHighPos()[0], box.GetHighPos()[1], 0.5);
	glVertex3f(box.GetLowPos()[0], box.GetHighPos()[1], 0.5);
	glEnd();
	//glRasterPos3f(box.GetHighPos()[0]+box.GetSize(0)*0.01, box.GetHighPos()[1], 0.7);
	//glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)MyString(index).c_str());

	if (segment->GetNumParents() > 0){
		MyBox2f pbox = mLayout->GetPosition(segment->GetParent().get());
		glBegin(GL_LINE_STRIP);
		//glColor4f(1, 0, 0, 1);
		glVertex2f((box.GetLowPos()[0] + box.GetHighPos()[0]) / 2, box.GetLowPos()[1]);
		//glColor4f(1, 1, 0, 1);
		glVertex2f((box.GetLowPos()[0] + box.GetHighPos()[0]) / 2, pbox.GetHighPos()[1]);
		//glColor4f(0, 1, 0, 1);
		glVertex2f((pbox.GetLowPos()[0] + pbox.GetHighPos()[0]) / 2, pbox.GetHighPos()[1]);
		glEnd();
	}
}