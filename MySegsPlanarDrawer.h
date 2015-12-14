#pragma once

#include "MySegNodeInfoAssemble.h"
#include "MyArray.h"
#include "MySegTrkNetwork.h"
#include "MyLabelManager.h"
#include "MySegNodeInfoLayout2D.h"
#include "MySegmentAssembleGroup.h"
#include "MySharedPointer.h"

class MySegsPlanarDrawer
{
public:
	MySegsPlanarDrawer();
	~MySegsPlanarDrawer();

	void SetSegmentAssembleGroup(MySegmentAssembleGroupSPtr segAssembleGroup){
		mSegAssembleGroup = segAssembleGroup;
	};

	void SetSegTrkNetwork(MySegTrkNetworkSPtr segTrkNetwork){
		mSegTrkNetwork = segTrkNetwork;
	};
	void SetLayout(MySegNodeInfoLayout2DSPtr layout){
		mLayoutManager = layout;
	}
	MySegmentAssembleGroupSPtr GetSegmentAssembleGroup(){
		return mSegAssembleGroup;
	};

	MySegNodeInfoAssembleScPtr GetSegmentNodeInfoAssemble(){
		return mSegAssembleGroup->front();
	};
	MyLabelManagerSPtr GetLabelManager(){
		return mLabelManager;
	}
	MySegNodeInfoLayout2DSPtr GetLayoutManager() const {
		return mLayoutManager;
	}
	MySegTrkNetworkSPtr GetNetwork(){
		return mSegTrkNetwork;
	}
	const MyVec2f& GetHistogramRange() const{
		return mHistogramRange;
	}
	void SetLabels(MyMapScPtr<int, MyString> labels);

	void SetLinkDrawThreshold(float thres){ mLinkDrawThreshold = thres; };

	float GetLinkDrawThreshold(){ return mLinkDrawThreshold; };

	void SetSegNodeColor(MyMapScPtr<const MySegmentNode*, MyVec4f> color){
		mSegNodeColor = color;
	};
	MyMapScPtr<const MySegmentNode*, MyVec4f> GetSegNodeColor() const{
		return mSegNodeColor;
	}
	void Update();

	void Render();
	void Resize(int width, int height);

protected:
	MyLabelManagerSPtr mLabelManager;
	MySegNodeInfoLayout2DSPtr mLayoutManager;

	MySegmentAssembleGroupSPtr mSegAssembleGroup;
	MySegTrkNetworkSPtr mSegTrkNetwork;

	MyMapScPtr<const MySegmentNode*, MyVec4f> mSegNodeColor;

	void DrawArrow(const MyVec2f fromPos, const MyVec2f toPos, const MyVec4f color,
		const MyVec4i name, float startWidth, float endWidth = 0);

	float mLinkDrawThreshold;
	MyVec2f mHistogramRange;

	// temp solution
	void DrawBoxes();
	void DrawDistribution(int idx);
	void DrawNetwork();
};

typedef MySharedPointer<MySegsPlanarDrawer> MySegsPlanarDrawerSPtr;
typedef MySharedPointer<const MySegsPlanarDrawer> MySegsPlanarDrawerScPtr;
