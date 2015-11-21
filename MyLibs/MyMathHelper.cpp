#include "MyMathHelper.h"

#include "svd.c"

#include <vector>
#include <algorithm>
#include <assert.h>

#include <Eigen/Dense>
using namespace Eigen;
using namespace std;
// more than 40 is hardly expressable
MyArrayi MyMathHelper::Factorials = MyArrayi(40, -1);

MyMathHelper::MyMathHelper(void)
{
}


MyMathHelper::~MyMathHelper(void)
{
}

void MyMathHelper::SingularValueDecomposition(const MyMatrixf* inMat, MyMatrixf* leftMat, float * sigValues){
	int m = inMat->GetNumRows();
	int n = inMat->GetNumCols();
	float** a = new float*[m];
	float** v = new float*[m];
	for (int i = 0;i<m;i++){
		a[i] = new float[n];
		v[i] = new float[n];
		for (int j = 0;j<n;j++){
			a[i][j] = inMat->At(i,j);
		}
	}
	sigValues = new float[n];
	
	dsvd(a, m, n, sigValues, v);

	leftMat = new MyMatrixf((const float **)a, m, n);

	delete[] a;
	delete[] v;
}

float *w;
bool largerThan(int i,int j){
	return w[i]>w[j];
};

MyMatrixf* MyMathHelper::PCA_Projection(const MyMatrixf* dataMat, int nDim){
	// zero-mean
	MyMatrixf mean = dataMat->GetMeanOfCol();
	MyMatrixf zeroMeanMat = *dataMat - mean.Duplicated(dataMat->GetNumRows(), 1);

	// equal variance
	MyMatrixf adjustMat = zeroMeanMat.GetEqualVarByCol();

	// get transpose
	MyMatrixf adjustMatTranspose = adjustMat.Transpose();

	// covariance matrix
	MyMatrixf covMat = adjustMatTranspose*adjustMat;


	// change to appropriate format
	int m = covMat.GetNumRows();
	int n = covMat.GetNumCols();
	MatrixXf covMatTfd(m, n);
	for (int i = 0; i<m; i++){
		for (int j = 0; j<n; j++){
			covMatTfd(i, j) = covMat.At(i, j);
		}
	}

	EigenSolver<MatrixXf> es;
	es.compute(covMatTfd);

	auto evalues = es.eigenvalues();
	w = new float[evalues.rows()];
	for (int i = 0; i < evalues.rows(); i++){
		w[i] = evalues(i).real();
	}

	// get the largest nDim eigen vectors
	vector<int> evs;
	for (int i = 0; i<n; i++){
		evs.push_back(i);
	}
	sort(evs.begin(), evs.end(), largerThan);
	evs.resize(nDim);

	// project the data to the low dimension
	auto evectors = es.eigenvectors();
	MyMatrixf vMat(m, n);
	for (int i = 0; i < m; i++){
		for (int j = 0; j < n; j++){
			vMat.At(i, j) = evectors(i, j).real();
		}
	}
	MyMatrixf largestEvs = vMat.GetCols(evs);
	MyMatrixf rst = zeroMeanMat*largestEvs;
	return new MyMatrixf(rst);
}


int MyMathHelper::BinomialCoefficient(int n, int i){
	if(i == 0 || i == n) return 1;
	return Factorial(n)/(Factorial(i)*Factorial(n-i));
}

int MyMathHelper::Factorial(int i){
	if(i == 0) return 1;
	if(Factorials[i] > 0){
		return Factorials[i];
	}
	else{
		int rst = i*Factorial(i-1);
		Factorials[i] = rst;
		return rst;
	}
}

float MyMathHelper::ComputeMean(const MyArrayf* values){
	if (values->empty()) return 0;
	float valueSum = 0;
	for (int i = 0; i < values->size(); i++){
		valueSum += values->at(i);
	}
	return valueSum / values->size();
}

float MyMathHelper::ComputeStandardDeviation(const MyArrayf* values, float mean){
	if (values->empty()) return 0;
	float variation = 0;
	for (int i = 0; i < values->size(); i++){
		float deviation = values->at(i) - mean;
		variation += deviation*deviation;
	}
	return sqrtf(variation / values->size());
}

MyVec2f MyMathHelper::ComputeRange(const MyArrayf* values){
	if (values->empty()) return MyVec2f();
	float v0 = values->front();
	MyVec2f range(v0, v0);
	for (int i = 1; i < values->size(); i++){
		float value = values->at(i);
		if (value < range[0]) range[0] = value;
		else if (value > range[1]) range[1] = value;
	}
	return range;
}

