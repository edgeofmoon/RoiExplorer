#include "MyRoiStatisticsDrawer.h"
#include "MyMathHelper.h"
#include "MyGLHeader.h"
#include <stack>
using namespace std;

// debug
#include <iostream>

MyRoiStatisticsDrawer::MyRoiStatisticsDrawer()
{
	mUseAllDescendents = false;
	mGroupVolumes.resize(2);
	mGroupColor = MyArray<MyVec4f>(2, MyVec4f(0, 0, 1, 1));
}


MyRoiStatisticsDrawer::~MyRoiStatisticsDrawer()
{
}

void MyRoiStatisticsDrawer::Render(){

	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	int steps = 20;
	//float minStd = mStdevRange[0];
	const MyMap<const MySegmentNode*, MyBox2f>* boxes = &mLayout->GetPositions();
	for (int groupIdx = 0; groupIdx < 2; groupIdx++){
		for (int i = 0; i < mRois.size(); i++){
			const MySegmentNode* roi = mRois[i];
			SegmentStatistics statistics = mRoiStatistics.at(roi);
			float stdev = statistics.Stdevs[groupIdx];
			if (stdev == 0) continue;
			float minStd = min(statistics.Stdevs[0], statistics.Stdevs[1]);
			if (minStd == 0) minStd = stdev;
			const MyBox2f& box = boxes->at(roi);
			MyVec2f gRange(box.GetLowPos()[1], box.GetHighPos()[1]);
			float ggRange = gRange[1] - gRange[0];
			float mean = statistics.Means[groupIdx];
			float stdev2 = stdev*stdev;
			MyVec2f base = box.GetLowPos();
			float maxHeight = box.GetSize(0);
			float maxWidth = box.GetSize(1);
			glColor4f(0, 0, 0, 1);
			glBegin(GL_LINE_LOOP);
			for (int ic = -steps; ic <= steps; ic++){
				float diff = ic / (float)steps * 3;
				float height = exp(-diff*diff)*maxHeight *(minStd / stdev);
				height = std::min(height, maxHeight);
				float yPos = base[1] + (diff*stdev + mean - gRange[0]) / ggRange*maxWidth;
				yPos = std::max(yPos, gRange[0]);
				yPos = std::min(yPos, gRange[1]);
				glVertex3f(base[0] + height, yPos, 0.7);
			}
			glEnd();
			glColor4fv(&mGroupColor[groupIdx][0]);
			glBegin(GL_QUAD_STRIP);
			for (int ic = -steps; ic <= steps; ic++){
				float diff = ic / (float)steps * 3;
				float height = exp(-diff*diff)*maxHeight *(minStd / stdev);
				height = std::min(height, maxHeight);
				float yPos = base[1] + (diff*stdev + mean - gRange[0]) / ggRange*maxWidth;
				yPos = std::max(yPos, gRange[0]);
				yPos = std::min(yPos, gRange[1]);
				glVertex3f(base[0], yPos, 0.6);
				glVertex3f(base[0] + height, yPos, 0.6);
			}
			glEnd();
		}
	}
	glPopAttrib();
}

void MyRoiStatisticsDrawer::Update(){
	mRoiStatistics.clear();
	for (int i = 0; i < mRois.size(); i++){
		const MySegmentNode* roi = mRois[i];
		float StdevRange[2] = { FLT_MAX, 0 };

		SegmentStatistics statisitcs;
		MyArrayf segNodeMeans[2];
		for (int groupIdx = 0; groupIdx < 2; groupIdx++){
			MyArrayf segNodeMeans;
			segNodeMeans.reserve(mGroupVolumes[groupIdx]->size());
			for (int i = 0; i < mGroupVolumes[groupIdx]->size(); i++){
				float thisMean = this->ComputeSegmentAverage(roi, i, groupIdx);
				segNodeMeans << thisMean;
			}
			statisitcs.Means[groupIdx] = MyMathHelper::ComputeMean(&segNodeMeans);
			statisitcs.Stdevs[groupIdx] = MyMathHelper::ComputeStandardDeviation(
				&segNodeMeans, statisitcs.Means[groupIdx]);
		}

		// just store the mean and stdev
		mRoiStatistics[roi] = statisitcs;
	}
}

void MyRoiStatisticsDrawer::ClearRois(){
	mRois.clear();
}

void MyRoiStatisticsDrawer::AddRoiByTree(const MySegmentNode* root){
	stack<const MySegmentNode*> stackSegments;
	stackSegments.push(root);
	while (!stackSegments.empty()){
		const MySegmentNode* thisSegment = stackSegments.top();
		stackSegments.pop();
		for (int i = 0; i < thisSegment->GetNumChildren(); i++){
			const MySegmentNode* child = thisSegment->GetChild(i).get();
			stackSegments.push(child);
		}
		mRois << thisSegment;
	}
	cout << "StatDrawer #ROIs: " << mRois.size() << endl;
}

void MyRoiStatisticsDrawer::AddRoiByArray(const MyArray<MySegmentNodeSPtr>* nodeArray){
	for (int i = 0; i < nodeArray->size(); i++){
		mRois.PushBack(nodeArray->at(i).get());
	}
}

float MyRoiStatisticsDrawer::ComputeSegmentAverage(
	const MySegmentNode* roi, int volIdx, int groupIdx) const{
	My3dArrayfScPtr vol = mGroupVolumes[groupIdx]->at(volIdx);
	float segmentSum = 0;
	int numVoxel = 0;
	if (mUseAllDescendents){
		MySegmentNode::const_VoxelIterator itr = roi->VoxelBegin();
		MySegmentNode::const_VoxelIterator itrEnd = roi->VoxelEnd();
		while (itr != itrEnd){
			float voxelValue = vol->At(itr.GetVoxelIndex());
			if (voxelValue != MyVoxContainerf::GetNullVoxelValue()
				&& voxelValue != 0){
				segmentSum += voxelValue;
				numVoxel++;
			}
			itr++;
		}
	}
	else{
		MyVoxContainerf::const_Iterator itr = roi->GetUniqueVoxes()->Begin();
		MyVoxContainerf::const_Iterator itrEnd = roi->GetUniqueVoxes()->End();
		while (itr != itrEnd){
			float voxelValue = vol->At(itr.GetVoxelIndex());
			if (voxelValue != MyVoxContainerf::GetNullVoxelValue()
				&& voxelValue != 0){
				segmentSum += voxelValue;
				numVoxel++;
			}
			itr++;
		}
	}
	//assert(numVoxel == mSegNode->GetNumTotalVoxes());
	if (numVoxel == 0){
		//cout << "#voxels==0: " << volIdx << endl;
		return 0;
	}
	return segmentSum / numVoxel;
}