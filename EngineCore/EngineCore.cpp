#include "EngineCore.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>


#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")
//#pragma comment(lib,"SDL2.lib")
//#pragma comment(lib,"SDL2main.lib")

#define CGAL_EIGEN3_ENABLED

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>
#include <CGAL/IO/facets_in_complex_2_to_triangle_mesh.h>
#include <CGAL/Poisson_reconstruction_function.h>
#include <CGAL/property_map.h>
#include <CGAL/IO/read_points.h>
#include <CGAL/compute_average_spacing.h>
#include <CGAL/Polygon_mesh_processing/distance.h>
#include <boost/iterator/transform_iterator.hpp>
#include <vector>
#include <fstream>
// Types
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::FT FT;
typedef Kernel::Point_3 Point;
typedef Kernel::Vector_3 Vector;
typedef std::pair<Point, Vector> Point_with_normal;
typedef CGAL::First_of_pair_property_map<Point_with_normal> Point_map;
typedef CGAL::Second_of_pair_property_map<Point_with_normal> Normal_map;
typedef Kernel::Sphere_3 Sphere;
typedef std::vector<Point_with_normal> PointList;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef CGAL::Poisson_reconstruction_function<Kernel> Poisson_reconstruction_function;
typedef CGAL::Surface_mesh_default_triangulation_3 STr;
typedef CGAL::Surface_mesh_complex_2_in_triangulation_3<STr> C2t3;
typedef CGAL::Implicit_surface_3<Kernel, Poisson_reconstruction_function> Surface_3;



void EngineCore::Start()
{
	EngineInit();

}

void EngineCore::EngineInit()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "CSGProject", NULL, NULL);
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

	glViewport(0, 0, 800, 600);

	//I dont set Window resize callback

	//IMGUI
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
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

	// Our state
	bool show_demo_window = true;
	bool show_another_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	//CGal
	CgalTestFunction();


	while (!glfwWindowShouldClose(window))
	{
		//Input
		ProcessInput(window);

		//IMGUI
		{
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

		//Render
		glClearColor(0.5f, 0.3f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Check Event & Swap buffer
		glfwPollEvents();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);


	}


	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return;
}

void EngineCore::ProcessInput(GLFWwindow* _Window)
{
	if (glfwGetKey(_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(_Window, true);
	}
}

int EngineCore::CgalTestFunction()
{
	// Poisson options
	FT sm_angle = 20.0; // Min triangle angle in degrees.
	FT sm_radius = 30; // Max triangle size w.r.t. point set average spacing.
	FT sm_distance = 0.375; // Surface Approximation error w.r.t. point set average spacing.
	// Reads the point set file in points[].
	// Note: read_points() requires an iterator over points
	// + property maps to access each point's position and normal.
	PointList points;
	if (!CGAL::IO::read_points(CGAL::data_file_path("C:/Users/DongWon/source/repos/CSGProject/data/kitten.xyz"), std::back_inserter(points),
		CGAL::parameters::point_map(Point_map())
		.normal_map(Normal_map())))
	{
		std::cerr << "Error: cannot read file input file!" << std::endl;
		return EXIT_FAILURE;
	}
	// Creates implicit function from the read points using the default solver.
	// Note: this method requires an iterator over points
	// + property maps to access each point's position and normal.
	Poisson_reconstruction_function function(points.begin(), points.end(), Point_map(), Normal_map());
	// Computes the Poisson indicator function f()
	// at each vertex of the triangulation.
	if (!function.compute_implicit_function())
		return EXIT_FAILURE;
	// Computes average spacing
	FT average_spacing = CGAL::compute_average_spacing<CGAL::Sequential_tag>
		(points, 6 /* knn = 1 ring */,
			CGAL::parameters::point_map(Point_map()));
	// Gets one point inside the implicit surface
	// and computes implicit function bounding sphere radius.
	Point inner_point = function.get_inner_point();
	Sphere bsphere = function.bounding_sphere();
	FT radius = std::sqrt(bsphere.squared_radius());
	// Defines the implicit surface: requires defining a
	// conservative bounding sphere centered at inner point.
	FT sm_sphere_radius = 5.0 * radius;
	FT sm_dichotomy_error = sm_distance * average_spacing / 1000.0; // Dichotomy error must be << sm_distance
	Surface_3 surface(function,
		Sphere(inner_point, sm_sphere_radius * sm_sphere_radius),
		sm_dichotomy_error / sm_sphere_radius);
	// Defines surface mesh generation criteria
	CGAL::Surface_mesh_default_criteria_3<STr> criteria(sm_angle,  // Min triangle angle (degrees)
		sm_radius * average_spacing,  // Max triangle size
		sm_distance * average_spacing); // Approximation error
// Generates surface mesh with manifold option
	STr tr; // 3D Delaunay triangulation for surface mesh generation
	C2t3 c2t3(tr); // 2D complex in 3D Delaunay triangulation
	CGAL::make_surface_mesh(c2t3,                                 // reconstructed mesh
		surface,                              // implicit surface
		criteria,                             // meshing criteria
		CGAL::Manifold_with_boundary_tag());  // require manifold mesh
	if (tr.number_of_vertices() == 0)
		return EXIT_FAILURE;
	// saves reconstructed surface mesh
	std::ofstream out("kitten_poisson-20-30-0.375.off");
	Polyhedron output_mesh;
	CGAL::facets_in_complex_2_to_triangle_mesh(c2t3, output_mesh);
	out << output_mesh;
	// computes the approximation error of the reconstruction
	double max_dist =
		CGAL::Polygon_mesh_processing::approximate_max_distance_to_point_set
		(output_mesh,
			CGAL::make_range(boost::make_transform_iterator
			(points.begin(), CGAL::Property_map_to_unary_function<Point_map>()),
				boost::make_transform_iterator
				(points.end(), CGAL::Property_map_to_unary_function<Point_map>())),
			4000);
	std::cout << "Max distance to point_set: " << max_dist << std::endl;
	return EXIT_SUCCESS;
}
