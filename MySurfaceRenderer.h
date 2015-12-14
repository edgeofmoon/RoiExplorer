#pragma once

#include "MyArray.h"
#include "MySharedPointer.h"

class MySurfaceRenderer
{
public:
	MySurfaceRenderer();
	~MySurfaceRenderer();

	void Render();

	void Update();

	void SetShaderProgram(unsigned int shader);

	void SetGeometry(const MyArray3f* vertices,
		const MyArray3f* normals, const MyArray3i* triangles);

	void SetName(const MyVec4i& name){
		mName = name;
	}
	const MyVec4i& GetName() const{
		return mName;
	}
	void SetColor(const MyVec4f& color){
		mColor = color;
	}
	const MyVec4f& GetColor() const{
		return mColor;
	}
	void SetTransparency(float t){
		mTransparency = t;
	}
	float GetTransparency() const{
		return mTransparency;
	}
protected:
	const MyArray3f* mVertices;
	const MyArray3f* mNormals;
	const MyArray3i* mTriangles;

	MyVec4i mName;
	MyVec4f mColor;
	int mIndexSize;

	// rendering parameter
	float mTransparency;

	// shader program pointers
	unsigned int mShaderProgram;
	int mNormalAttribute;
	int mPositionAttribute;

	// GPU data buffer pointers
	unsigned int mVertexArray;
	unsigned int mVertexBuffer;
	unsigned int mNormalBuffer;
	unsigned int mIndexBuffer;

	// load geometry to GPU
	void CompileShader();
	void BuildGeometryBuffers();
	void DestoryGeometryBuffers();
	void LoadGeometry();
};

typedef MySharedPointer<MySurfaceRenderer> MySurfaceRendererSPtr;
typedef MySharedPointer<const MySurfaceRenderer> MySurfaceRendererScPtr;