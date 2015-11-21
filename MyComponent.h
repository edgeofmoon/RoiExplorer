#pragma once
#include "MyArray.h"

class MyComponent{
protected:
	MyComponent *UfComponent;
public:
	MyComponent *nextHi, *lastHi;
	MyComponent *nextLo, *lastLo;
	int hiEnd, loEnd;
	int seedFrom, seedTo;

	// make it shared_pointer for light weight transfer
	MyArrayiSPtr mVertices;

	MyComponent() {
		UfComponent = this;
		nextHi = nextLo = lastHi = lastLo = NULL;
		seedFrom = seedTo = hiEnd = loEnd = NULL;
		mVertices = std::make_shared<MyArrayi>();
	}

	MyComponent *Component(){
		while (UfComponent->UfComponent != UfComponent)
			UfComponent = UfComponent->UfComponent;
		return UfComponent;
	}

	int GetNumVertices() const { return mVertices->size(); };
	void MergeTo(MyComponent *newUF) { UfComponent = newUF; }
};