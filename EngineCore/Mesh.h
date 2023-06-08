#pragma once

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

#include <glm/glm.hpp>

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

typedef CGAL::Simple_cartesian<float> Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> CGAL_Mesh;

using namespace glm;

struct Vertex_Rendering{
    vec3 position;
    vec3 normal;
    vec2 texcoord;

    Vertex_Rendering(float x, float y, float z,float normal_x, float normal_y, float normal_z, float tex_x, float tex_y);
};

class Transform;
class Mesh{
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    std::string name = "<Empty>";   
    CGAL_Mesh cgal_mesh;

    std::vector<Vertex_Rendering> vertices_rendering;
    std::vector<unsigned int> indices_rendering;

    bool is_buffer_created = false;
    bool is_buffer_valid = false;

    void calculate_rendering_data();

    void buffers_bind() const;
    void buffers_unbind() const;
    void buffers_update() const;

public:
    Mesh();
    Mesh(const Mesh& mesh);
    Mesh(const Mesh* mesh);
    Mesh(std::string name_, CGAL_Mesh cgal_mesh_);
    ~Mesh();

    void mark_edited();
    void render();

    std::string get_name() const;
    std::string get_mesh_info() const;
    void save(std::string path);

    static Mesh load(std::string path);
    static Mesh cube(float size);
    static Mesh pyramid(float radius, float height);
    static Mesh sphere(float radius, int step);
    static Mesh cylinder(float radius, float height, int step);
    static Mesh cone(float radius, float height, int step);
    static Mesh torus(float radius, float thickness, int step1, int step2);

    static bool compute_union(const Mesh& mesh1, Transform* transform1,
                              const Mesh& mesh2, Transform* transform2, Mesh& result);
    static bool compute_intersection(const Mesh& mesh1, Transform* transform1,
                                     const Mesh& mesh2, Transform* transform2, Mesh& result);
    static bool compute_difference(const Mesh& mesh1, Transform* transform1,
                                   const Mesh& mesh2, Transform* transform2, Mesh& result);
};

