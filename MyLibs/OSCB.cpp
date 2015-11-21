#include "OSCB.h"

OpenFileDialog::OpenFileDialog(void)
{
	this->DefaultExtension = 0;
	this->FileName = new TCHAR[MAX_PATH];
	this->Filter = 0;
	this->FilterIndex = 0;
	this->Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	this->InitialDir = 0;
	this->Owner = 0;
	this->Title = 0;
}

bool OpenFileDialog::ShowDialog()
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = this->Owner;
	ofn.lpstrDefExt = this->DefaultExtension;
	ofn.lpstrFile = this->FileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = this->Filter;
	ofn.nFilterIndex = this->FilterIndex;
	ofn.lpstrInitialDir = this->InitialDir;
	ofn.lpstrTitle = this->Title;
	ofn.Flags = this->Flags;

	GetOpenFileName(&ofn);

	if (_tcslen(this->FileName) == 0) return false;

	return true;
}


std::vector<std::string> get_all_files_names_within_folder(std::string folder)
{
	std::vector<std::string> names;
	char search_path[200];
	WIN32_FIND_DATA fd;
	sprintf(search_path, "%s/*.*", folder.c_str());
	WCHAR Wsearch_path[200];
	char DefChar = ' ';
	MultiByteToWideChar(CP_ACP, 0, search_path, -1, Wsearch_path, 200);
	HANDLE hFind = ::FindFirstFile(Wsearch_path, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				char ch[260];
				WideCharToMultiByte(CP_ACP, 0, fd.cFileName, -1, ch, 260, &DefChar, NULL);
				names.push_back(ch);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}