#include "MySegsVolRenderer.h"
#include "MyGLHeader.h"

#include <iostream>
#include <algorithm>

MySegsVolRenderer::MySegsVolRenderer()
{
}


MySegsVolRenderer::~MySegsVolRenderer()
{
}

void MySegsVolRenderer::Update(){
	MyVec3i volSizes = mSegNodeInfoAssemble->GetVoxMappedValueVolume()->GetDimSizes();
	MyVolumeRenderer::LoadVolume(&mSegNodeInfoAssemble->GetVoxMappedValueVolume()->GetDataArray()[0],
		volSizes[0], volSizes[1], volSizes[2]);
}
