#include "MySegsPlanarDrawer.h"
#include "MyGLHeader.h"
#include "MyGraphicsTool.h"
#include "Shader.h"
#include "ColorScaleTable.h"
#include <iostream>
using namespace std;

MySegsPlanarDrawer::MySegsPlanarDrawer()
{
	mLabelManager = std::make_shared<MyLabelManager>();
	mLinkDrawThreshold = 0.5;
}


MySegsPlanarDrawer::~MySegsPlanarDrawer()
{
}

void MySegsPlanarDrawer::SetLabels(MyMapScPtr<int, MyString> labels){
	mLabelManager->SetLabels(labels);
}

void MySegsPlanarDrawer::Update(){
	MyMapScPtr<const MySegmentNode*, MyBox2f> boxesOut = mLayoutManager->GetBoxesOut();
	mLabelManager->SetBoxes(boxesOut);
	mLabelManager->Update();
	mHistogramRange = mSegAssembleGroup->GetThreeSigmaRange();
	// round range
	mHistogramRange[1] = ceil(mHistogramRange[1] * 10) / 10;
	mHistogramRange[0] = floor(mHistogramRange[0] * 10) / 10;
}

void MySegsPlanarDrawer::Render(){
	this->DrawBoxes();
	for (int i = 0; i < mSegAssembleGroup->size(); i++){
		this->DrawDistribution(i);
	}
	this->DrawNetwork();
	mLabelManager->Render();
}

void MySegsPlanarDrawer::Resize(int width, int height){
	mLabelManager->SetWindowSize(width, height);
	mLabelManager->Update();
}

void MySegsPlanarDrawer::DrawNetwork(){
	if (mSegTrkNetwork){
		MyMapScPtr<const MySegmentNode*, MyBox2f> boxes
			= mLayoutManager->GetBoxesOut();
		MyMatrixfScPtr cMat = mSegTrkNetwork->GetConnectionMatrix();
		for (int i = 0; i < cMat->GetNumCols(); i++){
			const MySegmentNode* iNode 
				= mSegTrkNetwork->GetSegmentNodeInfos()->at(i)->GetSegmentNode().get();
			int iIdx = iNode->GetIndex();
			float regionTrks = cMat->At(i, i);
			if (regionTrks == 0) continue;
			for (int j = 0; j < cMat->GetNumRows(); j++){
				const MySegmentNode* jNode 
					= mSegTrkNetwork->GetSegmentNodeInfos()->at(j)->GetSegmentNode().get();
				int jIdx = jNode->GetIndex();
				if (i == j) continue;
				float conTrks = cMat->At(i, j);
				float ratio = conTrks / regionTrks;
				if (ratio > mLinkDrawThreshold){
					//MyVec4f color(1 - ratio, ratio, 0, 0.5);
					MyVec4f color(0, 0, 0, 0.5);
					MyVec2f fromAnchor;
					MyVec2f toAnchor;
					if (boxes->at(iNode).GetLowPos()[0] < boxes->at(jNode).GetLowPos()[0]){
						//fromAnchor = MyVec2f(boxes->at(iNode).GetCenter()[0], boxes->at(iNode).GetHighPos()[1]);
						//toAnchor = MyVec2f(boxes->at(jNode).GetCenter()[0], boxes->at(jNode).GetHighPos()[1]);
						toAnchor = MyVec2f(boxes->at(iNode).GetCenter()[0], boxes->at(iNode).GetLowPos()[1]);
						fromAnchor = MyVec2f(boxes->at(jNode).GetCenter()[0], boxes->at(jNode).GetLowPos()[1]);
					}
					else{
						fromAnchor = MyVec2f(boxes->at(iNode).GetCenter()[0], boxes->at(iNode).GetLowPos()[1]);
						toAnchor = MyVec2f(boxes->at(jNode).GetCenter()[0], boxes->at(jNode).GetLowPos()[1]);
					}
					DrawArrow(fromAnchor, toAnchor, color, MyVec4i(iIdx, jIdx, iIdx, jIdx), 0);
				}
			}
		}
	}
}

