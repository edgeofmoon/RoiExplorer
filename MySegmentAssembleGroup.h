#pragma once
#include "MyArray.h"
#include "MySegNodeInfoAssemble.h"
#include "MyMap.h"
#include "MySharedPointer.h"

class MySegmentAssembleGroup :
	public MyArray <MySegNodeInfoAssembleSPtr>
{
public:
	MySegmentAssembleGroup();
	~MySegmentAssembleGroup();

	void Update();
	const MyMap<const MySegmentNode*, float>& GetTScores() const {
		return mSegmentTScores;
	}

protected:

	MyMap<const MySegmentNode*, float> mSegmentTScores;
};

typedef MySharedPointer<MySegmentAssembleGroup> MySegmentAssembleGroupSPtr;
typedef MySharedPointer<const MySegmentAssembleGroup> MySegmentAssembleGroupScPtr;