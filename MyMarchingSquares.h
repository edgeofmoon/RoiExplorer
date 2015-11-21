#pragma once

#include "MyGridMD.h"
#include "MyLine.h"

class MyMarchingSquares
{
public:
	MyMarchingSquares();
	~MyMarchingSquares();

	void Update();
	void SetThreshold(float thres){ mThreshold = thres; };
	void SetField(My2dfGridfScPtr field){ mField = field; };
	const MyArray<MyLine2f>& GetLines() const{ return mLines; };

protected:
	My2dfGridfScPtr mField;
	MyArray<MyLine2f> mLines;

	float mThreshold;

	void MarchingSquare(const MyVec2i& cellIdx);
	MyVec4f ComputeCellValues(const MyVec2i& cellIdx) const;
	float ComputeCellCenterValue(const MyVec4f& cellValues) const;
	MyVec2f ComputeEdgeCenter(const MyVec2i& cellIdx, int edgeIndex) const;
	MyVec2f ComputeEdge(const MyVec2i& cellIdx, int edgeIndex, 
		const MyVec4f& values) const;
	char ComputeStatus(const MyVec4f& cellValues) const;

	static MyVec2i MS_CellVertices[4];
	static MyVec2i MS_CellEdges[4];
	static char MS_NumEdgeLUT[16];
	static char MS_EdgeEdgeLUT[16][4];
	static char MS_EdgeLUT[16][2][2];
	static char MS_IndexRedirect[16];
	static bool CheckTables();
};

