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
	inline std::string get_path()
	{
		return path.string();
	}

private:
	std::filesystem::path path;
};


class FileSystem
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