void MySegsPlanarDrawer::DrawBoxes(){
	MyMapScPtr<const MySegmentNode*, MyBox2f> boxes
		= mLayoutManager->GetBoxesOut();
	MyMapScPtr<const MySegmentNode*, MyVec4f> colors
		= mSegAssembleGroup->GetRoiColors();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = boxes->begin();
	while (itr != boxes->end()){
		MyBox2f box = itr->second;
		int idx = itr->first->GetIndex();
		MyVec2f highPos = box.GetHighPos();
		MyVec2f lowPos = box.GetLowPos();
		glColor4f(0, 0, 0, 0.8f);
		glBegin(GL_LINE_LOOP);
		glVertex3f(lowPos[0], lowPos[1], 0.7);
		glVertex3f(highPos[0], lowPos[1], 0.7);
		glVertex3f(highPos[0], highPos[1], 0.7);
		glVertex3f(lowPos[0], highPos[1], 0.7);
		glEnd();
		//MyVec4f color = colors->at(itr->first);
		//glColor4fv(&color[0]);
		glColor4f(1, 1, 1, 1);
		glBegin(GL_QUADS);
		glVertex2f(lowPos[0], lowPos[1]);
		glVertex2f(highPos[0], lowPos[1]);
		glVertex2f(highPos[0], highPos[1]);
		glVertex2f(lowPos[0], highPos[1]);
		glEnd();
		itr++;
	}
	glLineWidth(1);
}

void MySegsPlanarDrawer::DrawArrow(const MyVec2f fromPos, const MyVec2f toPos,
	const MyVec4f color, const MyVec4i name, float startWidth, float endWidth){
	if (fromPos == toPos) return;
	MyVec2f midWay = (fromPos + toPos) / 2;
	MyVec2f diff = toPos - fromPos;
	float length = diff.norm();
	MyVec2f dir = diff / length;
	MyVec3f dir3 = dir.toDim<3>(0);
	MyVec3f up(0, 0, 1);
	MyVec3f radDir = dir3^up;
	float offsetAlpha = 0.2f;
	float offsetBeta = 0.25f / offsetAlpha;
	float radius = length * (offsetBeta + offsetAlpha) / 2;
	MyVec3f center = midWay.toDim<3>(0) + radDir*(offsetBeta*length - radius);
	float startAngle = atan2(fromPos[1] - center[1], fromPos[0] - center[0]);
	float endAngle = atan2(toPos[1] - center[1], toPos[0] - center[0]);
	if (abs(endAngle + 2 * 3.1415926f - startAngle) < abs(endAngle - startAngle)){
		endAngle += 2 * 3.1415926f;
	}
	int numStep = 100;
	float delAngle = (endAngle - startAngle) / numStep;
	float delWidth = (endWidth - startWidth) / numStep;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i <= numStep; i++){
		//glColor4f(color[0], color[1], color[2], 1.f - (i / (float)numStep) / 2);
		glColor4f(color[0], color[1], color[2], 1.f);
		float angle = startAngle + i*delAngle;
		float halfWidth = (startWidth + i*delWidth) / 2;
		MyVec2f leftPos(center[0] + cos(angle)*(radius - halfWidth),
			center[1] + sin(angle)*(radius - halfWidth));
		MyGraphicsTool::Vertex(leftPos);
	}
	for (int i = numStep; i >= 0; i--){
		//glColor4f(color[0], color[1], color[2], 1.f - (i / (float)numStep) / 2);
		glColor4f(color[0], color[1], color[2], 1.f);
		float angle = startAngle + i*delAngle;
		float halfWidth = (startWidth + i*delWidth) / 2;
		MyVec2f RightPos(center[0] + cos(angle)*(radius + halfWidth),
			center[1] + sin(angle)*(radius + halfWidth));
		MyGraphicsTool::Vertex(RightPos);
	}
	glEnd();
}

