#include "MyJoinTree.h"
#include <algorithm>
#include <utility>
#include <stack>

// for debug
#include "MyMap.h"
#include <iostream>
#include <fstream>
using namespace std;
// debug ends

MyJoinTree::MyJoinTree()
{
}


MyJoinTree::~MyJoinTree()
{
}

void MyJoinTree::Update(){
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
	MyArrayMD<MyComponent*, 3> joinComponents(mVolumn->GetDimSizes(), NULL);
	//My3dArrayi joinArcs(mVolumn->GetDimSizes(), -1);

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
					//MyVec3i join = mVolumn->ComputePosition(neighborComponent->loEnd);
					//joinArcs[join] = index;
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
					//MyVec3i join = mVolumn->ComputePosition(neighborComponent->loEnd);
					//joinArcs[join] = index;
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
					//MyVec3i join = mVolumn->ComputePosition(neighborComponent->loEnd);
					//joinArcs[join] = index;
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
	//MyVec3i join = mVolumn->ComputePosition(joinComponent->loEnd);
	//joinArcs[join] = -1;
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

	CheckJoinTreeStatus(mJoinRoot.get());
}

bool MyJoinTree::VoxelSortLess::operator()(int index1, int index2){
	if (index1 >= 0 && index2 >= 0){
		float value1 = mVolumn->At(index1);
		float value2 = mVolumn->At(index2);
		if (value1 < value2){
			return true;
		}
		else if (value1 > value2){
			return false;
		}
	}
	return index1 < index2;
}

bool MyJoinTree::VoxelSortLess::operator()(const MyVec3i& pos1, const MyVec3i& pos2){
	int index1 = mVolumn->ComputeIndex(pos1);
	int index2 = mVolumn->ComputeIndex(pos2);
	return this->operator()(index1, index2);
}

void MyJoinTree::QueueJoinNeighbors(const MyVec3i& pos, MyArray3i& joinNeighbors){
	joinNeighbors.clear();
	if (pos[0]>0) joinNeighbors << pos + MyVec3i(-1, 0, 0);
	if (pos[0]<mVolumn->GetDimSize(0) - 1) joinNeighbors << pos + MyVec3i(+1, 0, 0);
	if (pos[1]>0) joinNeighbors << pos + MyVec3i(0, -1, 0);
	if (pos[1]<mVolumn->GetDimSize(1) - 1) joinNeighbors << pos + MyVec3i(0, +1, 0);
	if (pos[2]>0) joinNeighbors << pos + MyVec3i(0, 0, -1);
	if (pos[2]<mVolumn->GetDimSize(2) - 1) joinNeighbors << pos + MyVec3i(0, 0, +1);
}

void MyJoinTree::CheckComponentsStatus(const MyComponent* joinRoot) const{
	/*
	// check components
	MyMap<MyComponent*, bool> componentChecked;
	for (int iVoxel = nVoxels - 1; iVoxel >= 0; iVoxel--){
		if (joinComponents[iVoxel] == NULL){
			cout << iVoxel << " has no component.\n";
		}
	}
	*/

	// traverse all components and count
	// from root
	ofstream arcStats("arcStats.txt");
	int checkNumComponents = 0;
	int checkNumVertices = 0;
	stack<const MyComponent*> stackComponents;
	stackComponents.push(joinRoot);
	while (!stackComponents.empty()){
		const MyComponent* thisComponent = stackComponents.top();
		stackComponents.pop();
		int numParents = 0;
		const MyComponent* parent = thisComponent->nextHi;
		while (parent != thisComponent){
			stackComponents.push(parent);
			numParents++;
			parent = parent->nextLo;
			if (parent == thisComponent->lastHi) {
				stackComponents.push(parent);
				numParents++;
				break;
			}
		}

		// output stats
		if (thisComponent->loEnd >= 0 && thisComponent->hiEnd <= mVolumn->GetVolume() - 1){
			arcStats << mVolumn->At(thisComponent->hiEnd) << ' ' << mVolumn->At(thisComponent->loEnd) << ' '
				<< numParents << ' ' << thisComponent->GetNumVertices() << endl;
		}

		checkNumVertices += thisComponent->GetNumVertices();
		checkNumComponents++;
	}
	cout << "Join tree checks in " << checkNumComponents << " components\n";
	cout << "Join tree checks in " << checkNumVertices
		<< "(" << mVolumn->GetVolume() << ") vertices\n";
}

