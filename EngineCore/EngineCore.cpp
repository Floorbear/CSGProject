#include "EngineCore.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>

#include "CGAL/Surface_mesh.h"


#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")


#include "EngineCore/EngineModel.h"
#include "EngineCore/EngineShader.h"


#define CGAL_EIGEN3_ENABLED

EngineCore* EngineCore::inst = nullptr;


EngineCore::EngineCore():
	window(),
	renderer(nullptr)
{
	assert(inst == nullptr && "Already exists Core");
	inst = this;
	windowSize.x = 1280;
	windowSize.y = 720;
}

EngineCore::~EngineCore()
{
}

void EngineCore::Start()
{
	InitEngine();

}

EngineCore* EngineCore::GetInst()
{
	assert(inst != nullptr && "Inst is nullptr");
	return inst;
}

void EngineCore::InitEngine()
{
	InitGlfw();
	//I dont set Window resize callback
	InitImgui();

	renderer = new EngineModel();
	renderer->GetTransform()->SetLocalPosition(vector3(0.0, 0.f, 0.f));
	while (!glfwWindowShouldClose(window))
	{
		UpdateEngine();
	}
	EndEngine();
}

void EngineCore::InitGlfw()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow((int)windowSize.x, (int)windowSize.y, "CSGProject", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	// Set window Context to main Context
	glfwMakeContextCurrent(window);

	// Init GLAD
	// To manage OpenGL Function Pointer
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	glViewport(0, 0, (int)windowSize.x, (int)windowSize.y);

	
}

void EngineCore::InitImgui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

void EngineCore::UpdateImgui()
{
	{
		// Our state
		bool show_demo_window = true;
		bool show_another_window = true;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

	}
}

void EngineCore::Render()
{
	////Render
	glClearColor(0.5f, 0.3f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//----------------------------- view & projection
	glm::mat4 view = glm::mat4(1.0f);
	view = translate(view, vector3(0.0, 0., -3.f));

	glm::mat4 projection = glm::mat4(1.0f);
	float fov = 45.f;
	projection = glm::perspective(glm::radians(fov), windowSize.x/ windowSize.y, 0.1f, 100.f);

	int viewLocation = glGetUniformLocation(renderer->GetShader()->GetShaderProgram(), "view");
	int projectionLocation = glGetUniformLocation(renderer->GetShader()->GetShaderProgram(), "projection");

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));



	renderer->RenderTriangle();

	//Check Event & Swap buffer
	glfwPollEvents();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

void EngineCore::UpdateEngine()
{
	//Input
	ProcessInput(window);
	//IMGUI
	UpdateImgui();
	
	Render();
}

void EngineCore::EndEngine()
{
	delete renderer;

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}


void EngineCore::ProcessInput(GLFWwindow* _Window)
{
	if (glfwGetKey(_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(_Window, true);
	}
}


