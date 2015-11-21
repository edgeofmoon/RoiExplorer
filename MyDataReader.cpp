#include "MyDataReader.h"
#include "MySegNodeInfoAssemble.h"
#include "OSCB.h"
#include "RicVolume.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

MyDataReader::MyDataReader()
{
}


MyDataReader::~MyDataReader()
{
}

My3dArrayfSPtr MyDataReader::LoadVolumeFromFile(const char* fn){
	RicVolume vol;
	if (vol.Read(fn) == 0){
		cerr << "Error: " << fn << "Cannot be loaded.\n";
	}
	My3dArrayf* dataPtr = new My3dArrayf(MyVec3i(vol.get_numx(), vol.get_numy(), vol.get_numz()));
	My3dArrayf& data = *dataPtr;
	for (int x = 0; x < vol.get_numx(); x++){
		for (int y = 0; y < vol.get_numy(); y++){
			for (int z = 0; z < vol.get_numz(); z++){
				data[MyVec3i(x, y, z)] = vol.vox[x][y][z];
			}
		}
	}
	return My3dArrayfSPtr(dataPtr);
}

MyMapSPtr<int, MyString> MyDataReader::LoadRegionLabel(const char* fn){
	MyMapSPtr<int, MyString> indexToLabel = std::make_shared<MyMap<int, MyString>>();
	ifstream infile;
	infile.open(fn);
	if (infile.is_open()){
		while (!infile.eof()){
			char line[1000];
			infile.getline(line, 1000);
			stringstream ss(line);
			int index;
			MyString label;
			ss >> index >> label;
			if (!label.empty()){
				indexToLabel->operator[](index) = label;
			}
		}
	}
	for (int i = 0; i < 51; i++){
		if (!indexToLabel->HasKey(i)) {
			indexToLabel->operator[](i) = "R" + to_string(i);
		}
	}
	return indexToLabel;
}

MySegNodeInfoAssembleSPtr MyDataReader::ConstructAssembleFromDirectory(const char* folderStr,
	const MyArray<MySegmentNodeSPtr>* ROIs){
	MySegNodeInfoAssembleSPtr segAsmb = std::make_shared<MySegNodeInfoAssemble>();
	MyArraySPtr<My3dArrayfScPtr> vols = std::make_shared<MyArray<My3dArrayfScPtr>>();
	vector<string> skeletonfiles = get_all_files_names_within_folder(folderStr);
	for (int i = 0; i < skeletonfiles.size(); i++){
		My3dArrayfSPtr ske = MyDataReader::LoadVolumeFromFile((folderStr + skeletonfiles[i]).c_str());
		vols->PushBack(ske);
	}
	//ofstream statFile(folderStr + string("stats.txt"));
	//statFile << vols->size() << endl;
	for (int i = 0; i < ROIs->size(); i++){
		MySegmentNodeInfoSPtr segInfo = std::make_shared<MySegmentNodeInfo>();
		segInfo->SetSegmentNode(ROIs->at(i));
		segInfo->SetVolumes(vols);
		segInfo->Update();
		//statFile << segInfo->GetSegmentNodeMeanAverage() << ' '
		//	<< segInfo->GetSegmentNodeMeanStdev() << ' '
		//	<< segInfo->GetSegmentNodeMeanRange()[0] << ' '
		//	<< segInfo->GetSegmentNodeMeanRange()[1] << endl;
		segAsmb->AddSegmentNodeInfo(segInfo);
	}
	//statFile.close();
	segAsmb->Update();
	return segAsmb;
}