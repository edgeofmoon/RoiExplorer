#pragma once
#include "MyJoinTree.h"
#include "MyJoinTreeLayout.h"

class MyJoinTreeDrawer
{
public:
	MyJoinTreeDrawer();
	~MyJoinTreeDrawer();

	void SetJoinTree(MyJoinTreeScPtr joinTree);
	void SetJoinTreeLayout(MyJoinTreeLayoutSPtr layout);
	void Update();
	void Render(int width, int height);

protected:
	MyJoinTreeScPtr mJoinTree;
	MyJoinTreeLayoutSPtr mLayout;
	void RenderSegment(const MySegmentNode* segment);
};

