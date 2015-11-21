#include "MyMarchingSquares.h"

// debug
#include <iostream>
using namespace std;

// source: https://en.wikipedia.org/wiki/Marching_squares

/*
0000, 0001, 0010, 0011
0100, 0101, 0110, 0111
1000, 1001, 1010, 1011
1100, 1101, 1110, 1111
*/

MyVec2i MyMarchingSquares::MS_CellVertices[4] = {
	MyVec2i(0, 1),
	MyVec2i(1, 1),
	MyVec2i(1, 0),
	MyVec2i(0, 0)
};

MyVec2i MyMarchingSquares::MS_CellEdges[4] = {
	MyVec2i(0, 1),
	MyVec2i(1, 2),
	MyVec2i(2, 3),
	MyVec2i(3, 0)
};

char MyMarchingSquares::MS_NumEdgeLUT[16] = {
	0, 1, 1, 1,
	1, 2, 1, 1,
	1, 1, 2, 1,
	1, 1, 1, 0
};

char MyMarchingSquares::MS_EdgeEdgeLUT[16][4] = {
	{ 0, 0, 0, 0 }, { 0, 0, 1, 1 }, { 0, 1, 1, 0 }, { 0, 1, 0, 1 },
	{ 1, 1, 0, 0 }, { 1, 1, 1, 1 }, { 1, 0, 1, 0 }, { 1, 0, 0, 1 },
	{ 1, 0, 0, 1 }, { 1, 0, 1, 0 }, { 1, 1, 1, 1 }, { 1, 1, 0, 0 },
	{ 0, 1, 0, 1 }, { 0, 1, 1, 0 }, { 0, 0, 1, 1 }, { 0, 0, 0, 0 }
};

char MyMarchingSquares::MS_EdgeLUT[16][2][2] = {
	//case 0
	{ { -1, -1 }, { -1, -1 } },
	//case 1
	{ { 3, 2 }, { -1, -1 } },
	//case 2
	{ { 2, 1 }, { -1, -1 } },
	//case 3
	{ { 3, 1 }, { -1, -1 } },
	//case 4
	{ { 0, 1 }, { -1, -1 } },
	//case 5
	{ { 3, 0 }, { 1, 2 } },
	//case 6
	{ { 2, 0 }, { -1, -1 } },
	//case 7
	{ { 3, 0 }, { -1, -1 } },
	//case 8
	{ { 0, 3 }, { -1, -1 } },
	//case 9
	{ { 0, 2 }, { -1, -1 } },
	//case 10
	{ { 0, 1 }, { 2, 3 } },
	//case 11
	{ { 0, 1 }, { -1, -1 } },
	//case 12
	{ { 1, 3 }, { -1, -1 } },
	//case 13
	{ { 1, 2 }, { -1, -1 } },
	//case 14
	{ { 2, 3 }, { -1, -1 } },
	//case 15
	{ { -1, -1 }, { -1, -1 } },
};

bool MyMarchingSquares::CheckTables(){
	for (char i = 0; i < 16; i++){
		char numEdge = MS_NumEdgeLUT[i];
		for (char ie = 0; ie < numEdge; ie++){
			if (MS_EdgeEdgeLUT[i][MS_EdgeLUT[i][ie][0]] != 1){
				return false;
			}
			if (MS_EdgeEdgeLUT[i][MS_EdgeLUT[i][ie][1]] != 1){
				return false;
			}
		}
	}
	return true;
}

char MyMarchingSquares::MS_IndexRedirect[16] = {
	0, 1, 2, 3,
	4, 10,6, 7,
	8, 9, 5,11,
	12,13,14,15
};

MyMarchingSquares::MyMarchingSquares()
{
	mThreshold = 0;
}


MyMarchingSquares::~MyMarchingSquares()
{
}

void MyMarchingSquares::Update(){
	if (!CheckTables()){
		cout << "Table Check Failed.\n" << endl;
	}
	mLines.clear();
	for (int i = 0; i < mField->GetDimSize(0) - 1; i++){
		for (int j = 0; j < mField->GetDimSize(1) - 1; j++){
			MyVec2i cellIdx(i, j);
			this->MarchingSquare(cellIdx);
		}
	}
}

