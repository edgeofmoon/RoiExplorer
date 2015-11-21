#pragma once

#include "MyArrayMD.h"
#include "MyArray.h"
#include "MySegmentNode.h"

class MyVolumeSegmenterVox
{
public:
	MyVolumeSegmenterVox();
	~MyVolumeSegmenterVox();

	virtual MyArraySPtr<MySegmentNodeSPtr> MakeSegments() const = 0;

	virtual void SetVolumeData(My3dArrayfScPtr vol){ mVolume = vol; };

protected:
	My3dArrayfScPtr mVolume;
};

