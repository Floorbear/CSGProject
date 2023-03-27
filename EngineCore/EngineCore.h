#pragma once
#include <iostream>
#include "EngineMinimal.h"

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
	 unsigned int vertexShader;


	 //---------------------- Test Member --------------------------------------------
private:
	std::vector<float> vertices = {-0.5f,-0.5f,0.0f,0.5f,-0.5f,0.0f,0.0f,0.5f,0.0f};
};

