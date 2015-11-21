#include "MyBubbleSetsDrawingHelper.h"
#include "MyMathHelper.h"

// debug
#include <iostream>
using namespace std;


float MyBubbleSetsDrawingHelper::EnergyMaxRadius = 0.25;
float MyBubbleSetsDrawingHelper::EnergyCoreRadius = 0.025;

MyBubbleSetsDrawingHelper::MyBubbleSetsDrawingHelper()
{
}


MyBubbleSetsDrawingHelper::~MyBubbleSetsDrawingHelper()
{
}

void MyBubbleSetsDrawingHelper::Update(){
	// retrieve center line
	return;
	mExcluded.clear();
	const MyMap<const MySegmentNode*, SegmentLayoutInfo>& info 
		= mLayout->GetLayoutInfo();
	const MySegmentNode* highest 
		= info.at(mLayout->GetJoinTreeRoot().get()).highest;
	const MySegmentNode* excluded = highest;
	while (excluded->GetNumParents() > 0){
		excluded = excluded->GetParent().get();
		mExcluded[excluded] = true;
	}
}

MyBox2f MyBubbleSetsDrawingHelper::ComputeBoundingBox(
	const MyArray<const MySegmentNode*>* segments) const{
	MyBox2f box(MyVec2f(FLT_MAX, FLT_MAX), MyVec2f(-FLT_MAX, -FLT_MAX));
	for (int i = 0; i < segments->size(); i++){
		const MySegmentNode* seg = segments->at(i);
		//if (mExcluded.HasKey(seg)) continue;
		MyBox2f segBox = mLayout->GetPosition(seg);
		box.Engulf(segBox);
	}
	if (box.GetHighPos() < box.GetLowPos()){
		cout << "All segments are on the center line." << endl;
	}
	return box;
}

My2dfGridfSPtr MyBubbleSetsDrawingHelper::ComputeEnergyFieldSimple(
	const MyArray<const MySegmentNode*>* segments) const{
	MyBox2f box = this->ComputeBoundingBox(segments);
	MyArraySPtr<const MySegmentNode*> allSegments 
		= this->ComputeSegmentsInBox(box);
	MyMap<const MySegmentNode*, bool> segmentClass;
	for (int i = 0; i < allSegments->size(); i++){
		segmentClass[allSegments->at(i)] = false;
	}
	for (int i = 0; i < segments->size(); i++){
		segmentClass[segments->at(i)] = true;
	}
	My2dfGridfSPtr grid = std::make_shared<My2dfGridf>(MyVec2i(1920, 540) / 2);
	grid->SetType(My2dfGridf::GridType_Lattice);
	//grid->Set(box.GetLowPos(), box.GetHighPos());
	grid->Set(mLayout->GetTreeBox());
	for (int i = 0; i < grid->GetDimSize(0); i++){
		for (int j = 0; j < grid->GetDimSize(1); j++){
			MyVec2i index(i, j);
			MyVec2f pos = grid->ComputePosition(index);
			MyMap<const MySegmentNode*, bool>::const_iterator itr 
				= segmentClass.begin();
			float totalEnergy = 0;
			while (itr != segmentClass.end()){
				MyBox2f segBox = mLayout->GetPosition(itr->first);
				float distance = segBox.ComputeNearestDistance(pos);
				float energy = this->ComputeEnergy(distance);
				//float weight = segBox.GetSize(1);
				float weight = 1;
				if (!itr->second) weight = -0.8;
				totalEnergy += weight*energy;
				itr++;
			}
			grid->operator[](index) = totalEnergy;
		}
	}
	return grid;
}

My2dfGridfSPtr MyBubbleSetsDrawingHelper::ComputeEnergyField(
	const MyArray<const MySegmentNode*>* segments) const{
	MyBox2f box = this->ComputeBoundingBox(segments);
	My2dfGridfSPtr field = std::make_shared<My2dfGridf>(MyVec2i(1920, 540) / 2);
	field->SetType(My2dfGridf::GridType_Lattice);
	field->Set(box.GetLowPos(), box.GetHighPos());
	field->Set(mLayout->GetTreeBox());
	MyArraySPtr<const MySegmentNode*> allSegments
		= this->ComputeSegmentsInBox(box);
	MyMap<const MySegmentNode*, bool> segmentClass;
	for (int i = 0; i < allSegments->size(); i++){
		segmentClass[allSegments->at(i)] = false;
	}
	for (int i = 0; i < segments->size(); i++){
		segmentClass[segments->at(i)] = true;
	}
	MyArray<const MySegmentNode*> segmentsObs;
	MyMap<const MySegmentNode*, bool>::const_iterator itrObs
		= segmentClass.begin();
	while (itrObs != segmentClass.end()){
		if (!itrObs->second){
			segmentsObs << itrObs->first;
		}
		itrObs++;
	}
	MyVec2f centroid = this->ComputeCentroid(segments);
	MyMap<float, const MySegmentNode*> segmentsDistance;
	for (int i = 0; i < segments->size(); i++){
		const MySegmentNode* seg = segments->at(i);
		MyVec2f segCenter = mLayout->GetPosition(seg).GetCenter();
		float distance = (centroid - segCenter).norm();
		segmentsDistance[distance] = seg;
	}
	MyArray<const MySegmentNode*> segmentsOrdered;
	MyMap<float, const MySegmentNode*>::const_iterator itr
		= segmentsDistance.begin();
	while (itr != segmentsDistance.end()){
		segmentsOrdered << itr->second;
		itr++;
	}
	for (int i = 0; i < segmentsOrdered.size(); i++){
		this->UpdateEnergyField(&segmentsObs, &segmentsOrdered, field.get(), i);
	}
	return field;
}

