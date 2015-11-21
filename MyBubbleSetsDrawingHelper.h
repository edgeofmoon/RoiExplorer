#pragma once

#include "MyJoinTreeLayout.h"
#include "MyMap.h"
#include "MyArrayMD.h"
#include "MyGridMD.h"
#include "MyPolyLine.h"

class MyBubbleSetsDrawingHelper
{
public:
	MyBubbleSetsDrawingHelper();
	~MyBubbleSetsDrawingHelper();

	void Update();

	void SetLayout(MyJoinTreeLayoutScPtr layout){
		mLayout = layout;
	}

	MyBox2f ComputeBoundingBox(
		const MyArray<const MySegmentNode*>* segments) const;
	My2dfGridfSPtr ComputeEnergyFieldSimple(
		const MyArray<const MySegmentNode*>* segments) const;
	My2dfGridfSPtr ComputeEnergyField(
		const MyArray<const MySegmentNode*>* segments) const;
	MyArraySPtr<const MySegmentNode*> ComputeSegmentsInBox(
		const MyBox2f& box) const;
	void UpdateEnergyField(
		const MyArray<const MySegmentNode*>* segmentsObs,
		const MyArray<const MySegmentNode*>* segmentsOrdered,
		My2dfGridf* field, int currentSegIdx) const;
	void UpdateEnergyField(
		My2dfGridf* field, const MyBox2f box, float weight = 1) const;
	void UpdateEnergyField(
		My2dfGridf* field, const MyPolyline2f* line, float weight = 1) const;
	MyPolyline2fSPtr ComputeLink(
		const MyArray<const MySegmentNode*>* segmentsObs,
		const MyArray<const MySegmentNode*>* segmentsOrdered,
		int currentSegIdx) const;
	MyPolyline2fSPtr ComputeLink(
		const MyArray<const MySegmentNode*>* segmentsObs,
		const MyLine2f& line) const;
	int ComputeIntersectionCount(const MyArray<const MySegmentNode*>* segments,
		const MyLine2f& line) const;
	MyVec2f ComputeCentroid(const MyArray<const MySegmentNode*>* segments) const;

	static void SetMaxEnergyRadius(float maxRadius){ EnergyMaxRadius = maxRadius; };
	static void SetCoreEnergyRadius(float coreRadius){ EnergyCoreRadius = coreRadius; };

protected:
	MyJoinTreeLayoutScPtr mLayout;
	MyMap<const MySegmentNode*, bool> mExcluded;

	static float EnergyMaxRadius;
	static float EnergyCoreRadius;
	float ComputeEnergy(float distance) const;
};

