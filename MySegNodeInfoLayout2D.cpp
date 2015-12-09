#include "MySegNodeInfoLayout2D.h"
#include <algorithm>

// debug
#include <iostream>
using namespace std;

MySegNodeInfoLayout2D::MySegNodeInfoLayout2D()
{
	mBoxesOut = std::make_shared<MyMap<const MySegmentNode*, MyBox2f>>();
	mBaseBoxVerticalRange = MyVec2f(0.4, 0.6);
	mTScoreBoxHeight = 0.2;
	mBaseBoxWidth = 0.05;
	mSmallBoxWidth = 0.005;
}


MySegNodeInfoLayout2D::~MySegNodeInfoLayout2D()
{
}
/*
void MySegNodeInfoLayout2D::Update(){
	// first copy
	mBoxesOut = std::make_shared<MyArray<MyBox2f>>(*(mBoxesIn.get()));

	std::sort(mBoxesOut->begin(), mBoxesOut->end(), MySegNodeInfoLayout2D::IsBoxLeft);

	// then sort
	MyArrayi* rst = mBoxesOut->MakeSortResultArray(MySegNodeInfoLayout2D::IsBoxLeft);

	// reassign layout
	int numBoxes = mBoxesOut->size();
	float boxSpace = 0.9f / numBoxes;
	// this 0.5 is a tmp hack
	float boxWidth = boxSpace*0.5;

	// evenly arrange from left to right
	for (int i = 0; i < numBoxes; i++){
		// add small offset to avoid viewport clipping
		float boxX = i*boxSpace + 0.05f;
		MyVec2f lowPos(boxX, 0.4);
		MyVec2f highPos = lowPos + MyVec2f(boxWidth, 0.2f);
		//mBoxesOut->operator[](rst->at(i)) = MyBox2f(lowPos, highPos);
		// ignore the sorting
		mBoxesOut->operator[](i) = MyBox2f(lowPos, highPos);
	}

	delete rst;
}
*/
/*
void MySegNodeInfoLayout2D::Update(){
	// first copy
	mBoxesOut = std::make_shared<MyArray<MyBox2f>>(mBoxesIn->size());

	// get all indexes
	MyArrayi indexes;
	for (int i = 0; i < mSegInfos->size(); i++){
		indexes << mSegInfos->at(i)->GetSegmentNode()->GetIndex();
	}
	std::sort(indexes.begin(), indexes.end());

	MyArrayi indexesOrdered;
	int symIdx;
	for (symIdx = indexes.size() - 1; symIdx >= 0; symIdx -= 2){
		indexesOrdered << indexes[symIdx];
	}
	symIdx++;
	while (symIdx < 0)symIdx += 2;
	for (; symIdx < indexes.size(); symIdx += 2){
		indexesOrdered << indexes[symIdx];
	}

	// reassign layout
	int numBoxes = mBoxesOut->size();
	float boxSpace = 0.9f / numBoxes;
	// this 0.5 is a tmp hack
	float boxWidth = boxSpace*0.5;

	// evenly arrange from left to right
	for (int i = 0; i < numBoxes; i++){
		// add small offset to avoid viewport clipping
		float boxX = i*boxSpace + 0.05f;
		MyVec2f lowPos(boxX, 0.4);
		MyVec2f highPos = lowPos + MyVec2f(boxWidth, 0.2f);
		// ignore the sorting
		mBoxesOut->operator[](indexesOrdered[i]-1) = MyBox2f(lowPos, highPos);
	}
}
*/

