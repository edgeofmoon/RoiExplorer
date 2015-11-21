
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include "MyVolumeRenderer.h"
#include "Shader.h"
#include "MyGraphicsTool.h"
MyVolumeRenderer::MyVolumeRenderer()
{
	mThreshold = 0;
	mSampleRate = 1024;
	mDecayFactor = 1024;
}


MyVolumeRenderer::~MyVolumeRenderer()
{
	if (glIsVertexArray(mCubeVertexArray)){
		glDeleteVertexArrays(1, &mCubeVertexArray);
	}
	if (glIsBuffer(mCubeVertexBuffer)){
		glDeleteBuffers(1, &mCubeVertexBuffer);
	}
	if (glIsBuffer(mCubeIndexBuffer)){
		glDeleteBuffers(1, &mCubeIndexBuffer);
	}
	if (glIsTexture(mCubeFrameBuffer.colorTexture)) {
		glDeleteTextures(1, &(mCubeFrameBuffer.colorTexture));
	}
	if (glIsTexture(mCubeFrameBuffer.depthTexture)) {
		glDeleteTextures(1, &(mCubeFrameBuffer.depthTexture));
	}
	if (glIsTexture(mPreIntegralLUT)) {
		glDeleteTextures(1, &mPreIntegralLUT);
	}
	if (glIsTexture(mVolumeTexture)) {
		glDeleteTextures(1, &mVolumeTexture);
	}
	if (glIsFramebuffer(mCubeFrameBuffer.frameBuffer)) {
		glDeleteFramebuffers(1, &(mCubeFrameBuffer.frameBuffer));
	}
	if (glIsProgram(mShaderProgram)){
		glDeleteProgram(mShaderProgram);
	}
	if (glIsProgram(mCubeProgram)){
		glDeleteProgram(mCubeProgram);
	}
}

void MyVolumeRenderer::Resize(int width, int height){
	mCubeFrameBuffer.width = width;
	mCubeFrameBuffer.height = height;
	SetupCubeFrameBuffer();
	LoadCubeData();
	ComputePreIntegralLUT(256);
}

void MyVolumeRenderer::CompileShader(int shader){
	if (glIsProgram(mShaderProgram)){
		glDeleteProgram(mShaderProgram);
	}
	if (shader == 0) {
		mShaderProgram = InitShader("Shaders\\trackVol.vert", "Shaders\\trackVol.frag", "fragColour");
	}
	else {
		mShaderProgram = InitShader("Shaders\\trackVol.vert", "Shaders\\trackVol_pcwLinear.frag", "fragColour");
	}

	if (glIsProgram(mCubeProgram)){
		glDeleteProgram(mCubeProgram);
	}
	mCubeProgram = InitShader("Shaders\\coord.vert", "Shaders\\coord.frag", "fragColour");
}

