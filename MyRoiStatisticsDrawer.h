#pragma once

#include "MyMap.h"
#include "MySegmentNode.h"
#include "MySegmentNodeInfo.h"
#include "MyJoinTreeLayout.h"
#include "MyBox.h"
#include "MySharedPointer.h"

class MyRoiStatisticsDrawer
{
public:
	MyRoiStatisticsDrawer();
	~MyRoiStatisticsDrawer();

	void Render();
	void Update();

	// set recursive
	void SetUseAllDescendentNode(bool ud){ mUseAllDescendents = ud; }
	void SetLayout(MyJoinTreeLayoutScPtr layout){ mLayout = layout; }

	void SetGroupColor(const MyVec4f& color, int groupIdx){ 
		mGroupColor[groupIdx] = color;
	}
	void SetGroupVolume(MyArrayScPtr<My3dArrayfScPtr> vols, int groupIdx){ 
		mGroupVolumes[groupIdx] = vols;
	}

	void ClearRois();
	void AddRoiByTree(const MySegmentNode* root);
	void AddRoiByArray(const MyArray<MySegmentNodeSPtr>* mRois);

	struct SegmentStatistics{
		float Means[2];
		float Stdevs[2];
	};

	SegmentStatistics GetRoiStatistics(const MySegmentNode* roi) const {
		return mRoiStatistics.at(roi);
	}

protected:
	// input data
	MyArray<const MySegmentNode*> mRois;
	MyJoinTreeLayoutScPtr mLayout;

	MyArray<MyArrayScPtr<My3dArrayfScPtr>> mGroupVolumes;
	MyArray<MyVec4f> mGroupColor;

	MyMap<const MySegmentNode*, SegmentStatistics> mRoiStatistics;

	// segment-mean for all subjects
	float ComputeSegmentAverage(const MySegmentNode* roi, int volIdx, int groupIdx) const;

	// mode
	bool mUseAllDescendents;
};

typedef MySharedPointer<MyRoiStatisticsDrawer> MyRoiStatisticsDrawerSPtr;
typedef MySharedPointer<const MyRoiStatisticsDrawer> MyRoiStatisticsDrawerScPtr;