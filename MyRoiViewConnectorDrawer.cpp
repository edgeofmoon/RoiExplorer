#include "MyRoiViewConnectorDrawer.h"
#include "MyGLHeader.h"

MyRoiViewConnectorDrawer::MyRoiViewConnectorDrawer()
{
	mRoiViews[0] = mRoiViews[1] = 0;
}


MyRoiViewConnectorDrawer::~MyRoiViewConnectorDrawer()
{
}

void MyRoiViewConnectorDrawer::Render(){

	if (!mRoiViews[0] || !mRoiViews[1]) return;
	mViewport = *mRoiViews[0];
	mViewport.Engulf(*mRoiViews[1]);

	glViewport(mViewport.GetLowPos()[0], mViewport.GetLowPos()[1],
		mViewport.GetSize(0), mViewport.GetSize(1));
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	MyMapScPtr<const MySegmentNode*, MyBox2f> roiBoxes0
		= mRoiViews[0]->GetRoiDrawer()->GetLayoutManager()->GetBoxesOut();
	MyMapScPtr<const MySegmentNode*, MyBox2f> roiBoxes1
		= mRoiViews[1]->GetRoiDrawer()->GetLayoutManager()->GetBoxesOut();
	MyMap<const MySegmentNode*, MyBox2f>::const_iterator itr;
	for (itr = roiBoxes0->begin(); itr != roiBoxes0->end(); itr++){
		if (roiBoxes1->HasKey(itr->first)){
			if (this->IsConnectionVisible(itr->first)){
				if (this->IsConnectionHighlighted(itr->first)){
					glLineWidth(3);
				}
				else glLineWidth(0.5);
				const MyBox2f& roiBox0 = itr->second;
				const MyBox2f& roiBox1 = roiBoxes1->at(itr->first);
				MyVec4f color0 = mRoiViews[0]->GetRoiDrawer()->GetSegNodeColor()->at(itr->first);
				MyVec4f color1 = mRoiViews[1]->GetRoiDrawer()->GetSegNodeColor()->at(itr->first);
				this->RenderBoxConnectorLine(roiBox0, roiBox1, color0, color1);
			}
		}
	}
	glLineWidth(1);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void MyRoiViewConnectorDrawer::SetRoiViews(MyRoiViewScPtr view0, MyRoiViewScPtr view1){
	mRoiViews[0] = view0;
	mRoiViews[1] = view1;
}

void MyRoiViewConnectorDrawer::SetViewport(const MyBox2i& viewport){
	mViewport = viewport;
}

MyVec2f MyRoiViewConnectorDrawer::ComputeViewPosition(MyRoiViewScPtr roiView, const MyVec2f& pos) const{
	MyVec2f pixelPos = roiView->ComputePixelPosition(pos).toType<float>();
	MyVec2f offset = pixelPos - mViewport.GetLowPos().toType<float>();
	MyVec2f rst(offset[0] / mViewport.GetSize(0), offset[1] / mViewport.GetSize(1));
	return rst;
}

void MyRoiViewConnectorDrawer::RenderBoxConnectorLine(const MyBox2f& roiBox0, const MyBox2f& roiBox1,
	const MyVec4f& color0, const MyVec4f& color1) const{
	MyVec2f p[4];
	MyVec2f point0(roiBox0.GetCenter()[0], roiBox0.GetLowPos()[1]);
	MyVec2f point1(roiBox1.GetCenter()[0], roiBox1.GetHighPos()[1]);
	p[0] = this->ComputeViewPosition(mRoiViews[0], point0);
	p[3] = this->ComputeViewPosition(mRoiViews[1], point1);
	p[1] = this->ComputeViewPosition(mRoiViews[0], point0 + MyVec2f(0.f, -0.2));
	p[2] = this->ComputeViewPosition(mRoiViews[1], point1 + MyVec2f(0.f, 0.2));


	MyPolyline2f polyLine;
	polyLine.Clear();
	polyLine.Append(p[0]).Append(p[1]).Append(p[2]).Append(p[3]);
	MyPolyline2f* curves = polyLine.MakeBezierCurve(100);
	// draw connect lines
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < curves->GetNumPoints(); i++){
		MyVec2f point = curves->GetPoint(i);
		if (point[0] < 0 || point[0]>1) continue;
		float frac = i / (float)(curves->GetNumPoints() - 1);
		MyVec4f color = (1 - frac)*color0 + frac*color1;
		glColor4fv(&color[0]);
		//glVertex2fv(&point[0]);
		glVertex3f(point[0], point[1], 0.99f);
	}
	glEnd();
	delete curves;
}

bool MyRoiViewConnectorDrawer::IsConnectionVisible(const MySegmentNode* roi) const{
	return !(mRoiViews[0]->IsDisable(roi) && mRoiViews[1]->IsDisable(roi));
}

bool MyRoiViewConnectorDrawer::IsConnectionHighlighted(const MySegmentNode* roi) const{
	return mRoiViews[0]->IsSelected(roi) || mRoiViews[1]->IsSelected(roi);
}