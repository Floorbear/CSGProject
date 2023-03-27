#pragma once
#include <filesystem>
#include <string>

class EnginePath
{
public:
	EnginePath();
	EnginePath(std::string _Path);
	EnginePath(std::filesystem::path _Path);
	~EnginePath();

	void Move(std::string _Path);
	std::string ReadFile();

private:
	std::filesystem::path path;
};


class EngineFilesystem
{
public:
	static EnginePath GetProjectPath()
	{
		EnginePath NewPath(std::filesystem::current_path());
		return NewPath;
	}

	static EnginePath GetPath(std::string _RelativePath)
	{
		EnginePath NewPath(std::filesystem::current_path());
		return NewPath;
	}
};


