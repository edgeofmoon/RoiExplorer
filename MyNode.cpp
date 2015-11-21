#include "MyNode.h"
#include <list>
using namespace std;

MyNode::MyNode()
{
}


MyNode::~MyNode()
{
}

MyNodeSPtr MyNode::GetInNode(int index){
	return mInNodes[index];
}

MyNodeScPtr MyNode::GetInNode(int index) const{
	return mInNodes[index];
}

int MyNode::GetNumInNode() const{
	return mInNodes.size();
}


MyNodeSPtr MyNode::GetOutNode(int index){
	return mOutNodes[index];
}

MyNodeScPtr MyNode::GetOutNode(int index) const{
	return mOutNodes[index];
}

int MyNode::GetNumOutNode() const{
	return mOutNodes.size();
}

int MyNode::Connect(MyNodeSPtr fromNode, MyNodeSPtr toNode){
	int rst = fromNode->AddOutNode(toNode);
	rst += toNode->AddInNode(fromNode);
	return rst;
}

int MyNode::Disconnect(MyNodeSPtr fromNode, MyNodeSPtr toNode){
	int rst = fromNode->RemoveOutNode(toNode);
	rst += toNode->RemoveInNode(fromNode);
	return rst;
}

int MyNode::IsConnected(const MyNode* fromNode, const MyNode* toNode){
	bool isConnected = false;
	for (int i = 0; i < fromNode->GetNumOutNode(); i++){
		if (fromNode->GetOutNode(i).get() == toNode){
			isConnected = true;
		}
	}
	if (!isConnected) return false;
	for (int i = 0; i < toNode->GetNumInNode(); i++){
		if (toNode->GetInNode(i).get() == fromNode){
			return true;
		}
	}
	return false;
}

MyArraySPtr<const MyNode*> MyNode::FindPathInSubTree(
	const MyNode* root, const MyNode* target, MyNode::Direction dir){
	MyArraySPtr<const MyNode*> path = make_shared<MyArray<const MyNode*>>();
	list<const MyNode*> nodes_to_visit;
	list<const MyNode*> nodes_to_visit_parents;
	nodes_to_visit.push_front(root);
	nodes_to_visit_parents.push_front(0);
	while (!nodes_to_visit.empty()){
		const MyNode* thisNode = nodes_to_visit.front();
		nodes_to_visit.pop_front();
		nodes_to_visit_parents.pop_front();
		path->push_back(thisNode);

		if (dir == MyNode::Direction_Out){
			for (int i = 0; i < thisNode->GetNumOutNode(); i++){
				nodes_to_visit.push_front(thisNode->GetOutNode(i).get());
				nodes_to_visit_parents.push_front(thisNode);
			}
		}
		else{
			for (int i = 0; i < thisNode->GetNumInNode(); i++){
				nodes_to_visit.push_front(thisNode->GetInNode(i).get());
				nodes_to_visit_parents.push_front(thisNode);
			}
		}

		// check find
		if (thisNode == target){
			return path;
		}

		// path retract
		while (path->back() != nodes_to_visit_parents.front()){
			path->pop_back();
		}
	}
	// find nothing, return nothing
	return make_shared<MyArray<const MyNode*>>();
}

int MyNode::AddOutNode(MyNodeSPtr outNode){
	if (this->mOutNodes.IndexOf(outNode) >= 0){
		return 0;
	}
	mOutNodes << outNode;
	return 1;
}

int MyNode::AddInNode(MyNodeSPtr inNode){
	if (this->mInNodes.IndexOf(inNode) >= 0){
		return 0;
	}
	mInNodes << inNode;
	return 1;
}

int MyNode::RemoveOutNode(MyNodeSPtr outNode){
	int outNodeIndex = this->mOutNodes.IndexOf(outNode);
	if (outNodeIndex < 0){
		return 0;
	}
	this->mOutNodes.EraseAt(outNodeIndex);
	return 1;
}

int MyNode::RemoveInNode(MyNodeSPtr inNode){
	int inNodeIndex = this->mInNodes.IndexOf(inNode);
	if (inNodeIndex < 0){
		return 0;
	}
	this->mInNodes.EraseAt(inNodeIndex);
	return 1;
}

/********* Iterator Definition *********/

int MyNode::TreeIterator::GetNumNeighbors() const{
	if (mDirection == Direction_In){
		return mCurrent->GetNumInNode();
	}
	else{
		return mCurrent->GetNumOutNode();
	}
}

MyNode* MyNode::TreeIterator::GetNeighbor(int index) const{
	if (mDirection == Direction_In){
		return mCurrent->GetInNode(index).get();
	}
	else{
		return mCurrent->GetOutNode(index).get();
	}
}

// pre-increment
MyNode::TreeIterator& MyNode::TreeIterator::operator++(){
	// depth first search
	// just for reference
	/*
	MyNodeSPtr root = std::make_shared<MyNode>();
	MyArray<MyNodeSPtr> stack;
	stack << root;
	while (!stack.empty()){
	MyNodeSPtr current = stack.back();
	stack.pop_back();

	// do something

	for (int i = 0; i < current->GetNumInNode(); i++){
	stack << current->GetInNode(i);
	}
	}
	*/

	for (int i = 0; i < mCurrent->GetNumInNode(); i++){
		mStack << mCurrent->GetInNode(i).get();
	}
	if (mStack.empty()){
		mCurrent = 0;
	}
	else {
		mCurrent = mStack.back();
		mStack.pop_back();
	}
	return *this;
}

// pre-increment
MyNode::TreeIterator MyNode::TreeIterator::operator++(int){
	TreeIterator itr(*this);
	for (int i = 0; i < this->GetNumNeighbors(); i++){
		mStack << this->GetNeighbor(i);
	}
	if (mStack.empty()){
		mCurrent = 0;
	}
	else {
		mCurrent = mStack.back();
		mStack.pop_back();
	}
	return itr;
}

int MyNode::const_TreeIterator::GetNumNeighbors() const{
	if (mDirection == Direction_In){
		return mCurrent->GetNumInNode();
	}
	else{
		return mCurrent->GetNumOutNode();
	}
}

const MyNode* MyNode::const_TreeIterator::GetNeighbor(int index) const{
	if (mDirection == Direction_In){
		return mCurrent->GetInNode(index).get();
	}
	else{
		return mCurrent->GetOutNode(index).get();
	}
}

// pre-increment
MyNode::const_TreeIterator& MyNode::const_TreeIterator::operator++(){
	for (int i = 0; i < this->GetNumNeighbors(); i++){
		mStack << this->GetNeighbor(i);
	}
	if (mStack.empty()){
		mCurrent = 0;
	}
	else {
		mCurrent = mStack.back();
		mStack.pop_back();
	}
	return *this;
}

// pre-increment
MyNode::const_TreeIterator MyNode::const_TreeIterator::operator++(int){
	const_TreeIterator itr(*this);
	for (int i = 0; i < this->GetNumNeighbors(); i++){
		mStack << this->GetNeighbor(i);
	}
	if (mStack.empty()){
		mCurrent = 0;
	}
	else {
		mCurrent = mStack.back();
		mStack.pop_back();
	}
	return itr;
}

MyNode::TreeIterator MyNode::Begin(Direction dir){
	return TreeIterator(this, dir);
}

MyNode::TreeIterator MyNode::End(Direction dir){
	return TreeIterator(0, dir);
}

MyNode::const_TreeIterator MyNode::Begin(Direction dir) const{
	return const_TreeIterator(this, dir);
}

MyNode::const_TreeIterator MyNode::End(Direction dir) const{
	return const_TreeIterator(0, dir);
}