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
	//MyMapScPtr<const MySegmentNode*, MyBox2f> boxes
	//	= mSegNodeInfoAssembles.front()->GetSegment2DBoxes();
	//mLayoutManager->SetBoxesIn(boxes);
	//mLayoutManager->Update();
	MyMapScPtr<const MySegmentNode*, MyBox2f> boxesOut = mLayoutManager->GetBoxesOut();
	MyMapSPtr<int, MyBox2f> objBoxes = std::make_shared<MyMap<int, MyBox2f>>();
	mLabelManager->SetBoxes(boxesOut);
	mLabelManager->Update();

	// use shader
	LoadGeometry();
}

void MySegsPlanarDrawer::Render(int winWidth, int winHeight){
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int numBoxes = mSegAssembleGroup->front()->GetSegment2DBoxes()->size();
	glUseProgram(mShaderProgram);

	int mvmatLocation = glGetUniformLocation(mShaderProgram, "mvMat");
	float modelViewMat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMat);
	glUniformMatrix4fv(mvmatLocation, 1, GL_FALSE, modelViewMat);

	int projmatLocation = glGetUniformLocation(mShaderProgram, "projMat");
	float projMat[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projMat);
	glUniformMatrix4fv(projmatLocation, 1, GL_FALSE, projMat);

	glBindVertexArray(mVertexArray);
	for (int iStart = 0; iStart <(mVertices.size() - numBoxes * 4) * sizeof(int); iStart += 200 * sizeof(int)){
		glDrawElements(GL_QUAD_STRIP, 200, GL_UNSIGNED_INT, (const void *)iStart);
	}
	glDrawElements(GL_QUADS, numBoxes * 4, GL_UNSIGNED_INT,
		(const void *)((mVertices.size() - numBoxes * 4) * sizeof(int)));
	glBindVertexArray(0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	DrawBoundary();
	for (int i = 0; i < mSegAssembleGroup->size(); i++){
		DrawDistribution(i);
	}
	glPopAttrib();
	mLabelManager->Render(winWidth, winHeight);
}

void MySegsPlanarDrawer::Resize(int width, int height){
	mLabelManager->SetWindowSize(width, height);
	mLabelManager->Update();
}

void MySegsPlanarDrawer::CompileShader(int shader){
	glDeleteProgram(mShaderProgram);
	mShaderProgram = InitShader("Shaders\\base.vert", "Shaders\\base.frag", "fragColour", "name");


	mPositionAttribute = glGetAttribLocation(mShaderProgram, "position");
	mColorAttribute = glGetAttribLocation(mShaderProgram, "color");
	mNameAttribute = glGetAttribLocation(mShaderProgram, "name");
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
	float offsetAlpha = 0.4;
	float offsetBeta = 0.25 / offsetAlpha;
	float radius = length * (offsetBeta+offsetAlpha)/2;
	MyVec3f center = midWay.toDim<3>(0) + radDir*(offsetBeta*length-radius);
	float startAngle = atan2(fromPos[1] - center[1], fromPos[0] - center[0]);
	float endAngle = atan2(toPos[1] - center[1], toPos[0] - center[0]);
	if (abs(endAngle + 2 * 3.1415926 - startAngle) < abs(endAngle - startAngle)){
		endAngle += 2 * 3.1415926;
	}
	int numStep = 100;
	float delAngle = (endAngle - startAngle) / numStep;
	float delWidth = (endWidth - startWidth) / numStep;

	// to add to data arrays
	for (int i = 0; i < numStep; i++){
		float angle = startAngle + i*delAngle;
		float halfWidth = (startWidth + i*delWidth) / 2;
		MyVec2f leftPos(center[0] + cos(angle)*(radius - halfWidth),
			center[1] + sin(angle)*(radius - halfWidth));
		MyVec2f RightPos(center[0] + cos(angle)*(radius + halfWidth),
			center[1] + sin(angle)*(radius + halfWidth));
		mVertices << leftPos << RightPos;
		mColors << color << color;
		mNames << name << name;
	}
}

