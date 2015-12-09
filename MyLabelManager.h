#pragma once

#include "MyMap.h"
#include "MyString.h"
#include "MyBox.h"
#include "MyVec.h"
#include "MyFont.h"
#include "MySharedPointer.h"
#include "MySegmentNodeInfo.h"

class MyLabelManager
{
public:
	MyLabelManager();
	~MyLabelManager();

	void SetWindowSize(int w, int h);
	void SetLabels(MyMapScPtr<int, MyString> labels);
	void SetBoxes(MyMapScPtr<const MySegmentNode*, MyBox2f> boxes);
	void SetFont(MyFontScPtr font);

	MyMapScPtr<int, MyString> GetLabels() const { return mLabels; };
	MyFontScPtr GetFont() const{ return mFont; };
	void Update();

	void Render();

protected:
	// input info
	MyMapScPtr<int, MyString> mLabels;
	MyMapScPtr<const MySegmentNode*, MyBox2f> mBoxes;
	int mWidth, mHeight;
	MyFontScPtr mFont;

	// computed position
	MyMapSPtr<const MySegmentNode*, MyBox2f> mLabelBoxes;
};

typedef MySharedPointer<MyLabelManager> MyLabelManagerSPtr;
typedef MySharedPointer<const MyLabelManager> MyLabelManagerScPtr;