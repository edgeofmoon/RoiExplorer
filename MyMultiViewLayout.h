#pragma once

#include "MyBox.h"
#include "MyMap.h"
#include "MySharedPointer.h"

class MyMultiViewLayout
{
public:
	MyMultiViewLayout();
	~MyMultiViewLayout();

	void Update();

	void SetGlobalViewport(const MyBox2i& viewport){
		mGlobalViewport = viewport;
	}

	const MyBox2i& GetViewport(int index) const{
		return mViewports.at(index);
	}

	int GetViewportIndex(const MyVec2i& pos) const;
protected:
	MyMap<int, MyBox2i> mViewports;
	MyBox2i mGlobalViewport;

	MyBox2i CutBox(const MyBox2i& box, int iDim, 
		float startRatio, float endRatio) const;
};

typedef MySharedPointer<MyMultiViewLayout> MyMultiViewLayoutSPtr;
typedef MySharedPointer<const MyMultiViewLayout> MyMultiViewLayoutScPtr;