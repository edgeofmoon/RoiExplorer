#pragma once

#include "MyTracks.h"
#include "MySegmentNodeInfo.h"
#include "MyMatrix.h"
#include <atomic>
#include <mutex>

class MySegTrkNetwork
{
public:
	MySegTrkNetwork();
	~MySegTrkNetwork();

	void SetTracks(MyTracksScPtr tracks){
		mTracks = tracks;
	}
	MyTracksScPtr GetTracks() const { 
		return mTracks; 
	}

	void SetSegmentNodeInfos(MyArrayScPtr<MySegmentNodeInfoScPtr> segNodeInfos){
		mSegNodeInfos = segNodeInfos;
	}
	MyArrayScPtr<MySegmentNodeInfoScPtr> GetSegmentNodeInfos() const {
		return mSegNodeInfos;
	}
	MyMatrixfScPtr GetConnectionMatrix() const{
		return mConnectionMatrix;
	}
	MyArrayScPtr<MyArrayiSPtr> GetSegmentTrackIndices()const {
		return mSegmentTrackIndices;
	}
	void Update();

protected:
	MyTracksScPtr mTracks;

	MyArrayScPtr<MySegmentNodeInfoScPtr> mSegNodeInfos;
	MyArraySPtr<MyArrayiSPtr> mSegmentTrackIndices;
	MyMatrixfSPtr mConnectionMatrix;

	static atomic<int> TotalAssignmentCompleted;

	void UpdateConnectionMatrix();

	// each thread process a region 
	// and whole track set at a time
	static atomic<int> NextAssignmentIndex;
	static void MakeTracksCollection_RB(const MyTracks* tracks, 
		const MyArray<MySegmentNodeInfoScPtr>* segNodeInfos, 
		MyVec3i volSize, MyArray<MyArrayiSPtr>* result);
	void UpdateRegionTrackIndices_RB();

	// each thread process all regions 
	// and procUnit tracks at a time
	static std::mutex AssignmentLock;
	static volatile int StartAssignmentIndex;
	static void MakeTracksCollections_TB(const MyTracks* tracks,
		MyArray<MyVoxContainerfSPtr>* volumes, MyArray<std::mutex>* regionlocks,
		MyVec3i volSize, MyArray<MyArrayiSPtr>* result, int procUnit);
	void UpdateRegionTrackIndices_TB();


	// each thread process a region 
	// and some tracks tracks set at a time
	// sync when all these sub tracks are completed
	static void MakeTracksCollection_RTB(const MyTracks* tracks,
		MyArray<MyVoxContainerfSPtr>* volumes, int startTrk, int endTrk, 
		MyVec3i volSize, MyArray<MyArrayiSPtr>* result);
	void UpdateRegionTrackIndices_RTB();
};

typedef MySharedPointer<MySegTrkNetwork> MySegTrkNetworkSPtr;
typedef MySharedPointer<const MySegTrkNetwork> MySegTrkNetworkScPtr;

