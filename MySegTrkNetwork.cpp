#include "MySegTrkNetwork.h"
#include <thread>
using namespace std;

// debug
#include <iostream>
using namespace std;

atomic<int> MySegTrkNetwork::NextAssignmentIndex = 0;
atomic<int> MySegTrkNetwork::TotalAssignmentCompleted = 0;
std::mutex MySegTrkNetwork::AssignmentLock;
volatile int MySegTrkNetwork::StartAssignmentIndex = 0;

MySegTrkNetwork::MySegTrkNetwork()
{
}


MySegTrkNetwork::~MySegTrkNetwork()
{
}

void MySegTrkNetwork::Update(){
	UpdateRegionTrackIndices_RTB();
	UpdateConnectionMatrix();
}

void MySegTrkNetwork::UpdateRegionTrackIndices_RB(){
	if (mSegNodeInfos->size() == 0) return;
	int numTracks = mTracks->GetNumTracks();
	MyVec3i volSize = mSegNodeInfos->front()->GetSegmentNode()->GetVolumeSize();
	MySegTrkNetwork::NextAssignmentIndex = 0;
	MySegTrkNetwork::TotalAssignmentCompleted = 0;
	mSegmentTrackIndices = std::make_shared<MyArray<MyArrayiSPtr>>(mSegNodeInfos->size());
	int numThread = std::thread::hardware_concurrency() - 1;
	numThread = min(numThread, (int)mSegNodeInfos->size());
	std::thread *tt = new std::thread[numThread];
	for (int i = 0; i < numThread; i++){
		tt[i] = std::thread(MakeTracksCollection_RB, mTracks.get(), mSegNodeInfos.get(),
			volSize, mSegmentTrackIndices.get());
	}
	//MakeTracksCollection(mTracks.get(), mSegNodeInfos.get(), volSize, mSegmentTrackIndices.get());
	int lastAssignment = TotalAssignmentCompleted;
	float lastProgress = -1;
	while (lastAssignment < numTracks*mSegNodeInfos->size()){
		if (TotalAssignmentCompleted != lastAssignment){
			lastAssignment = TotalAssignmentCompleted;
			int progress = lastAssignment / ((float)numTracks*mSegNodeInfos->size()) * 100;
			if (lastProgress != progress){
				cout << "Region Tracks Computing " <<
					progress << "% \r";
				lastProgress = progress;
			}
		}
	}
	for (int i = 0; i < numThread; i++){
		tt[i].join();
	}
	delete[] tt;
	cout << "Region Tracks Computing Completed.\n";
}

void MySegTrkNetwork::UpdateRegionTrackIndices_TB(){
	if (mSegNodeInfos->size() == 0) return;
	int numTracks = mTracks->GetNumTracks();
	MyVec3i volSize = mSegNodeInfos->front()->GetSegmentNode()->GetVolumeSize();
	mSegmentTrackIndices = std::make_shared<MyArray<MyArrayiSPtr>>(mSegNodeInfos->size());
	MyArray<MyVoxContainerfSPtr> volumes;
	for (int i = 0; i < mSegNodeInfos->size(); i++){
		// force make large to ensure fast access
		MyVoxContainerfScPtr autoContainer = mSegNodeInfos->at(i)->GetSegmentNode()->MakeAllVoxes();
		volumes << MyVoxContainerf::MakeVoxContainer(
			autoContainer.get(), MyVoxContainerf::ContainerType_Large);
		mSegmentTrackIndices->operator[](i) = std::make_shared<MyArrayi>();
	}
	TotalAssignmentCompleted = 0;
	// generating threads for working
	int numThread = std::thread::hardware_concurrency() - 1;
	numThread = min(numThread, numTracks);
	std::thread *tt = new std::thread[numThread];
	// generate mutex locks
	MyArray<std::mutex> regionlocks(mSegNodeInfos->size());
	for (int i = 0; i < numThread; i++){
		tt[i] = std::thread(MakeTracksCollections_TB, mTracks.get(), &volumes, &regionlocks,
			volSize, mSegmentTrackIndices.get(), 100000);
	}
	//MakeTracksCollection(mTracks.get(), mSegNodeInfos.get(), volSize, mSegmentTrackIndices.get());
	int lastAssignment = TotalAssignmentCompleted;
	float lastProgress = -1;
	while (lastAssignment < numTracks*mSegNodeInfos->size()){
		if (TotalAssignmentCompleted != lastAssignment){
			lastAssignment = TotalAssignmentCompleted;
			int progress = lastAssignment / (float)(numTracks*mSegNodeInfos->size()) * 100;
			if (lastProgress != progress){
				cout << "Region Tracks Computing " <<
					progress << "% \r";
				lastProgress = progress;
			}
		}
	}
	for (int i = 0; i < numThread; i++){
		tt[i].join();
	}
	for (int i = 0; i < mSegmentTrackIndices->size(); i++){
		sort(mSegmentTrackIndices->at(i)->begin(), mSegmentTrackIndices->at(i)->end());
	}
	delete[] tt;
	cout << "Region Tracks Computing Completed.\n";
}


