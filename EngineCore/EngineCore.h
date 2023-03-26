#pragma once
#include <iostream>
#include "Imgui/imgui.h"

class EngineCore
{
public:
	EngineCore();
	~EngineCore();
	 void Start();

	 EngineCore* GetInst();

private:
	 void ProcessInput(struct GLFWwindow* _Window);
	 void InitEngine();
	 void UpdateEngine();
	 void EndEngine();

	 void InitGlfw();

	 void InitImgui();
	 void UpdateImgui();

	 void Render();

	 //Singleton
private:
	 static EngineCore* inst;

private:
	 struct GLFWwindow* window;
	 struct ImGuiIO io;
	 unsigned int VBO;
};