/*
// this one does not care about status
void MySegNodeInfoLayout2D::Update(){
	if (mSegAsmGroup->size() < 2){
		mBoxesOut->clear();
		// reassign layout
		int numBoxes = mBoxesIn->size();
		float boxSpace = 1.0f / numBoxes;
		boxSpace = std::min(boxSpace, 0.1f);
		// this 0.5 is a tmp hack
		float boxWidth = boxSpace*0.5;
		// evenly arrange from left to right
		int i = 0;
		for (MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = mBoxesIn->begin();
			itr != mBoxesIn->end(); itr++){
			// add small offset to avoid viewport clipping
			float boxX = i++*boxSpace + boxWidth / 2;
			MyVec2f lowPos(boxX, 0.4);
			MyVec2f highPos = lowPos + MyVec2f(boxWidth, 0.2f);
			mBoxesOut->operator[](itr->first) = MyBox2f(lowPos, highPos);
		}
	}
	else{
		
		class SegmentSort{
		public:
			SegmentSort(const MyMap<const MySegmentNode*, float>& sv)
				:segValue(sv){};
			bool operator()(const MySegmentNode* seg1, const MySegmentNode* seg2) const{
				// its important to ensure fully ordered
				// or std::map will merge
				return fabs(segValue.at(seg1)) > fabs(segValue.at(seg2)) || seg1 > seg2;
			}
			const MyMap<const MySegmentNode*, float>& segValue;
		};

		SegmentSort segSort(mSegAsmGroup->GetTScores());

		std::map<const MySegmentNode*, MyBox2f, SegmentSort> segOrdered(segSort);
		for (MyMap<const MySegmentNode*, MyBox2f>::const_iterator itrBoxIn = mBoxesIn->begin();
			itrBoxIn != mBoxesIn->end(); itrBoxIn++){
			// add negativity to ensure reverse order
			// absolution value of t score is important
			segOrdered[itrBoxIn->first] = itrBoxIn->second;
		}

		mBoxesOut->clear();
		// reassign layout
		int numBoxes = mBoxesIn->size();
		float boxSpace = 1.0f / numBoxes;
		boxSpace = std::min(boxSpace, 0.1f);
		// this 0.5 is a tmp hack
		float boxWidth = boxSpace*0.5;
		// evenly arrange from left to right
		int i = 0;
		for (MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = segOrdered.begin();
			itr != segOrdered.end(); itr++){
			// add small offset to avoid viewport clipping
			float boxX = i++*boxSpace + boxWidth / 2;
			MyVec2f lowPos(boxX, 0.4);
			MyVec2f highPos = lowPos + MyVec2f(boxWidth, 0.2f);
			mBoxesOut->operator[](itr->first) = MyBox2f(lowPos, highPos);
		}
	}
}
*/

void MySegNodeInfoLayout2D::Update(){
	class SegmentSort{
	public:
		SegmentSort(const MyMap<const MySegmentNode*, float>& sv)
			:segValue(sv){};
		bool operator()(const MySegmentNode* seg1, const MySegmentNode* seg2) const{
			// its important to ensure fully ordered
			// or std::map will merge
			return fabs(segValue.at(seg1)) > fabs(segValue.at(seg2)) || seg1 > seg2;
		}
		const MyMap<const MySegmentNode*, float>& segValue;
	};
	SegmentSort segSort(mSegAsmGroup->GetTScores());
	std::map<const MySegmentNode*, MyBox2f, SegmentSort> segOrdered(segSort);
	for (MyMap<const MySegmentNode*, MyBox2f>::const_iterator itrBoxIn = mBoxesIn->begin();
		itrBoxIn != mBoxesIn->end(); itrBoxIn++){
		// add negativity to ensure reverse order
		// absolution value of t score is important
		segOrdered[itrBoxIn->first] = itrBoxIn->second;
	}

	mBoxesOut->clear();
	// reassign layout
	// totally ignore the encoding for now
	int numBoxes = mBoxesIn->size();
	MyVec2f currentOffset(0, 0);
	float interval = mSmallBoxWidth;
	for (MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = segOrdered.begin();
		itr != segOrdered.end(); itr++){
		MyBox2f box = this->ComputeBox(itr->first);
		box += currentOffset;
		currentOffset[0] += (box.GetSize(0) + interval);
		mBoxesOut->operator[](itr->first) = box;
		//cout << "Box width: " << box.GetSize(0) << endl;
	}
}


bool MySegNodeInfoLayout2D::IsBoxLeft(const MyBox2f& box0, const MyBox2f& box1){
	return box0.GetLowPos()[0] < box1.GetLowPos()[0];
}

MyBox2f MySegNodeInfoLayout2D::ComputeBox(const MySegmentNode* seg) const{
	float boxWidth = mBaseBoxWidth;
	if (mBoxStatus){
		if (mBoxStatus->HasKey(seg)){
			MyObjectStatus status = mBoxStatus->at(seg);
			if (status.IsBitSet(MyObjectStatus::STATUS_DISABLE_BIT)){
				boxWidth = mSmallBoxWidth;
			}
		}
	}
	float maxTScoreHeight = mTScoreBoxHeight;
	MyVec2f tScoreRange = mSegAsmGroup->GetTScoreRange();
	float tScoreRangeAbs = max(fabs(tScoreRange[0]), fabs(tScoreRange[1]));
	float tScore = mSegAsmGroup->GetTScores().at(seg);
	float tScoreHeight = fabs(tScore) / tScoreRangeAbs * maxTScoreHeight;
	MyVec2f lowPos(0, mBaseBoxVerticalRange[0]);
	MyVec2f highPos(boxWidth, mBaseBoxVerticalRange[1] + tScoreHeight);
	return MyBox2f(lowPos, highPos);
}