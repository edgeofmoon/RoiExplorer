#pragma once

#include "MyArray.h"
#include "MyMatrix.h"
#include "MyBoundingBox.h"

class MyDistanceLayout2D
{
public:
	MyDistanceLayout2D();
	~MyDistanceLayout2D();

	void Update();
	void SetDistanceMatrix(MyMatrixfScPtr distMat){
		mDistanceMatrix = distMat;
	}
	const MyArray2f& GetPositions() {
		return mPos;
	}
	// unused
	void SetDistanceEdgeWeightRatio(float ratio){
		mDistWeightRatio = ratio;
	}

	void SetBoundingBox(const MyBoundingBox& box){
		mBoundingBox = box;
	}

protected:

	float GetDistanceEdgeWeightRatioHint(const MyBoundingBox& box) const;
	float GetDistanceEdgeWeightRatio() const;

	virtual bool Advance();

	virtual bool ShouldContinue() const;

	void init();
	float normConst(int i, int j) const;
	float dist(int i, int j) const;

	float normConstSum() const;

	float energy() const;
	float mLastEnergy;
	float mCurrentEnergy;
	float mEnergyDecreaseRationLimit;

	MyMatrixfScPtr mDistanceMatrix;
	MyArray2f mPos;
	MyBoundingBox mBoundingBox;
	float mDistWeightRatio;
	MyMatrixf mNormConstMat;
	float mNormConstSum;
};