MyArraySPtr<const MySegmentNode*> MyBubbleSetsDrawingHelper::ComputeSegmentsInBox(
	const MyBox2f& box) const{
	MyArraySPtr<const MySegmentNode*> rst 
		= std::make_shared<MyArray<const MySegmentNode*>>();
	const MyMap<const MySegmentNode*, MyBox2f>& pos = mLayout->GetPositions();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = pos.begin();
	while (itr != pos.end()){
		if (box.IsIntersected(itr->second)){
			rst->PushBack(itr->first);
		}
		itr++;
	}
	return rst;
}

MyPolyline2fSPtr MyBubbleSetsDrawingHelper::ComputeLink(
	const MyArray<const MySegmentNode*>* segmentsObs,
	const MyArray<const MySegmentNode*>* segmentsOrdered,
	int currentSegIdx) const{
	if (currentSegIdx == 1) {
		MyVec2f c0 = mLayout->GetPosition(segmentsOrdered->at(0)).GetCenter();
		MyVec2f c1 = mLayout->GetPosition(segmentsOrdered->at(1)).GetCenter();
		return ComputeLink(segmentsObs, MyLine2f(c0, c1));
	}
	const MySegmentNode* theSegment = segmentsOrdered->at(currentSegIdx);
	const MyBox2f& theSegmentBox = mLayout->GetPosition(theSegment);
	MyVec2f theSegmentCenter = theSegmentBox.GetCenter();
	MyVec2f minSegCenter;
	int minObsCount = segmentsObs->size() + 1;
	for (int i = 0; i < currentSegIdx - 1; i++){
		const MySegmentNode* thisSegment = segmentsOrdered->at(i);
		const MyBox2f& thisSegmentBox = mLayout->GetPosition(thisSegment);
		MyVec2f thisSegmentCenter = thisSegmentBox.GetCenter();
		MyLine2f directLink(theSegmentCenter, thisSegmentCenter);
		int obsCount = ComputeIntersectionCount(segmentsObs, directLink);
		if (obsCount < minObsCount){
			minObsCount = obsCount;
			minSegCenter = thisSegmentCenter;
			if (minObsCount == 0) break;
		}
	}
	MyLine2f bestLink(theSegmentCenter, minSegCenter);
	return ComputeLink(segmentsObs, bestLink);
}

void MyBubbleSetsDrawingHelper::UpdateEnergyField(
	const MyArray<const MySegmentNode*>* segmentsObs,
	const MyArray<const MySegmentNode*>* segmentsOrdered,
	My2dfGridf* field, int currentSegIdx) const{
	// add potential due to i = currentSegIdx
	const MySegmentNode* theSegment = segmentsOrdered->at(currentSegIdx);
	const MyBox2f theBox = mLayout->GetPosition(theSegment);
	this->UpdateEnergyField(field, theBox, 1);
	// add potential due to nearest virtual edge i -> j
	if (currentSegIdx > 1){
		MyPolyline2fSPtr link = this->ComputeLink(
			segmentsObs, segmentsOrdered, currentSegIdx);
		this->UpdateEnergyField(field, link.get(), 1);
	}
	// subtract potential due to nearby non - set members k not in s
	// put global subtracting here for convenience
	// only do it at last segment index
	if (currentSegIdx == segmentsOrdered->size() - 1){
		for (int i = 0; i < segmentsObs->size(); i++){
			const MySegmentNode* segObs = segmentsObs->at(i);
			const MyBox2f theBoxObs = mLayout->GetPosition(segObs);
			this->UpdateEnergyField(field, theBoxObs, -0.8);
		}
	}
}

