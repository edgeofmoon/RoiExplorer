#pragma once
#include "MyVolumeSegmenterVox.h"
class MyVolumeSegmenterTemplate :
	public MyVolumeSegmenterVox
{
public:
	MyVolumeSegmenterTemplate();
	~MyVolumeSegmenterTemplate();

	virtual MyArraySPtr<MySegmentNodeSPtr> MakeSegments() const;
};

