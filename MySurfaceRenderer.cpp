#include "MySurfaceRenderer.h"
#include "Shader.h"
#include "MyGLHeader.h"

MySurfaceRenderer::MySurfaceRenderer()
{
	mIndexSize = 0;
	mName = MyVec4i(-1, -1, -1, -1);
	mColor = MyVec4f(1, 0, 0, 1);
}


MySurfaceRenderer::~MySurfaceRenderer()
{
	if (glIsProgram(mShaderProgram)){
		glDeleteProgram(mShaderProgram);
	}
	DestoryGeometryBuffers();
}

void MySurfaceRenderer::Render(){
	glUseProgram(mShaderProgram);
	glBindVertexArray(mVertexArray);

	int mvmatLocation = glGetUniformLocation(mShaderProgram, "mvMat");
	float modelViewMat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMat);
	glUniformMatrix4fv(mvmatLocation, 1, GL_FALSE, modelViewMat);

	int projmatLocation = glGetUniformLocation(mShaderProgram, "projMat");
	float projMat[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projMat);
	glUniformMatrix4fv(projmatLocation, 1, GL_FALSE, projMat);

	int colorLocation = glGetUniformLocation(mShaderProgram, "color");
	if (colorLocation >= 0){
		glUniform4f(colorLocation, mColor[0], mColor[1], mColor[2], mColor[3]);
	}

	if (mName[0] >= 0){
		int nameLocation = glGetUniformLocation(mShaderProgram, "name");
		glUniform4i(nameLocation, mName[0], mName[1], mName[2], mName[3]);
	}

	glDrawElements(GL_TRIANGLES, mIndexSize, GL_UNSIGNED_INT, 0);

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

void MySurfaceRenderer::SetShaderProgram(unsigned int shader){
	if (glIsProgram(mShaderProgram)){
		glDeleteProgram(mShaderProgram);
	}
	mShaderProgram = shader;
}
void MySurfaceRenderer::CompileShader(){
	if (!glIsProgram(mShaderProgram)){
		if (mName[0] >= 0){
			mShaderProgram = InitShader(
				"shaders\\simple.vert", "shaders\\simple.frag", "fragColour", "name");
		}
		else{
			mShaderProgram = InitShader(
				"shaders\\simple.vert", "shaders\\simple.frag", "fragColour");
		}
	}

	mNormalAttribute = glGetAttribLocation(mShaderProgram, "normal");
	mPositionAttribute = glGetAttribLocation(mShaderProgram, "position");
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
		mIndexSize = mTriangles->size() * 3;
	}
	else{
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
		glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);
		mIndexSize = 0;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
