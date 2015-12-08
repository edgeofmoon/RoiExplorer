#pragma once

#include "MySharedPointer.h"
#include "MyArray.h"

class MyNode;

typedef MySharedPointer<MyNode> MyNodeSPtr;
typedef MySharedPointer<const MyNode> MyNodeScPtr;

class MyNode
{
public:
	MyNode();
	virtual ~MyNode();

	enum Direction{
		Direction_In = 1,
		Direction_Out = 2
	};

	MyNodeSPtr GetInNode(int index = 0);
	MyNodeScPtr GetInNode(int index = 0) const;
	int GetNumInNode() const;

	MyNodeSPtr GetOutNode(int index = 0);
	MyNodeScPtr GetOutNode(int index = 0) const;
	int GetNumOutNode() const;

	static int Connect(MyNodeSPtr fromNode, MyNodeSPtr toNode);
	static int Disconnect(MyNodeSPtr fromNode, MyNodeSPtr toNode);
	static int IsConnected(const MyNode* fromNode, const MyNode* toNode);

	static MyArraySPtr<const MyNode*> FindPathInSubTree(
		const MyNode* root, const MyNode* target, Direction dir = Direction_Out);

protected:
	// will not change in/out node
	virtual int AddOutNode(MyNodeSPtr outNode);
	virtual int AddInNode(MyNodeSPtr inNode);
	virtual int RemoveOutNode(MyNodeSPtr outNode);
	virtual int RemoveInNode(MyNodeSPtr inNode);

	MyArray<MyNodeSPtr> mInNodes;
	MyArray<MyNodeSPtr> mOutNodes;

	/********* Iterator Definition *********/
public:

	class TreeIterator;
	class const_TreeIterator;

	class TreeIterator
		: public std::iterator < std::forward_iterator_tag, MyNode* >{
	public:
		friend const_TreeIterator;
		TreeIterator(MyNode* root, Direction dir = Direction_Out)
		:mCurrent(root), mDirection(dir) {
		};
		// Operators : misc
		inline MyNode* operator*() {
			return mCurrent;
		}
		inline const MyNode* operator*() const {
			return mCurrent;
		}

	public:
		// pre-increment
		inline virtual TreeIterator& operator++();

		// post-increment
		inline virtual TreeIterator operator++(int);

		// Operators : comparison
	public:
		inline bool operator==(const TreeIterator& rhs) const {
			return mCurrent == rhs.mCurrent && mDirection == rhs.mDirection;;
		}
		inline bool operator==(const const_TreeIterator& rhs) const {
			return mCurrent == rhs.mCurrent && mDirection == rhs.mDirection;;
		}
		inline bool operator!=(const TreeIterator& rhs) const {
			return mCurrent != rhs.mCurrent || mDirection != rhs.mDirection;;
		}
		inline bool operator!=(const const_TreeIterator& rhs) const {
			return mCurrent != rhs.mCurrent || mDirection != rhs.mDirection;;
		}
	protected:
		Direction mDirection;
		MyNode* mCurrent;
		MyArray<MyNode*> mStack;

		int GetNumNeighbors() const;
		MyNode* GetNeighbor(int index) const;
	};

	class const_TreeIterator
		: public std::iterator < std::forward_iterator_tag, const MyNode* >{
	public:
		friend TreeIterator;

		const_TreeIterator(const MyNode* root, Direction dir)
			:mCurrent(root), mDirection(dir = Direction_Out) {
		};
		const_TreeIterator(const TreeIterator& itr)
			:mCurrent(itr.mCurrent), mDirection(itr.mDirection){
			mStack.clear();
			for (int i = 0; i < itr.mStack.size(); i++){
				mStack << itr.mStack[i];
			}
		};
		// Operators : misc
		inline const MyNode* operator*() const {
			return mCurrent;
		}

	public:
		// pre-increment
		inline virtual const_TreeIterator& operator++();

		// post-increment
		inline virtual const_TreeIterator operator++(int);

		// Operators : comparison
	public:
		inline bool operator==(const const_TreeIterator& rhs) const {
			return mCurrent == rhs.mCurrent && mDirection == rhs.mDirection;
		}
		inline bool operator==(const TreeIterator& rhs) const {
			return mCurrent == rhs.mCurrent && mDirection == rhs.mDirection;
		}
		inline bool operator!=(const const_TreeIterator& rhs) const {
			return mCurrent != rhs.mCurrent || mDirection != rhs.mDirection;
		}
		inline bool operator!=(const TreeIterator& rhs) const {
			return mCurrent != rhs.mCurrent || mDirection != rhs.mDirection;;
		}
	protected:
		Direction mDirection;
		const MyNode* mCurrent;
		MyArray<const MyNode*> mStack;

		int GetNumNeighbors() const;
		const MyNode* GetNeighbor(int index) const;
	};

	TreeIterator Begin(Direction dir = Direction_Out);
	TreeIterator End(Direction dir = Direction_Out);
	const_TreeIterator Begin(Direction dir = Direction_Out) const;
	const_TreeIterator End(Direction dir = Direction_Out) const;

};
