#include "Mesh.h"
#include "Shader.h"

#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/transform.h>
#include <CGAL/Aff_transformation_3.h>
#include <CGAL/Polygon_mesh_processing/compute_normal.h>

// ===== Vertex ===== //

Vertex_Rendering::Vertex_Rendering(float x, float y, float z, float normal_x, float normal_y, float normal_z, float tex_x, float tex_y) :
    position(vec3(x, y, z)),
    normal(vec3(normal_x, normal_y, normal_z)),
    texcoord(vec2(tex_x, tex_y)){
}


// ===== Mesh ===== //

typedef CGAL_Mesh::Vertex_index     Vertex_index;
typedef CGAL_Mesh::Edge_index       Edge_index;
typedef CGAL_Mesh::Halfedge_index   Halfedge_index;
typedef CGAL_Mesh::Face_index       Face_index;

void Mesh::calculate_rendering_data(){
    vertices_rendering.clear();
    indices_rendering.clear();

    int vertex_cnt = 0;
    for (Face_index fd : cgal_mesh.faces()){
        CGAL_Mesh::Property_map<Face_index, Kernel::Vector_3> fnormals = cgal_mesh.property_map<Face_index, Kernel::Vector_3>("f:normals").first;
        CGAL_Mesh::Property_map<Face_index, std::map<Vertex_index, vec2>> ftexcoords = cgal_mesh.property_map<Face_index, std::map<Vertex_index, vec2>>("f:texcoords").first;
        for (Vertex_index vd : cgal_mesh.vertices_around_face(cgal_mesh.halfedge(fd))){
            vertices_rendering.push_back(Vertex_Rendering(
                cgal_mesh.point(vd).x(), cgal_mesh.point(vd).y(), cgal_mesh.point(vd).z(), // position
                fnormals[fd].x(), fnormals[fd].y(), fnormals[fd].z(), // normal
                0, 0));
            //               ftexcoords[fd][vd].x, ftexcoords[fd][vd].y)); // TODO : texcoord
            indices_rendering.push_back(vertex_cnt++);
        }
    }
}

void Mesh::buffers_bind() const{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void Mesh::buffers_unbind() const{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::buffers_update() const{
    buffers_bind();
    glBufferData(GL_ARRAY_BUFFER, vertices_rendering.size() * sizeof(Vertex_Rendering), vertices_rendering.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices_rendering.size(), indices_rendering.data(), GL_STATIC_DRAW);
}

Mesh::Mesh(){
}

Mesh::Mesh(const Mesh& mesh) : cgal_mesh(mesh.cgal_mesh){
}

Mesh::Mesh(const Mesh* mesh) : cgal_mesh(mesh->cgal_mesh){
}

Mesh::Mesh(CGAL_Mesh cgal_mesh_){
    cgal_mesh = cgal_mesh_;
}

Mesh::~Mesh(){
    if (VAO != 0){
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO != 0){
        glDeleteBuffers(1, &VBO);
    }
    if (EBO != 0){
        glDeleteBuffers(1, &EBO);
    }
}

// TODO : vertex, index 하나만 변경되어도 is_buffer_valid = false 해야한다!


void Mesh::render(){
    if (!is_buffer_valid){
        calculate_rendering_data();
        is_buffer_valid = true;
    }
    if (!is_buffer_created){ // 버퍼 지연 초기화
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        is_buffer_created = true;

        buffers_bind();

        glBufferData(GL_ARRAY_BUFFER, vertices_rendering.size() * sizeof(Vertex_Rendering), vertices_rendering.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices_rendering.size(), indices_rendering.data(), GL_STATIC_DRAW);

        // Linking Vertex Attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Rendering), (void*)0);
        glEnableVertexAttribArray(1); //Vertex normal Data
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Rendering), (void*)(offsetof(Vertex_Rendering, Vertex_Rendering::normal)));
        glEnableVertexAttribArray(2); //Vertex texcoord Data
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_Rendering), (void*)(offsetof(Vertex_Rendering, Vertex_Rendering::texcoord)));
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (int)indices_rendering.size(), GL_UNSIGNED_INT, 0);
}