void MyVolumeRenderer::Render(int winWidth, int winHeight){

	// cube
	int currentFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFbo);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glBindFramebuffer(GL_FRAMEBUFFER, mCubeFrameBuffer.frameBuffer);
	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, DrawBuffers);
	glEnable(GL_CULL_FACE);
	glClearColor(0.5, 0.5, 0.5, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	glUseProgram(mCubeProgram);
	int location = glGetUniformLocation(mCubeProgram, "mvMat");
	float modelViewMat[16];
	glPushMatrix();
	glScalef(mVolX, mVolY, mVolZ);
	glTranslatef(-0.5,-0.5,-0.5);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMat);
	glUniformMatrix4fv(location, 1, GL_FALSE, modelViewMat);
	glPopMatrix();

	location = glGetUniformLocation(mCubeProgram, "projMat");
	float projMat[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projMat);
	glUniformMatrix4fv(location, 1, GL_FALSE, projMat);

	glBindVertexArray(mCubeVertexArray);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, currentFbo);

	////test
	//MyGraphicsTool::EnableTexture2D();
	//MyGraphicsTool::BindTexture2D(mCubeFrameBuffer.colorTexture);
	//MyGraphicsTool::BeginTriangleFan();
	//MyGraphicsTool::TextureCoordinate(MyVec2f(0, 0));
	//MyGraphicsTool::Color(MyColor4f(1, 1, 1));
	//MyGraphicsTool::Vertex(MyVec3f(-50, -50, -5));
	//MyGraphicsTool::TextureCoordinate(MyVec2f(1, 0));
	////MyGraphicsTool::Color(MyColor4f(1, 0, 0));
	//MyGraphicsTool::Vertex(MyVec3f(50, -50, -5));
	//MyGraphicsTool::TextureCoordinate(MyVec2f(1, 1));
	////MyGraphicsTool::Color(MyColor4f(1, 1, 0));
	//MyGraphicsTool::Vertex(MyVec3f(50, 50, -5));
	//MyGraphicsTool::TextureCoordinate(MyVec2f(0, 1));
	////MyGraphicsTool::Color(MyColor4f(0, 1, 0));
	//MyGraphicsTool::Vertex(MyVec3f(-50, 50, -5));
	//MyGraphicsTool::EndPrimitive();
	//MyGraphicsTool::BindTexture2D(0);
	//MyGraphicsTool::DisableTexture2D();
	//return;

	// render ray
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(mShaderProgram);
	location = glGetUniformLocation(mShaderProgram, "mvMat");
	glUniformMatrix4fv(location, 1, GL_FALSE, modelViewMat);
	location = glGetUniformLocation(mShaderProgram, "projMat");
	glUniformMatrix4fv(location, 1, GL_FALSE, projMat);


	location = glGetUniformLocation(mShaderProgram, "threshold");
	glUniform1f(location, mThreshold);

	location = glGetUniformLocation(mShaderProgram, "vol");
	glUniform1i(location, 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_3D, mVolumeTexture);

	location = glGetUniformLocation(mShaderProgram, "backFace");
	glUniform1i(location, 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, mCubeFrameBuffer.colorTexture);

	location = glGetUniformLocation(mShaderProgram, "lookupTable");
	glUniform1i(location, 2);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, mPreIntegralLUT);

	//location = glGetUniformLocation(shaderProgram, "colorMap");
	//glUniform1i(location, 3);
	//glActiveTexture(GL_TEXTURE0 + 3);
	//glBindTexture(GL_TEXTURE_2D, colorTex);

	location = glGetUniformLocation(mShaderProgram, "volSize");
	glUniform3f(location, mVolX, mVolY, mVolZ);

	location = glGetUniformLocation(mShaderProgram, "windowWidth");
	glUniform1f(location, winWidth);

	location = glGetUniformLocation(mShaderProgram, "windowHeight");
	glUniform1f(location, winHeight);

	location = glGetUniformLocation(mShaderProgram, "decayFactor");
	glUniform1f(location, mDecayFactor);

	location = glGetUniformLocation(mShaderProgram, "sampeRate");
	glUniform1f(location, mSampleRate);


	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glBindVertexArray(mCubeVertexArray);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_3D);
	glPopAttrib();

}

void MyVolumeRenderer::LoadVolume(const float* vol, int x, int y, int z){
	if (glIsTexture(mVolumeTexture)){
		glDeleteTextures(1, &mVolumeTexture);
	}
	glGenTextures(1, &mVolumeTexture);
	glBindTexture(GL_TEXTURE_3D, mVolumeTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, x, y, z, 0, GL_RED, GL_FLOAT, vol);
	glBindTexture(GL_TEXTURE_3D, 0);
	mVolX = x;
	mVolY = y;
	mVolZ = z;
}

