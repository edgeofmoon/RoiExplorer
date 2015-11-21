#pragma once

#include "MyArrayMD.h"
#include "MyMap.h"
#include "MyString.h"
#include "MySegmentNode.h"
#include "MySegNodeInfoAssemble.h"

class MyDataReader
{
public:
	MyDataReader();
	~MyDataReader();

	static My3dArrayfSPtr LoadVolumeFromFile(const char* fn);

	static MyMapSPtr<int, MyString> LoadRegionLabel(const char* fn);

	static MySegNodeInfoAssembleSPtr ConstructAssembleFromDirectory(
		const char* folderStr, const MyArray<MySegmentNodeSPtr>* ROIs);
};

