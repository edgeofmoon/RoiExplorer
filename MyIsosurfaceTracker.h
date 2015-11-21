#pragma once

#include "MyArrayMD.h"
#include "MyBox.h"

class MyIsosurfaceTracker
{
public:
	MyIsosurfaceTracker();
	~MyIsosurfaceTracker();

	void Update();

	void SetVolumn(My3dArrayfScPtr vol){
		mVolumn = vol;
	}
	void SetStartIndex(int sidx){
		mStartIndex = sidx;
	}
	void SetIsovalue(float value){
		mIsovalue = value;
	}

	int GetStartIndex() const{
		return mStartIndex;
	}

	MyArray3fScPtr GetVertices() const{
		return mVertices;
	}

	MyArray3fScPtr GetNormals() const{
		return mNormals;
	}

	MyArray3iScPtr GetTriangles() const{
		return mTriangles;
	}

	const MyBox3f& GetBoundingBox() const{
		return mBoundingBox;
	}

protected:
	My3dArrayfScPtr mVolumn;
	float mIsovalue;
	int mStartIndex;
	MyBox3f mBoundingBox;

	MyArray3fSPtr mVertices;
	MyArray3fSPtr mNormals;
	MyArray3iSPtr mTriangles;

	// search for seeds
	MyVec2i FindIsoseeds();
	bool CheckSeeds(float value0, float value1) const;
	void QueueJoinNeighbors(const MyVec3i& pos, MyArray3i& joinNeighbors) const;

	// do surface tracking
	static MyVec3i CellVertices[8];
	My3dArraycSPtr mSurfaceVisit;
	void FlagSurface(const MyVec4i& surface);
	void UnFlagSurface(const MyVec4i& surface);
	bool CheckSurfaceFlag(const MyVec4i& surface) const;
	void UpdateSurfaceVisit();
	void SurfaceTracking(const MyVec2i& seeds);
	void IntersectSurface(const MyVec4i& cellEntryEdge);
	typedef MyVec<float, 8> MyVec8f;
	void AddTriangle(const MyVec3i& cellIdx, 
		const MyVec8f& cellValues, const MyVec3i& edgeIdxes);
	void AddVertex(const MyVec3i& cellIdx,
		const MyVec8f& cellValues, int edgeIdx);
	MyVec3f ComputeNormal(const MyVec3i& pos) const;

};

