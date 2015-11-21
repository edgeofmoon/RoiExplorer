#include "MySurfaceRenderer.h"
#include "Shader.h"
#include "MyGLHeader.h"

MySurfaceRenderer::MySurfaceRenderer()
{
}


MySurfaceRenderer::~MySurfaceRenderer()
{
	glDeleteProgram(mContourShaderProgram);
	DestoryGeometryBuffers();
}

void MySurfaceRenderer::Render(int width, int height){
	glUseProgram(mContourShaderProgram);
	glBindVertexArray(mVertexArray);

	int mvmatLocation = glGetUniformLocation(mContourShaderProgram, "mvMat");
	float modelViewMat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMat);
	glUniformMatrix4fv(mvmatLocation, 1, GL_FALSE, modelViewMat);

	int projmatLocation = glGetUniformLocation(mContourShaderProgram, "projMat");
	float projMat[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projMat);
	glUniformMatrix4fv(projmatLocation, 1, GL_FALSE, projMat);

	glDrawElements(GL_TRIANGLES, mTriangles->size() * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void MySurfaceRenderer::Update(){
	CompileShader();
	BuildGeometryBuffers();
	LoadGeometry();
}

void MySurfaceRenderer::SetGeometry(const MyArray3f* vertices,
	const MyArray3f* normals, const MyArray3i* triangles){
	mVertices = vertices;
	mNormals = normals;
	mTriangles = triangles;
}

void MySurfaceRenderer::CompileShader(){
	glDeleteProgram(mContourShaderProgram);
	mContourShaderProgram = InitShader(
		"shaders\\simple.vert", "shaders\\simple.frag", "fragColour", "name");

	mNormalAttribute = glGetAttribLocation(mContourShaderProgram, "normal");
	mPositionAttribute = glGetAttribLocation(mContourShaderProgram, "position");
}

void MySurfaceRenderer::BuildGeometryBuffers(){
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);
	// vertex
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glEnableVertexAttribArray(mPositionAttribute);
	glVertexAttribPointer(mPositionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// normal
	glGenBuffers(1, &mNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
	glEnableVertexAttribArray(mNormalAttribute);
	glVertexAttribPointer(mNormalAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// index
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBindVertexArray(0);
}

void MySurfaceRenderer::DestoryGeometryBuffers(){
	if (glIsVertexArray(mVertexArray)){
		glDeleteVertexArrays(1, &mVertexArray);
	}
	// vertex
	if (glIsBuffer(mVertexBuffer)){
		glDeleteBuffers(1, &mVertexBuffer);
	}
	// normal
	if (glIsBuffer(mNormalBuffer)){
		glDeleteBuffers(1, &mNormalBuffer);
	}
	// index
	if (glIsBuffer(mIndexBuffer)){
		glDeleteBuffers(1, &mIndexBuffer);
	}
}

void MySurfaceRenderer::LoadGeometry(){
	if (mVertices->size() > 0){
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, mVertices->size() * sizeof(MyVec3f), &mVertices->at(0), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, mNormals->size() * sizeof(MyVec3f), &mNormals->at(0), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mTriangles->size() * sizeof(MyVec3i), &mTriangles->at(0), GL_DYNAMIC_DRAW);
	}
	else{
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
