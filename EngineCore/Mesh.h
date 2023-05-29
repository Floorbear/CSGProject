#pragma once

#include "Transform.h"

#include <glm/glm.hpp>

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

typedef CGAL::Simple_cartesian<float> Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> CGAL_Mesh;

using namespace glm;

struct Vertex_Rendering{
    vec3 position;
    vec3 normal;
    vec2 texcoord;

    Vertex_Rendering(float x, float y, float z,float normal_x, float normal_y, float normal_z, float tex_x, float tex_y);
};

class Mesh{
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

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
    Mesh(CGAL_Mesh cgal_mesh_);
    ~Mesh();

    void render();

    static Mesh load(std::string path);
    static Mesh triangle();
    static Mesh square();
    static Mesh cube(float size);
    static Mesh sphere();

    static bool compute_union(const Mesh& mesh1, Transform* transform1,
                              const Mesh& mesh2, Transform* transform2, Mesh& result);
    static bool compute_intersection(const Mesh& mesh1, Transform* transform1,
                                     const Mesh& mesh2, Transform* transform2, Mesh& result);
    static bool compute_difference(const Mesh& mesh1, Transform* transform1,
                                   const Mesh& mesh2, Transform* transform2, Mesh& result);
};

