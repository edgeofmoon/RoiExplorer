#include "MyJoinTreeROI.h"

// debug
#include <iostream>
using namespace std;

MyJoinTreeROI::MyJoinTreeROI()
{
}


MyJoinTreeROI::~MyJoinTreeROI()
{
}

void MyJoinTreeROI::SetROIs(const MyArray<MySegmentNodeSPtr>* ROIs){
	mROIs = *ROIs;
}

void MyJoinTreeROI::Update(){

	MyArrayMD<MyComponent*, 3> joinComponents(mVolumn->GetDimSizes(), NULL);
	// first make each ROI a component already
	for (int i = 0; i < mROIs.size(); i++){
		const MySegmentNode* roi = mROIs[i].get();
		const MyVoxContainerf* voxels = roi->GetUniqueVoxes().get();
		MyComponent* roiComp = new MyComponent;
		MyVoxContainerf::const_Iterator itr = voxels->Begin();
		MyVoxContainerf::const_Iterator itrEnd = voxels->End();
		int hiEndIdx = itr.GetVoxelIndex();
		float hiEndValue = mVolumn->At(hiEndIdx);
		int loEndIdx = itr.GetVoxelIndex();
		float loEndValue = mVolumn->At(loEndIdx);
		while (itr != itrEnd){
			int voxelIdx = itr.GetVoxelIndex();
			float value = mVolumn->At(voxelIdx);;
			if (value > 0){
				roiComp->mVertices->PushBack(voxelIdx);
				if (value > hiEndValue){
					hiEndValue = value;
					hiEndIdx = voxelIdx;
				}
				if (value < loEndValue || loEndValue == 0){
					loEndValue = value;
					loEndIdx = voxelIdx;
				}
				joinComponents[voxelIdx] = roiComp;
			}
			itr++;
		}
		roiComp->hiEnd = hiEndIdx;
		roiComp->loEnd = loEndIdx;
		roiComp->nextHi = roiComp;
		roiComp->lastHi = roiComp;
		roiComp->nextLo = roiComp;
		roiComp->lastLo = roiComp;
	}


	// copy from old join tree class
	// tmp variables
	MyComponent* joinRoot = 0;
	int numSuperNodes = 0;
	int numJoinSuperNodes = 0;
	// end tmp variables

	int nVoxels = mVolumn->GetVolume();
	MyArrayi voxelSorted = MyArrayi::GetSequence(0, nVoxels - 1);
	VoxelSortLess compareLess;
	compareLess.mVolumn = mVolumn;
	sort(voxelSorted.begin(), voxelSorted.end(), compareLess);
	MyComponent* joinComponent = 0;
	MyArray3i joinNeighbors;

	//	A.	create the array holding the join components, all initialized to NULL
	// except removing redifinition
	//MyArrayMD<MyComponent*, 3> joinComponents(mVolumn->GetDimSizes(), NULL);
	My3dArrayi joinArcs(mVolumn->GetDimSizes(), -1);

	//	B.	do a loop in downwards order, adding each vertex to the union-find:
	for (int iVoxel = nVoxels - 1; iVoxel >= 0; iVoxel--){
		int index = voxelSorted[iVoxel];
		// i.	queue up the neighbours of the vertex
		MyVec3i pos = mVolumn->ComputePosition(index);
		QueueJoinNeighbors(pos, joinNeighbors);
		int numNeighborComponents = 0;

		// ii.	loop through all neighbours of the vertex:
		joinComponent = joinComponents[pos];
		for (int iNeighbor = 0; iNeighbor < joinNeighbors.size(); iNeighbor++){
			MyVec3i neighborPos = joinNeighbors[iNeighbor];
			int neighborIndex = mVolumn->ComputeIndex(neighborPos);
			// a.	if the neighbour is lower than the vertex, skip
			if (compareLess(neighborPos, pos)) continue;
			MyComponent* neighborComponent = joinComponents[neighborPos]->Component();
			// b.	if the neighbour belongs to a different component than the vertex
			// (otherwise do nothing)
			if (joinComponent != neighborComponent){
				// 1.	and the vertex has no component yet, add the vertex to the component
				if (numNeighborComponents == 0){
					joinComponents[pos] = neighborComponent;
					joinComponent = neighborComponent;
					neighborComponent->seedTo = neighborIndex;
					neighborComponent->seedFrom = index;
					MyVec3i join = mVolumn->ComputePosition(neighborComponent->loEnd);
					joinArcs[join] = index;
					numNeighborComponents++;
					// increase the count
					joinComponent->mVertices->PushBack(index);
				}
				// 2.	the vertex is a component, but is not yet a join, make it a join
				else if (numNeighborComponents == 1){
					MyComponent *newComponent = new MyComponent;
					// create a new component
					newComponent->hiEnd = index;
					newComponent->loEnd = index;
					newComponent->nextHi = joinComponent;
					newComponent->lastHi = neighborComponent;
					// increase the count
					newComponent->mVertices->PushBack(index);
					// update the neighbor's component
					neighborComponent->seedTo = neighborIndex;
					neighborComponent->seedFrom = index;
					MyVec3i join = mVolumn->ComputePosition(neighborComponent->loEnd);
					joinArcs[join] = index;
					neighborComponent->loEnd = index;
					neighborComponent->nextLo = newComponent;
					neighborComponent->lastLo = joinComponent;
					// update the existing pointer for the vertex's component
					joinComponent->loEnd = index;
					joinComponent->nextLo = neighborComponent;
					joinComponent->lastLo = newComponent;
					// decrease the count
					// because it's transferred to an another component
					joinComponent->mVertices->EraseOne(index);
					// perform the merge
					neighborComponent->MergeTo(newComponent);
					joinComponent->MergeTo(newComponent);
					// and reset the join component
					joinComponents[pos] = newComponent;
					joinComponent = newComponent;
					numNeighborComponents++;
					numSuperNodes++;
					numJoinSuperNodes++;
				}
				// 3.	the vertex is a join, merge the additional component
				else{
					// update the neighbor's component
					neighborComponent->seedTo = neighborIndex;
					neighborComponent->seedFrom = index;
					MyVec3i join = mVolumn->ComputePosition(neighborComponent->loEnd);
					joinArcs[join] = index;
					neighborComponent->loEnd = index;
					neighborComponent->nextLo = joinComponent;
					neighborComponent->lastLo = joinComponent->lastHi;
					neighborComponent->lastLo->nextLo = neighborComponent;
					// perform the merge
					neighborComponent->MergeTo(joinComponent);
					joinComponent->lastHi = neighborComponent;
				}
			}
		}
		// iii.	if the vertex still has no (NULL) component, start a new one
		// (i.e. the vertex is a local maximum)
		if (joinComponent == NULL){
			joinComponent = new MyComponent;
			joinComponents[index] = joinComponent;
			joinComponent->hiEnd = index;
			joinComponent->loEnd = index;
			joinComponent->nextHi = joinComponent;
			joinComponent->lastHi = joinComponent;
			joinComponent->nextLo = joinComponent;
			joinComponent->lastLo = joinComponent;
			numSuperNodes++;
			numJoinSuperNodes++;
			// increase the count
			joinComponent->mVertices->PushBack(index);
		}
	}
	//	C.	tie off the final component to minus_infinity
	MyVec3i join = mVolumn->ComputePosition(joinComponent->loEnd);
	joinArcs[join] = -1;
	joinComponent->loEnd = -1;
	joinComponent->nextLo = joinComponent;
	joinComponent->lastLo = joinComponent;
	joinRoot = joinComponent;

	// check status
	//cout << "Join tree has " << numJoinSuperNodes << " components\n";
	//CheckComponentsStatus(joinRoot);

	mFilter->SetComponents(joinRoot, &joinComponents);
	mFilter->Update();
	mJoinRoot = MakeJoinTree(joinRoot);

	//CheckJoinTreeStatus(mJoinRoot.get());
}