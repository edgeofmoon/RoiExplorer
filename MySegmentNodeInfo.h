#pragma once

#include "MyVoxContainer.h"
#include "MySharedPointer.h"
#include "MySegmentNode.h"

class MySegmentNodeInfo
{
public:
	MySegmentNodeInfo();
	~MySegmentNodeInfo();

	// set data
	void SetSegmentNode(MySegmentNodeScPtr segNode){ mSegNode = segNode; };
	void SetVolumes(MyArrayScPtr<My3dArrayfScPtr> vols){ mVolumes = vols; };

	// query data
	MySegmentNodeScPtr GetSegmentNode() const { return mSegNode; };
	MyArrayScPtr<My3dArrayfScPtr> GetVolumes() const { return mVolumes; };

	// query statistics
	const MyArrayf& GetSegmentNodeMeans() const{ return mSegNodeMeans; };
	float GetSegmentNodeMeanAverage() const{ return mSegNodeMeanAverage; };
	float GetSegmentNodeMeanStdev() const{ return mSegNodeMeanStdev; };
	MyVec3f GetSegmentNodeMassCenter() const{ return mSegmentMassCenter; };
	MyVec2f GetSegmentNodeMeanRange() const{ return mRange; };

	// generate information
	void Update();

protected:
	MySegmentNodeScPtr mSegNode;
	MyArrayScPtr<My3dArrayfScPtr> mVolumes;

	// compute segment average value
	// across multiple volumes
	float ComputeSegmentAverage(int volIdx) const;

	/********* computed information of segment *********/
	// segment-mean for all subjects
	MyArrayf mSegNodeMeans;
	// segment-mean average value across subjects 
	float mSegNodeMeanAverage;
	// segment-mean standard deviation across subjects
	float mSegNodeMeanStdev;
	// the mass center of the segment
	MyVec3f mSegmentMassCenter;
	MyVec3f ComputeSegmentMassCenter() const;
	// compute range
	MyVec2f mRange;
};

typedef MySharedPointer<MySegmentNodeInfo> MySegmentNodeInfoSPtr;
typedef MySharedPointer<const MySegmentNodeInfo> MySegmentNodeInfoScPtr;