void MyVolumeRenderer::LoadCubeData(){
	float vertices[8][3] = {
		{ 0, 0, 1 }, { 1, 0, 1 },
		{ 0, 1, 1 }, { 1, 1, 1 },
		{ 0, 0, 0 }, { 1, 0, 0 },
		{ 0, 1, 0 }, { 1, 1, 0 },
	};
	int faces[6][4] = {
		{ 0, 1, 3, 2 },
		{ 2, 3, 7, 6 },
		{ 6, 7, 5, 4 },
		{ 4, 5, 1, 0 },
		{ 4, 0, 2, 6 },
		{ 1, 5, 7, 3 },
	};
	int index[36];
	for (int i = 0; i < 6; i++){
		index[i * 6 + 0] = faces[i][0];
		index[i * 6 + 1] = faces[i][1];
		index[i * 6 + 2] = faces[i][2];
		index[i * 6 + 3] = faces[i][0];
		index[i * 6 + 4] = faces[i][2];
		index[i * 6 + 5] = faces[i][3];
	}
	if (glIsVertexArray(mCubeVertexArray)){
		glDeleteVertexArrays(1, &mCubeVertexArray);
	}
	glGenVertexArrays(1, &mCubeVertexArray);
	glBindVertexArray(mCubeVertexArray);
	if (glIsBuffer(mCubeVertexBuffer)){
		glDeleteBuffers(1, &mCubeVertexBuffer);
	}
	glGenBuffers(1, &mCubeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mCubeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);
	int location = glGetAttribLocation(mCubeProgram, "position");
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (glIsBuffer(mCubeIndexBuffer)){
		glDeleteBuffers(1, &mCubeIndexBuffer);
	}
	glGenBuffers(1, &mCubeIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mCubeIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(int), &index, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void MyVolumeRenderer::SetupCubeFrameBuffer(){
	if (glIsTexture(mCubeFrameBuffer.colorTexture)) {
		glDeleteTextures(1, &(mCubeFrameBuffer.colorTexture));
	}
	glGenTextures(1, &(mCubeFrameBuffer.colorTexture));
	glBindTexture(GL_TEXTURE_2D, mCubeFrameBuffer.colorTexture);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mCubeFrameBuffer.width, mCubeFrameBuffer.height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// DEPTH
	if (glIsTexture(mCubeFrameBuffer.depthTexture)) {
		glDeleteTextures(1, &(mCubeFrameBuffer.depthTexture));
	}
	glGenTextures(1, &(mCubeFrameBuffer.depthTexture));
	glBindTexture(GL_TEXTURE_2D, mCubeFrameBuffer.depthTexture);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, mCubeFrameBuffer.width, mCubeFrameBuffer.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// FRAMEBUFFER ASSEMBLE
	if (glIsFramebuffer(mCubeFrameBuffer.frameBuffer)) {
		glDeleteFramebuffers(1, &(mCubeFrameBuffer.frameBuffer));
	}
	glGenFramebuffers(1, &(mCubeFrameBuffer.frameBuffer));
	glBindFramebuffer(GL_FRAMEBUFFER, (mCubeFrameBuffer.frameBuffer));
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mCubeFrameBuffer.colorTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mCubeFrameBuffer.depthTexture, 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MyVolumeRenderer::ComputePreIntegralLUT(int nDim){
	std::cout << "Computing Volume Rendering LUT...\r";
	int nSteps = nDim;
	float *table = new float[nDim*nDim * 4];
	float step = 1.f / nSteps;
	for (int i = 0; i < nSteps; i++){
		float Di = i*step;
		for (int j = i; j < nSteps; j++){
			float Dj = j*step;
			MyVec4f color = ComputeLUTEntry(Di, Dj);
			// it is symmetrical
			int idx0 = i*nSteps + j;
			int idx1 = j*nSteps + i;
			memcpy(table + idx0 * 4, color.d(), 4 * sizeof(float));
			memcpy(table + idx1 * 4, color.d(), 4 * sizeof(float));
		}
	}


	if (glIsTexture(mPreIntegralLUT)) {
		glDeleteTextures(1, &mPreIntegralLUT);
	}
	glGenTextures(1, &mPreIntegralLUT);
	glBindTexture(GL_TEXTURE_2D, mPreIntegralLUT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nDim, nDim, 0, GL_RGBA, GL_FLOAT, table);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] table;
	std::cout << "Volume Rendering LUT Computing Completed\n";
}

float MyVolumeRenderer::ExtinctionCoefficent(float density) const{
	// simplest for now
	return density;
}

MyVec3f MyVolumeRenderer::ColorFromDensity(float density) const{
	// constant color for now
	return MyVec3f(density, 1 - density, 0);
}

MyVec4f MyVolumeRenderer::ComputeLUTEntry(float densityIn, float densityOut) const{
	float nSteps = 100;
	float step = 1.f / nSteps;
	MyVec3f colorAccum(0, 0, 0);
	for (float imd = 0; imd <= 1; imd+=step){
		float densityImd = (1 - imd)*densityIn + imd*densityOut;
		float extf = ExtinctionCoefficent(densityImd);
		MyVec3f color = ColorFromDensity(densityImd);
		float extfItgr = ExtinctionCoefficentIntegral(0, imd, densityIn, densityOut, step);
		colorAccum += extfItgr*color*step;
	}
	float alpha = 1 - ExtinctionCoefficentIntegral(0, 1, densityIn, densityOut, step);
	return MyVec4f(colorAccum[0], colorAccum[1], colorAccum[2], alpha);
}


float MyVolumeRenderer::ExtinctionCoefficentIntegral(
	float st, float ed, float densityIn, float densityOut, float itgrStep) const{
	float accm = 0;
	for (float imd = st; imd <= ed; imd += itgrStep){
		float densityImd = (1 - imd)*densityIn + imd*densityOut;
		float extf = ExtinctionCoefficent(densityImd);
		accm += extf*itgrStep;
	}
	return exp(-accm);
}