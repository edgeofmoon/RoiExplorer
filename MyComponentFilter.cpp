#include "MyComponentFilter.h"
#include <queue>
#include <stack>
using namespace std;

// for debug
#include <iostream>
MyComponentFilter::MyComponentFilter()
{
	mSizeThreshold = 5;
}


MyComponentFilter::~MyComponentFilter()
{
}

void MyComponentFilter::Update(){
	// first find all leaves
	queue<MyComponent*> leaveQueue;
	stack<MyComponent*> stackComponents;
	stackComponents.push(mRoot);
	int numComponents = 0;
	while (!stackComponents.empty()){
		MyComponent* thisComponent = stackComponents.top();
		stackComponents.pop();
		MyComponent* parent = thisComponent->nextHi;
		while (parent != thisComponent){
			stackComponents.push(parent);
			parent = parent->nextLo;
			if (parent == thisComponent->lastHi) {
				stackComponents.push(parent);
				break;
			}
		}
		if (IsLeaveComponent(thisComponent)){
			leaveQueue.push(thisComponent);
		}
		numComponents++;
	}
	cout << "Number Componets before filter: " << numComponents << endl;
	// prune leaves
	int numPruned = 0;
	while (!leaveQueue.empty()){
		MyComponent* thisComponent = leaveQueue.front();
		leaveQueue.pop();
		if (IsComponentToBePruned(thisComponent)){
			// first store its lower component
			MyComponent* child = mComponents->At(thisComponent->loEnd);
			// prune itself
			PruneComponentFromUp(thisComponent);
			numPruned++;
			// check if child needs to be pruned
			if (IsLeaveComponent(child)){
				if (IsComponentToBePruned(child)){
					// add child to leave queue
					leaveQueue.push(child);
				}
			}
		}
	}
	cout << "Number Componets Pruned: " << numPruned << endl;
	// check pruned
	stackComponents.push(mRoot);
	numComponents = 0;
	while (!stackComponents.empty()){
		MyComponent* thisComponent = stackComponents.top();
		stackComponents.pop();
		MyComponent* parent = thisComponent->nextHi;
		while (parent != thisComponent){
			stackComponents.push(parent);
			parent = parent->nextLo;
			if (parent == thisComponent->lastHi) {
				stackComponents.push(parent);
				break;
			}
		}
		numComponents++;
	}
	cout << "Number Componets after prune: " << numComponents << endl;

	// merge regular components
	int numMerged = 0;
	stackComponents.push(mRoot);
	while (!stackComponents.empty()){
		MyComponent* thisComponent = stackComponents.top();
		stackComponents.pop();
		while (IsComponentToBeMerged(thisComponent)){
			MergeComponentWithUp(thisComponent);
			numMerged++;
		}
		MyComponent* parent = thisComponent->nextHi;
		while (parent != thisComponent){
			stackComponents.push(parent);
			parent = parent->nextLo;
			if (parent == thisComponent->lastHi) {
				stackComponents.push(parent);
				break;
			}
		}
	}
	cout << "Number Componets Merged: " << numMerged << endl;

	stackComponents.push(mRoot);
	numComponents = 0;
	while (!stackComponents.empty()){
		MyComponent* thisComponent = stackComponents.top();
		stackComponents.pop();
		MyComponent* parent = thisComponent->nextHi;
		while (parent != thisComponent){
			stackComponents.push(parent);
			parent = parent->nextLo;
			if (parent == thisComponent->lastHi) {
				stackComponents.push(parent);
				break;
			}
		}
		numComponents++;
	}
	cout << "Number Componets after merge: " << numComponents << endl;
}

bool MyComponentFilter::IsLeaveComponent(const MyComponent* comp) const{
	MyComponent* parent = comp->nextHi;
	return parent == comp;
}

bool MyComponentFilter::IsComponentToBePruned(const MyComponent* comp) const{
	return comp->GetNumVertices() < mSizeThreshold;
}

void MyComponentFilter::PruneComponentFromUp(MyComponent* comp){
	MyComponent* child = mComponents->At(comp->loEnd);
	if (comp->lastLo != child) comp->lastLo->nextLo = comp->nextLo;
	else child->nextHi = comp->nextLo;
	if (comp->nextLo != child) comp->nextLo->lastLo = comp->lastLo;
	else child->lastHi = comp->lastLo;
	//comp->lastHi->nextHi = comp->nextHi;
	//comp->nextHi->lastHi = comp->lastHi;
	delete comp;
}

bool MyComponentFilter::IsComponentToBeMerged(const MyComponent* comp) const{
	return comp->nextHi->nextLo == comp;
}

void MyComponentFilter::MergeComponentWithUp(MyComponent* comp){
	MyComponent* parent = comp->nextHi;
	comp->mVertices->PushBack(parent->mVertices.get());
	if (parent->nextHi == parent){
		comp->nextHi = comp;
	}
	else comp->nextHi = parent->nextHi;
	if (parent->lastHi == parent){
		comp->lastHi = comp;
	}
	else comp->lastHi = parent->lastHi;
	if (parent->lastHi->nextHi == parent){
		parent->lastHi->nextHi = comp;
	}
	if (parent->lastLo->nextLo == parent){
		parent->lastLo->nextLo = comp;
	}
	if (parent->nextHi->lastHi == parent){
		parent->nextHi->lastHi = comp;
	}
	if (parent->nextLo->lastLo == parent){
		parent->nextLo->lastLo = comp;
	}
	// these will be true if its a long line
	if (parent->nextHi->nextLo == parent){
		parent->nextHi->nextLo = comp;
	}
	if (parent->nextHi->lastLo == parent){
		parent->nextHi->lastLo = comp;
	}
	delete parent;
	//CheckStatus(mRoot);
}

void MyComponentFilter::CheckStatus(const MyComponent* root) const{
	int numComponents = 0;
	stack<MyComponent*> stackComponents;
	stackComponents.push(mRoot);
	while (!stackComponents.empty()){
		MyComponent* thisComponent = stackComponents.top();
		stackComponents.pop();
		MyComponent* parent = thisComponent->nextHi;
		while (parent != thisComponent){
			stackComponents.push(parent);
			parent = parent->nextLo;
			if (parent == thisComponent->lastHi) {
				stackComponents.push(parent);
				break;
			}
		}
		int a = thisComponent->lastHi->hiEnd;
		int b = thisComponent->nextHi->hiEnd;
		int c = thisComponent->lastLo->hiEnd;
		int d = thisComponent->nextLo->hiEnd;
		numComponents++;
	}
}