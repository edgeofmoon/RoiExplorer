#include "MyLineConnectorDrawer.h"
#include "MyGLHeader.h"
#include "MyPolyLine.h"
#include "MySpaceFillingNaive.h"

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
// this one does not find the best arc
// nor render labels
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
		}
		const MatchCount& matchCount = mMatchCounter->GetMatchCount(itr->first);
		MatchCount::const_iterator matchItr = matchCount.begin();
		const MySegmentNode* biggestNode = matchItr->first;
		int biggestMatch = 0;
		for (; matchItr != matchCount.end(); matchItr++){
			if (mJoinTreeLayout->GetLayoutInfo().at(matchItr->first).range[1] == 0){
				//ignore the root
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
	MyMap<const MySegmentNode*, const MySegmentNode*> matchDrawn;
	while (itr != roiBoxes->end()){
		bool isRoiInView = true;
		if (mRoiView->IsDisable(itr->first)) {
			itr++;
			continue;
		}
		MyBox2f roiBox = this->ComputeRoiViewBox(itr->second);
		if (roiBox.GetHighPos()[0] < 0 || roiBox.GetLowPos()[0]>1) {
			isRoiInView = false;
		}
		glLineWidth(0.2);
		if (mRoiView->IsSelected(itr->first)){
			glLineWidth(2);
		}
		const MatchCount& matchCount = mMatchCounter->GetMatchCount(itr->first);
		MatchCount::const_iterator matchItr = matchCount.begin();
		const MySegmentNode* biggestNode = matchItr->first;
		//int biggestMatch = 0;
		float biggestMatch = 0;
		for (; matchItr != matchCount.end(); matchItr++){
			if (mJoinTreeLayout->GetLayoutInfo().at(matchItr->first).range[1] == 0){
				//ignore the root
			}
			else{
				//int numMatch = matchItr->second;
				float numMatch = this->ComputeMatchIndex(itr->first, 
					matchItr->first, matchItr->second);
				//float numMatch = matchItr->second 
				//	/ (float)matchItr->first->GetUniqueVoxes()->GetNumVoxes();
				if (numMatch > biggestMatch){
					biggestNode = matchItr->first;
					biggestMatch = numMatch;
				}
			}
		}
		const MySegmentNode* bestMatch = this->FindBestNode(itr->first, biggestNode);
		if(isRoiInView) RenderConnectorLine(itr->first, bestMatch);
		matchDrawn[itr->first] = bestMatch;
		//this->RenderLabel()
		itr++;
	}
	glLineWidth(1);
	this->RenderLabel(matchDrawn);
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

void MyLineConnectorDrawer::RenderLabel(
	const MyMap<const MySegmentNode*, const MySegmentNode*>& matchToDraw){
	glLineWidth(1);
	float labelHeight = 0.012f;
	MySpaceFillingSpiral spaceFill;
	const MyMap<const MySegmentNode*, MyBox2f>& boxes
		= mJoinTreeLayout->GetPositions();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itrBox;
	for (itrBox = boxes.begin(); itrBox != boxes.end(); itrBox++){
		if (mJoinTreeView->IsSegmentInView(itrBox->first)){
			MyBox2f segBox =
				this->ComputeJoinTreeViewBox(
				mJoinTreeLayout->GetPosition(itrBox->first));
			spaceFill.ForceAddBox(segBox);
		}
	}
	MyMap<const MySegmentNode*, const MySegmentNode*>::const_iterator itr;
	for (itr = matchToDraw.begin(); itr != matchToDraw.end(); itr++){
		const MySegmentNode* arc = itr->second;
		if (!mJoinTreeView->IsSegmentInView(arc)) continue;
		const MySegmentNode* roi = itr->first;
		if (mRoiView->GetRoiDrawer()->GetLabelManager()
			->GetLabels()->HasKey(roi->GetIndex())){
			MyString label = mRoiView->GetRoiDrawer()->GetLabelManager()
				->GetLabels()->at(roi->GetIndex());
			MyBox2f segBox =
				this->ComputeJoinTreeViewBox(mJoinTreeLayout->GetPosition(arc));
			MyVec2f fontSize = mFont->ComputeSizeInPixel(label).toType<float>();
			float fontScale = labelHeight / fontSize[1];
			fontSize *= fontScale;
			MyBox2f labelBoxBase(MyVec2f::zero(), fontSize);
			MyBox2f labelBox = spaceFill.PushBox(labelBoxBase + 
				MyVec2f(segBox.GetLowPos()[0], segBox.GetHighPos()[1]));
			glColor4f(0, 0, 0, 1);
			labelBox.ScaleAtCenter(MyVec2f(0.9, 0.9));
			mFont->RenderFill(label, labelBox, 0.99);

			/*
			glColor4f(0, 0, 0, 1);
			glBegin(GL_LINE_LOOP);
			glVertex3f(labelBox.GetLowPos()[0], labelBox.GetLowPos()[1], 0.99);
			glVertex3f(labelBox.GetHighPos()[0], labelBox.GetLowPos()[1], 0.99);
			glVertex3f(labelBox.GetHighPos()[0], labelBox.GetHighPos()[1], 0.99);
			glVertex3f(labelBox.GetLowPos()[0], labelBox.GetHighPos()[1], 0.99);
			glEnd();
			*/

			/*
			glColor4f(1, 1, 1, 1);
			glBegin(GL_QUADS);
			glVertex3f(labelBox.GetLowPos()[0], labelBox.GetLowPos()[1], 0.9);
			glVertex3f(labelBox.GetHighPos()[0], labelBox.GetLowPos()[1], 0.9);
			glVertex3f(labelBox.GetHighPos()[0], labelBox.GetHighPos()[1], 0.9);
			glVertex3f(labelBox.GetLowPos()[0], labelBox.GetHighPos()[1], 0.9);
			glEnd();
			*/

			glColor4f(0, 0, 0, 1);
			this->RenderBoxConnectingLine(segBox, labelBox);
		}
	}
}

void MyLineConnectorDrawer::RenderBoxConnectingLine(
	const MyBox2f& box0, const MyBox2f& box1){
	MyVec2f p0[4] = {
		box0.GetLowPos(),
		MyVec2f(box0.GetHighPos()[0], box0.GetLowPos()[1]),
		box0.GetHighPos(),
		MyVec2f(box0.GetLowPos()[0], box0.GetHighPos()[1])
	};
	MyVec2f p1[4] = {
		box1.GetLowPos(),
		MyVec2f(box1.GetHighPos()[0], box1.GetLowPos()[1]),
		box1.GetHighPos(),
		MyVec2f(box1.GetLowPos()[0], box1.GetHighPos()[1])
	};
	MyVec2i nearPointIndex;
	float nearDistSqd = FLT_MAX;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			float distSqd = (p0[i] - p1[j]).squared();
			if (distSqd < nearDistSqd){
				nearDistSqd = distSqd;
				nearPointIndex = MyVec2i(i, j);
			}
		}
	}

	glBegin(GL_LINES);
	glVertex3f(p0[nearPointIndex[0]][0], p0[nearPointIndex[0]][1], 0.99);
	glVertex3f(p1[nearPointIndex[1]][0], p1[nearPointIndex[1]][1], 0.99);
	glEnd();
}

// reference: https://en.wikipedia.org/wiki/Jaccard_index
float MyLineConnectorDrawer::ComputeMatchIndex(const MySegmentNode* node0,
	const MySegmentNode* node1, int numVoxelMatch) const{
	float n0 = node0->GetUniqueVoxes()->GetNumVoxes();
	float n1 = node1->GetUniqueVoxes()->GetNumVoxes();
	return numVoxelMatch / (n0 + n1 - numVoxelMatch);
}

const MySegmentNode* MyLineConnectorDrawer::FindBestNode(
	const MySegmentNode* node, const MySegmentNode* localMatch) const{
	// best suited node has the highest Jaccard index
	return localMatch;
	//if (localMatch->GetNumChildren() == 0) return localMatch;
}