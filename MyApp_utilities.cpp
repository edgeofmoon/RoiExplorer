#include "MyApp.h"
#include "MyIsosurfaceTracker.h"

int MyApp::FindLowestIndex(const MyVoxContainerf* voxels,
	const My3dArrayf* vol) const{
	MyVoxContainerf::const_Iterator itr = voxels->Begin();
	MyVoxContainerf::const_Iterator itrEnd = voxels->End();
	int currentIndex = itr.GetVoxelIndex();
	float currentValue = vol->At(currentIndex);
	for (itr; itr != itrEnd; itr++){
		int index = itr.GetVoxelIndex();
		float value = vol->At(index);
		if (value < currentValue){
			currentValue = value;
			currentIndex = index;
		}
	}
	return currentIndex;
}

int MyApp::FindLowestIndex(const MyVoxContainerf* voxels) const{
	MyVoxContainerf::const_Iterator itr = voxels->Begin();
	MyVoxContainerf::const_Iterator itrEnd = voxels->End();
	int currentIndex = itr.GetVoxelIndex();
	float currentValue = *itr;
	for (itr; itr != itrEnd; itr++){
		float value = *itr;
		if (value < currentValue){
			currentValue = value;
			currentIndex = itr.GetVoxelIndex();
		}
	}
	return currentIndex;
}