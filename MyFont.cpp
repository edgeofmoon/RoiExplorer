#include "MyFont.h"

#include "MyGLHeader.h"

//FreeType Headers
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <algorithm>
// debug
#include <iostream>
using namespace std;

namespace MyFont_Helper{
	///This function gets the first power of 2 >= the
	///int that we pass it.
	inline int next_p2(int a)
	{
		int rval = 1;
		// rval<<=1 is a prettier way of writing rval*=2; 
		while (rval<a) rval <<= 1;
		return rval;
	}

	int BuildDisplayList(FT_Face face, char ch, unsigned int list_base, unsigned int *tex_base){

		//The first thing we do is get FreeType to render our character
		//into a bitmap.  This actually requires a couple of FreeType commands:

		//Load the Glyph for our character.
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
			throw std::runtime_error("FT_Load_Glyph failed");

		//Move the face's glyph into a Glyph object.
		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph))
			throw std::runtime_error("FT_Get_Glyph failed");

		//Convert the glyph to a bitmap.
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

		//This reference will make accessing the bitmap easier
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;

		//Use our helper function to get the widths of
		//the bitmap data that we will need in order to create
		//our texture.
		int width = next_p2(bitmap.width);
		int height = next_p2(bitmap.rows);

		//Allocate memory for the texture data.
		GLubyte* expanded_data = new GLubyte[2 * width * height];

		//Here we fill in the data for the expanded bitmap.
		//Notice that we are using a two channel bitmap (one for
		//channel luminosity and one for alpha), but we assign
		//both luminosity and alpha to the value that we
		//find in the FreeType bitmap. 
		//We use the ?: operator to say that value which we use
		//will be 0 if we are in the padding zone, and whatever
		//is the the Freetype bitmap otherwise.
		for (int j = 0; j <height; j++) {
			for (int i = 0; i < width; i++){
				expanded_data[2 * (i + j*width)] = expanded_data[2 * (i + j*width) + 1] =
					(i >= bitmap.width || j >= bitmap.rows) ?
					0 : bitmap.buffer[i + bitmap.width*j];
			}
		}

		//Now we just setup some texture parameters.
		glBindTexture(GL_TEXTURE_2D, tex_base[ch]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//Here we actually create the texture itself, notice
		//that we are using GL_LUMINANCE_ALPHA to indicate that
		//we are using 2 channel data.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
			0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);

		//With the texture created, we don't need the expanded data anymore
		delete[] expanded_data;

		//Now we create the display list
		glNewList(list_base + ch, GL_COMPILE);

		glBindTexture(GL_TEXTURE_2D, tex_base[ch]);

		//first we need to move over a little so that
		//the character has the right amount of space
		//between it and the one before it.
		glPushMatrix();
		glTranslatef(bitmap_glyph->left, 0, 0);

		//Now we move down a little in the case that the
		//bitmap extends past the bottom of the line 
		//(this is only true for characters like 'g' or 'y'.
		glTranslatef(0, bitmap_glyph->top - (int)bitmap.rows, 0);

		//Now we need to account for the fact that many of
		//our textures are filled with empty padding space.
		//We figure what portion of the texture is used by 
		//the actual character and store that information in 
		//the x and y variables, then when we draw the
		//quad, we will only reference the parts of the texture
		//that we contain the character itself.
		float   x = (float)bitmap.width / (float)width,
			y = (float)bitmap.rows / (float)height;

		//Here we draw the texturemaped quads.
		//The bitmap that we got from FreeType was not 
		//oriented quite like we would like it to be,
		//but we link the texture to the quad
		//in such a way that the result will be properly aligned.
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex2f(0, bitmap.rows);
		glTexCoord2d(0, y); glVertex2f(0, 0);
		glTexCoord2d(x, y); glVertex2f(bitmap.width, 0);
		glTexCoord2d(x, 0); glVertex2f(bitmap.width, bitmap.rows);
		glEnd();
		glPopMatrix();
		glTranslatef(face->glyph->advance.x >> 6, 0, 0);
		//glTranslatef(bitmap.width, 0, 0);


		//increment the raster position as if we were a bitmap font.
		//(only needed if you want to calculate text length)
		//glBitmap(0,0,0,0,face->glyph->advance.x >> 6,0,NULL);

		//Finnish the display list
		glEndList();

		// return width of the character
		return face->glyph->advance.x >> 6;
		//return bitmap.width;
	}
};

