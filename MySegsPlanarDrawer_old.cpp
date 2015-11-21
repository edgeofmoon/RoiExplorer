#include "MySegsPlanarDrawer.h"
#include "MyGLHeader.h"
#include "MyGraphicsTool.h"
#include "Shader.h"

#include <iostream>
using namespace std;

MySegsPlanarDrawer::MySegsPlanarDrawer()
{
	mLabelManager = std::make_shared<MyLabelManager>();
}


MySegsPlanarDrawer::~MySegsPlanarDrawer()
{
}

void MySegsPlanarDrawer::SetLabels(MyMapScPtr<int, MyString> labels){
	mLabelManager->SetLabels(labels);
}

void MySegsPlanarDrawer::Update(){
	MyArrayScPtr<MyBox2f> boxes = mSegNodeInfoAssemble->GetSegment2DBoxes();
	MyMapSPtr<int, MyBox2f> objBoxes = std::make_shared<MyMap<int, MyBox2f>>();
	for (int i = 0; i < boxes->size(); i++){
		const MyBox2f &box = boxes->at(i);
		// please make sure this mapping is correct!!!!
		objBoxes->operator[](i) = box;
	}
	mLabelManager->SetBoxes(objBoxes);
	mLabelManager->Update();

	// use shader
	LoadGeometry();
}

void MySegsPlanarDrawer::Render(int winWidth, int winHeight){
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	MyArrayScPtr<MyBox2f> boxes = mSegNodeInfoAssemble->GetSegment2DBoxes();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBegin(GL_LINES);
	if (mSegTrkNetwork){
		MyMatrixfScPtr cMat = mSegTrkNetwork->GetConnectionMatrix();
		for (int i = 0; i < cMat->GetNumCols(); i++){
			float regionTrks = cMat->At(i, i);
			for (int j = 0; j < cMat->GetNumRows(); j++){
				float conTrks = cMat->At(i, j);
				float ratio = conTrks / regionTrks;
				if (ratio > 0.2){
					MyVec4f color(ratio, 1 - ratio, 0, 0.5);
					//MyGraphicsTool::Vertex(boxes->at(i).GetCenter());
					//MyGraphicsTool::Vertex(boxes->at(j).GetCenter());
					DrawArrow(boxes->at(i).GetCenter(), boxes->at(j).GetCenter(),
						color, MyVec4i(i, j, i, j), ratio / 100);
				}
			}
		}
	}

	MyArray4fSPtr colors = mSegNodeInfoAssemble->GetSegment2DColors();
	glBegin(GL_QUADS);
	for (int i = 0; i < boxes->size(); i++){
		const MyBox2f &box = boxes->at(i);
		MyVec2f highPos = box.GetHighPos();
		MyVec2f lowPos = box.GetLowPos();
		MyVec4f color = colors->at(i);
		glColor4f(color[0], color[1], color[2], color[3]);
		glVertex2f(lowPos[0], lowPos[1]);
		glVertex2f(highPos[0], lowPos[1]);
		glVertex2f(highPos[0], highPos[1]);
		glVertex2f(lowPos[0], highPos[1]);
	}
	glEnd();

	glColor4f(0, 0, 0, 0.8);
	for (int i = 0; i < boxes->size(); i++){
		const MyBox2f &box = boxes->at(i);
		MyVec2f highPos = box.GetHighPos();
		MyVec2f lowPos = box.GetLowPos();
		glBegin(GL_LINE_LOOP);
		glVertex2f(lowPos[0], lowPos[1]);
		glVertex2f(highPos[0], lowPos[1]);
		glVertex2f(highPos[0], highPos[1]);
		glVertex2f(lowPos[0], highPos[1]);
		glEnd();
	}
	glPopAttrib();

	mLabelManager->Render(winWidth, winHeight);
}

void MySegsPlanarDrawer::Resize(int width, int height){
	mLabelManager->SetWindowSize(width, height);
}

void MySegsPlanarDrawer::CompileShader(int shader){
	glDeleteProgram(mShaderProgram);
	mShaderProgram = InitShader("Shaders\\contour.vert", "Shaders\\contour.frag", "fragColour", "name");


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
	MyVec3f radDir = up^dir3;
	float offsetAlpha = 0.05;
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
	glColor4f(color[0], color[1], color[2], color[3]);
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= numStep; i++){
		float angle = startAngle + i*delAngle;
		float halfWidth = (startWidth + i*delWidth) / 2;
		MyVec2f leftPos(center[0] + cos(angle)*(radius - halfWidth),
			center[1] + sin(angle)*(radius - halfWidth));
		MyGraphicsTool::Vertex(leftPos);
		MyVec2f RightPos(center[0] + cos(angle)*(radius + halfWidth),
			center[1] + sin(angle)*(radius + halfWidth));
		MyGraphicsTool::Vertex(RightPos);
	}
	glEnd();

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


	glColor4f(0, 0, 0, 0.8);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= numStep; i++){
		float angle = startAngle + i*delAngle;
		float halfWidth = (startWidth + i*delWidth) / 2;
		MyVec2f leftPos(center[0] + cos(angle)*(radius - halfWidth),
			center[1] + sin(angle)*(radius - halfWidth));
		MyGraphicsTool::Vertex(leftPos);
	}
	for (int i = numStep; i >= 0; i--){
		float angle = startAngle + i*delAngle;
		float halfWidth = (startWidth + i*delWidth) / 2;
		MyVec2f RightPos(center[0] + cos(angle)*(radius + halfWidth),
			center[1] + sin(angle)*(radius + halfWidth));
		MyGraphicsTool::Vertex(RightPos);
	}
	glEnd();
}

void MySegsPlanarDrawer::LoadGeometry(){
	// vertex
	mVertices.clear();
	mColors.clear();
	mNames.clear();
	MyArrayScPtr<MyBox2f> boxes = mSegNodeInfoAssemble->GetSegment2DBoxes();
	MyArray4fSPtr colors = mSegNodeInfoAssemble->GetSegment2DColors();
	for (int i = 0; i < boxes->size(); i++){
		const MyBox2f &box = boxes->at(i);
		MyVec2f highPos = box.GetHighPos();
		MyVec2f lowPos = box.GetLowPos();
		MyVec4f color = colors->at(i);
		MyVec4i name(i, i, i, i);
		mVertices << lowPos;
		mVertices << MyVec2f(highPos[0], lowPos[1]);
		mVertices << highPos;
		mVertices << MyVec2f(lowPos[0], highPos[1]);
		mColors << color << color << color << color;
		mNames << name << name << name << name;
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