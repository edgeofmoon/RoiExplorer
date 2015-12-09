#include "MyLineConnectorDrawer.h"
#include "MyGLHeader.h"
#include "MyPolyLine.h"

MyLineConnectorDrawer::MyLineConnectorDrawer()
{
	mMatchDrawThreshold = 5;
}


MyLineConnectorDrawer::~MyLineConnectorDrawer()
{
}

void MyLineConnectorDrawer::Update(){

}
/*
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
*/

void MyLineConnectorDrawer::Render(){
	MyMapScPtr<const MySegmentNode*, MyBox2f> roiBoxes
		= mRoiLayout->GetBoxesOut();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr
		= roiBoxes->begin();

	while (itr != roiBoxes->end()){
		if (mRoiView->IsDisable(itr->first)) {
			itr++;
			continue;
		}
		MyBox2f roiBox = this->ComputeRoiViewBox(itr->second);
		if (roiBox.GetHighPos()[0] < 0 || roiBox.GetLowPos()[0]>1) {
			itr++;
			continue;
		}
		glLineWidth(0.2);
		if (mRoiView->IsSelected(itr->first)){
			glLineWidth(2);
			/*
			const MatchCount& matchCount = mMatchCounter->GetMatchCount(itr->first);
			MatchCount::const_iterator matchItr;
			for (matchItr = matchCount.begin(); matchItr != matchCount.end(); matchItr++){
				float matchRatio = matchItr->second;
				if (matchRatio >= mMatchDrawThreshold){
					RenderConnectorLine(itr->first, matchItr->first);
				}
			}
			*/
		}
		//else{
			//glLineWidth(0.2);
			const MatchCount& matchCount = mMatchCounter->GetMatchCount(itr->first);
			MatchCount::const_iterator matchItr = matchCount.begin();
			const MySegmentNode* biggestNode = matchItr->first;
			int biggestMatch = 0;
			for (; matchItr != matchCount.end(); matchItr++){
				if (mJoinTreeLayout->GetLayoutInfo().at(matchItr->first).range[1] == 0){
				}
				else{
					int numMatch = matchItr->second;
					if (numMatch > biggestMatch){
						biggestNode = matchItr->first;
						biggestMatch = numMatch;
					}
				}
			}
			RenderConnectorLine(itr->first, biggestNode);
		//}
		itr++;
	}
	glLineWidth(1);
}
bool MyLineConnectorDrawer::IsOnCenterBranch(const MySegmentNode* segment) const{
	return mJoinTreeLayout->GetLayoutInfo().at(segment).highest
		== mJoinTreeLayout->GetLayoutInfo().at(mJoinTreeLayout->GetJoinTreeRoot().get()).highest;
}

MyVec2f MyLineConnectorDrawer::ComputeJoinTreeViewPosition(const MyVec2f& pos) const{
	//MyVec2f pixelPos(mJoinTreeView->GetLowPos()[0] + pos[0] * mJoinTreeView->GetSize(0), 
	//	mJoinTreeView->GetLowPos()[1] + pos[1] * mJoinTreeView->GetSize(1));
	MyVec2f pixelPos = mJoinTreeView->ComputePixelPosition(pos).toType<float>();
	MyVec2f offset = pixelPos - mViewport.GetLowPos().toType<float>();
	MyVec2f rst(offset[0] / mViewport.GetSize(0), offset[1] / mViewport.GetSize(1));
	return rst;
}

MyVec2f MyLineConnectorDrawer::ComputeRoiViewPosition(const MyVec2f& pos) const{
	//MyVec2f pixelPos(mRoiView->GetLowPos()[0] + pos[0] * mRoiView->GetSize(0),
	//	mRoiView->GetLowPos()[1] + pos[1] * mRoiView->GetSize(1));
	MyVec2f pixelPos = mRoiView->ComputePixelPosition(pos).toType<float>();
	MyVec2f offset = pixelPos - mViewport.GetLowPos().toType<float>();
	MyVec2f rst(offset[0] / mViewport.GetSize(0), offset[1] / mViewport.GetSize(1));
	return rst;
}

MyBox2f MyLineConnectorDrawer::ComputeJoinTreeViewBox(const MyBox2f& box) const{
	return MyBox2f(this->ComputeJoinTreeViewPosition(box.GetLowPos()),
		this->ComputeJoinTreeViewPosition(box.GetHighPos()));
}

MyBox2f MyLineConnectorDrawer::ComputeRoiViewBox(const MyBox2f& box) const{
	return MyBox2f(this->ComputeRoiViewPosition(box.GetLowPos()),
		this->ComputeRoiViewPosition(box.GetHighPos()));
}

void MyLineConnectorDrawer::RenderConnectorLine(
	const MySegmentNode* roi, const MySegmentNode* seg){
	// make curves
	MyBox2f roiBox = this->ComputeRoiViewBox(mRoiLayout->GetSegmentPosition(roi));
	MyBox2f segBox = this->ComputeJoinTreeViewBox(mJoinTreeLayout->GetPosition(seg));
	float joinTreeBottom = (mJoinTreeView->GetLowPos()[1] - mViewport.GetLowPos()[1])
		/ (float)mViewport.GetSize(1);
	MyArray2f ctrPoints;
	if (IsOnCenterBranch(seg)){
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
			ctrPoints << MyVec2f(segBox.GetCenter()[0], joinTreeBottom);
		}
		ctrPoints << MyVec2f(segBox.GetCenter()[0], segBox.GetLowPos()[1]);
	}

	MyPolyline2f polyLine(ctrPoints);
	MyPolyline2f* curves = polyLine.MakeBezierCurve(100);
	// draw connect lines
	MyVec4f color = mRoiColors->at(roi);
	glColor4fv(&color[0]);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < curves->GetNumPoints(); i++){
		MyVec2f point = curves->GetPoint(i);
		glVertex2fv(&point[0]);
	}
	glEnd();
	delete curves;
}