#include "MyVolumeSegmenterTemplate.h"
#include "MyMap.h"
#include <list>
using namespace std;

MyVolumeSegmenterTemplate::MyVolumeSegmenterTemplate()
{
}


MyVolumeSegmenterTemplate::~MyVolumeSegmenterTemplate()
{
}

MyArraySPtr<MySegmentNodeSPtr> MyVolumeSegmenterTemplate::MakeSegments() const{
	mVolume;
	MyMap<float, MyArrayi> regions;
	for (int index = 0; index < mVolume->GetVolume(); index++){
		float regionIdx = mVolume->At(index);
		if (regionIdx > 0){
			regions[regionIdx] << index;
		}
	}
	MyArraySPtr<MySegmentNodeSPtr> segNodes = std::make_shared<MyArray<MySegmentNodeSPtr>>();
	MyMap<float, MyArrayi>::iterator itr = regions.begin();
	while (itr != regions.end()){
		MyArrayi &indices = itr->second;
		MyVoxContainerfSPtr voxes = 
			MyVoxContainerf::MakeVoxContainer(&indices, itr->first, mVolume->GetDimSizes());
		MySegmentNodeSPtr segNode = std::make_shared<MySegmentNode>();
		segNode->SetUniqueVoxes(voxes);
		segNode->SetIndex((int)itr->first+0.5);
		segNodes->push_back(segNode);
		itr++;
	}

	return segNodes;
}