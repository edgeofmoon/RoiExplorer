#include "MyLineConnectorDrawer.h"
#include "MyGLHeader.h"
#include "MyPolyLine.h"

// for debug
#include <iostream>
using namespace std;

MyLineConnectorDrawer::MyLineConnectorDrawer()
{
}


MyLineConnectorDrawer::~MyLineConnectorDrawer()
{
}

void MyLineConnectorDrawer::Update(){

}

void MyLineConnectorDrawer::Render(int width, int height){
	MyMapScPtr<const MySegmentNode*, MyBox2f> roiBoxes
		= mRoiLayout->GetBoxesOut();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr
		= roiBoxes->begin();
	glLineWidth(0.2);
	while (itr != roiBoxes->end()){
		MyBox2f roiBox = itr->second + mRoiLayoutOffset;
		const MatchCount& matchCount = mMatchCounter->GetMatchCount(itr->first);
		MatchCount::const_iterator matchItr = matchCount.begin();
		const MySegmentNode* biggestNode = matchItr->first;
		int biggestMatch = 0;
		while (matchItr != matchCount.end()){
			if (mJoinTreeLayout->GetLayoutInfo().at(matchItr->first).range[1] == 0){
			//	cout << "Hit 0 Root" << endl;
			}
			else{
				//float theLeft = mJoinTreeLayout->GetPosition(matchItr->first).GetLowPos()[0];
				int numMatch = matchItr->second;
				if (numMatch > biggestMatch){
					biggestNode = matchItr->first;
					biggestMatch = numMatch;
				}
			}
			matchItr++;
		}
		MyBox2f segBox = mJoinTreeLayout->GetPosition(biggestNode)
			+ mJoinTreeLayoutOffset;

		// make curves
		MyArray2f ctrPoints;
		if (IsOnCenterBranch(biggestNode)){
			MyVec2f dir = roiBox.GetCenter() - segBox.GetCenter();
			dir[1] = 0;
			dir.normalize();
			ctrPoints << MyVec2f(roiBox.GetCenter()[0], roiBox.GetHighPos()[1]);
			ctrPoints << MyVec2f(roiBox.GetCenter()[0], roiBox.GetHighPos()[1] + 0.2);
			ctrPoints << MyVec2f(segBox.GetCenter()[0], segBox.GetLowPos()[1])
				+ dir*0.05;
			ctrPoints << MyVec2f(segBox.GetCenter()[0], segBox.GetLowPos()[1]);
		}
		else{
			ctrPoints << MyVec2f(roiBox.GetCenter()[0], roiBox.GetHighPos()[1]);
			ctrPoints << MyVec2f(roiBox.GetCenter()[0], roiBox.GetHighPos()[1] + 0.2);
			if (segBox.GetLowPos()[1] != 0){
				ctrPoints << MyVec2f(segBox.GetCenter()[0], mJoinTreeLayoutOffset[1]);
			}
			ctrPoints << MyVec2f(segBox.GetCenter()[0], segBox.GetLowPos()[1]);
		}

		MyPolyline2f polyLine(ctrPoints);
		MyPolyline2f* curves = polyLine.MakeBezierCurve(100);
		// draw connect lines
		MyVec4f color = mRoiColors->at(itr->first);
		glColor4fv(&color[0]);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < curves->GetNumPoints(); i++){
			MyVec2f point = curves->GetPoint(i);
			glVertex2fv(&point[0]);
		}
		glEnd();
		delete curves;
		itr++;
	}
	glLineWidth(1);
}

bool MyLineConnectorDrawer::IsOnCenterBranch(const MySegmentNode* segment) const{
	return mJoinTreeLayout->GetLayoutInfo().at(segment).highest
		== mJoinTreeLayout->GetLayoutInfo().at(mJoinTreeLayout->GetJoinTreeRoot().get()).highest;
}