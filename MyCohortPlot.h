#pragma once

#include "MySegNodeInfoAssemble.h"
#include "MyDistanceLayout2D.h"

class MyCohortPlot
{
public:
	MyCohortPlot();
	~MyCohortPlot();

	void Render(int w, int h);
	void Update();

	void SetSegmentNodeInfoAssemble(MySegNodeInfoAssembleScPtr segNodeInfoAsmb){
		mSegNodeInfoAssemble = segNodeInfoAsmb;
	}

protected:
	MySegNodeInfoAssembleScPtr mSegNodeInfoAssemble;
	MyMatrixfSPtr mDistanceMatrix;

	MyDistanceLayout2D mLayout;
};

