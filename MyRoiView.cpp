#include "MyRoiView.h"
#include "MyGLHeader.h"
#include "MyMathHelper.h"

// debug
#include <iostream>
using namespace std;

MyRoiView::MyRoiView()
{
	mClipBox.Set(MyVec2f(-0.02, -0.02), MyVec2f(1.02, 1.02));
	mStatus = std::make_shared<MyMap<const MySegmentNode*, MyObjectStatus>>();
}


MyRoiView::~MyRoiView()
{
}

void MyRoiView::Render(){
	mCanvas.On();
	mCanvas.Clear();

	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	RenderDistributionFrame();
	RenderStatisticsFrame();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//glOrtho(0, 1, 0, 1, -1, 1);
	glOrtho(mClipBox.GetLowPos()[0], mClipBox.GetHighPos()[0],
		mClipBox.GetLowPos()[1], mClipBox.GetHighPos()[1], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	RenderMarks();
	mRoiDrawer->Render();
	this->RenderBrushLine();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();

	mCanvas.Off();
	mCanvas.Show(MyView::GetViewport());

}

MyVec4i MyRoiView::GetName(const MyVec2i& pixelPos) const{
	const MySegmentNode* seg = this->ComputeSegmentNodeAt(pixelPos);
	return this->GetSegmentNodeName(seg);
}

MyVec2i MyRoiView::ComputePixelPosition(const MyVec2f& geoPos) const{
	MyVec2f normPos = mClipBox.ComputeNormalizedPosition(geoPos);
	return MyBox2i::ComputeDenormalizedPosition(normPos);
}

MyVec2f MyRoiView::ComputeGeometryPosition(const MyVec2i& pixPos) const{
	MyVec2f normPos = MyBox2i::ComputeNormalizedPosition(pixPos);
	return mClipBox.ComputeDenormalizedPosition(normPos);
}

const MySegmentNode* MyRoiView::ComputeSegmentNodeAt(const MyVec2i& pixelPos) const{
	if (!this->IsIntersected(pixelPos)){
		return 0;
	}
	MyVec2f pos = this->ComputeGeometryPosition(pixelPos);
	MySegNodeInfoLayout2DSPtr layout = mRoiDrawer->GetLayoutManager();
	MyMapScPtr<const MySegmentNode*, MyBox2f> positions = layout->GetBoxesOut();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr;
	for (itr = positions->begin(); itr != positions->end(); itr++){
		if (itr->second.IsIntersected(pos)){
			return itr->first;
		}
	}
	return 0;
}

MyVec4i MyRoiView::GetSegmentNodeName(const MySegmentNode* seg) const{
	int index = -1;
	if (seg){
		index = seg->GetIndex();
	}
	return MyVec4i(mIndex, index, index, index);
}

const MySegmentNode* MyRoiView::GetSegmentNodeByName(const MyVec4i& name) const{
	if (name[0] != mIndex || name[1] < 0 || name[2] < 0 || name[3] < 0) return 0;
	MySegNodeInfoLayout2DSPtr layout = mRoiDrawer->GetLayoutManager();
	MyMapScPtr<const MySegmentNode*, MyBox2f> positions = layout->GetBoxesOut();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr;
	for (itr = positions->begin(); itr != positions->end(); itr++){
		if (itr->first->GetIndex() == name[1]){
			return itr->first;
		}
	}
	return 0;
}

int MyRoiView::HandleMouseBottonEvent(int button, int state, int x, int y){
	if (state == GLUT_DOWN){
		if (button == GLUT_LEFT_BUTTON){
			// brush
			MyVec2f geoPos = this->ComputeGeometryPosition(MyVec2i(x, y));
			mBrushTrajectory.Clear();
			mBrushTrajectory.Append(geoPos);
			// push twice to form at least one line
			mBrushTrajectory.Append(geoPos);
			mMouseAction = Action_Brush;
			this->UpdateLastBrushedBoxes();
		}
		else if (button == GLUT_MIDDLE_BUTTON){
			// drag
			mMouseAnchor = this->ComputeGeometryPosition(MyVec2i(x, y));
			mMouseAction = Action_Drag;
		}
		else if (button == GLUT_RIGHT_BUTTON){
			// cancel
			const MySegmentNode* roi = this->ComputeSegmentNodeAt(MyVec2i(x, y));
			if (roi){
				this->UnsetSelect(roi);
				this->Signal_SegmentUnselected(this->GetSegmentNodeName(roi));
			}
			else this->UnselectAll();
			mMouseAction = Action_None;
		}
	}
	else{
		mMouseAction = Action_None;
		mBrushTrajectory.Clear();
	}
	return 1;
}

int MyRoiView::HandleMouseWheelEvent(int button, int dir, int x, int y){
	MyVec2f current = this->ComputeGeometryPosition(MyVec2i(x, y));
	if (dir > 0){
		mClipBox.ScaleAtPosition(current, MyVec2f(0.95, 1));
	}
	else{
		mClipBox.ScaleAtPosition(current, MyVec2f(1 / 0.95, 1));
	}
	return 1;
}

int MyRoiView::HandleMouseMoveEvent(int x, int y){
	if (mMouseAction == Action_Drag){
		MyVec2f current = this->ComputeGeometryPosition(MyVec2i(x, y));
		MyVec2f offset = mMouseAnchor - current;
		offset[1] = 0;
		mClipBox += offset;
		mMouseAnchor = this->ComputeGeometryPosition(MyVec2i(x, y));
	}
	else if (mMouseAction == Action_Brush){
		MyVec2f geoPos = this->ComputeGeometryPosition(MyVec2i(x, y));
		mBrushTrajectory.Append(geoPos);
		this->UpdateLastBrushedBoxes();
	}
	return 1;
}

int MyRoiView::HandleResizeEvent(){
	MyView::HandleResizeEvent();
	mRoiDrawer->Resize(MyBox2i::GetSize(0), MyBox2i::GetSize(1));
	return 1;
}

void MyRoiView::UpdateLastBrushedBoxes(){
	int segIdx = mBrushTrajectory.GetNumLineSegments() - 1;
	MyLine2f line = mBrushTrajectory.GetLineSegment(segIdx);
	MySegNodeInfoLayout2DSPtr layout = mRoiDrawer->GetLayoutManager();
	MyMapScPtr<const MySegmentNode*, MyBox2f> positions = layout->GetBoxesOut();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr;
	for (itr = positions->begin(); itr != positions->end(); itr++){
		if (this->IsSelected(itr->first)) continue;
		else if (MyMathHelper::IsIntersected(itr->second, line)){
			this->SetSelect(itr->first);
			this->Signal_SegmentSelected(this->GetSegmentNodeName(itr->first));
		}
	}
}

void MyRoiView::UnselectAll(){
	MySegNodeInfoLayout2DSPtr layout = mRoiDrawer->GetLayoutManager();
	MyMapScPtr<const MySegmentNode*, MyBox2f> positions = layout->GetBoxesOut();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr;
	for (itr = positions->begin(); itr != positions->end(); itr++){
		if (!this->IsSelected(itr->first)) continue;
		else {
			this->UnsetSelect(itr->first);
			this->Signal_SegmentUnselected(this->GetSegmentNodeName(itr->first));
		}
	}
}

void MyRoiView::RenderBrushLine(){
	glLineWidth(4);
	glColor4f(0, 0, 1, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < mBrushTrajectory.GetNumPoints(); i++){
		MyVec3f vertex = mBrushTrajectory.GetPoint(i).toDim<3>(0.999);
		glVertex3fv(&vertex[0]);
	}
	glEnd();
	glLineWidth(1);
}

void MyRoiView::RenderDistributionFrame(){
	MyVec2f boxRange_Y = mRoiDrawer->GetLayoutManager()->GetBaseBoxVerticalRange();
	float boarder_x = 0.00001;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-boarder_x * 2, 1 + boarder_x * 2,
		mClipBox.GetLowPos()[1], mClipBox.GetHighPos()[1], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	MyVec2f range = mRoiDrawer->GetHistogramRange();
	float rangeSize = range[1] - range[0];
	float step = 0.1;
	float subStep = step / 5;
	float nStep = round(rangeSize / subStep);
	float subStepHeight = (boxRange_Y[1] - boxRange_Y[0]) / nStep;

	//cout << "substepSize: " << subStep << endl;
	//cout << "subStepHeight: " << subStepHeight << endl;
	for (int i = 0; i <= nStep; i++){
		float height = boxRange_Y[0] + subStepHeight*i;
		if (i % 5 == 0){
			glColor4f(0.3, 0.3, 3.f, 1.f);
			glRasterPos2f(-boarder_x, height);
			string str = to_string(range[0] + subStep*i);
			str.resize(3);
			glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)str.c_str());

			glColor4f(0.5, 0.5, 0.5, 0.8);
			glLineWidth(1);
		}
		else{
			glColor4f(0.8, 0.8, 0.8, 0.3);
			glLineWidth(0.5);
		}
		glBegin(GL_LINES);
		glVertex3f(-boarder_x, height, -0.1);
		glVertex3f(1 + boarder_x, height, -0.1);
		glEnd();
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void MyRoiView::RenderStatisticsFrame(){
	float baseY = mRoiDrawer->GetLayoutManager()->GetBaseBoxVerticalRange()[1];
	float topY = baseY + mRoiDrawer->GetLayoutManager()->GetBarChartHeight();
	MyVec2f boxRange_Y(baseY, topY);
	float boarder_x = 0.00001;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-boarder_x * 2, 1 + boarder_x * 2,
		mClipBox.GetLowPos()[1], mClipBox.GetHighPos()[1], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// t score starts with 0
	MyVec2f statisticsRange = mRoiDrawer->GetSegmentAssembleGroup()->GetTScoreRange();
	MyVec2f absStatisticsSizeRange(0, std::max(fabs(statisticsRange[0]), fabs(statisticsRange[1])));
	int magnitude = floor(log10f(absStatisticsSizeRange[1]));
	float step = pow(10.f, magnitude);
	float subStep = step / 5;
	float nStep = round(absStatisticsSizeRange[1] / subStep);
	float subStepHeight = (boxRange_Y[1] - boxRange_Y[0]) / nStep;

	//cout << "substepSize: " << subStep << endl;
	//cout << "subStepHeight: " << subStepHeight << endl;
	// starts from 1 to avoid overlapping
	for (int i = 1; i <= nStep; i++){
		float height = boxRange_Y[0] + subStepHeight*i;
		if (i % 5 == 0){
			glColor4f(1.f, 0.3, 0.3, 1.f);
			glRasterPos2f(-boarder_x, height);
			string str = to_string(subStep*i);
			str.resize(3);
			glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)str.c_str());

			glColor4f(0.5, 0.5, 0.5, 0.8);
			glLineWidth(1);
		}
		else{
			glColor4f(0.8, 0.8, 0.8, 0.3);
			glLineWidth(0.5);
		}
		glBegin(GL_LINES);
		glVertex3f(-boarder_x, height, -0.1);
		glVertex3f(1 + boarder_x, height, -0.1);
		glEnd();
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void MyRoiView::RenderMarks() const{
	glLineWidth(3);
	glColor4f(0.f, 0.f, 0.f, 1);
	MyMap<const MySegmentNode*, MyObjectStatus>::const_iterator itr;
	for (itr = mStatus->begin(); itr != mStatus->end(); itr++){
		if (itr->second.IsBitSet(MyObjectStatus::STATUS_SELECT_BIT)){
			MyBox2f box = mRoiDrawer->GetLayoutManager()->GetBoxesOut()->at(itr->first);
			glBegin(GL_LINE_LOOP);
			glVertex3f(box.GetLowPos()[0], box.GetLowPos()[1], 0.8);
			glVertex3f(box.GetHighPos()[0], box.GetLowPos()[1], 0.8);
			glVertex3f(box.GetHighPos()[0], box.GetHighPos()[1], 0.8);
			glVertex3f(box.GetLowPos()[0], box.GetHighPos()[1], 0.8);
			glEnd();
			/*
			box.ScaleAtCenter(MyVec2f(1.1, 1.1));
			glBegin(GL_TRIANGLE_FAN);
			glColor4f(1, 1, 1, 1);
			glVertex3f(box.GetCenter()[0], box.GetCenter()[1], -0.1);
			glVertex3f(box.GetLowPos()[0], box.GetLowPos()[1], -0.1);
			glVertex3f(box.GetHighPos()[0], box.GetLowPos()[1], -0.1);
			glVertex3f(box.GetHighPos()[0], box.GetHighPos()[1], -0.1);
			glVertex3f(box.GetLowPos()[0], box.GetHighPos()[1], -0.1);
			glVertex3f(box.GetLowPos()[0], box.GetLowPos()[1], -0.1);
			glEnd();
			*/
			box.ScaleAtCenter(MyVec2f(1.05, 1.02));
			glBegin(GL_TRIANGLE_FAN);
			glVertex3f(box.GetCenter()[0], box.GetCenter()[1], -0.05);
			glVertex3f(box.GetLowPos()[0], box.GetLowPos()[1], -0.05);
			glVertex3f(box.GetHighPos()[0], box.GetLowPos()[1], -0.05);
			glVertex3f(box.GetHighPos()[0], box.GetHighPos()[1], -0.05);
			glVertex3f(box.GetLowPos()[0], box.GetHighPos()[1], -0.05);
			glVertex3f(box.GetLowPos()[0], box.GetLowPos()[1], -0.05);
			glEnd();
		}
	}
	glLineWidth(1);
}