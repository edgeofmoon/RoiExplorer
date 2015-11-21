#include "MySegmentAssembleGroup.h"
#include <cassert>

MySegmentAssembleGroup::MySegmentAssembleGroup()
{
}


MySegmentAssembleGroup::~MySegmentAssembleGroup()
{
}

void MySegmentAssembleGroup::Update(){
	mSegmentTScores.clear();
	if (size() < 2) return;
	for (int i = 0; i < front()->GetSegmentNodeInfos()->size(); i++){
		const MySegmentNodeInfo* nodeInfo0 = at(0)->GetSegmentNodeInfos()->at(i).get();
		const MySegmentNodeInfo* nodeInfo1 = at(1)->GetSegmentNodeInfos()->at(i).get();
		assert(nodeInfo0->GetSegmentNode().get() == nodeInfo1->GetSegmentNode().get());
		float m0 = nodeInfo0->GetSegmentNodeMeanAverage();
		float m1 = nodeInfo1->GetSegmentNodeMeanAverage();
		float s0 = nodeInfo0->GetSegmentNodeMeanStdev();
		float s1 = nodeInfo1->GetSegmentNodeMeanStdev();
		float n0 = nodeInfo0->GetVolumes()->size();
		float n1 = nodeInfo0->GetVolumes()->size();
		float mDiff = m0 - m1;
		float div0 = (n0 - 1)*s0*s0 + (n1 - 1)*s1*s1;
		float div1 = n0 + n1 - 2;
		float div2 = 1 / n0 + 1 / n1;
		float tScore = (m0 - m1) / sqrtf(div0 / div1*div2);
		mSegmentTScores[nodeInfo0->GetSegmentNode().get()] = tScore;
	}
}
