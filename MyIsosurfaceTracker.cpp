#include "MyIsosurfaceTracker.h"
#include "FollowCubeTables.h"
#include <queue>

//debug
#include <iostream>
using namespace std;


MyVec3i MyIsosurfaceTracker::CellVertices[8] = {
	MyVec3i(0, 0, 0),
	MyVec3i(0, 0, 1),
	MyVec3i(0, 1, 0),
	MyVec3i(0, 1, 1),
	MyVec3i(1, 0, 0),
	MyVec3i(1, 0, 1),
	MyVec3i(1, 1, 0),
	MyVec3i(1, 1, 1),
};

MyIsosurfaceTracker::MyIsosurfaceTracker()
{
	mSurfaceVisit = 0;
	mVertices = std::make_shared < MyArray3f >();
	mNormals = std::make_shared < MyArray3f >();
	mTriangles = std::make_shared < MyArray3i >();
}


MyIsosurfaceTracker::~MyIsosurfaceTracker()
{
}

void MyIsosurfaceTracker::Update(){
	mVertices->clear();
	mNormals->clear();
	mTriangles->clear();
	mBoundingBox.Set(MyVec3f(FLT_MAX, FLT_MAX, FLT_MAX),
		MyVec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	MyVec2i seeds = this->FindIsoseeds();
	if (seeds[0] < 0) return;
	UpdateSurfaceVisit();
	SurfaceTracking(seeds);
}

MyVec2i MyIsosurfaceTracker::FindIsoseeds(){
	int sign;
	if (mVolumn->At(mStartIndex) > mIsovalue){
		sign = 1;
	}
	else sign = -1;
	MyArray3i neighbors;
	MyArrayMD<char, 3> visited(mVolumn->GetDimSizes(), 0);
	MyVec2i seeds(mStartIndex, mStartIndex);
	while (true){
		seeds[0] = seeds[1];
		MyVec3i current = mVolumn->ComputePosition(seeds[0]);
		float neighborValue = mVolumn->At(seeds[1]);
		float currentValue = neighborValue;
		QueueJoinNeighbors(current, neighbors);
		for (int i = 0; i < neighbors.size(); i++){
			int nbrIndex = mVolumn->ComputeIndex(neighbors[i]);
			float nbrValue = mVolumn->At(nbrIndex);
			// if neighbor is better: i.e. going further
			// grab it
			if ((nbrValue - neighborValue)*sign < 0){
				seeds[1] = nbrIndex;
				neighborValue = nbrValue;
			}
		}
		if (CheckSeeds(currentValue, neighborValue)){
			//cout << "Seed Values: " << currentValue << ", " << neighborValue << endl;
			return seeds;
		}
		if (seeds[0] == seeds[1]){
			// no progress is made, abort
			cout << "Cannot find seeds!" << endl;
			return MyVec2i(-1, -1);
		}
	}
	return seeds;
}

bool MyIsosurfaceTracker::CheckSeeds(float value0, float value1) const{
	return (value0 - mIsovalue)*(value1 - mIsovalue) < 0;
}

void MyIsosurfaceTracker::QueueJoinNeighbors(const MyVec3i& pos, MyArray3i& joinNeighbors) const{
	joinNeighbors.clear();
	if (pos[0]>0) joinNeighbors << pos + MyVec3i(-1, 0, 0);
	if (pos[0]<mVolumn->GetDimSize(0) - 1) joinNeighbors << pos + MyVec3i(+1, 0, 0);
	if (pos[1]>0) joinNeighbors << pos + MyVec3i(0, -1, 0);
	if (pos[1]<mVolumn->GetDimSize(1) - 1) joinNeighbors << pos + MyVec3i(0, +1, 0);
	if (pos[2]>0) joinNeighbors << pos + MyVec3i(0, 0, -1);
	if (pos[2]<mVolumn->GetDimSize(2) - 1) joinNeighbors << pos + MyVec3i(0, 0, +1);
}

void MyIsosurfaceTracker::UpdateSurfaceVisit(){
	if (mSurfaceVisit){
		if (mSurfaceVisit->GetDimSizes() != mVolumn->GetDimSizes()){
			mSurfaceVisit = std::make_shared<My3dArrayc>(mVolumn->GetDimSizes(), 0);
		}
		else{
			mSurfaceVisit->operator=(0);
		}
	}
	else{
		mSurfaceVisit = std::make_shared<My3dArrayc>(mVolumn->GetDimSizes(), 0);
	}
}

void MyIsosurfaceTracker::FlagSurface(const MyVec4i& surface){
	mSurfaceVisit->operator[](surface.toDim<3>()) |= (1 << surface[3]);
}

void MyIsosurfaceTracker::UnFlagSurface(const MyVec4i& surface){
	mSurfaceVisit->operator[](surface.toDim<3>()) &= ~(1 << surface[3]);
}

bool MyIsosurfaceTracker::CheckSurfaceFlag(const MyVec4i& surface) const{
	return (mSurfaceVisit->operator[](surface.toDim<3>()) & (1 << surface[3])) != 0;
}

void MyIsosurfaceTracker::SurfaceTracking(const MyVec2i& seeds){
	int i1, i2;
	long whichCubeEdge, theSurface, whichFaceEdge;
	long theCase = 0;
	long whichVertex;
	float cubeVert[8];
	int xDim = mVolumn->GetDimSize(0);
	int yDim = mVolumn->GetDimSize(1);
	int zDim = mVolumn->GetDimSize(2);
	MyVec3i pos1 = mVolumn->ComputePosition(seeds[0]);
	MyVec3i pos2 = mVolumn->ComputePosition(seeds[1]);
	MyVec3i posLow, posCenter;
	//	find the common cell to which they both belong (make sure it's in bounds)
	posLow[0] = pos1[0] < pos2[0] ? pos1[0] : pos2[0];
	posLow[1] = pos1[1] < pos2[1] ? pos1[1] : pos2[1];
	posLow[2] = pos1[2] < pos2[2] ? pos1[2] : pos2[2];				//	take the minimum in each dimension
	posCenter[0] = posLow[0];
	posCenter[1] = posLow[1];
	posCenter[2] = posLow[2];
	if (posCenter[0] == xDim - 1) posCenter[0]--;
	if (posCenter[1] == yDim - 1) posCenter[1]--;
	if (posCenter[2] == zDim - 1) posCenter[2]--;
	//	compute indices of vertices with respect to this cell
	i1 = ((pos1[0] - posCenter[0]) << 2) + ((pos1[1] - posCenter[1]) << 1) + (pos1[2] - posCenter[2]);
	i2 = ((pos2[0] - posCenter[0]) << 2) + ((pos2[1] - posCenter[1]) << 1) + (pos2[2] - posCenter[2]);
	//	compute indices
	//	first, go into the cell we have identified
	whichCubeEdge = vertex2edge[i1][i2];
	if (whichCubeEdge == -1)
	{
		printf("Major problem: %ld to %ld is not a valid seed edge\n", i1, i2); return;
	}

	//	now figure out which case the cube uses
	if ((posCenter[0] < 0) || (posCenter[1] < 0) || (posCenter[2] < 0)) return;
	if ((posCenter[0] > xDim - 2) || (posCenter[1] > yDim - 2) || (posCenter[2] > zDim - 2)) return;

	for (int i = 0; i < 8; i++){
		cubeVert[i] = mVolumn->At(posCenter + CellVertices[i]);
	}

	theCase = 0;
	for (whichVertex = 0; whichVertex < 8; whichVertex++)
		if (mIsovalue < cubeVert[whichVertex])
			theCase |= (1 << whichVertex);

	theSurface = seedEdge2Surface[theCase][whichCubeEdge];
	if (theSurface == -1)
	{
		printf("Major problem: Edge %ld in case %ld is not a valid seed edge\n", whichCubeEdge, theCase); return;
	}

	whichFaceEdge = surface2exitEdges[theCase][theSurface][0];

	IntersectSurface(posCenter.toDim<4>(whichFaceEdge));
}

void MyIsosurfaceTracker::IntersectSurface(const MyVec4i& cellEntryEdge){	
	long whichVertex;
	int xDim = mVolumn->GetDimSize(0);
	int yDim = mVolumn->GetDimSize(1);
	int zDim = mVolumn->GetDimSize(2);

	std::queue<MyVec4i> theQueue;
	theQueue.push(cellEntryEdge);

	while (!theQueue.empty()){
		MyVec4i thisCellEntryEdge = theQueue.front();
		theQueue.pop();
		if ((thisCellEntryEdge[0] < 0) 
			|| (thisCellEntryEdge[1] < 0) 
			|| (thisCellEntryEdge[2] < 0))
			continue;
		if ((thisCellEntryEdge[0] > xDim - 2) 
			|| (thisCellEntryEdge[1] > yDim - 2) 
			|| (thisCellEntryEdge[2] > zDim - 2))
			continue;

		MyVec8f cubeVert;
		for (int i = 0; i < 8; i++){
			cubeVert[i] = mVolumn->At(thisCellEntryEdge.toDim<3>() + CellVertices[i]);
		}

		int theCase = 0;
		for (whichVertex = 0; whichVertex < 8; whichVertex++){
			if (mIsovalue < cubeVert[whichVertex]){
				theCase |= (1 << whichVertex);
			}
		}
 
		int theSurface = entryEdge2Surface[theCase][thisCellEntryEdge[3]];
		MyVec4i thisCellSurface(thisCellEntryEdge[0], thisCellEntryEdge[1],
			thisCellEntryEdge[2], theSurface);
		if (CheckSurfaceFlag(thisCellSurface)){
			continue;
		}
		FlagSurface(thisCellSurface);

		for (int whichTri = 0; whichTri < 3 * nTriangles[theCase][theSurface]; whichTri += 3)
			AddTriangle(thisCellSurface.toDim<3>(), cubeVert, MyVec3i(
			mcFollowTriangles[theCase][theSurface][whichTri + 0],
			mcFollowTriangles[theCase][theSurface][whichTri + 1],
			mcFollowTriangles[theCase][theSurface][whichTri + 2]));

		//	now follow the contour out each face
		for (int whichExitFaceEdge = 0; 
			whichExitFaceEdge < nExitEdges[theCase][theSurface]; 
			whichExitFaceEdge++){
			int theExitFaceEdge = surface2exitEdges[theCase][theSurface][whichExitFaceEdge];
			theQueue.push(MyVec4i(
				thisCellEntryEdge[0] + exitDirection[theExitFaceEdge][0],
				thisCellEntryEdge[1] + exitDirection[theExitFaceEdge][1],
				thisCellEntryEdge[2] + exitDirection[theExitFaceEdge][2],
				exit2entryEdge[theExitFaceEdge]));
		}
	}	
}

void MyIsosurfaceTracker::AddTriangle(const MyVec3i& cellIdx, 
	const MyVec8f& cellValues, const MyVec3i& edgeIdxes){
	AddVertex(cellIdx, cellValues, edgeIdxes[0]);
	AddVertex(cellIdx, cellValues, edgeIdxes[1]);
	AddVertex(cellIdx, cellValues, edgeIdxes[2]);
	int startIdx = mTriangles->size() * 3;
	MyVec3i triangle(startIdx, startIdx + 1, startIdx + 2);
	mTriangles->PushBack(triangle);
}

void MyIsosurfaceTracker::AddVertex(const MyVec3i& cellIdx, 
	const MyVec8f& cellValues, int edgeIdx){
	int v0 = edgeVertices[edgeIdx][0], v1 = edgeVertices[edgeIdx][1];
	MyVec3i edgeVertex0, edgeVertex1;
	for (int i = 0; i < 3; i++){
		edgeVertex0[i] = cellIdx[i] + mcFollowVertexCoords[v0][i];
		edgeVertex1[i] = cellIdx[i] + mcFollowVertexCoords[v1][i];
	}
	MyVec3f norm0 = ComputeNormal(edgeVertex0);
	MyVec3f norm1 = ComputeNormal(edgeVertex1);
	float frac;
	if (cellValues[v0] == cellValues[v1]) frac = 0;
	else frac = (cellValues[v0] - mIsovalue) / (cellValues[v0] - cellValues[v1]);
	MyVec3f norm = (1 - frac)*norm0 + frac*norm1;
	norm.normalize();
	mNormals->PushBack(norm);
	MyVec3f vertex = MyVec3f((1 - frac)*edgeVertex0[0] + frac*edgeVertex1[0],
		(1 - frac)*edgeVertex0[1] + frac*edgeVertex1[1],
		(1 - frac)*edgeVertex0[2] + frac*edgeVertex1[2]);
	mVertices->PushBack(vertex);
	mBoundingBox.Engulf(vertex);
}

MyVec3f MyIsosurfaceTracker::ComputeNormal(const MyVec3i& pos) const{
	MyVec3f normal;
	float value0 = mVolumn->At(pos);
	for (int i = 0; i < 3; i++){
		if (pos[i] == 0)
			normal[i] = 2 * (mVolumn->At(pos + MyVec3i::baseUnit(i)) - value0);
		else if (pos[i] == mVolumn->GetDimSize(i) - 1)
			normal[0] = 2 * (value0 - mVolumn->At(pos - MyVec3i::baseUnit(i)));
		else
			normal[i] = mVolumn->At(pos + MyVec3i::baseUnit(i))
			- mVolumn->At(pos - MyVec3i::baseUnit(i));
	}
	return normal;
}