void MyBubbleSetsDrawingHelper::UpdateEnergyField(
	My2dfGridf* field, const MyBox2f box, float weight) const{
	MyVec2f theCenter = box.GetCenter();
	MyVec2f expand(EnergyMaxRadius, EnergyMaxRadius);
	MyBox2f theBoxExp(box.GetLowPos() - expand, box.GetHighPos() + expand);
	MyVec2i startIdx = field->ComputeIndex(theBoxExp.GetLowPos());
	MyVec2i endIdx = field->ComputeIndex(theBoxExp.GetHighPos());
	MyVec2i maxIdx = field->GetDimSize();
	MyVec2i::Clamp(startIdx, MyVec2i::zero(), maxIdx);
	MyVec2i::Clamp(endIdx, MyVec2i::zero(), maxIdx);
	// negative energy only influence postive field
	if (weight < 0){
		for (int i = startIdx[0]; i < endIdx[0]; i++){
			for (int j = startIdx[1]; j < endIdx[1]; j++){
				MyVec2i idx(i, j);
				float fieldValue = field->At(idx);
				// this is a postive field, skip
				if (fieldValue <= 0) continue;
				MyVec2f pos = field->ComputePosition(idx);
				float distance = (pos - theCenter).norm();
				float energy = ComputeEnergy(distance);
				field->operator[](idx) += energy*weight;
			}
		}
	}
	else{
		for (int i = startIdx[0]; i < endIdx[0]; i++){
			for (int j = startIdx[1]; j < endIdx[1]; j++){
				MyVec2i idx(i, j);
				MyVec2f pos = field->ComputePosition(idx);
				float distance = (pos - theCenter).norm();
				float energy = ComputeEnergy(distance);
				field->operator[](idx) += energy*weight;
			}
		}
	}
}

void MyBubbleSetsDrawingHelper::UpdateEnergyField(
	My2dfGridf* field, const MyPolyline2f* line, float weight) const{
	MyBox2f box = line->GetBoundingBox();
	MyVec2f expand(EnergyMaxRadius, EnergyMaxRadius);
	MyBox2f boxExp(box.GetLowPos() - expand, box.GetHighPos() + expand);
	MyVec2i startIdx = field->ComputeIndex(boxExp.GetLowPos());
	MyVec2i endIdx = field->ComputeIndex(boxExp.GetHighPos());
	MyVec2i maxIdx = field->GetDimSize();
	MyVec2i::Clamp(startIdx, MyVec2i::zero(), maxIdx);
	MyVec2i::Clamp(endIdx, MyVec2i::zero(), maxIdx);
	for (int i = startIdx[0]; i < endIdx[0]; i++){
		for (int j = startIdx[1]; j < endIdx[1]; j++){
			MyVec2i idx(i, j);
			MyVec2f pos = field->ComputePosition(idx);
			float minDist = FLT_MAX;
			for (int iSeg = 0; iSeg < line->GetNumLineSegments(); iSeg++){
				MyLine2f lineSeg = line->GetLineSegment(iSeg);
				float dist = MyMathHelper::MinDistance(lineSeg, pos);
				if (dist < minDist){
					minDist = dist;
				}
			}
			float energy = ComputeEnergy(minDist);
			field->operator[](idx) += energy*weight;
		}
	}
}

MyPolyline2fSPtr MyBubbleSetsDrawingHelper::ComputeLink(
	const MyArray<const MySegmentNode*>* segmentsObs,
	const MyLine2f& line) const{
	// tmp sol
	return make_shared<MyPolyline2f>(line.GetVertices());
}

int MyBubbleSetsDrawingHelper::ComputeIntersectionCount(
	const MyArray<const MySegmentNode*>* segmentsObs,
	const MyLine2f& line) const{
	int count = 0;
	for (int i = 0; i < segmentsObs->size(); i++){
		const MyBox2f& box = mLayout->GetPosition(segmentsObs->at(i));
		if (MyMathHelper::IsIntersected(box, line)){
			count++;
		}
	}
	return count;
}

MyVec2f MyBubbleSetsDrawingHelper::ComputeCentroid(
	const MyArray<const MySegmentNode*>* segments) const{
	MyVec2f centroid(0, 0);
	for (int i = 0; i < segments->size(); i++){
		centroid += mLayout->GetPosition(segments->at(i)).GetCenter();
	}
	centroid /= segments->size();
	return centroid;
}

float MyBubbleSetsDrawingHelper::ComputeEnergy(float distance) const{
	if (distance > EnergyMaxRadius) return 0;
	float absEnergy = (EnergyMaxRadius - distance)*(EnergyMaxRadius - distance)
		/ ((EnergyMaxRadius - EnergyCoreRadius)*(EnergyMaxRadius - EnergyCoreRadius));
	return absEnergy;
}