void MySegTrkNetwork::UpdateRegionTrackIndices_RTB(){
	if (mSegNodeInfos->size() == 0) return;
	int numTracks = mTracks->GetNumTracks();
	MyVec3i volSize = mSegNodeInfos->front()->GetSegmentNode()->GetVolumeSize();
	MySegTrkNetwork::NextAssignmentIndex = 0;
	MySegTrkNetwork::TotalAssignmentCompleted = 0;
	mSegmentTrackIndices = std::make_shared<MyArray<MyArrayiSPtr>>(mSegNodeInfos->size());
	MyArray<MyVoxContainerfSPtr> volumes;
	for (int i = 0; i < mSegNodeInfos->size(); i++){
		// force make large to ensure fast access
		MyVoxContainerfSPtr autoContainer = mSegNodeInfos->at(i)->GetSegmentNode()->MakeAllVoxes();
		volumes <<  MyVoxContainerf::MakeVoxContainer(
			autoContainer.get(), MyVoxContainerf::ContainerType_Large);
		mSegmentTrackIndices->operator[](i) = std::make_shared<MyArrayi>();
	}
	// single core
	//MakeTracksCollection_RTB(mTracks.get(), &volumes, 0, numTracks-1,
	//	volSize, mSegmentTrackIndices.get());
	// multithread
	int procUnit = 8000000;
	for (int startTrk = 0; startTrk < numTracks; startTrk += procUnit){
		int endTrk = min(startTrk+procUnit - 1, numTracks-1);

		MySegTrkNetwork::NextAssignmentIndex = 0;
		int numThread = std::thread::hardware_concurrency() - 1;
		numThread = min(numThread, (int)mSegNodeInfos->size());
		std::thread *tt = new std::thread[numThread];
		for (int i = 0; i < numThread; i++){
			tt[i] = std::thread(MakeTracksCollection_RTB, mTracks.get(), 
				&volumes, startTrk, endTrk,
				volSize, mSegmentTrackIndices.get());
		}
		int lastAssignment = TotalAssignmentCompleted;
		float lastProgress = -1;
		while (lastAssignment < endTrk*mSegNodeInfos->size()){
			if (TotalAssignmentCompleted != lastAssignment){
				lastAssignment = TotalAssignmentCompleted;
				int progress = lastAssignment / ((float)numTracks*mSegNodeInfos->size()) * 100;
				if (lastProgress != progress){
					cout << "Region Tracks Computing " <<
						progress << "% \r";
					lastProgress = progress;
				}
			}
		}
		for (int i = 0; i < numThread; i++){
			tt[i].join();
		}
		delete[] tt;
	}
	cout << "Region Tracks Computing Completed.\n";
}

void MySegTrkNetwork::UpdateConnectionMatrix(){
	int n = mSegNodeInfos->size();
	mConnectionMatrix = std::make_shared<MyMatrixf>(n, n);
	for (int i = 0; i < mSegNodeInfos->size(); i++){
		MyArrayi& arrayi = *(mSegmentTrackIndices->at(i).get());
		mConnectionMatrix->At(i, i) = arrayi.size();
		for (int j = i+1; j < mSegNodeInfos->size(); j++){
			MyArrayi& arrayj = *(mSegmentTrackIndices->at(j).get());
			int numCommon = MyArrayi::CountCommonElementsSorted(&arrayi, &arrayj);
			mConnectionMatrix->At(i, j) = numCommon;
			mConnectionMatrix->At(j, i) = numCommon;
		}
	}
}