void MySegsPlanarDrawer::DrawDistribution(int idx){
	int steps = 20;
	MyVec2f gRange = mHistogramRange;
	// just use 0~1  range
	//MyVec2f gRange(0.f,1.f);
	float ggRange = gRange[1] - gRange[0];
	MyArrayScPtr<MySegmentNodeInfoScPtr> segNodeInfos
		= mSegAssembleGroup->at(idx)->GetSegmentNodeInfos();
	MyMapScPtr<const MySegmentNode*, MyBox2f> boxes = mLayoutManager->GetBoxesOut();
	MyVec2f tScoreRange = mSegAssembleGroup->GetTScoreRange();
	float tScoreRangeAbs = max(fabs(tScoreRange[0]), fabs(tScoreRange[1]));
	for (int i = 0; i < segNodeInfos->size(); i++){
		const MySegmentNode* node = segNodeInfos->at(i)->GetSegmentNode().get();
		MyBox2f theBox = boxes->at(node);
		float minStd = mSegAssembleGroup->GetStdevRanges().at(node)[0];
		float mean = segNodeInfos->at(i)->GetSegmentNodeMeanAverage();
		float stdev = segNodeInfos->at(i)->GetSegmentNodeMeanStdev();
		float stdev2 = stdev*stdev;
		MyVec2f base = theBox.GetLowPos();
		float maxHeight = theBox.GetSize(0);
		//float maxWidth = theBox.GetSize(1);
		float maxWidth = 0.2;
		glColor4f(0, 0, 0, 1);
		glBegin(GL_LINE_LOOP);
		for (int ic = -steps; ic <= steps; ic++){
			float diff = ic / (float)steps * 3;
			float height = exp(-diff*diff)*maxHeight *(minStd / stdev);
			glVertex3f(base[0] + height,
				base[1] + (diff*stdev + mean - gRange[0]) / ggRange*maxWidth, 0.6);
		}
		glEnd();
		glColor4f(0, 0, 1, 0.5);
		if (idx != 0) glColor4f(1, 0, 0, 0.5);
		glBegin(GL_QUAD_STRIP);
		for (int ic = -steps; ic <= steps; ic++){
			float diff = ic / (float)steps * 3;
			float height = exp(-diff*diff)*maxHeight *(minStd / stdev);
			glVertex3f(base[0],
				base[1] + (diff*stdev + mean - gRange[0]) / ggRange*maxWidth, 0.5);
			glVertex3f(base[0] + height,
				base[1] + (diff*stdev + mean - gRange[0]) / ggRange*maxWidth, 0.5);
		}
		glEnd();

		// draw t score bar charts
		glColor4f(0, 0, 0, 1);
		glBegin(GL_LINE_LOOP);
		glVertex3f(base[0], base[1] + maxWidth, 0.6);
		glVertex3f(base[0] + maxHeight, base[1] + maxWidth, 0.6);
		glVertex3f(base[0] + maxHeight, theBox.GetHighPos()[1], 0.6);
		glVertex3f(base[0], theBox.GetHighPos()[1], 0.6);
		glEnd();
		MyVec4f tScoreColor = mSegAssembleGroup->GetRoiColors()->at(node);
		glColor4fv(&tScoreColor[0]);
		glBegin(GL_QUADS);
		glVertex3f(base[0], base[1] + maxWidth, 0.5);
		glVertex3f(base[0] + maxHeight, base[1] + maxWidth, 0.5);
		glVertex3f(base[0] + maxHeight, theBox.GetHighPos()[1], 0.5);
		glVertex3f(base[0], theBox.GetHighPos()[1], 0.5);
		glEnd();

		// draw text
		// a hack: here small width means the roi is disabled
		if (theBox.GetSize(0) < 0.01) continue;
		glColor4f(0, 0, 0, 1);
		float tScore = mSegAssembleGroup->GetTScores().at(node);
		MyString tStr(tScore);
		tStr.resize(4);
		if (tScore < 0){
			glColor4f(1, 0, 0, 1);
		}
		else{
			glColor4f(0, 0, 1, 1);
		}
		glRasterPos2f(base[0], base[1] - maxWidth / 5);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)tStr.c_str());

	}
}