MySegmentNodeSPtr MyJoinTree::MakeSegment(MyComponent* component){
	MySegmentNodeSPtr segment = std::make_shared<MySegmentNode>();
	MyArrayf values;
	for (int i = 0; i < component->mVertices->size(); i++){
		values << mVolumn->At(component->mVertices->at(i));
	}
	segment->SetUniqueVoxes(MyVoxContainerf::MakeVoxContainer(
		component->mVertices.get(), &values, mVolumn->GetDimSizes()));
	return segment;
}

MySegmentNodeSPtr MyJoinTree::MakeJoinTree(MyComponent* joinRoot){
	MySegmentNodeSPtr root = 0;
	int nSegs = 0;
	// stack of parent-child pairs
	stack<pair<MySegmentNodeSPtr, MyComponent*>> stackComponents;
	stackComponents.push(make_pair(MySegmentNodeSPtr(0), joinRoot));
	while (!stackComponents.empty()){
		// process the current one
		MyComponent* thisComponent = stackComponents.top().second;
		MySegmentNodeSPtr thisParent = stackComponents.top().first;
		MySegmentNodeSPtr thisSegment = MakeSegment(thisComponent);
		thisSegment->SetAutoIndex();
		nSegs++;
		// this is the root
		// and root has no parent
		if (!root) root = thisSegment;
		else MySegmentNode::Connect(thisParent, thisSegment);
		stackComponents.pop();
		// adding parents to stack
		MyComponent* parent = thisComponent->nextHi;
		while (parent != thisComponent){
			stackComponents.push(make_pair(thisSegment, parent));
			parent = parent->nextLo;
			if (parent == thisComponent->lastHi) {
				stackComponents.push(make_pair(thisSegment, parent));
				break;
			}
		}
	}
	root->UpdateDescendantTotalVoxelCount();
	cout << "Join tree has " << nSegs << " segments\n";
	cout << "Join tree has " << root->GetNumTotalVoxes() 
		<< " (" << mVolumn->GetVolume() << ") voxels\n";
	return root;
}

void MyJoinTree::CheckJoinTreeStatus(const MySegmentNode* joinRoot) const{
	stack<const MySegmentNode*> stackSegments;
	stackSegments.push(joinRoot);
	My3dArrayi checked(mVolumn->GetDimSizes(), 0);
	int numVoxelsChecked = 0;
	int numSegmentsChecked = 0;
	while (!stackSegments.empty()){
		const MySegmentNode* thisSegment = stackSegments.top();
		stackSegments.pop();
		int numVoxels = 0;
		for (int i = 0; i < thisSegment->GetNumChildren(); i++){
			stackSegments.push(thisSegment->GetChild(i).get());
			numVoxels += thisSegment->GetChild(i)->GetNumTotalVoxes();
		}
		if (thisSegment->HasUniqueVoxel()){
			MyVoxContainerfScPtr voxels = thisSegment->GetUniqueVoxes();
			MyVoxContainerf::const_Iterator itr = voxels->Begin();
			MyVoxContainerf::const_Iterator itrEnd = voxels->End();
			while (itr != itrEnd){
				int index = itr.GetVoxelIndex();
				int numTimesChecked = ++checked[index];
				if (numTimesChecked>1){
					cout << "Error: voxel " << index 
						<< " checked " << numTimesChecked << " times.\n";
				}
				itr++;
			}
			numVoxelsChecked += voxels->GetNumVoxes();
			numSegmentsChecked++;
			numVoxels += voxels->GetNumVoxes();
			if (numVoxels != thisSegment->GetNumTotalVoxes()){
				cout << "Error: segment total voxel counting wrong!\n";
			}
		}
	}
	cout << "Join tree checks in " << numSegmentsChecked << " segments\n";
	cout << "Join tree checks in " << numVoxelsChecked
		<< "(" << mVolumn->GetVolume() << ") vertices\n";
}