#pragma once

#include "MyFrameBuffer.h"
#include "MyArrayMD.h"

class MyCanvas
{
public:
	MyCanvas();
	~MyCanvas();

	// clear buffers
	void Clear();

	// bind the canvas
	void On();

	// load the nameMap
	void Off();

	// return the name of the pixel
	MyVec4i GetName(const MyVec2i& pos) const;

	// control
	void Resize(int w, int h);
	void Show(MyVec4i viewport);


protected:
	MyFrameBuffer mFrameBuffer;

	// nameMap
	MyArrayMDSPtr<MyVec4i, 2> mNameMap;
};

