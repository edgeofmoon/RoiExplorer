#pragma once

#include <Windows.h>
#include <Commdlg.h>
#include <tchar.h>
#include <vector>
#include <string>

class OpenFileDialog
{
public:
	OpenFileDialog(void);

	TCHAR*DefaultExtension;
	TCHAR*FileName;
	TCHAR*Filter;
	int FilterIndex;
	int Flags;
	TCHAR*InitialDir;
	HWND Owner;
	TCHAR*Title;

	bool ShowDialog();
};

std::vector<std::string> get_all_files_names_within_folder(std::string folder);