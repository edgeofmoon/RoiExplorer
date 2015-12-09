#include "MyJoinTreeView.h"
#include "MyGraphicsTool.h"
#include "MyGLHeader.h"

// debug
#include <iostream>
using namespace std;

MyJoinTreeView::MyJoinTreeView()
{
	mClipBox.Set(MyVec2f(-0.02, -0.02), MyVec2f(1.02, 1.02));
}


MyJoinTreeView::~MyJoinTreeView()
{
}

void MyJoinTreeView::Render(){
	mCanvas.On();
	mCanvas.Clear();

	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	RenderFrame();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//MyMatrixf projMat = MyMatrixf::OrthographicMatrix(0, 1, 0, 1, -1, 1);
	//MyGraphicsTool::LoadProjectionMatrix(&projMat);
	glOrtho(mClipBox.GetLowPos()[0], mClipBox.GetHighPos()[0], 
		mClipBox.GetLowPos()[1], mClipBox.GetHighPos()[1], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	mJoinTreeDrawer->Render();
	mBloomDrawer->Render();
	mStatisticsDrawer->Render();
	RenderMarks();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();

	mCanvas.Off();
	mCanvas.Show(MyView::GetViewport());
}

MyVec4i MyJoinTreeView::GetName(const MyVec2i& pixelPos) const{
	const MySegmentNode* seg = this->ComputeSegmentNodeAt(pixelPos);
	return this->GetSegmentNodeName(seg);
}

MyVec2i MyJoinTreeView::ComputePixelPosition(const MyVec2f& geoPos) const{
	MyVec2f normPos = mClipBox.ComputeNormalizedPosition(geoPos);
	return MyBox2i::ComputeDenormalizedPosition(normPos);
}

MyVec2f MyJoinTreeView::ComputeGeometryPosition(const MyVec2i& pixPos) const{
	MyVec2f normPos = MyBox2i::ComputeNormalizedPosition(pixPos);
	return mClipBox.ComputeDenormalizedPosition(normPos);
}

const MySegmentNode* MyJoinTreeView::ComputeSegmentNodeAt(const MyVec2i& pixelPos) const{
	if (!this->IsIntersected(pixelPos)){
		return 0;
	}
	MyVec2f pos = this->ComputeGeometryPosition(pixelPos);
	MyJoinTreeLayoutSPtr layout = mJoinTreeDrawer->GetLayout();
	const MyMap<const MySegmentNode*, MyBox2f>& positions = layout->GetPositions();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr;
	for (itr = positions.begin(); itr != positions.end(); itr++){
		if (itr->second.IsIntersected(pos)){
			return itr->first;
		}
	}
	return 0;
}

MyVec4i MyJoinTreeView::GetSegmentNodeName(const MySegmentNode* seg) const{
	int index = -1;
	if (seg){
		index = seg->GetIndex();
	}
	return MyVec4i(mIndex, index, index, index);
}

const MySegmentNode* MyJoinTreeView::GetSegmentNodeByName(const MyVec4i& name) const{
	if (name[0] != mIndex || name[1] < 0 || name[2] < 0 || name[3] < 0) return 0;
	MyJoinTreeLayoutSPtr layout = mJoinTreeDrawer->GetLayout();
	const MyMap<const MySegmentNode*, MyBox2f>& positions = layout->GetPositions();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr;
	const MySegmentNode* seg = 0;
	for (itr = positions.begin(); itr != positions.end(); itr++){
		if (itr->first->GetIndex()==name[1]){
			return itr->first;
		}
	}
	return 0;
}

int MyJoinTreeView::HandleMouseBottonEvent(int button, int state, int x, int y){
	mMouseAnchor = this->ComputeGeometryPosition(MyVec2i(x, y));
	return 1;
}

int MyJoinTreeView::HandleMouseWheelEvent(int button, int dir, int x, int y){
	MyVec2f current = this->ComputeGeometryPosition(MyVec2i(x, y));
	if (dir > 0){
		mClipBox.ScaleAtPosition(current, MyVec2f(0.95, 1));
	}
	else{
		mClipBox.ScaleAtPosition(current, MyVec2f(1 / 0.95, 1));
	}
	return 1;
}

int MyJoinTreeView::HandleMouseMoveEvent(int x, int y){
	MyVec2f current = this->ComputeGeometryPosition(MyVec2i(x, y));
	MyVec2f offset = mMouseAnchor - current;
	offset[1] = 0;
	mClipBox += offset;
	mMouseAnchor = this->ComputeGeometryPosition(MyVec2i(x, y));
	return 1;
}

int MyJoinTreeView::HandleResizeEvent(){
	MyView::HandleResizeEvent();
	return 1;
}

void MyJoinTreeView::RenderMarks() const{
	glColor4f(0, 0, 0, 1);
	glLineWidth(2);
	MyMap<const MySegmentNode*, float>::const_iterator itr;
	for (itr = mIsoMarks.begin(); itr != mIsoMarks.end(); itr++){
		const MyBox2f& box = mJoinTreeDrawer->GetLayout()->GetPosition(itr->first);
		glBegin(GL_LINES);
		glVertex3f(box.GetLowPos()[0], itr->second, 0.99);
		glVertex3f(box.GetHighPos()[0], itr->second, 0.99);
		glEnd();
	}
	glLineWidth(1);
}

void MyJoinTreeView::RenderFrame(){
	float boarder_x = 0.00001;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-boarder_x * 2, 1 + boarder_x * 2,
		mClipBox.GetLowPos()[1], mClipBox.GetHighPos()[1], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor4f(0, 0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex3f(-boarder_x, 0, 0.99);
	glVertex3f(1 + boarder_x, 0, 0.99);
	glVertex3f(1 + boarder_x, 1, 0.99);
	glVertex3f(-boarder_x, 1, 0.99);
	glEnd();

	glBegin(GL_LINES);
	for (int i = 1; i <= 49; i++){
		float height = i*0.02;
		if (i % 5 == 0){
			glColor4f(0.5, 0.5, 0.5, 0.8);
			glLineWidth(2);
		}
		else{
			glColor4f(0.8, 0.8, 0.8, 0.3);
			glLineWidth(0.5);
		}
		glVertex2f(-boarder_x, height);
		glVertex2f(1 + boarder_x, height);
	}
	glEnd();
	glColor4f(0, 0, 0, 1);
	for (int i = 0; i <= 10; i++){
		glRasterPos2f(-boarder_x, i*0.1);
		string str = to_string(i*0.1);
		str.resize(3);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)str.c_str());
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	
}

MyMapSPtr<const MySegmentNode*, float> MyJoinTreeView::ComputeSeparableMarks() const{
	MyMapSPtr<const MySegmentNode*, float> sMarks
		= std::make_shared<MyMap<const MySegmentNode*, float>>(mIsoMarks);
	MyMap<const MySegmentNode*, float>::iterator itr = sMarks->begin();
	while (itr != sMarks->end()){
		const MySegmentNode* node = itr->first;
		// find if already has ancestor
		while (node->GetNumParents()!=0){
			node = node->GetParent().get();
			if (sMarks->HasKey(node)){
				const MySegmentNode* theNode = itr->first;
				// first advance
				itr++;
				// then remove, bc removal invalidates itr
				sMarks->erase(theNode);
				// go to next in queue
				continue;
			}
		}
		itr++;
	}
	return sMarks;
}