void MyMarchingSquares::MarchingSquare(const MyVec2i& cellIdx){
	MyVec4f vtValues = this->ComputeCellValues(cellIdx);
	char status = this->ComputeStatus(vtValues);
	char numEdge = MS_NumEdgeLUT[status];
	if (numEdge == 0) return;
	char edgeLUTid = status;
	// flipped cases
	bool flip = false;
	if (edgeLUTid != MS_IndexRedirect[edgeLUTid]){
		float centerValue = this->ComputeCellCenterValue(vtValues);
		if (centerValue < mThreshold){
			edgeLUTid = MS_IndexRedirect[edgeLUTid];
			flip = true;
		}
	}
	if (!(numEdge <= 2)){
		cout << "More than 2 edges in a cell??\n";
		cout << "\t status: " << (int)status 
			<< " has " << (int)numEdge << " edges!!" << endl;;
	}

	// now add edges
	// simple fast version: just use center
	/*
	for (int ie = 0; ie < numEdge; ie++){
		char edgeE0 = MS_EdgeLUT[edgeLUTid][ie][0];
		char edgeE1 = MS_EdgeLUT[edgeLUTid][ie][1];
		MyVec2f pos0 = this->ComputeEdgeCenter(cellIdx, edgeE0);
		MyVec2f pos1 = this->ComputeEdgeCenter(cellIdx, edgeE1);
		// flipped cases also need to flip line ends
		// to ensure clock-wise enclosure of iso-surface
		if (!flip) mLines << MyLine2f(pos0, pos1);
		else mLines << MyLine2f(pos1, pos0);
	}
	*/

	// linear version
	for (int ie = 0; ie < numEdge; ie++){
		char edgeE0 = MS_EdgeLUT[edgeLUTid][ie][0];
		char edgeE1 = MS_EdgeLUT[edgeLUTid][ie][1];
		MyVec2f pos0 = this->ComputeEdge(cellIdx, edgeE0, vtValues);
		MyVec2f pos1 = this->ComputeEdge(cellIdx, edgeE1, vtValues);
		// flipped cases also need to flip line ends
		// to ensure clock-wise enclosure of iso-surface
		if (!flip) mLines << MyLine2f(pos0, pos1);
		else mLines << MyLine2f(pos1, pos0);
	}
}

MyVec4f MyMarchingSquares::ComputeCellValues(const MyVec2i& cellIdx) const{
	MyVec4f values;
	for (int i = 0; i < 4; i++){
		values[i] = mField->At(cellIdx + MS_CellVertices[i]);
	}
	return values;
}

float MyMarchingSquares::ComputeCellCenterValue(const MyVec4f& cellValues) const{
	float values = 0;
	for (int i = 0; i < 4; i++){
		values += cellValues[i];
	}
	return values / 4;
}

MyVec2f MyMarchingSquares::ComputeEdgeCenter(const MyVec2i& cellIdx, int edgeIndex) const{
	MyVec2i edgeV0 = MS_CellVertices[MS_CellEdges[edgeIndex][0]];
	MyVec2i edgeV1 = MS_CellVertices[MS_CellEdges[edgeIndex][1]];
	MyVec2f pos0 = mField->ComputePosition(cellIdx + edgeV0);
	MyVec2f pos1 = mField->ComputePosition(cellIdx + edgeV1);
	return (pos0 + pos1) / 2;
}

MyVec2f MyMarchingSquares::ComputeEdge(const MyVec2i& cellIdx, int edgeIndex,
	const MyVec4f& values) const{
	MyVec2i edgeV0 = MS_CellVertices[MS_CellEdges[edgeIndex][0]];
	MyVec2i edgeV1 = MS_CellVertices[MS_CellEdges[edgeIndex][1]];
	float v0value = values[MS_CellEdges[edgeIndex][0]];
	float v1value = values[MS_CellEdges[edgeIndex][1]];
	float frac = (mThreshold - v0value) / (v1value - v0value);
	MyVec2f pos0 = mField->ComputePosition(cellIdx + edgeV0);
	MyVec2f pos1 = mField->ComputePosition(cellIdx + edgeV1);
	return (1 - frac)*pos0 + frac*pos1;
}

char MyMarchingSquares::ComputeStatus(const MyVec4f& cellValues) const{
	char status = 0;
	const char byte[4] = { 8, 4, 2, 1 };
	for (int i = 0; i < 4; i++){
		if (cellValues[i]>mThreshold){
			status += byte[i];
		}
	}
	return status;
}