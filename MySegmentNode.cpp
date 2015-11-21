#include "MySegmentNode.h"


MySegmentNode::MySegmentNode()
{
	mTotalVoxels = 0;
}


MySegmentNode::~MySegmentNode()
{
}

MySegmentNodeSPtr MySegmentNode::GetParent(int i){
	return std::static_pointer_cast<MySegmentNode>(MyNode::GetInNode(i));
}

MySegmentNodeScPtr MySegmentNode::GetParent(int i) const{
	return std::static_pointer_cast<const MySegmentNode>(MyNode::GetInNode(i));
}

int MySegmentNode::GetNumParents() const{
	return MyNode::GetNumInNode();
}

MySegmentNodeSPtr MySegmentNode::GetChild(int i){
	return std::static_pointer_cast<MySegmentNode>(MyNode::GetOutNode(i));
}

MySegmentNodeScPtr MySegmentNode::GetChild(int i) const{
	return std::static_pointer_cast<const MySegmentNode>(MyNode::GetOutNode(i));
}

int MySegmentNode::GetNumChildren() const{
	return MyNode::GetNumOutNode();
}

MyVoxContainerfSPtr MySegmentNode::MakeAllVoxes() const{
	const_VoxelIterator voxItr = this->VoxelBegin();
	const_VoxelIterator voxEnd = this->VoxelEnd();
	MyArrayi voxIndices;
	MyArrayf voxValues;
	voxIndices.reserve(this->GetNumTotalVoxes());
	while (voxItr != voxEnd){
		voxIndices << voxItr.GetVoxelIndex();
		voxValues << *voxItr;
		voxItr++;
	}
	return MyVoxContainerf::MakeVoxContainer(&voxIndices, &voxValues, mVolumeSize);
}

void MySegmentNode::SetUniqueVoxes(MyVoxContainerfSPtr voxel){
	if (mUniqueVoxes){
		mTotalVoxels -= mUniqueVoxes->GetNumVoxes();
	}
	mUniqueVoxes = voxel;
	mTotalVoxels += mUniqueVoxes->GetNumVoxes();
	this->UpdateVolumeSize();
}

bool MySegmentNode::HasUniqueVoxel() const{
	if (mUniqueVoxes) return true;
	return false;
}

bool MySegmentNode::IsVoxelIn(const MyVec3i& pos) const{
	int index = My3dArrayf::ComputeIndex(pos, mVolumeSize);
	return this->IsVoxelIn(index);
}

bool MySegmentNode::IsVoxelIn(int index) const{
	MyNode::const_TreeIterator treeItr = this->Begin();
	MyNode::const_TreeIterator treeEnd = this->End();
	while (treeItr != treeEnd){
		if (static_cast<const MySegmentNode*>(*treeItr)->HasUniqueVoxel()){
			if (static_cast<const MySegmentNode*>(*treeItr)->GetUniqueVoxes()->IsVoxelIn(index)){
				return true;
			}
		}
	}
	return false;
}

MyVoxContainerfScPtr MySegmentNode::GetUniqueVoxes() const{
	return mUniqueVoxes;
}

MyVoxContainerfSPtr MySegmentNode::GetUniqueVoxes(){
	return mUniqueVoxes;
}

void MySegmentNode::UpdateVolumeSize(){
	MyNode::TreeIterator treeItr = this->Begin();
	while (!static_cast<const MySegmentNode*>(*treeItr)->HasUniqueVoxel()){
		treeItr++;
	}
	mVolumeSize = static_cast<const MySegmentNode*>(*treeItr)->GetUniqueVoxes()->GetVolumeSize();
}

// pre-increment
MySegmentNode::VoxelIterator& MySegmentNode::VoxelIterator::operator++(){
	if (mVoxelIterator != static_cast<MySegmentNode*>(*mTreeIterator)->GetUniqueVoxes()->End()){
		++mVoxelIterator;
	}
	else if (mTreeIterator != mRoot->End()){
		++mTreeIterator;
		mVoxelIterator = static_cast<MySegmentNode*>(*mTreeIterator)->GetUniqueVoxes()->Begin();
	}
	return *this;
}

// post-increment
MySegmentNode::VoxelIterator MySegmentNode::VoxelIterator::operator++(int){
	MySegmentNode::VoxelIterator itr(*this);
	if (mVoxelIterator != static_cast<MySegmentNode*>(*mTreeIterator)->GetUniqueVoxes()->End()){
		++mVoxelIterator;
	}
	else if (mTreeIterator != mRoot->End()){
		++mTreeIterator;
		mVoxelIterator = static_cast<MySegmentNode*>(*mTreeIterator)->GetUniqueVoxes()->Begin();
	}
	return itr;
}

