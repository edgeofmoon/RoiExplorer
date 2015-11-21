#pragma once

#include "MyArray.h"
#include "MyArrayMD.h"
#include "MySharedPointer.h"
#include "MySegmentNodeInfo.h"
#include "MyMap.h"

class MySegNodeInfoAssemble
{
public:
	MySegNodeInfoAssemble();
	~MySegNodeInfoAssemble();

	My3dArrayfScPtr GetVoxMappedValueVolume() const{
		return mVoxMappedValueVolume;
	};
	MyMapScPtr<const MySegmentNode*, MyBox2f> GetSegment2DBoxes() const{
		return mSegment2DBoxes;
	}
	MyMapScPtr<const MySegmentNode*, MyVec4f> GetSegment2DColors() const{
		return mSegment2DColors;
	}
	MyArrayScPtr<MySegmentNodeInfoScPtr> GetSegmentNodeInfos() const {
		return mSegNodeInfos;
	}
	void Clear();
	void AddSegmentNodeInfo(MySegmentNodeInfoScPtr segNodeInfo);
	// sort and collect voxel mapped values
	void Update();

protected:
	MyArraySPtr<MySegmentNodeInfoScPtr> mSegNodeInfos;

	// graphics data members
	My3dArrayfSPtr mVoxMappedValueVolume;
	MyMapSPtr<const MySegmentNode*, MyBox2f> mSegment2DBoxes;
	MyMapSPtr<const MySegmentNode*, MyVec4f> mSegment2DColors;

	// different type of update
	void UpdateAllSegmentVolumes();
	void UpdateAllSegmentBoxes();

	// for sorting
	static bool SegNodeSmaller(const MySegmentNodeInfoScPtr& seg0,
		MySegmentNodeInfoScPtr& seg1);
	static bool SegNodeLarger(const MySegmentNodeInfoScPtr& seg0,
		MySegmentNodeInfoScPtr& seg1);
};

typedef MySharedPointer<MySegNodeInfoAssemble> MySegNodeInfoAssembleSPtr;
typedef MySharedPointer<const MySegNodeInfoAssemble> MySegNodeInfoAssembleScPtr;