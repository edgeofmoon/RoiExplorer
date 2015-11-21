#pragma once

#include "MyArray.h"

class MySurfaceRenderer
{
public:
	MySurfaceRenderer();
	~MySurfaceRenderer();

	void Render(int width, int height);

	void Update();

	void SetGeometry(const MyArray3f* vertices,
		const MyArray3f* normals, const MyArray3i* triangles);

protected:
	const MyArray3f* mVertices;
	const MyArray3f* mNormals;
	const MyArray3i* mTriangles;

	// shader program pointers
	int mContourShaderProgram;
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