void MySegsPlanarDrawer::LoadGeometry(){
	// vertex
	mVertices.clear();
	mColors.clear();
	mNames.clear();
	MyMapScPtr<const MySegmentNode*, MyBox2f> boxes
		= mLayoutManager->GetBoxesOut();
	MyMapScPtr<const MySegmentNode*, MyVec4f> colors
		= mSegAssembleGroup->front()->GetSegment2DColors();
	//glBegin(GL_LINES);
	if (mSegTrkNetwork){
		MyMatrixfScPtr cMat = mSegTrkNetwork->GetConnectionMatrix();
		for (int i = 0; i < cMat->GetNumCols(); i++){
			const MySegmentNode* iNode = mSegTrkNetwork->GetSegmentNodeInfos()->at(i)->GetSegmentNode().get();
			int iIdx = iNode->GetIndex();
			float regionTrks = cMat->At(i, i);
			for (int j = 0; j < cMat->GetNumRows(); j++){
				const MySegmentNode* jNode = mSegTrkNetwork->GetSegmentNodeInfos()->at(j)->GetSegmentNode().get();
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
						fromAnchor = MyVec2f(boxes->at(iNode).GetCenter()[0], boxes->at(iNode).GetHighPos()[1]);
						toAnchor = MyVec2f(boxes->at(jNode).GetCenter()[0], boxes->at(jNode).GetHighPos()[1]);
					}
					else{
						fromAnchor = MyVec2f(boxes->at(iNode).GetCenter()[0], boxes->at(iNode).GetLowPos()[1]);
						toAnchor = MyVec2f(boxes->at(jNode).GetCenter()[0], boxes->at(jNode).GetLowPos()[1]);
						DrawArrow(fromAnchor, toAnchor, color, MyVec4i(iIdx, jIdx, iIdx, jIdx), 0);
					}
					//DrawArrow(fromAnchor, toAnchor, color, MyVec4i(iIdx, jIdx, iIdx, jIdx), ratio / 1000);
					//DrawArrow(fromAnchor, toAnchor, color, MyVec4i(iIdx, jIdx, iIdx, jIdx), 0);
				}
			}
		}
	}
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = boxes->begin();
	while (itr != boxes->end()){
		const MyBox2f &box = itr->second;
		int idx = itr->first->GetIndex();
		MyVec2f highPos = box.GetHighPos();
		MyVec2f lowPos = box.GetLowPos();
		MyVec4f color = colors->at(itr->first);
		if (mSegNodeColor){
			color = mSegNodeColor->at(itr->first);
		}
		MyVec4i name(idx, idx, idx, idx);
		mVertices << lowPos;
		mVertices << MyVec2f(highPos[0], lowPos[1]);
		mVertices << highPos;
		mVertices << MyVec2f(lowPos[0], highPos[1]);
		mColors << color  << color << color << color;
		mNames << name << name << name << name;
		itr++;
	}

	if (glIsVertexArray(mVertexArray)){
		glDeleteVertexArrays(1, &mVertexArray);
	}
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);
	// vertex
	if (glIsBuffer(mPositionBuffer)){
		glDeleteBuffers(1, &mPositionBuffer);
	}
	glGenBuffers(1, &mPositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mPositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(MyVec2f), &mVertices[0][0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(mPositionAttribute);
	glVertexAttribPointer(mPositionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// color
	if (glIsBuffer(mColorBuffer)){
		glDeleteBuffers(1, &mColorBuffer);
	}
	glGenBuffers(1, &mColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, mColors.size() * sizeof(MyVec4f), &mColors[0][0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(mColorAttribute);
	glVertexAttribPointer(mColorAttribute, 4, GL_FLOAT, GL_FALSE, 0, 0);
	// name
	if (glIsBuffer(mNameBuffer)){
		glDeleteBuffers(1, &mNameBuffer);
	}
	glGenBuffers(1, &mNameBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mNameBuffer);
	glBufferData(GL_ARRAY_BUFFER, mNames.size() * sizeof(MyVec4i), &mNames[0][0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(mNameAttribute);
	glVertexAttribIPointer(mNameAttribute, 4, GL_INT, 0, 0);

	if (glIsBuffer(mIndexBuffer)){
		glDeleteBuffers(1, &mIndexBuffer);
	}
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	MyArrayi index = MyArrayi::GetSequence(0, mVertices.size() - 1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(int), &index[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void MySegsPlanarDrawer::DrawBoundary(){
	MyMapScPtr<const MySegmentNode*, MyBox2f> boxes
		= mLayoutManager->GetBoxesOut();
	MyMapScPtr<const MySegmentNode*, MyVec4f> colors
		= mSegAssembleGroup->front()->GetSegment2DColors();
	//glBegin(GL_LINES);
	if (mSegTrkNetwork){
		MyMatrixfScPtr cMat = mSegTrkNetwork->GetConnectionMatrix();
		for (int i = 0; i < cMat->GetNumCols(); i++){
			const MySegmentNode* iNode = mSegTrkNetwork->GetSegmentNodeInfos()->at(i)->GetSegmentNode().get();
			int iIdx = iNode->GetIndex();
			float regionTrks = cMat->At(i, i);
			if (regionTrks == 0) continue;
			for (int j = 0; j < cMat->GetNumRows(); j++){
				const MySegmentNode* jNode = mSegTrkNetwork->GetSegmentNodeInfos()->at(j)->GetSegmentNode().get();
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
						fromAnchor = MyVec2f(boxes->at(iNode).GetCenter()[0], boxes->at(iNode).GetHighPos()[1]);
						toAnchor = MyVec2f(boxes->at(jNode).GetCenter()[0], boxes->at(jNode).GetHighPos()[1]);
					}
					else{
						fromAnchor = MyVec2f(boxes->at(iNode).GetCenter()[0], boxes->at(iNode).GetLowPos()[1]);
						toAnchor = MyVec2f(boxes->at(jNode).GetCenter()[0], boxes->at(jNode).GetLowPos()[1]);
						DrawArrowBoundary(fromAnchor, toAnchor, color, MyVec4i(iIdx, jIdx, iIdx, jIdx), 0);
					}
					//DrawArrowBoundary(fromAnchor, toAnchor, color, MyVec4i(iIdx, jIdx, iIdx, jIdx), ratio / 1000);
					//DrawArrowBoundary(fromAnchor, toAnchor, color, MyVec4i(iIdx, jIdx, iIdx, jIdx), 0);
				}
			}
		}
	}
	glPopAttrib();
	glColor4f(0, 0, 0, 0.8);
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = boxes->begin();
	while (itr != boxes->end()){
		const MyBox2f &box = itr->second;
		int idx = itr->first->GetIndex();
		MyVec2f highPos = box.GetHighPos();
		MyVec2f lowPos = box.GetLowPos();
		glBegin(GL_LINE_LOOP);
		glVertex2f(lowPos[0], lowPos[1]);
		glVertex2f(highPos[0], lowPos[1]);
		glVertex2f(highPos[0], highPos[1]);
		glVertex2f(lowPos[0], highPos[1]);
		glEnd();
		itr++;
	}
	glLineWidth(1);
}

void MySegsPlanarDrawer::DrawArrowBoundary(const MyVec2f fromPos, const MyVec2f toPos,
	const MyVec4f color, const MyVec4i name, float startWidth, float endWidth){
	if (fromPos == toPos) return;
	MyVec2f midWay = (fromPos + toPos) / 2;
	MyVec2f diff = toPos - fromPos;
	float length = diff.norm();
	MyVec2f dir = diff / length;
	MyVec3f dir3 = dir.toDim<3>(0);
	MyVec3f up(0, 0, 1);
	MyVec3f radDir = dir3^up;
	float offsetAlpha = 0.4;
	float offsetBeta = 0.25 / offsetAlpha;
	float radius = length * (offsetBeta + offsetAlpha) / 2;
	MyVec3f center = midWay.toDim<3>(0) + radDir*(offsetBeta*length - radius);
	float startAngle = atan2(fromPos[1] - center[1], fromPos[0] - center[0]);
	float endAngle = atan2(toPos[1] - center[1], toPos[0] - center[0]);
	if (abs(endAngle + 2 * 3.1415926 - startAngle) < abs(endAngle - startAngle)){
		endAngle += 2 * 3.1415926;
	}
	int numStep = 100;
	float delAngle = (endAngle - startAngle) / numStep;
	float delWidth = (endWidth - startWidth) / numStep;
	
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i <= numStep; i++){
		glColor4f(color[0], color[1], color[2], 1.f - (i / (float)numStep));
		float angle = startAngle + i*delAngle;
		float halfWidth = (startWidth + i*delWidth) / 2;
		MyVec2f leftPos(center[0] + cos(angle)*(radius - halfWidth),
			center[1] + sin(angle)*(radius - halfWidth));
		MyGraphicsTool::Vertex(leftPos);
	}
	for (int i = numStep; i >= 0; i--){
		glColor4f(color[0], color[1], color[2], 1.f - (i / (float)numStep));
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
	//ColorScaleTable::CategoricalColor(idx, 0, mSegNodeInfoAssembles.size() - 1, color);
	//glColor4fv(color);
	glColor4f(0, 0, 0, 1);
	MyArrayScPtr<MySegmentNodeInfoScPtr> segNodeInfos 
		= mSegAssembleGroup->at(idx)->GetSegmentNodeInfos();
	MyMapScPtr<const MySegmentNode*, MyBox2f> boxes = mLayoutManager->GetBoxesOut();
	for (int i = 0; i < segNodeInfos->size(); i++){
		const MySegmentNode* node = segNodeInfos->at(i)->GetSegmentNode().get();
		float mean = segNodeInfos->at(i)->GetSegmentNodeMeanAverage();
		float stdev = segNodeInfos->at(i)->GetSegmentNodeMeanStdev();
		float stdev2 = stdev*stdev;
		MyVec2f base = boxes->at(node).GetLowPos();
		float maxHeight = boxes->at(node).GetSize(0);
		float maxWidth = boxes->at(node).GetSize(1);
		glBegin(GL_LINE_LOOP);
		for (int ic = -steps; ic <= steps; ic++){
			float diff = ic / (float)steps * 3;
			float height = exp(-diff*diff)*maxHeight;
			glVertex2f(base[0] + height, base[1] + (diff*stdev + mean)*maxWidth);
		}
		glEnd();
	}
	glColor4f(0, 0, 1, 0.5);
	if (idx != 0) glColor4f(1, 0, 0, 0.5);
	for (int i = 0; i < segNodeInfos->size(); i++){
		const MySegmentNode* node = segNodeInfos->at(i)->GetSegmentNode().get();
		float mean = segNodeInfos->at(i)->GetSegmentNodeMeanAverage();
		float stdev = segNodeInfos->at(i)->GetSegmentNodeMeanStdev();
		float stdev2 = stdev*stdev;
		MyVec2f base = boxes->at(node).GetLowPos();
		float maxHeight = boxes->at(node).GetSize(0);
		float maxWidth = boxes->at(node).GetSize(1);
		glBegin(GL_QUAD_STRIP);
		for (int ic = -steps; ic <= steps; ic++){
			float diff = ic / (float)steps * 3;
			float height = exp(-diff*diff)*maxHeight;
			glVertex2f(base[0] , base[1] + (diff*stdev + mean)*maxWidth);
			glVertex2f(base[0] + height, base[1] + (diff*stdev + mean)*maxWidth);
		}
		glEnd();
	}

	// draw text
	glColor4f(0, 0, 0, 1);
	for (int i = 0; i < segNodeInfos->size(); i++){
		const MySegmentNode* node = segNodeInfos->at(i)->GetSegmentNode().get();
		MyVec2f base = boxes->at(node).GetLowPos();
		float maxHeight = boxes->at(node).GetSize(0);
		float maxWidth = boxes->at(node).GetSize(1);
		glColor4f(0, 0, 0, 0.5);
		glRasterPos2f(base[0] - maxHeight / 2, base[1]);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '0');
		glRasterPos2f(base[0] - maxHeight / 2, base[1] + maxWidth);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, '1');
		float tScore = mSegAssembleGroup->GetTScores().at(node);
		MyString tStr(tScore);
		tStr.resize(4);
		if (tScore < 0){
			glColor4f(1, 0, 0, 1);
		}
		else{
			glColor4f(0, 0, 1, 1);
		}
		glRasterPos2f(base[0], base[1] - maxWidth / 10);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)tStr.c_str());
	}
}