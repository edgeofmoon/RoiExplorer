#include "MySegNodeInfoLayout2D.h"
#include <algorithm>

MySegNodeInfoLayout2D::MySegNodeInfoLayout2D()
{
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

void MySegNodeInfoLayout2D::Update(){
	if (mSegAsmGroup->size() < 2){
		mBoxesOut = std::make_shared<MyMap<const MySegmentNode*, MyBox2f>>();
		// reassign layout
		int numBoxes = mBoxesIn->size();
		float boxSpace = 0.9f / numBoxes;
		// this 0.5 is a tmp hack
		float boxWidth = boxSpace*0.5;
		// evenly arrange from left to right
		int i = 0;
		for (MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = mBoxesIn->begin();
			itr != mBoxesIn->end(); itr++){
			// add small offset to avoid viewport clipping
			float boxX = i++*boxSpace + 0.05f;
			MyVec2f lowPos(boxX, 0.4);
			MyVec2f highPos = lowPos + MyVec2f(boxWidth, 0.2f);
			mBoxesOut->operator[](itr->first) = MyBox2f(lowPos, highPos);
		}
	}
	else{
		MyMap<float, const MySegmentNode*> segOrdered;
		for (MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr = mBoxesIn->begin();
			itr != mBoxesIn->end(); itr++){
			// add negativity to ensure reverse order
			// absolution value of t score is important
			segOrdered[-fabs(mSegAsmGroup->GetTScores().at(itr->first))] = itr->first;
		}

		mBoxesOut = std::make_shared<MyMap<const MySegmentNode*, MyBox2f>>();
		// reassign layout
		int numBoxes = mBoxesIn->size();
		float boxSpace = 0.9f / numBoxes;
		// this 0.5 is a tmp hack
		float boxWidth = boxSpace*0.5;
		// evenly arrange from left to right
		int i = 0;
		for (MyMap<float, const MySegmentNode*>::const_iterator itr = segOrdered.begin();
			itr != segOrdered.end(); itr++){
			// add small offset to avoid viewport clipping
			float boxX = i++*boxSpace + 0.05f;
			MyVec2f lowPos(boxX, 0.4);
			MyVec2f highPos = lowPos + MyVec2f(boxWidth, 0.2f);
			mBoxesOut->operator[](itr->second) = MyBox2f(lowPos, highPos);
		}
	}
}

bool MySegNodeInfoLayout2D::IsBoxLeft(const MyBox2f& box0, const MyBox2f& box1){
	return box0.GetLowPos()[0] < box1.GetLowPos()[0];
}