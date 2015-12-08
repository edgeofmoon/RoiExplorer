#pragma once
#include "MyJoinTree.h"
#include "MyJoinTreeLayout.h"
#include "MySharedPointer.h"

class MyJoinTreeDrawer
{
public:
	MyJoinTreeDrawer();
	~MyJoinTreeDrawer();

	void SetJoinTree(MyJoinTreeScPtr joinTree);
	void SetJoinTreeLayout(MyJoinTreeLayoutSPtr layout);
	MyJoinTreeScPtr GetJoinTree() const { return mJoinTree; };
	MyJoinTreeLayoutSPtr GetLayout() { return mLayout; };
	void Update();
	void Render();

protected:
	MyJoinTreeScPtr mJoinTree;
	MyJoinTreeLayoutSPtr mLayout;
	void RenderSegment(const MySegmentNode* segment);
};

typedef MySharedPointer<MyJoinTreeDrawer> MyJoinTreeDrawerSPtr;
typedef MySharedPointer<const MyJoinTreeDrawer> MyJoinTreeDrawerScPtr;