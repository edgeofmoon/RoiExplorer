#include "MyBloomDrawingHelper.h"
#include "MyGLHeader.h"
#include "MyUtility.h"

MyBloomDrawingHelper::MyBloomDrawingHelper()
{
}


MyBloomDrawingHelper::~MyBloomDrawingHelper()
{
}

void MyBloomDrawingHelper::Update(){
}

void MyBloomDrawingHelper::Render(){
	const MyArray<const MySegmentNode*>& nodes
		= mMatchCounter->GetNodeGroups().back();
	const MyMap<const MySegmentNode*, MatchCount>& nodeMatchCount
		= mMatchCounter->GetNodeMatchCount();
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < nodes.size(); i++){
		const MySegmentNode* node = nodes[i];
		MyMap<const MySegmentNode*, MatchCount>::const_iterator itr
			= nodeMatchCount.find(node);
		if (itr != nodeMatchCount.end()){
			const MyBox2f& bigBox = mLayout->GetPosition(node);
			const MatchCount& matchCount = itr->second;
			// count total size
			MatchCount::const_iterator nmItr = matchCount.begin();
			int totalMatched = 0;
			while (nmItr != matchCount.end()){
				totalMatched += nmItr->second;
				nmItr++;
			}
			totalMatched = std::max(totalMatched, node->GetUniqueVoxes()->GetNumVoxes());
			// loop again to draw boxes
			nmItr = matchCount.begin();
			float totalUsedRatio = 0;
			while (nmItr != matchCount.end()){
				float ratio = nmItr->second / (float)totalMatched;
				// fill from bottom
				//MyBox2f cutBox = this->CutBox(
				//	bigBox, totalUsedRatio, totalUsedRatio + ratio);
				// fill from top
				MyBox2f cutBox = this->CutBox(
					bigBox, 1 - totalUsedRatio - ratio, 1-totalUsedRatio);
				MyVec4f color = mRoiColors->at(nmItr->first);
				MyBox2f bloomBox = cutBox;
				//bloomBox.ScaleAtCenter(MyVec2f(2, 1.2));
				this->DrawBloomBox(bloomBox, color);
				totalUsedRatio += ratio;
				nmItr++;
			}
			itr++;
		}
	}
	glPopAttrib();
}

void MyBloomDrawingHelper::DrawBloomBox(
	const MyBox2f& bloomBox, const MyVec4f& color) const{
	MyVec2f center = bloomBox.GetCenter();
	glBegin(GL_QUADS);
	glColor4f(color[0], color[1], color[2], 1);
	glVertex3f(bloomBox.GetLowPos()[0], bloomBox.GetLowPos()[1], 0.3);
	glVertex3f(bloomBox.GetHighPos()[0], bloomBox.GetLowPos()[1], 0.3);
	glVertex3f(bloomBox.GetHighPos()[0], bloomBox.GetHighPos()[1], 0.3);
	glVertex3f(bloomBox.GetLowPos()[0], bloomBox.GetHighPos()[1], 0.3);
	glEnd();
}
/*
void MyBloomDrawingHelper::DrawBloomBox(
	const MyBox2f& bloomBox, const MyVec4f& color) const{
	MyVec2f center = bloomBox.GetCenter();
	//MyVec2f halfSize = bloomBox.GetSize() / 2;
	MyVec2f halfSize(0.1 / 4, 0.1);
	float dimColor[4] = { 1, 1, 1, 0 };
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(color[0], color[1], color[2], 0.2);
	glVertex2fv(&center[0]);
	//glColor4fv(dimColor);
	for (float i = 0; i <= 2 * MY_PI; i += MY_PI / 36){
		glVertex2f(center[0] + halfSize[0] * cos(i), center[1] + halfSize[1] * sin(i));
	}
	glEnd();
}
*/
MyBox2f MyBloomDrawingHelper::CutBox(
	const MyBox2f& box, float startRatio, float endRatio) const{
	float height = box.GetSize(1);
	float width = box.GetSize(0);
	return MyBox2f(box.GetLowPos() + MyVec2f(0, height*startRatio), 
		box.GetLowPos() + MyVec2f(width, height*endRatio));
}