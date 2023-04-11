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

private:
	 static EngineCore* inst;


	 //---------------------- Init GLFW Related  --------------------------------------------
private:
	 struct GLFWwindow* window;
	 struct ImGuiIO io;
	 vector2 windowSize;

	 //---------------------- Model Related  --------------------------------------------
public:
	class EngineModel* CreateEngineModel();
	class EngineModel* CreateEngineModel(std::string_view _name);
	class EngineModel* FindModel(std::string_view _name);

private:
	 std::list<class EngineModel*> engineModels;
private:
	
	//---------------------- Test Related  --------------------------------------------
	float Time = 0;
	void UpdateTestCode();
};

