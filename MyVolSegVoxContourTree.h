#pragma once

#include "MyVolumeSegmenterVox.h"
#include <vector>

class MyContourTree;

class MyVolSegVoxContourTree :
	public MyVolumeSegmenterVox
{
public:
	MyVolSegVoxContourTree();
	~MyVolSegVoxContourTree();

	virtual MyArraySPtr<MySegmentNodeSPtr> MakeSegments() const;

protected:
	static void CovertToVoxContainer(
		MyArraySPtr<MyVoxContainerfSPtr> containers,
		const MyArray<const std::vector<float*>* > *voxesVector,
		int startIdx, int endIdx, const float* firstVolIdx, 
		const MyContourTree* ct, const My3dArrayf* vol);
};

