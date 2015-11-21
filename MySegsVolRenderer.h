#pragma once

#include "MySegNodeInfoAssemble.h"
#include "MyVolumeRenderer.h"

class MySegsVolRenderer
	:public MyVolumeRenderer
{
public:
	MySegsVolRenderer();
	~MySegsVolRenderer();
	
	void SetSegmentNodeInfoAssemble(MySegNodeInfoAssembleScPtr segNodeInfoAssemble){
		mSegNodeInfoAssemble = segNodeInfoAssemble;
	};

	MySegNodeInfoAssembleScPtr GetSegmentNodeInfoAssemble(){
		return mSegNodeInfoAssemble;
	};

	void Update();

protected:
	MySegNodeInfoAssembleScPtr mSegNodeInfoAssemble;
};

