#pragma once

#include "MyComponent.h"
#include "MyArrayMD.h"
#include "MySharedPointer.h"

class MyComponentFilter
{
public:
	MyComponentFilter();
	~MyComponentFilter();

	void SetComponents(MyComponent* root, const MyArrayMD<MyComponent*, 3>* components){
		mRoot = root; 
		mComponents = components;
	}
	void SetSizeThreshold(int thres){ mSizeThreshold = thres; };
	void Update();

protected:
	int mSizeThreshold;
	MyComponent* mRoot;
	const MyArrayMD<MyComponent*, 3>* mComponents;

	bool IsLeaveComponent(const MyComponent* comp) const;
	bool IsComponentToBePruned(const MyComponent* comp) const;
	void PruneComponentFromUp(MyComponent* comp);
	bool IsComponentToBeMerged(const MyComponent* comp) const;
	void MergeComponentWithUp(MyComponent* comp);

	void CheckStatus(const MyComponent* root) const;
};

typedef MySharedPointer<MyComponentFilter> MyComponentFilterSPtr;
typedef MySharedPointer<const MyComponentFilter> MyComponentFilterScPtr;