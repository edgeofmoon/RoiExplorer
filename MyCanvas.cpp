#include "MyCanvas.h"
#include "MyGLHeader.h"
#include "MyGraphicsTool.h"

MyCanvas::MyCanvas()
{
	mFrameBuffer.width = 0;
	mFrameBuffer.height = 0;
}


MyCanvas::~MyCanvas()
{
}

void MyCanvas::Clear(){
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLint initNames[] = { -1, -1, -1, -1 };
	glClearBufferiv(GL_COLOR, 1, initNames);
}

void MyCanvas::On(){
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer.frameBuffer);
}

void MyCanvas::Off(){
	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer.frameBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + 1);
	glReadPixels(0, 0, mFrameBuffer.width, mFrameBuffer.height,
		GL_RGBA_INTEGER, GL_INT, &(mNameMap->operator[](0)[0]));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

MyVec4i MyCanvas::GetName(int x, int y){
	if (mNameMap){
		return mNameMap->At(MyVec2i(x, y));
	}
	return MyVec4i(-1, -1, -1, -1);
}

void MyCanvas::Resize(int w, int h){
	mFrameBuffer.width = w;
	mFrameBuffer.height = h;
	mNameMap = std::make_shared<MyArrayMD<MyVec4i, 2>>(MyVec2i(w, h));
	// COLOR
	if (glIsTexture(mFrameBuffer.colorTexture)) {
		glDeleteTextures(1, &(mFrameBuffer.colorTexture));
	}
	glGenTextures(1, &(mFrameBuffer.colorTexture));
	glBindTexture(GL_TEXTURE_2D, mFrameBuffer.colorTexture);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mFrameBuffer.width, 
		mFrameBuffer.height, 0, GL_RGBA, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// DEPTH
	if (glIsTexture(mFrameBuffer.depthTexture)) {
		glDeleteTextures(1, &(mFrameBuffer.depthTexture));
	}
	glGenTextures(1, &(mFrameBuffer.depthTexture));
	glBindTexture(GL_TEXTURE_2D, mFrameBuffer.depthTexture);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, mFrameBuffer.width, 
		mFrameBuffer.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// name
	if (glIsTexture(mFrameBuffer.nameTexture)) {
		glDeleteTextures(1, &(mFrameBuffer.nameTexture));
	}
	glGenTextures(1, &(mFrameBuffer.nameTexture));
	glBindTexture(GL_TEXTURE_2D, mFrameBuffer.nameTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, mFrameBuffer.width,
		mFrameBuffer.height, 0, GL_RGBA_INTEGER, GL_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// FRAMEBUFFER ASSEMBLE
	if (glIsFramebuffer(mFrameBuffer.frameBuffer)) {
		glDeleteFramebuffers(1, &(mFrameBuffer.frameBuffer));
	}
	glGenFramebuffers(1, &(mFrameBuffer.frameBuffer));
	glBindFramebuffer(GL_FRAMEBUFFER, (mFrameBuffer.frameBuffer));
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mFrameBuffer.colorTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, mFrameBuffer.nameTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mFrameBuffer.depthTexture, 0);
	GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0 + 1 };
	glDrawBuffers(2, DrawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MyCanvas::Show(MyVec4i viewport){
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	MyGraphicsTool::SetViewport(viewport);
	MyGraphicsTool::PushProjectionMatrix();
	MyGraphicsTool::PushMatrix();
	MyGraphicsTool::LoadProjectionMatrix(&MyMatrixf::OrthographicMatrix(-1, 1, -1, 1, 1, 10));
	MyGraphicsTool::LoadModelViewMatrix(&MyMatrixf::IdentityMatrix());
	MyGraphicsTool::EnableTexture2D();
	MyGraphicsTool::BindTexture2D(mFrameBuffer.colorTexture);
	MyGraphicsTool::BeginTriangleFan();
	MyGraphicsTool::TextureCoordinate(MyVec2f(0, 0));
	MyGraphicsTool::Color(MyColor4f(1, 1, 1));
	MyGraphicsTool::Vertex(MyVec3f(-1, -1, -5));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1, 0));
	MyGraphicsTool::Vertex(MyVec3f(1, -1, -5));
	MyGraphicsTool::TextureCoordinate(MyVec2f(1, 1));
	MyGraphicsTool::Vertex(MyVec3f(1, 1, -5));
	MyGraphicsTool::TextureCoordinate(MyVec2f(0, 1));
	MyGraphicsTool::Vertex(MyVec3f(-1, 1, -5));
	MyGraphicsTool::EndPrimitive();
	MyGraphicsTool::BindTexture2D(0);
	MyGraphicsTool::DisableTexture2D();
	MyGraphicsTool::PopMatrix();
	MyGraphicsTool::PopProjectionMatrix();
	glPopAttrib();
}