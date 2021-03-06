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

	void SetSegmentAssembleGroup(MySegmentAssembleGroupScPtr segAssembleGroup){
		mSegAssembleGroup = segAssembleGroup;
	};

	void SetSegTrkNetwork(MySegTrkNetworkSPtr segTrkNetwork){
		mSegTrkNetwork = segTrkNetwork;
	};
	void SetLayout(MySegNodeInfoLayout2DSPtr layout){
		mLayoutManager = layout;
	}
	MySegmentAssembleGroupScPtr GetSegmentAssembleGroup(){
		return mSegAssembleGroup;
	};

	MySegNodeInfoAssembleScPtr GetSegmentNodeInfoAssemble(){
		return mSegAssembleGroup->front();
	};
	MySegNodeInfoLayout2DSPtr GetLayoutManager(){
		return mLayoutManager;
	}
	MySegTrkNetworkSPtr GetNetwork(){
		return mSegTrkNetwork;
	}
	void SetLabels(MyMapScPtr<int, MyString> labels);

	void SetLinkDrawThreshold(float thres){ mLinkDrawThreshold = thres; };
	
	void SetSegNodeColor(MyMapScPtr<const MySegmentNode*, MyVec4f> color){
		mSegNodeColor = color;
	};
	void Update();

	void Render();
	void Resize(int width, int height);
	void CompileShader(int shader = 0);

protected:
	MyLabelManagerSPtr mLabelManager;
	MySegNodeInfoLayout2DSPtr mLayoutManager;

	MySegmentAssembleGroupScPtr mSegAssembleGroup;
	MySegTrkNetworkSPtr mSegTrkNetwork;

	MyMapScPtr<const MySegmentNode*, MyVec4f> mSegNodeColor;

	void DrawArrow(const MyVec2f fromPos, const MyVec2f toPos, const MyVec4f color,
		const MyVec4i name, float startWidth, float endWidth = 0);

	float mLinkDrawThreshold;

	// temp solution
	void DrawBoundary();
	void DrawArrowBoundary(const MyVec2f fromPos, const MyVec2f toPos, const MyVec4f color,
		const MyVec4i name, float startWidth, float endWidth = 0);
	void DrawDistribution(int idx);

	int mShaderProgram;

	MyArray2f mVertices;
	MyArray4f mColors;
	MyArray4i mNames;

	unsigned int mVertexArray;
	unsigned int mPositionBuffer;
	unsigned int mColorBuffer;
	unsigned int mNameBuffer;
	unsigned int mIndexBuffer;
	unsigned int mPositionAttribute;
	unsigned int mColorAttribute;
	unsigned int mNameAttribute;

	void DestoryGeometry();
	void LoadGeometry();
};

typedef MySharedPointer<MySegsPlanarDrawer> MySegsPlanarDrawerSPtr;
typedef MySharedPointer<const MySegsPlanarDrawer> MySegsPlanarDrawerScPtr;
