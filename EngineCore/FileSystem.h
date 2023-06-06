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

	EnginePath move(std::string path_);
	std::string ReadFile();
	inline std::string get_path()
	{
		return path.string();
	}

	static EnginePath get_shader_path();
	static EnginePath get_texture_path();

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

	static EnginePath getFilePath();
};


