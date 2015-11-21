#pragma once

#include "MyVoxContainer.h"
#include "MyArrayMD.h"

class MyVolumnSimilarity
{
public:
	MyVolumnSimilarity();
	~MyVolumnSimilarity();
	
	static float ComputeSimilarity(const MyVoxContainerf* vol1, 
		const MyVoxContainerf* vol2, int method = 0);

	static float ComputeSimilarity(const My3dArrayf* vol1,
		const My3dArrayf* vol2, int method = 0);

	static float ComputeSimilarity(const MyArrayf* array1,
		const MyArrayf* array2, int method = 0);
};

