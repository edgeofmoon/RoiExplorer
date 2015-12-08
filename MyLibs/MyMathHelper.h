#pragma once

#include "MyMatrix.h"
#include "MyArray.h"
#include "MyVec.h"
#include "MyBox.h"
#include "MyLine.h"
#include "MyPolyLine.h"

class MyMathHelper
{
public:
	MyMathHelper(void);
	~MyMathHelper(void);

	// memory leak alert
	// outMat and eigens are new
	static void SingularValueDecomposition(const MyMatrixf* inMat, 
		MyMatrixf* leftMat, float * sigValues);

	static MyMatrixf* PCA_Projection(const MyMatrixf* dataMat, int nDim = 2);

	static int BinomialCoefficient(int n, int i);

	static int Factorial(int i);

	static float ComputeMean(const MyArrayf* values);

	static float ComputeStandardDeviation(const MyArrayf* values, float mean);

	static MyVec2f ComputeRange(const MyArrayf* values);

	// values must have at least 2 samples
	static float ComputeStandardDeviationUnbiased(const MyArrayf* values, float mean);

	static bool IsIntersected(const MyBox2f& box, const MyLine2f& line);

	//static bool IsIntersected(const MyBox2f& box, const MyPolyline2f& polyline);

	static float MinDistance(const MyLine2f& line, const MyVec2f& point);

protected:
	static MyArrayi Factorials;

	static const char csBIT_INSIDE = 0; // 0000
	static const char csBIT_LEFT = 1;   // 0001
	static const char csBIT_RIGHT = 2;  // 0010
	static const char csBIT_BOTTOM = 4; // 0100
	static const char csBIT_TOP = 8;    // 1000
	char static GetCohenSutherlandByte(const MyVec2f pos, const MyBox2f& box);
};

