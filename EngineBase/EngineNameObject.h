#pragma once
#include <string>
class EngineNameObject
{
public:
	EngineNameObject();
	virtual ~EngineNameObject();
	inline void SetName(std::string_view _name)
	{
		name = _name;
	}

	inline std::string GetNameCopy()
	{
		return name;
	}
private:
	std::string name = "";

};