Mesh Mesh::cube(float size){
    CGAL_Mesh cgal_result;

    std::vector<Vertex_index> vd;
    vd.push_back(cgal_result.add_vertex(Kernel::Point_3(0.0f, 0.0f, 0.0f)));
    vd.push_back(cgal_result.add_vertex(Kernel::Point_3(0.0f, size, 0.0f)));
    vd.push_back(cgal_result.add_vertex(Kernel::Point_3(size, size, 0.0f)));
    vd.push_back(cgal_result.add_vertex(Kernel::Point_3(size, 0.0f, 0.0f)));
    vd.push_back(cgal_result.add_vertex(Kernel::Point_3(0.0f, 0.0f, size)));
    vd.push_back(cgal_result.add_vertex(Kernel::Point_3(0.0f, size, size)));
    vd.push_back(cgal_result.add_vertex(Kernel::Point_3(size, size, size)));
    vd.push_back(cgal_result.add_vertex(Kernel::Point_3(size, 0.0f, size)));


    Face_index f1 = cgal_result.add_face(vd[0], vd[3], vd[2]);
    Face_index f2 = cgal_result.add_face(vd[1], vd[0], vd[2]);
    Face_index f3 = cgal_result.add_face(vd[3], vd[0], vd[4]);
    Face_index f4 = cgal_result.add_face(vd[3], vd[4], vd[7]);
    Face_index f5 = cgal_result.add_face(vd[1], vd[2], vd[5]);
    Face_index f6 = cgal_result.add_face(vd[5], vd[2], vd[6]);
    Face_index f7 = cgal_result.add_face(vd[5], vd[6], vd[7]);
    Face_index f8 = cgal_result.add_face(vd[5], vd[7], vd[4]);
    Face_index f9 = cgal_result.add_face(vd[2], vd[3], vd[7]);
    Face_index f10 = cgal_result.add_face(vd[2], vd[7], vd[6]);
    Face_index f11 = cgal_result.add_face(vd[0], vd[1], vd[4]);
    Face_index f12 = cgal_result.add_face(vd[4], vd[1], vd[5]);

    auto tex_coord_map = cgal_result.add_property_map<Face_index, std::map<Vertex_index, vec3>>("f:texcoords", std::map<Vertex_index, vec3>()).first;
    // TODO : texcoord 채우기
    /*Vertex(-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f ,  0.0f, 0.0f                          ),
        Vertex(0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f  ,  1.0f, 0.0f                          ),
        Vertex(0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f  ,  1.0f, 1.0f                          ),
        Vertex(0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f  ,  1.0f, 1.0f                          ),
        Vertex(-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f ,  0.0f, 1.0f                          ),
        Vertex(-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f ,  0.0f, 0.0f                          ),

        Vertex(-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f  ,  0.0f, 0.0f                          ),
        Vertex(0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f   ,  1.0f, 0.0f                          ),
        Vertex(0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f   ,  1.0f, 1.0f                          ),
        Vertex(0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f   ,  1.0f, 1.0f                          ),
        Vertex(-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f  ,  0.0f, 1.0f                          ),
        Vertex(-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f  ,  0.0f, 0.0f                          ),

        Vertex(-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f ,  1.0f, 0.0f                          ),
        Vertex(-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f ,  1.0f, 1.0f                          ),
        Vertex(-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f ,  0.0f, 1.0f                          ),
        Vertex(-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f ,  0.0f, 1.0f                          ),
        Vertex(-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f ,  0.0f, 0.0f                          ),
        Vertex(-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f ,  1.0f, 0.0f                          ),

        Vertex(0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f  ,  1.0f, 0.0f                          ),
        Vertex(0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f  ,  1.0f, 1.0f                          ),
        Vertex(0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f  ,  0.0f, 1.0f                          ),
        Vertex(0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f  ,  0.0f, 1.0f                          ),
        Vertex(0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f  ,  0.0f, 0.0f                          ),
        Vertex(0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f  ,  1.0f, 0.0f                          ),

        Vertex(-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f ,  0.0f, 1.0f                          ),
        Vertex(0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f  ,  1.0f, 1.0f                          ),
        Vertex(0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f  ,  1.0f, 0.0f                          ),
        Vertex(0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f  ,  1.0f, 0.0f                          ),
        Vertex(-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f ,  0.0f, 0.0f                          ),
        Vertex(-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f ,  0.0f, 1.0f                          ),

        Vertex(-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f ,  0.0f, 1.0f                          ),
        Vertex(0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f  ,  1.0f, 1.0f                          ),
        Vertex(0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f  ,  1.0f, 0.0f                          ),
        Vertex(0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f  ,  1.0f, 0.0f                          ),
        Vertex(-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f ,  0.0f, 0.0f                          ),
        Vertex(-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f ,  0.0f, 1.0f                          )*/
    auto fnormals = cgal_result.add_property_map<Face_index, Kernel::Vector_3>("f:normals", CGAL::NULL_VECTOR).first;
    CGAL::Polygon_mesh_processing::compute_face_normals(cgal_result, fnormals);
    return Mesh(cgal_result);
}

/*
Mesh Mesh::Sphere(){
}

*/