MyFont::MyFont()
{
	mNumberCharacters = 0;
}


MyFont::~MyFont()
{
	if (mNumberCharacters > 0){
		glDeleteLists(mDisplyListBase, mNumberCharacters);
		glDeleteTextures(mNumberCharacters, mTextures);
	}
}

void MyFont::Render(const MyString& str, const MyBox2f& box, float zDepth) const{
	MyVec2f strSize = this->ComputeSizeInPixel(str).toType<float>();
	float widthScale = box.GetSize(0) / strSize[0];
	float heightScale = box.GetSize(1) / strSize[1];
	float scale = std::min(widthScale, heightScale);

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glListBase(mDisplyListBase);
	glPushMatrix();
	glTranslatef(box.GetLowPos()[0], box.GetLowPos()[1], zDepth);
	glScalef(scale, scale, 1);
	glCallLists(str.length(), GL_UNSIGNED_BYTE, str.c_str());
	glPopMatrix();
	glPopAttrib();
}

void MyFont::RenderFill(const MyString& str, const MyBox2f& box, float zDepth) const{
	MyVec2f strSize = this->ComputeSizeInPixel(str).toType<float>();
	float widthScale = box.GetSize(0) / strSize[0];
	float heightScale = box.GetSize(1) / strSize[1];

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glListBase(mDisplyListBase);
	glPushMatrix();
	glTranslatef(box.GetLowPos()[0], box.GetLowPos()[1], zDepth);
	glScalef(widthScale, heightScale, 1);
	glCallLists(str.length(), GL_UNSIGNED_BYTE, str.c_str());
	glPopMatrix();
	glPopAttrib();
}

void MyFont::Load(const char* filename, int height){
	//Allocate some memory to store the texture ids.
	mNumberCharacters = 128;
	mCharWidth.clear();
	mTextures = new unsigned int[mNumberCharacters];

	this->mFontHeight = height;

	//Create and initilize a freetype font library.
	FT_Library library;
	if (FT_Init_FreeType(&library))
		cout << ("FT_Init_FreeType failed");

	//The object in which Freetype holds information on a given
	//font is called a "face".
	FT_Face face;

	//This is where we load in the font information from the file.
	//Of all the places where the code might die, this is the most likely,
	//as FT_New_Face will fail if the font file does not exist or is somehow broken.
	if (FT_New_Face(library, filename, 0, &face))
		cout << ("FT_New_Face failed (there is probably a problem with your font file)");

	//For some twisted reason, Freetype measures font size
	//in terms of 1/64ths of pixels.  Thus, to make a font
	//h pixels high, we need to request a size of h*64.
	//(h << 6 is just a prettier way of writing h*64)
	FT_Set_Char_Size(face, height << 6, height << 6, 96, 96);

	//Here we ask opengl to allocate resources for
	//all the textures and displays lists which we
	//are about to create.  
	mDisplyListBase = glGenLists(mNumberCharacters);
	glGenTextures(mNumberCharacters, mTextures);

	//This is where we actually create each of the fonts display lists.
	for (unsigned char i = 0; i<mNumberCharacters; i++)
		mCharWidth[i] = MyFont_Helper::BuildDisplayList(face, i, mDisplyListBase, mTextures);

	//We don't need the face information now that the display
	//lists have been created, so we free the assosiated resources.
	FT_Done_Face(face);

	//Ditto for the font library.
	FT_Done_FreeType(library);
}

MyVec2i MyFont::ComputeSizeInPixel(const MyString& str) const{
	int totalWidth = 0;
	for (int i = 0; i < str.length(); i++){
		totalWidth += mCharWidth.at(str[i]);
	}
	return MyVec2i(totalWidth, mFontHeight);
}