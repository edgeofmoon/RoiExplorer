#pragma once

#include "MySharedPointer.h"
#include "MyVoxContainer.h"
#include "MyNode.h"

class MySegmentNode;
typedef MySharedPointer<MySegmentNode> MySegmentNodeSPtr;
typedef MySharedPointer<const MySegmentNode> MySegmentNodeScPtr;

class MySegmentNode
	:public MyNode
{
public:
	MySegmentNode();
	virtual ~MySegmentNode();

	MySegmentNodeSPtr GetParent(int i = 0);
	MySegmentNodeScPtr GetParent(int i = 0) const;
	int GetNumParents() const;

	MySegmentNodeSPtr GetChild(int i = 0);
	MySegmentNodeScPtr GetChild(int i = 0) const;
	int GetNumChildren() const;

	MyVoxContainerfSPtr MakeAllVoxes() const;
	void SetUniqueVoxes(MyVoxContainerfSPtr voxel) ;
	bool HasUniqueVoxel() const;

	bool IsVoxelIn(const MyVec3i& pos) const;
	bool IsVoxelIn(int index) const;

	MyVoxContainerfScPtr GetUniqueVoxes() const;
	MyVoxContainerfSPtr GetUniqueVoxes();
	MyVec3i GetVolumeSize() const{
		return mVolumeSize;
	};
	int GetNumTotalVoxes() const{
		return mTotalVoxels;
	};
	void SetIndex(int index);
	void SetAutoIndex();
	int GetIndex() const{ return mIndex; };

protected:
	MyVoxContainerfSPtr mUniqueVoxes;
	int mIndex;

	static int MaxIndex;

	int mTotalVoxels;
	MyVec3i mVolumeSize;
	// will not change in/out node
	virtual int AddInNode(MyNodeSPtr inNode);
	virtual int RemoveInNode(MyNodeSPtr inNode);
	void UpdateVolumeSize();

	/**************** Iterator Definition *******************/
public:
	class VoxelIterator;
	class const_VoxelIterator;
	class VoxelIterator
		: public std::iterator < std::forward_iterator_tag, float >{
	public:
		friend const_VoxelIterator;
		VoxelIterator(MySegmentNode* root,
			MyNode::TreeIterator treeItr,
			MyVoxContainerf::Iterator voxelItr)
			:mRoot(root),
			mTreeIterator(treeItr),
			mVoxelIterator(voxelItr) {};
		// Operators : misc
		inline float& operator*() {
			return *mVoxelIterator;
		}
		inline const float& operator*() const {
			return *mVoxelIterator;
		}

		inline int GetVoxelIndex() const {
			return mVoxelIterator.GetVoxelIndex();
		}

	public:
		// pre-increment
		inline virtual VoxelIterator& operator++();

		// post-increment
		inline virtual VoxelIterator operator++(int);

		// Operators : comparison
	public:
		inline bool operator==(const VoxelIterator& rhs) const {
			return mTreeIterator == rhs.mTreeIterator 
				&& mVoxelIterator == rhs.mVoxelIterator;
		}
		inline bool operator!=(const VoxelIterator& rhs) const {
			return mTreeIterator != rhs.mTreeIterator 
				|| mVoxelIterator != rhs.mVoxelIterator;
		}
	protected:
		MyNode::TreeIterator mTreeIterator;
		MyVoxContainerf::Iterator mVoxelIterator;
		MySegmentNode* mRoot;
	};

	class const_VoxelIterator
		: public std::iterator < std::forward_iterator_tag, const float >{
	public:
		const_VoxelIterator(const MySegmentNode* root,
			MyNode::const_TreeIterator treeItr,
			MyVoxContainerf::const_Iterator voxelItr)
			:mRoot(root),
			mTreeIterator(treeItr),
			mVoxelIterator(voxelItr) {};
		const_VoxelIterator(const VoxelIterator& itr)
			:mRoot(itr.mRoot),
			mTreeIterator(itr.mTreeIterator),
			mVoxelIterator(itr.mVoxelIterator) {};
		// Operators : misc
		inline const float& operator*() const {
			return *mVoxelIterator;
		}

		inline int GetVoxelIndex() const {
			return mVoxelIterator.GetVoxelIndex();
		}

	public:
		// pre-increment
		inline virtual const_VoxelIterator& operator++();

		// post-increment
		inline virtual const_VoxelIterator operator++(int);

		// Operators : comparison
	public:
		inline bool operator==(const const_VoxelIterator& rhs) const {
			return mTreeIterator == rhs.mTreeIterator 
				&& mVoxelIterator == rhs.mVoxelIterator;
		}
		inline bool operator!=(const const_VoxelIterator& rhs) const {
			return mTreeIterator != rhs.mTreeIterator
				|| mVoxelIterator != rhs.mVoxelIterator;
		}
	protected:
		MyNode::const_TreeIterator mTreeIterator;
		MyVoxContainerf::const_Iterator mVoxelIterator;
		const MySegmentNode* mRoot;
	};

public:
	VoxelIterator VoxelBegin(MyNode::Direction dir = MyNode::Direction_Out);
	VoxelIterator VoxelEnd(MyNode::Direction dir = MyNode::Direction_Out);
	const_VoxelIterator VoxelBegin(MyNode::Direction dir = MyNode::Direction_Out) const;
	const_VoxelIterator VoxelEnd(MyNode::Direction dir = MyNode::Direction_Out) const;

};

