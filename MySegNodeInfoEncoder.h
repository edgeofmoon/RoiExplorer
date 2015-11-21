#pragma once

#include "MySegmentNodeInfo.h"

class MySegNodeInfoEncoder
{
public:
	MySegNodeInfoEncoder();
	~MySegNodeInfoEncoder();

	void SetSegmentInfo(MySegmentNodeInfoScPtr segInfo){ mSegNodeInfo = segInfo; };
	MySegmentNodeInfoScPtr GetSegmentInfo() const { return mSegNodeInfo; };
	MyVoxContainerfScPtr GetVoxMappedValues() const { 
		return mVoxMappedValues;
	};

	void Update();

protected:
	MySegmentNodeInfoScPtr mSegNodeInfo;

	// encoded volume
	MyVoxContainerfSPtr mVoxMappedValues;
};

typedef MySharedPointer<MySegNodeInfoEncoder> MySegNodeInfoEncoderSPtr;
typedef MySharedPointer<const MySegNodeInfoEncoder> MySegNodeInfoEncoderScPtr;