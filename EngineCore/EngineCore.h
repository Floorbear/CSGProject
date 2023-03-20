#pragma once
#include <iostream>

class EngineCore
{
public:
	static void Start();

	static void EngineInit();
private:
	static void ProcessInput(struct GLFWwindow* _Window);
};

