#pragma once

#include "Transform.h"

#include <glm/glm.hpp>

#include <vector>

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

    Mesh();
    Mesh(const Mesh& ref);
    Mesh(const Mesh* mesh);
    Mesh(std::vector<Vertex> vertices_, std::vector<unsigned int> indices_);
    ~Mesh();

    void set(const Mesh* mesh);
    void set(std::vector<Vertex> vertices_, std::vector<unsigned int> indices_);
    Mesh* clone_new() const;

    void render();
};

