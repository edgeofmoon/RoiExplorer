#include "MySegNodeInfoEncoder2D.h"
#include <algorithm>
using namespace std;

MySegNodeInfoEncoder2D::MySegNodeInfoEncoder2D()
{
}


MySegNodeInfoEncoder2D::~MySegNodeInfoEncoder2D()
{
}

void MySegNodeInfoEncoder2D::Update(){
	float randX = (rand() % 1001) / (float)1000;
	float randY = (rand() % 1001) / (float)1000;
	MyVec3f massCenter = mSegNodeInfo->GetSegmentNodeMassCenter();
	MyVec3i volSize = mSegNodeInfo->GetSegmentNode()->GetVolumeSize();
	MyVec2f lowPos(massCenter[0] / (volSize[0]/2) - 0.5,
		massCenter[1] / (volSize[1]/2) - 0.5);
	float avg = mSegNodeInfo->GetSegmentNodeMeanAverage();
	float nVox = mSegNodeInfo->GetSegmentNode()->GetNumTotalVoxes();
	float width = sqrt(nVox)/100;
	MyVec2f size(0.02f*width, 0.02f*width);
	mBox.SetLow(lowPos);
	mBox.SetHigh(lowPos + size);
	mColor = MyVec4f(avg, 1-avg, 0, 0.5);
}