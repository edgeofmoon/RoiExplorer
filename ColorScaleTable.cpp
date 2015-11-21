#include "ColorScaleTable.h"

unsigned char ColorScaleTable::colorBrewer_sequential_8_singlehue_3[8][3] = {
	{ 0xff, 0xff, 0xff },
	{ 0xf0, 0xf0, 0xf0 },
	{ 0xd9, 0xd9, 0xd9 },
	{ 0xbd, 0xbd, 0xbd },
	{ 0x96, 0x96, 0x96 },
	{ 0x73, 0x73, 0x73 },
	{ 0x52, 0x52, 0x52 },
	{ 0x25, 0x25, 0x25 },
};

/*
unsigned char colorBrewer_sequential_8_multihue_9[8][3] = {
	{ 0xff, 0xff, 0xe5 },
	{ 0xf7, 0xfc, 0xb9 },
	{ 0xd9, 0xf0, 0xa3 },
	{ 0xad, 0xdd, 0x8e },
	{ 0x78, 0xc6, 0x79 },
	{ 0x41, 0xab, 0x5d },
	{ 0x23, 0x84, 0x43 },
	{ 0x00, 0x5a, 0x32 },
};
*/

// this is actually faked from sequential_9_multihue
unsigned char ColorScaleTable::colorBrewer_sequential_8_multihue_9[8][3] = {
//	{ 0xf7, 0xfc, 0xf5 },
	{ 0xe5, 0xf5, 0xe0 },
	{ 0xc7, 0xe9, 0xc0 },
	{ 0xa1, 0xd9, 0x9b },
	{ 0x74, 0xc4, 0x76 },
	{ 0x41, 0xab, 0x5d },
	{ 0x23, 0x8b, 0x45 },
	{ 0x00, 0x6d, 0x2c },
	{ 0x00, 0x44, 0x1b },
};

// source: http://geog.uoregon.edu/datagraphics/color/StepSeq_25.txt
unsigned char ColorScaleTable::ColorTable[25][3] = {
	{ 153, 15, 15 },
	{ 178, 44, 44 },
	{ 204, 81, 81 },
	{ 229, 126, 126 },
	{ 255, 178, 178 },
	{ 153, 84, 15 },
	{ 178, 111, 44 },
	{ 204, 142, 81 },
	{ 229, 177, 126 },
	{ 255, 216, 178 },
	{ 107, 153, 15 },
	{ 133, 178, 44 },
	{ 163, 204, 81 },
	{ 195, 229, 126 },
	{ 229, 255, 178 },
	{ 15, 107, 153 },
	{ 44, 133, 178 },
	{ 81, 163, 204 },
	{ 126, 195, 229 },
	{ 178, 229, 255 },
	{ 38, 15, 153 },
	{ 66, 44, 178 },
	{ 101, 81, 204 },
	{ 143, 126, 229 },
	{ 191, 178, 255 }
};

void ColorScaleTable::DiffValueToColor(float diff, float minDiff, float maxDiff, float color_rgba[4]){
	if (diff <= 0){
		color_rgba[2] = 1;
		color_rgba[1] = 1 - diff / minDiff;
		color_rgba[0] = 1 - diff / minDiff;
	}
	else{
		color_rgba[0] = 1;
		color_rgba[1] = 1 - diff / maxDiff;
		color_rgba[2] = 1 - diff / maxDiff;
	}
	color_rgba[3] = 1;
}

void ColorScaleTable::SequentialColor(float value, float minValue, float maxValue, float color_rgba[4]){
	float c = (value - minValue) / (maxValue - minValue);
	/*
	color_rgba[0] = c;
	color_rgba[1] = c;
	color_rgba[2] = c;
	color_rgba[3] = 1;
	*/
	int idx = c * 24 + 0.5;
	color_rgba[0] = ColorTable[idx][0] / (float)255;
	color_rgba[1] = ColorTable[idx][1] / (float)255;
	color_rgba[2] = ColorTable[idx][2] / (float)255;
	color_rgba[3] = 1;
}

void ColorScaleTable::CategoricalColor(float value, float minValue, float maxValue, float color_rgba[4]){
	float c = (value - minValue) / (maxValue - minValue);
	int allByte = c * (int)0xffffff;
	char color[3] = { 0, 0, 0 };
	for (int i = 0; i < 8; i++){
		char byte = 1 << i;
		color[0] |= (allByte &(1 << (i * 3 + 0))) == 0 ? 0 : byte;
		color[1] |= (allByte &(1 << (i * 3 + 1))) == 0 ? 0 : byte;
		color[2] |= (allByte &(1 << (i * 3 + 2))) == 0 ? 0 : byte;
	}
	color_rgba[0] = color[0] / (float)255;
	color_rgba[1] = color[1] / (float)255;
	color_rgba[2] = color[2] / (float)255;
	color_rgba[3] = 1;
}