void MySegTrkNetwork::MakeTracksCollection_RB(const MyTracks* tracks,
	const MyArray<MySegmentNodeInfoScPtr>* segNodeInfos, MyVec3i volSize,
	MyArray<MyArrayiSPtr>* result){
	while (true){
		// must be done this way
		// to ensure no assignment duplication
		int thisAssignemnt = NextAssignmentIndex++;
		// quit if assignment pool exhausted
		if (thisAssignemnt >= segNodeInfos->size()){
			break;
		}
		MySegmentNodeInfoScPtr segNodeInfo = segNodeInfos->at(thisAssignemnt);
		MyArrayiSPtr intersected = std::make_shared<MyArrayi>();
		MyVoxContainerfScPtr autoContainer = segNodeInfo->GetSegmentNode()->MakeAllVoxes();
		MyVoxContainerfSPtr voxes =  MyVoxContainerf::MakeVoxContainer(
			autoContainer.get(), MyVoxContainerf::ContainerType_Large);
		for (int i = 0; i < tracks->GetNumTracks(); i++){
			for (int j = 0; j < tracks->GetNumVertex(i); j++){
				MyVec3f fpos = tracks->GetCoord(i, j);
				MyVec3i ipos;
				ipos[0] = std::min((int)(fpos[0] + 0.5), volSize[0]-1);
				ipos[1] = std::min((int)(fpos[1] + 0.5), volSize[1]-1);
				ipos[2] = std::min((int)(fpos[2] + 0.5), volSize[2]-1);
				int index = My3dArrayf::ComputeIndex(ipos, volSize);
				if (voxes->IsVoxelIn(index)){
					intersected->PushBack(i);
					break;
				}
			}
			++TotalAssignmentCompleted;
		}
		sort(intersected->begin(), intersected->end());
		result->operator[](thisAssignemnt) = intersected;
	}
}

void MySegTrkNetwork::MakeTracksCollections_TB(const MyTracks* tracks,
	MyArray<MyVoxContainerfSPtr>* volumes, MyArray<std::mutex>* regionlocks,
	MyVec3i volSize, MyArray<MyArrayiSPtr>* result, int procUnit){
	MyArray<MyArrayi> tempRsts(result->size());
	while (true){
		if (StartAssignmentIndex > tracks->GetNumTracks()){
			break;
		}
		// must be done this way
		// to ensure no assignment duplication
		AssignmentLock.lock();
		int thisAssignment = StartAssignmentIndex;
		StartAssignmentIndex += procUnit;
		AssignmentLock.unlock();
		for (int iTrk = thisAssignment; iTrk < thisAssignment + procUnit
			&& iTrk < tracks->GetNumTracks(); iTrk++){
			MyArrayi trackRegions(result->size(), false);
			for (int j = 0; j < tracks->GetNumVertex(iTrk); j++){
				MyVec3f fpos = tracks->GetCoord(iTrk, j);
				MyVec3i ipos;
				ipos[0] = std::min((int)(fpos[0] + 0.5), volSize[0] - 1);
				ipos[1] = std::min((int)(fpos[1] + 0.5), volSize[1] - 1);
				ipos[2] = std::min((int)(fpos[2] + 0.5), volSize[2] - 1);
				int index = My3dArrayf::ComputeIndex(ipos, volSize);
				for (int i = 0; i < volumes->size(); i++){
					if (!trackRegions[i]){
						if (volumes->at(i)->IsVoxelIn(index)){
							trackRegions[i] = true;
						}
					}
				}
			}
			for (int i = 0; i < trackRegions.size(); i++){
				if (trackRegions[i]){
					tempRsts[i].PushBack(iTrk);
				}
			}
			TotalAssignmentCompleted += result->size();
		}

		for (int i = 0; i < tempRsts.size(); i++){
			if (!tempRsts[i].empty()){
				// lock to append this track index
				regionlocks->at(i).lock();
				result->operator[](i)->PushBack(&tempRsts[i]);
				// release lock
				regionlocks->at(i).unlock();
				tempRsts[i].clear();
			}
		}
	}
}


void MySegTrkNetwork::MakeTracksCollection_RTB(const MyTracks* tracks,
	MyArray<MyVoxContainerfSPtr>* volumes, int startTrk, int endTrk,
	MyVec3i volSize, MyArray<MyArrayiSPtr>* result){
	while (true){
		// must be done this way
		// to ensure no assignment duplication
		int thisAssignemnt = NextAssignmentIndex++;
		// quit if assignment pool exhausted
		if (thisAssignemnt >= volumes->size()){
			break;
		}
		MyArrayi intersected;
		MyVoxContainerfSPtr voxes = volumes->at(thisAssignemnt);
		for (int i = startTrk; i <= endTrk; i++){
			for (int j = 0; j < tracks->GetNumVertex(i); j++){
				MyVec3f fpos = tracks->GetCoord(i, j);
				MyVec3i ipos;
				ipos[0] = std::min((int)(fpos[0] + 0.5), volSize[0] - 1);
				ipos[1] = std::min((int)(fpos[1] + 0.5), volSize[1] - 1);
				ipos[2] = std::min((int)(fpos[2] + 0.5), volSize[2] - 1);
				int index = My3dArrayf::ComputeIndex(ipos, volSize);
				if (voxes->IsVoxelIn(index)){
					intersected.PushBack(i);
					break;
				}
			}
			++TotalAssignmentCompleted;
		}
		sort(intersected.begin(), intersected.end());
		result->operator[](thisAssignemnt)->PushBack(&intersected);
	}
}