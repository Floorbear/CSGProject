#pragma once
#include <iostream>
#include "EngineMinimal.h"
#include "EngineCore/EngineShader.h"

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

private:
	 static EngineCore* inst;


	 //---------------------- Init GLFW Related  --------------------------------------------
private:
	 struct GLFWwindow* window;
	 struct ImGuiIO io;

	 //---------------------- Shader Related  --------------------------------------------
	 class EngineShader shader;

private:
	
};