CGAL::Aff_transformation_3<Kernel> mat4_to_cgal_transform(const mat4& matrix){
    return CGAL::Aff_transformation_3<Kernel>(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
                                              matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
                                              matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2]);
}

// TODO : union 함수처럼 수정
bool Mesh::compute_difference(const Mesh& mesh1, Transform* transform1,
                              const Mesh& mesh2, Transform* transform2, Mesh& result){
    CGAL_Mesh cgal_mesh1 = mesh1.cgal_mesh;
    CGAL_Mesh cgal_mesh2 = mesh2.cgal_mesh;

    // TODO : log창에 연산 결과 추가
    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh1)){
        printf("mesh1 self intersects!\n");
        return false;
    }
    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh2)){
        printf("mesh2 self intersects!\n");
        return false;
    }
    return CGAL::Polygon_mesh_processing::corefine_and_compute_union(cgal_mesh1, cgal_mesh2, result.cgal_mesh);
    // TODO : texcoord 작동됨??
}

bool Mesh::compute_intersection(const Mesh& mesh1, Transform* transform1,
                                const Mesh& mesh2, Transform* transform2, Mesh& result){
    CGAL_Mesh cgal_mesh1 = mesh1.cgal_mesh;
    CGAL_Mesh cgal_mesh2 = mesh2.cgal_mesh;

    // TODO : log창에 연산 결과 추가
    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh1)){
        printf("mesh1 self intersects!\n");
        return false;
    }
    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh2)){
        printf("mesh2 self intersects!\n");
        return false;
    }
    return CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(cgal_mesh1, cgal_mesh2, result.cgal_mesh);
}

bool Mesh::compute_union(const Mesh& mesh1, Transform* transform1,
                         const Mesh& mesh2, Transform* transform2, Mesh& result){
    CGAL_Mesh cgal_mesh1 = mesh1.cgal_mesh;
    CGAL_Mesh cgal_mesh2 = mesh2.cgal_mesh;
    CGAL_Mesh cgal_result;

    CGAL::Polygon_mesh_processing::transform(mat4_to_cgal_transform(transform1->get_local_matrix()), cgal_mesh1);
    CGAL::Polygon_mesh_processing::transform(mat4_to_cgal_transform(transform2->get_local_matrix()), cgal_mesh2);

    // TODO : log창에 연산 결과 추가
    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh1)){
        printf("mesh1 self intersects!\n");
        return false;
    }//CGAL::Polygon_mesh_processing::does_bound_a_volume(tm1)
    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh2)){
        printf("mesh2 self intersects!\n");
        return false;
    }

    CGAL_Mesh::Property_map<Edge_index, bool> is_constrained_map =
        cgal_result.add_property_map<Edge_index, bool>("e:is_constrained", false).first;

    bool result_valid = CGAL::Polygon_mesh_processing::corefine_and_compute_difference(cgal_mesh1, cgal_mesh2, cgal_result,
                                                                                       CGAL::parameters::default_values(),
                                                                                       CGAL::parameters::default_values(),
                                                                                       CGAL::parameters::edge_is_constrained_map(is_constrained_map));

    printf("%zd %zd %zd\n", cgal_mesh1.vertices().size(), cgal_mesh1.edges().size(), cgal_mesh1.faces().size());
    printf("%zd %zd %zd\n", cgal_mesh2.vertices().size(), cgal_mesh2.edges().size(), cgal_mesh2.faces().size());
    printf("%zd %zd %zd\n\n", cgal_result.vertices().size(), cgal_mesh2.edges().size(), cgal_result.faces().size());

    auto tex_coord_map = cgal_result.add_property_map<Face_index, std::map<Vertex_index, vec3>>("f:texcoords", std::map<Vertex_index, vec3>()).first;

    // collect faces incident to a constrained edge (미완성, texcoord 구하기위함)
    std::vector<Face_index> selected_faces;
    std::vector<bool> is_selected(num_faces(cgal_result), false);
    for (Edge_index e : CGAL::edges(cgal_result)){
        if (is_constrained_map[e]){
            // insert all faces incident to the target vertex
            for (Halfedge_index h : cgal_result.halfedges_around_target(cgal_result.halfedge(e))){
                if (!is_border(h, cgal_result)){
                    Face_index f = cgal_result.face(h);
                    if (!is_selected[f]){
                        selected_faces.push_back(f);
                        is_selected[f] = true;
                    }
                }
            }
        }
    }

    auto fnormals = cgal_result.add_property_map<Face_index, Kernel::Vector_3>("f:normals", CGAL::NULL_VECTOR).first;
    CGAL::Polygon_mesh_processing::compute_face_normals(cgal_result, fnormals);

    result.cgal_mesh = cgal_result;
    return result_valid;
}
