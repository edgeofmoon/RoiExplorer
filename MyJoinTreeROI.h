#pragma once
#include "MyJoinTree.h"

class MyJoinTreeROI :
	public MyJoinTree
{
public:
	MyJoinTreeROI();
	~MyJoinTreeROI();

	void SetROIs(const MyArray<MySegmentNodeSPtr>* ROIs);

	void Update();

protected:
	MyArray<MySegmentNodeSPtr> mROIs;

};


typedef MySharedPointer<MyJoinTreeROI> MyJoinTreeROISPtr;
typedef MySharedPointer<const MyJoinTreeROI> MyJoinTreeROIScPtr;