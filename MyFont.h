#pragma once
#include "MyBox.h"
#include "MyString.h"
#include "MyMap.h"
#include "MySharedPointer.h"

class MyFont
{
public:
	MyFont();
	~MyFont();

	void Render(const MyString& str, const MyBox2f& box, float zDepth = 0) const;
	void RenderFill(const MyString& str, const MyBox2f& box, float zDepth = 0) const;
	void Load(const char* filename, int height);

	MyVec2i ComputeSizeInPixel(const MyString& str) const;

protected:
	int mNumberCharacters;
	float mFontHeight;						//< Holds the height of the font.
	unsigned int *mTextures;				///< Holds the texture id's 
	unsigned int mDisplyListBase;			///< Holds the first display list id
	MyMap<int, int> mCharWidth;
};

typedef MySharedPointer<MyFont> MyFontSPtr;
typedef MySharedPointer<const MyFont> MyFontScPtr;