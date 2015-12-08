#pragma once

#include "MyArray.h"
#include "MyBoundingBox.h"
#include "MySharedPointer.h"
class MyMesh
{
public:
	MyMesh();
	~MyMesh();

	int Read(const MyString& fileName);

	int GenPerVertexNormal();

	int Merge(const MyMesh& mesh);

	MyVec3f GetVertex(int idx) const;
	MyVec3f GetNormal(int idx) const;
	MyVec3i GetTriangle(int idx) const;
	MyBoundingBox GetBoundingBox() const;

	int GetNumVertex() const;
	int GetNumNormal() const;
	int GetNumTriangle() const;

	const MyArray3f& GetVertices() const;
	const MyArray3f& GetNormals() const;
	const MyArray3i& GetTriangles() const;

	const float* GetVertexData() const;
	const float* GetNormalData() const;
	const int* GetTriangleData() const;

	MyVec3f ComputeTriangleNormal(int triangleIdx) const;
	MyVec3f ComputeTriangleNormal(const MyVec3i& triangle) const;

	int ClearNonRegularFaces();

protected:
	MyArray3f mVertices;
	MyArray3f mNormals;
	MyArray3i mTriangles;
	MyBoundingBox mBox;

	static MyVec2i makeEdge(int i, int j);

	static bool compareEdge(MyVec2i edge0, MyVec2i edge1);

};

typedef MySharedPointer<MyMesh> MyMeshSPtr;
typedef MySharedPointer<const MyMesh> MyMeshScPtr;