float MyMathHelper::ComputeStandardDeviationUnbiased(const MyArrayf* values, float mean){
	assert (values->size()>1);
	float variation = 0;
	for (int i = 0; i < values->size(); i++){
		float deviation = values->at(i) - mean;
		variation += deviation*deviation;
	}
	return variation / (values->size() - 1);
}

bool MyMathHelper::IsIntersected(const MyBox2f& box, const MyLine2f& line){
	

	MyVec2f offset = box.GetLowPos();
	MyBox2f tBox = box - offset;
	MyLine2f tLine = line - offset;
	
	char codeByte0 = MyMathHelper::GetCohenSutherlandByte(line.GetStart(), box);
	char codeByte1 = MyMathHelper::GetCohenSutherlandByte(line.GetEnd(), box);
	char codeByteOr = codeByte0 | codeByte1;
	if (!(codeByte0 | codeByte0)){
		// line is completely inside box
		return true;
	}
	else if (codeByte0 & codeByte1){
		// both ends are in the same regions
		return false;
	}
	else if (codeByteOr == 3 || codeByteOr == 12){
		// one end is left, the other is right
		// one end is top, the other is bottom
		return true;
	}
	else if (codeByteOr & csBIT_LEFT){
		// test left edge is enough
		MyLine2f leftEdge(box.GetLowPos(), MyVec2f(box.GetLowPos()[0], box.GetHighPos()[1]));
		return MyLine2f::Intersected(leftEdge, line);
	}
	else if (codeByteOr & csBIT_RIGHT){
		// test right edge is enough
		MyLine2f rightEdge(MyVec2f(box.GetHighPos()[0], box.GetLowPos()[1]), box.GetHighPos());
		return MyLine2f::Intersected(rightEdge, line);
	}
	else if (codeByteOr & csBIT_TOP){
		// test top edge is enough
		MyLine2f topEdge(MyVec2f(box.GetLowPos()[0], box.GetHighPos()[1]), box.GetHighPos());
		return MyLine2f::Intersected(topEdge, line);
	}
	else{
		// test bottom edge is enough
		MyLine2f bottomEdge(box.GetLowPos(), MyVec2f(box.GetHighPos()[0], box.GetLowPos()[1]));
		return MyLine2f::Intersected(bottomEdge, line);
	}
}

float MyMathHelper::MinDistance(const MyLine2f& line, const MyVec2f& pt){
	/*
	// source: 
	// http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
	float minimum_distance(MyVec2f v, MyVec2f w, MyVec2f p) {
		// Return minimum distance between line segment vw and point p
		const float l2 = length_squared(v, w);  // i.e. |w-v|^2 -  avoid a sqrt
		if (l2 == 0.0) return distance(p, v);   // v == w case
		// Consider the line extending the segment, parameterized as v + t (w - v).
		// We find projection of point p onto the line. 
		// It falls where t = [(p-v) . (w-v)] / |w-v|^2
		const float t = dot(p - v, w - v) / l2;
		if (t < 0.0) return distance(p, v);       // Beyond the 'v' end of the segment
		else if (t > 1.0) return distance(p, w);  // Beyond the 'w' end of the segment
		const MyVec2f projection = v + t * (w - v);  // Projection falls on the segment
		return distance(p, projection);
	};
	*/

	float l2 = line.GetLengthSquare();
	const MyVec2f& st = line.GetStart();
	const MyVec2f& ed = line.GetEnd();
	if (l2 == 0) return (st - pt).norm();
	float t = (pt - st).dotMultiply(ed - st) / l2;
	if (t < 0) return (pt - st).norm();
	else if (t > 1) return (pt - ed).norm();
	MyVec2f proj = st + t*(ed - st);
	return (pt - proj).norm();
}

char MyMathHelper::GetCohenSutherlandByte(const MyVec2f pos, const MyBox2f& box){
	char byte = 0;
	const MyVec2f& low = box.GetLowPos();
	const MyVec2f& high = box.GetHighPos();
	if (pos[0]<low[0]) byte |= csBIT_LEFT;
	if (pos[0]>high[0]) byte |= csBIT_RIGHT;
	if (pos[1]<low[1]) byte |= csBIT_BOTTOM;
	if (pos[1]>high[1]) byte |= csBIT_TOP;
	return byte;
}