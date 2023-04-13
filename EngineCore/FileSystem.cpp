#include "Filesystem.h"

#include <cassert>
#include <fstream>
#include <iostream>

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