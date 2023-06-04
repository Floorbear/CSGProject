#include "Filesystem.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <Windows.h>

EnginePath::EnginePath()
{

}
EnginePath::EnginePath(std::string _Path)
{
	path = _Path;
}

EnginePath::EnginePath(std::filesystem::path _Path)
{
	path = _Path;
}
EnginePath::~EnginePath()
{
	
}

void EnginePath::Move(std::string _Path)
{
	path += "\\"+_Path;
	assert(std::filesystem::exists(path) == true && "Not exists!");
}

std::string EnginePath::ReadFile()
{
	std::string TextData;
	std::string Line;
	assert(std::filesystem::exists(path) == true && "Not exists!");
	std::ifstream File;
	File.open(path);
	assert(File.is_open() == true && path.c_str());
	while (std::getline(File, Line))
	{
		TextData += Line + "\n";
	}
	TextData += '\0';
	File.close();
	return TextData;
}

EnginePath FileSystem::getFilePath()
{
	std::wstring filePath;

	OPENFILENAME ofn;
	wchar_t fileName[MAX_PATH] = L"";
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;

	//파일 로드 윈도우창에서 obj,fbx,jpg 만로드 할 수 있습니다.
	//만약 모든 파일을 로드 할 수 있게 하려면 L"All Files (*.*)\0*.*\0" 로 수정
	ofn.lpstrFilter = L"(*.obj;*.fbx;*.jpg)\0*.obj;*.fbx;*.jpg\0"; 

	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (GetOpenFileName(&ofn)) {
		filePath = fileName;
	}

	EnginePath NewPath = EnginePath(filePath);
	return NewPath;

}
