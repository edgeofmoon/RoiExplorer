#include "MySegNodeInfoAssemble.h"
#include "MySegNodeInfoEncoder.h"
#include "MySegNodeInfoEncoder2D.h"

#include <algorithm>
using namespace std;

MySegNodeInfoAssemble::MySegNodeInfoAssemble()
{
	mSegNodeInfos = std::make_shared<MyArray<MySegmentNodeInfoScPtr>>();
}


MySegNodeInfoAssemble::~MySegNodeInfoAssemble()
{
}

void MySegNodeInfoAssemble::Clear(){
	mSegNodeInfos->clear();
}

void MySegNodeInfoAssemble::AddSegmentNodeInfo(MySegmentNodeInfoScPtr segNodeInfo){
	mSegNodeInfos->PushBack(segNodeInfo);
}

void MySegNodeInfoAssemble::Update(){
	UpdateAllSegmentVolumes();
	UpdateAllSegmentBoxes();
}

void MySegNodeInfoAssemble::UpdateAllSegmentVolumes(){
	if (mSegNodeInfos->empty()) return;
	std::sort(mSegNodeInfos->begin(), mSegNodeInfos->end(), MySegNodeInfoAssemble::SegNodeLarger);
	MyVec3i volSizes = mSegNodeInfos->front()->GetSegmentNode()->GetVolumeSize();
	MyVoxContainer_Large<float> container(volSizes, 0);
	MySegNodeInfoEncoder encoder;
	for (int i = 0; i < mSegNodeInfos->size(); i++){
		encoder.SetSegmentInfo(mSegNodeInfos->at(i));
		encoder.Update();
		container.Add(encoder.GetVoxMappedValues().get());
	}
	mVoxMappedValueVolume = My3dArrayfSPtr(container.MakeVolume());
}

void MySegNodeInfoAssemble::UpdateAllSegmentBoxes(){
	if (mSegNodeInfos->empty()) return;
	std::sort(mSegNodeInfos->begin(), mSegNodeInfos->end(), MySegNodeInfoAssemble::SegNodeLarger);
	mSegment2DBoxes = make_shared<MyMap<const MySegmentNode*, MyBox2f>>();
	mSegment2DColors = make_shared<MyMap<const MySegmentNode*, MyVec4f>>();
	MySegNodeInfoEncoder2D encoder;
	for (int i = 0; i < mSegNodeInfos->size(); i++){
		const MySegmentNode* nodeInfo = mSegNodeInfos->at(i)->GetSegmentNode().get();
		encoder.SetSegmentInfo(mSegNodeInfos->at(i).get());
		encoder.Update();
		mSegment2DBoxes->operator[](nodeInfo) = encoder.GetBox();
		mSegment2DColors->operator[](nodeInfo) = encoder.GetColor();
	}
}

bool MySegNodeInfoAssemble::SegNodeSmaller(const MySegmentNodeInfoScPtr& seg0,
	MySegmentNodeInfoScPtr& seg1){
	return seg0->GetSegmentNode()->GetNumTotalVoxes()
		< seg1->GetSegmentNode()->GetNumTotalVoxes();
}

bool MySegNodeInfoAssemble::SegNodeLarger(const MySegmentNodeInfoScPtr& seg0,
	MySegmentNodeInfoScPtr& seg1){
	return seg0->GetSegmentNode()->GetNumTotalVoxes()
		> seg1->GetSegmentNode()->GetNumTotalVoxes();
}