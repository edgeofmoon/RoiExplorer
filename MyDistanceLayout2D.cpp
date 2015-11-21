#include "MyDistanceLayout2D.h"

/* reference:
http://www.graphviz.org/Documentation/GKN04.pdf
*/

MyDistanceLayout2D::MyDistanceLayout2D()
{
	mDistWeightRatio = -1.f;
	mEnergyDecreaseRationLimit = 1e-8;
	mLastEnergy = -1.f;
	mCurrentEnergy = -1.f;
}


MyDistanceLayout2D::~MyDistanceLayout2D()
{
}

float MyDistanceLayout2D::GetDistanceEdgeWeightRatioHint(const MyBoundingBox& box) const{
	return sqrtf(box.GetFrontFaceArea() / mDistanceMatrix->GetNumCols()) * 100;
}

float MyDistanceLayout2D::GetDistanceEdgeWeightRatio() const{
	return mDistWeightRatio;
}

void MyDistanceLayout2D::Update(){
	if (mDistWeightRatio <= 0){
		float hint = this->GetDistanceEdgeWeightRatioHint(mBoundingBox);
		this->SetDistanceEdgeWeightRatio(hint);
	}
	mLastEnergy = -1.f;
	this->init();
	mPos.clear();
	mPos.reserve(mDistanceMatrix->GetNumCols());
	for (int i = 0; i < mDistanceMatrix->GetNumCols(); i++){
		mPos << MyVec2f(rand() % 1000 / 1000.f, rand() % 1000 / 1000.f);
	}
	mCurrentEnergy = energy();
	while (Advance());
}

bool MyDistanceLayout2D::Advance(){
	if (this->ShouldContinue()){
		// update old energy
		mLastEnergy = mCurrentEnergy;

		// advance
		MyArray2f oldPos = mPos;
		for (int i = 0; i<mDistanceMatrix->GetNumRows(); i++){
			MyVec2f newPos = MyVec2f::zero();
			for (int j = 0; j<mDistanceMatrix->GetNumCols(); j++){
				if (i != j){
					float oldDist = (oldPos[i] - oldPos[j]).norm();
					if (oldDist != 0){
						MyVec2f jSum = normConst(i, j)*(oldPos[j] + dist(i, j)*(oldPos[i] - oldPos[j]) / oldDist);
						newPos += jSum / normConstSum();
					}
				}
			}
			mPos[i] = newPos;
		}

		// limit to bounding box
		MyBoundingBox box;
		for (int i = 0; i<mDistanceMatrix->GetNumRows(); i++){
			box.Engulf(mPos[i].toDim<3>(0));
		}

		MyArray2f newPos;
		mBoundingBox.MapPoints(mPos, newPos, box);
		mPos = newPos;
		//MyVec3f newCenter = box.GetCenter();
		//for(int i = 0;i<mGraph->GetNumNodes();i++){
		//	mPos[i] += newCenter-mBoundingBox.GetCenter();
		//}
		//for(int i = 0;i<mGraph->GetNumNodes();i++){
		//	mPos[i] = mBoundingBox.BoundPoint(mPos[i]);
		//}

		// update current energy
		mCurrentEnergy = energy();
		return true;
	}
	return false;
}


bool MyDistanceLayout2D::ShouldContinue() const{
	if (mCurrentEnergy == 0) return false;

	// not started yet;
	if (mLastEnergy < 0) return true;

	float energyDecreaseRation = (mLastEnergy - mCurrentEnergy) / mCurrentEnergy;

	if (energyDecreaseRation<0.f) return true;
	else return energyDecreaseRation > mEnergyDecreaseRationLimit;
}

void MyDistanceLayout2D::init(){
	int nNodes = mDistanceMatrix->GetNumRows();

	// for distance matrix

	//// use weight inverse as distance
	//MyMatrixf distMat(nNodes,nNodes);
	//for(int i = 0;i<nNodes;i++){
	//	for(int j = 0;j<nNodes;j++){
	//		if(i != j){
	//			int edgeIdx = mGraph->GetEdgeIndex(MyVec2i(i,j));
	//			if(edgeIdx >=0){
	//				float edgeWeight = mGraph->GetEdgeWeight(i);
	//				if(edgeWeight == 0){
	//					distMat.At(i,j) = MAXDIST;
	//				}
	//				else{
	//					distMat.At(i,j) = 1.f/std::fabs(edgeWeight)*mDistWeightRatio;
	//				}
	//			}
	//			else{
	//				// no connection
	//				distMat.At(i,j) = MAXDIST;
	//			}
	//		}
	//		else distMat.At(i,j) = 0.f;
	//	}
	//}
	//mDistMat = distMat;

	// for normConstMat;
	MyMatrixf normConstMat(nNodes, nNodes);
	for (int i = 0; i<nNodes; i++){
		for (int j = 0; j<nNodes; j++){
			if (i != j){
				float distance = dist(i, j);
				normConstMat.At(i, j) = 1.f / distance / distance;
			}
			else{
				normConstMat.At(i, j) = 1;
			}
		}
	}
	mNormConstMat = normConstMat;

	// for norm const sum
	mNormConstSum = 0.f;
	for (int i = 0; i<nNodes; i++){
		for (int j = 0; j<nNodes; j++){
			if (i != j){
				mNormConstSum += normConst(i, j);
			}
		}
	}
}

float MyDistanceLayout2D::normConst(int i, int j) const{
	return mNormConstMat.At(i, j);
}
float MyDistanceLayout2D::dist(int i, int j) const{
	return mDistanceMatrix->At(i, j);
}

float MyDistanceLayout2D::normConstSum() const{
	return mNormConstSum;
}

float MyDistanceLayout2D::energy() const{
	float energy = 0.f;
	for (int i = 0; i<mDistanceMatrix->GetNumRows(); i++){
		for (int j = i + 1; j<mDistanceMatrix->GetNumCols(); j++){
			float diff = (mPos[i] - mPos[j]).norm() - dist(i, j);
			energy += normConst(i, j)*diff*diff;
		}
	}
	return energy;
}
