#pragma once

#include "Transform.h"

#include <glm/glm.hpp>

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/draw_surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>

typedef CGAL::Simple_cartesian<double>                       Kernel;
typedef Kernel::Point_3                                      Point;
typedef CGAL::Surface_mesh<Point>                            cgal_Mesh;
typedef cgal_Mesh::Vertex_index vertex_descriptor;
typedef cgal_Mesh::Face_index face_descriptor;
typedef CGAL::Simple_cartesian<double> K;

using namespace glm;

struct Vertex{
    vec3 position;
    vec3 normal;
    //vec2 texcoords

    Vertex(const vec3& position_);
    Vertex(float x, float y, float z);
    Vertex(float x, float y, float z,float normal_x_, float normal_y_, float normal_z_);
};

class Mesh{
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    const std::vector<Vertex> vertices;
    const std::vector<unsigned int> indices;

    bool is_buffer_created;

    void buffers_bind() const;
    void buffers_unbind() const;

public:
    static Mesh Triangle;
    static Mesh Square;
    static Mesh Cube;
    static Mesh Cube2;
    static Mesh Sphere;

    Mesh();
    Mesh(const Mesh& ref);
    Mesh(const Mesh* mesh);
    Mesh(std::vector<Vertex> vertices_, std::vector<unsigned int> indices_);
    ~Mesh();

    void set(const Mesh* mesh);
    void set(std::vector<Vertex> vertices_, std::vector<unsigned int> indices_);
    Mesh* clone_new() const;

    void render();

    static cgal_Mesh create_cgal_cube(float size);
    static cgal_Mesh create_cgal_sphere(int radius, int stackCount, int sectorCount);

    static Mesh cgal_mesh_to_mesh(cgal_Mesh cg_Mesh);
    static cgal_Mesh mesh_to_cgal_mesh(Mesh m);

    static cgal_Mesh _cgal_Cube;
    static cgal_Mesh _cgal_Cube2;
    static cgal_Mesh _cgal_Sphere;

    static Mesh compute_intersection(Mesh m1, Mesh m2);

};

