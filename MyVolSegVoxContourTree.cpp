#include "MyVolSegVoxContourTree.h"

#include "MyContourTree.h"

#include <thread>

// debug
#include "MyTimer.h"
#include <iostream>

using namespace std;

MyVolSegVoxContourTree::MyVolSegVoxContourTree()
{
}


MyVolSegVoxContourTree::~MyVolSegVoxContourTree()
{
}

MyArraySPtr<MySegmentNodeSPtr> MyVolSegVoxContourTree::MakeSegments() const{
	MyContourTree ct(mVolume.get());
	const std::map<long, vector<float*>>& arcVoxes = ct.GetArcVoxes();
	std::map<long, vector<float*>>::const_iterator itr = arcVoxes.begin();
	MyArraySPtr<MySegmentNodeSPtr> containers(new MyArray<MySegmentNodeSPtr>);
	containers->reserve(arcVoxes.size());
	const float* firstVolIdx = &ct.GetValue(0, 0, 0);

	while (itr != arcVoxes.end()){
		const vector<float*>& voxes = itr->second;
		if (voxes.size() > 0){
			MyArrayi volIdx;
			MyArrayf volvalues;
			volIdx.reserve(voxes.size());
			volvalues.reserve(voxes.size());
			for (int j = 0; j < voxes.size(); j++){
				long x, y, z;
				ct.GetHeight().ComputeIndex(voxes[j], x, y, z);
				volIdx << mVolume->ComputeIndex(MyVec3i(x,y,z));
				volvalues << *voxes[j];
			}
			MyVoxContainerfSPtr container = MyVoxContainerf::MakeVoxContainer(
				&volIdx, &volvalues, mVolume->GetDimSizes());
			MySegmentNodeSPtr containerNode = std::make_shared<MySegmentNode>();
			containerNode->SetUniqueVoxes(container);
			containers->push_back(containerNode);
		}
		itr++;
	}
	return containers;
}

/*
// this is a multi-thread version
MyArraySPtr<MyVoxContainerfSPtr> MyVolSegVoxContourTree::MakeSegments() const{
	MyContourTree ct(mVolume.get());
	MyTimer timer;
	timer.Restart();
	const std::map<long, vector<float*>>& arcVoxes = ct.GetArcVoxes();
	std::map<long, vector<float*>>::const_iterator itr = arcVoxes.begin();
	MyArraySPtr<MyVoxContainerfSPtr> containers(new MyArray<MyVoxContainerfSPtr>);
	containers->resize(arcVoxes.size());
	MyArray<const vector<float*>*> arcVoxVector;
	arcVoxVector.reserve(arcVoxes.size());
	for (std::map<long, vector<float*>>::const_iterator itr = arcVoxes.begin();
		itr != arcVoxes.end(); itr++){
		arcVoxVector << &(itr->second);
	}

	// multi-thread edition
	int numThread = std::thread::hardware_concurrency() - 1;
	numThread = min(numThread, (int)arcVoxes.size());
	// no idea why a single thread is faster
	//numThread = 1;
	std::thread *tt = new std::thread[numThread - 1];
	int segsPerThread = arcVoxes.size() / numThread;
	const float* firstVox = &ct.GetValue(0, 0, 0);
	std::cout << "Overhead: " << timer.GetElapsed() << " seconds.\n";
	timer.Restart();
	for (int i = 0; i < numThread - 1; i++){
		int startIdx = segsPerThread*i;
		int endIdx = segsPerThread*(i + 1) - 1;
		tt[i] = std::thread(CovertToVoxContainer, containers, &arcVoxVector, 
		startIdx, endIdx, ct, mVolume.get());
	}

	CovertToVoxContainer(containers, &arcVoxVector, segsPerThread*(numThread - 1), 
		arcVoxes.size()-1, ct, mVolume.get());
	for (int i = 0; i < numThread - 1; i++){
		tt[i].join();
	}
	std::cout << "threaded computing: " << timer.GetElapsed() << " seconds.\n";
	timer.Restart();
	delete[] tt;
	std::cout << "threaded release: " << timer.GetElapsed() << " seconds.\n";

	return containers;
}
*/

void MyVolSegVoxContourTree::CovertToVoxContainer(
	MyArraySPtr<MyVoxContainerfSPtr> containers,
	const MyArray<const std::vector<float*>* > * voxesVector,
	int startIdx, int endIdx, const float* firstVolIdx,
	const MyContourTree* ct, const My3dArrayf* vol){
	for (int i = startIdx; i <= endIdx; i++){
		const vector<float*>& voxes = *(voxesVector->at(i));
		if (voxes.size() > 0){
			MyArrayi volIdx;
			MyArrayf volvalues;
			volIdx.reserve(voxes.size());
			volvalues.reserve(voxes.size());
			for (int j = 0; j < voxes.size(); j++){
				long x, y, z;
				ct->GetHeight().ComputeIndex(voxes[j], x, y, z);
				volIdx << vol->ComputeIndex(MyVec3i(x, y, z));
				volvalues << *voxes[j];
			}
			MyVoxContainerfSPtr container = MyVoxContainerf::MakeVoxContainer(
				&volIdx, &volvalues, vol->GetDimSizes());
			containers->operator[](i) = container;
		}
		else{
			containers->operator[](i) = 0;
		}
	}
}