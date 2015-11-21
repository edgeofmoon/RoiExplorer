#include "MyCohortPlot.h"
#include "MyMatrix.h"
#include "MyMathHelper.h"
#include "MyVolumnSimilarity.h"
#include "MyGLHeader.h"
#include "MyGraphicsTool.h"
MyCohortPlot::MyCohortPlot()
{
}


MyCohortPlot::~MyCohortPlot()
{
}

void MyCohortPlot::Render(int w, int h){
	const MyArray2f& pos = mLayout.GetPositions();
	glColor4f(0, 0, 1, 0);
	glBegin(GL_QUADS);
	for (int i = 0; i < pos.size(); i++){
		MyGraphicsTool::Vertex(pos[i] + MyVec2f(-0.01,-0.01));
		MyGraphicsTool::Vertex(pos[i] + MyVec2f(0.01,-0.01));
		MyGraphicsTool::Vertex(pos[i] + MyVec2f(0.01,0.01));
		MyGraphicsTool::Vertex(pos[i] + MyVec2f(-0.01,0.01));
	}
	glEnd();
}

void MyCohortPlot::Update(){
	MyArrayScPtr<My3dArrayfScPtr> vols = mSegNodeInfoAssemble->GetSegmentNodeInfos()->front()->GetVolumes();
	int nVols = vols->size();
	mDistanceMatrix = std::make_shared<MyMatrixf>(nVols, nVols);
	for (int i = 0; i < nVols; i++){
		mDistanceMatrix->At(i, i) = 0;
		for (int j = i + 1; j < nVols; j++){
			float similarity = MyVolumnSimilarity::ComputeSimilarity(vols->at(i).get(), vols->at(j).get());
			mDistanceMatrix->At(i, j) = 1/similarity;
			mDistanceMatrix->At(j, i) = 1/similarity;
		}
	}
	mLayout.SetDistanceMatrix(mDistanceMatrix);
	mLayout.SetBoundingBox(MyBoundingBox(MyVec3f(0,0,0),MyVec3f(1,1,0)));
	mLayout.Update();
}