// pre-increment
MySegmentNode::const_VoxelIterator& MySegmentNode::const_VoxelIterator::operator++(){
	if (mVoxelIterator != static_cast<const MySegmentNode*>(*mTreeIterator)->GetUniqueVoxes()->End()){
		++mVoxelIterator;
	}
	else if (mTreeIterator != mRoot->End()){
		++mTreeIterator;
		mVoxelIterator = static_cast<const MySegmentNode*>(*mTreeIterator)->GetUniqueVoxes()->Begin();
	}
	return *this;
}

// post-increment
MySegmentNode::const_VoxelIterator MySegmentNode::const_VoxelIterator::operator++(int){
	MySegmentNode::const_VoxelIterator itr(*this);
	if (mVoxelIterator != static_cast<const MySegmentNode*>(*mTreeIterator)->GetUniqueVoxes()->End()){
		++mVoxelIterator;
	}
	else if (mTreeIterator != mRoot->End()){
		++mTreeIterator;
		mVoxelIterator = static_cast<const MySegmentNode*>(*mTreeIterator)->GetUniqueVoxes()->Begin();
	}
	return itr;
}

MySegmentNode::VoxelIterator MySegmentNode::VoxelBegin(){
	TreeIterator treeItr = this->Begin(MyNode::Direction_In);
	while (!static_cast<MySegmentNode*>(*treeItr)->HasUniqueVoxel()){
		treeItr++;
	}
	MyVoxContainerf::Iterator voxelIterator = 
		static_cast<MySegmentNode*>(*treeItr)->GetUniqueVoxes()->Begin();
	return MySegmentNode::VoxelIterator(this, treeItr, voxelIterator);
}

MySegmentNode::VoxelIterator MySegmentNode::VoxelEnd(){
	TreeIterator treeItr = this->Begin(MyNode::Direction_In);
	TreeIterator treeItrTmp = this->Begin(MyNode::Direction_In);
	while (treeItrTmp != this->End(MyNode::Direction_In)){
		if (static_cast<const MySegmentNode*>(*treeItrTmp)->HasUniqueVoxel()){
			treeItr = treeItrTmp;
		}
		treeItrTmp++;
	}
	MyVoxContainerf::Iterator voxelIterator =
		static_cast<MySegmentNode*>(*treeItr)->GetUniqueVoxes()->End();
	return MySegmentNode::VoxelIterator(this, treeItr, voxelIterator);
}
MySegmentNode::const_VoxelIterator MySegmentNode::VoxelBegin() const{
	const_TreeIterator treeItr = this->Begin(MyNode::Direction_In);
	while (!static_cast<const MySegmentNode*>(*treeItr)->HasUniqueVoxel()){
		treeItr++;
	}
	MyVoxContainerf::const_Iterator voxelIterator =
		static_cast<const MySegmentNode*>(*treeItr)->GetUniqueVoxes()->Begin();
	return MySegmentNode::const_VoxelIterator(this, treeItr, voxelIterator);
}
MySegmentNode::const_VoxelIterator MySegmentNode::VoxelEnd() const{
	const_TreeIterator treeItr = this->Begin(MyNode::Direction_In);
	const_TreeIterator treeItrTmp = this->Begin(MyNode::Direction_In);
	while (treeItrTmp != this->End(MyNode::Direction_In)){
		if (static_cast<const MySegmentNode*>(*treeItrTmp)->HasUniqueVoxel()){
			treeItr = treeItrTmp;
		}
		treeItrTmp++;
	}
	MyVoxContainerf::const_Iterator voxelIterator =
		static_cast<const MySegmentNode*>(*treeItr)->GetUniqueVoxes()->End();
	return MySegmentNode::const_VoxelIterator(this, treeItr, voxelIterator);
}

int MySegmentNode::AddInNode(MyNodeSPtr inNode){
	if (MyNode::AddInNode(inNode) == 1){
		mTotalVoxels += static_cast<MySegmentNode*>(inNode.get())->GetNumTotalVoxes();
		return 1;
	}
	this->UpdateVolumeSize();
	return 0;
}

int MySegmentNode::RemoveInNode(MyNodeSPtr inNode){
	if (MyNode::RemoveInNode(inNode) == 1){
		mTotalVoxels -= static_cast<MySegmentNode*>(inNode.get())->GetNumTotalVoxes();
		return 1;
	}
	return 0;
}