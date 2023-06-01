#include "Mesh.h"
#include "Shader.h"
#include "Utils.h"

#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Labeled_mesh_domain_3.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/transform.h>
#include <CGAL/Aff_transformation_3.h>
#include <CGAL/Polygon_mesh_processing/compute_normal.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/make_mesh_3.h>

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
        // CGAL_Mesh::Property_map<Face_index, std::map<Vertex_index, vec2>> ftexcoords = cgal_mesh.property_map<Face_index, std::map<Vertex_index, vec2>>("f:texcoords").first;
        for (Vertex_index vd : cgal_mesh.vertices_around_face(cgal_mesh.halfedge(fd))){
            vertices_rendering.push_back(Vertex_Rendering(
                cgal_mesh.point(vd).x(), cgal_mesh.point(vd).y(), cgal_mesh.point(vd).z(), // position
                fnormals[fd].x(), fnormals[fd].y(), fnormals[fd].z(), // normal
                0, 0));
            // ftexcoords[fd][vd].x, ftexcoords[fd][vd].y)); // texcoord
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
    // empty mesh
}

Mesh::Mesh(const Mesh& mesh) : name(mesh.name), cgal_mesh(mesh.cgal_mesh){
}

Mesh::Mesh(const Mesh* mesh) : name(mesh->name), cgal_mesh(mesh->cgal_mesh){
}

Mesh::Mesh(std::string name_, CGAL_Mesh cgal_mesh_){
    name = name_;
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


void Mesh::mark_edited(){
    is_buffer_valid = false;
}

void Mesh::render(){
    if (!is_buffer_created){ // 버퍼 지연 초기화
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        is_buffer_created = true;
    }
    if (!is_buffer_valid){
        calculate_rendering_data();
        is_buffer_valid = true;

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

std::string Mesh::get_name() const{
    return name;
}

void Mesh::save(std::string path){
    CGAL::IO::write_polygon_mesh(path, cgal_mesh, CGAL::parameters::stream_precision(17));
    // TODO : 로그창에 추가
}

Mesh Mesh::load(std::string path){
    CGAL_Mesh cgal_result;
    if (!CGAL::IO::read_polygon_mesh(CGAL::data_file_path(path), cgal_result)){
        // TODO : 로그창에 추가
        return Mesh();
    }
    size_t path_filename_index = path.find("/");
    return Mesh(path.substr(path_filename_index == std::string::npos ? 0 : path_filename_index + 1, path.length()), cgal_result);
}

Mesh Mesh::cube(float size){
    CGAL_Mesh cgal_result;
    float r = size / 2;

    Vertex_index v0 = cgal_result.add_vertex(Kernel::Point_3(-r, -r, r)); // cgal 순서
    Vertex_index v1 = cgal_result.add_vertex(Kernel::Point_3(r, -r, r));
    Vertex_index v2 = cgal_result.add_vertex(Kernel::Point_3(r, -r, -r));
    Vertex_index v3 = cgal_result.add_vertex(Kernel::Point_3(-r, -r, -r));
    Vertex_index v4 = cgal_result.add_vertex(Kernel::Point_3(-r, r, -r));
    Vertex_index v5 = cgal_result.add_vertex(Kernel::Point_3(-r, r, r));
    Vertex_index v6 = cgal_result.add_vertex(Kernel::Point_3(r, r, r));
    Vertex_index v7 = cgal_result.add_vertex(Kernel::Point_3(r, r, -r));

    // auto tex_coord_map = cgal_result.add_property_map<Face_index, std::map<Vertex_index, vec2>>("f:texcoords", std::map<Vertex_index, vec2>()).first;

    Face_index f1 = cgal_result.add_face(v0, v3, v2);
    // tex_coord_map[f1][v0] = vec2(1, 0);
    // tex_coord_map[f1][v3] = vec2(1, 1);
    // tex_coord_map[f1][v2] = vec2(0, 1);
    Face_index f2 = cgal_result.add_face(v0, v2, v1);
    // tex_coord_map[f2][v0] = vec2(1, 0);
    // tex_coord_map[f2][v2] = vec2(0, 1);
    // tex_coord_map[f2][v1] = vec2(0, 0);

    Face_index f3 = cgal_result.add_face(v0, v1, v6);
    // tex_coord_map[f3][v0] = vec2(0, 0);
    // tex_coord_map[f3][v1] = vec2(0, 1);
    // tex_coord_map[f3][v6] = vec2(1, 1);
    Face_index f4 = cgal_result.add_face(v0, v6, v5);
    // tex_coord_map[f4][v0] = vec2(0, 0);
    // tex_coord_map[f4][v6] = vec2(1, 1);
    // tex_coord_map[f4][v5] = vec2(1, 0);

    Face_index f5 = cgal_result.add_face(v0, v5, v4);
    // tex_coord_map[f5][v0] = vec2(1, 0);
    // tex_coord_map[f5][v5] = vec2(1, 1);
    // tex_coord_map[f5][v4] = vec2(0, 1);
    Face_index f6 = cgal_result.add_face(v0, v4, v3);
    // tex_coord_map[f6][v0] = vec2(1, 0);
    // tex_coord_map[f6][v4] = vec2(0, 1);
    // tex_coord_map[f6][v3] = vec2(0, 0);

    Face_index f7 = cgal_result.add_face(v2, v3, v4);
    // tex_coord_map[f7][v2] = vec2(0, 0);
    // tex_coord_map[f7][v3] = vec2(0, 1);
    // tex_coord_map[f7][v4] = vec2(1, 1);
    Face_index f8 = cgal_result.add_face(v2, v4, v7);
    // tex_coord_map[f8][v2] = vec2(0, 0);
    // tex_coord_map[f8][v4] = vec2(1, 1);
    // tex_coord_map[f8][v7] = vec2(1, 0);

    Face_index f9 = cgal_result.add_face(v2, v7, v6);
    // tex_coord_map[f9][v2] = vec2(1, 0);
    // tex_coord_map[f9][v7] = vec2(1, 1);
    // tex_coord_map[f9][v6] = vec2(0, 1);
    Face_index f10 = cgal_result.add_face(v2, v6, v1);
    // tex_coord_map[f10][v2] = vec2(1, 0);
    // tex_coord_map[f10][v6] = vec2(0, 1);
    // tex_coord_map[f10][v1] = vec2(0, 0);

    Face_index f11 = cgal_result.add_face(v4, v5, v6);
    // tex_coord_map[f11][v4] = vec2(1, 0);
    // tex_coord_map[f11][v5] = vec2(1, 1);
    // tex_coord_map[f11][v6] = vec2(0, 1);
    Face_index f12 = cgal_result.add_face(v4, v6, v7);
    // tex_coord_map[f12][v4] = vec2(1, 0);
    // tex_coord_map[f12][v6] = vec2(0, 1);
    // tex_coord_map[f12][v7] = vec2(0, 0);

    auto fnormals = cgal_result.add_property_map<Face_index, Kernel::Vector_3>("f:normals", CGAL::NULL_VECTOR).first;
    CGAL::Polygon_mesh_processing::compute_face_normals(cgal_result, fnormals);

    return Mesh("<Cube>", cgal_result);
}


Mesh Mesh::pyramid(float radius, float height){
    return Mesh();
}

Mesh Mesh::sphere(float radius, float step){
    /*typedef CGAL::Mesh_triangulation_3<CGAL::Labeled_mesh_domain_3<Kernel>, CGAL::Default, CGAL::Sequential_tag >::type Tr;
    typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
    typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;
    auto sphere_function = [](const Kernel::Point_3& p){
        return CGAL::squared_distance(p, Kernel::Point_3(CGAL::ORIGIN)) - 1;
    };
    CGAL::Labeled_mesh_domain_3<Kernel> domain =
        CGAL::Labeled_mesh_domain_3<Kernel>::create_implicit_mesh_domain(sphere_function,
                                                 Kernel::Sphere_3(CGAL::ORIGIN, radius * radius));
    // facet_angle=30, facet_size = 0.1, facet_distance = 0.025, cell_radius_edge_ratio = 2, cell_size = 0.1;
    Mesh_criteria criteria(30, 0.1, 0.025, 2, 0.1);
    C3t3 a = CGAL::make_mesh_3<C3t3>(domain, criteria);*/
    return Mesh("<Sphere>", CGAL_Mesh());
}

Mesh Mesh::cylinder(float radius, float height, float step){
    return Mesh();
}

Mesh Mesh::cone(float radius, float height, float step){
    return Mesh();
}

Mesh Mesh::torus(float radius, float thickness, float step){
    return Mesh();
}

CGAL::Aff_transformation_3<Kernel> mat4_to_cgal_transform(const mat4& matrix){
    return CGAL::Aff_transformation_3<Kernel>(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
                                              matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
                                              matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2]);
}

vec3 pt3_to_vec3(Kernel::Point_3 point){
    return vec3(point.x(), point.y(), point.z());
}

/*struct Visitor : public CGAL::Polygon_mesh_processing::Corefinement::Default_visitor<CGAL_Mesh>{
    std::map<const CGAL_Mesh*, std::vector<Vertex_index>> new_vertices_src;
    std::map<std::pair<Vertex_index, const CGAL_Mesh*>, std::pair<Vertex_index, Vertex_index>> edge_split_map; // edge_split[new_vertex] = {old_edge_vertex1, old_edge_vertex2, mesh_containing_edge}
    std::map<std::pair<Vertex_index, const CGAL_Mesh*>, Vertex_index> vertex_copy_map;

    Face_index face_old;
    std::vector<Face_index> faces_new;
    //std::vector<Face_index> faces_new;
    const CGAL_Mesh* subface_tm;
    void before_subface_creations(Face_index f_old, const CGAL_Mesh& tm){
        face_old = f_old;
        subface_tm = &tm;
        faces_new.clear();
    }

    void after_subface_creations(const CGAL_Mesh& tm){
        auto texcoord_map = tm.property_map<Face_index, std::map<Vertex_index, vec2>>("f:texcoords").first;
        printf("subface creation - size %d of %d\n", faces_new.size(), &tm);
        for (Face_index face_new : faces_new){
            for (Vertex_index vd : tm.vertices_around_face(tm.halfedge(face_new))){
                if (edge_split_map.contains({vd, &tm})){
                    Vertex_index edge_v1_old = edge_split_map[{vd, & tm}].first;
                    Vertex_index edge_v2_old = edge_split_map[{vd, & tm}].second;

                    vec2 texcoord_old_v1 = texcoord_map[face_old][edge_v1_old];
                    vec2 texcoord_old_v2 = texcoord_map[face_old][edge_v2_old];
                    vec3 pt_old_v1 = pt3_to_vec3(tm.point(edge_v1_old));
                    vec3 pt_old_v2 = pt3_to_vec3(tm.point(edge_v2_old));
                    vec3 pt_new_v = pt3_to_vec3(tm.point(vd));
                    texcoord_map[face_new][vd] = (texcoord_old_v1 * glm::distance(pt_old_v2, pt_new_v) + texcoord_old_v2 * glm::distance(pt_new_v, pt_old_v1)) / glm::distance(pt_old_v2, pt_old_v1);
                    printf("texcoord (%f,%f,%f) of %d is (%f,%f)\n",pt_new_v.x,pt_new_v.y,pt_new_v.z,&tm,texcoord_map[face_new][vd].x, texcoord_map[face_new][vd].y);
                } else{
                    if(Utils::contains(new_vertices_src[&tm], vd)){
                        vec3 pt = pt3_to_vec3(tm.point(vd));
                        texcoord_map[face_new][vd] = vec2(0.5,0.5);
                        printf("??vertex - %d (%f,%f,%f) of %d\n", vd,pt.x,pt.y,pt.z,&tm);
                    }else{
                    texcoord_map[face_new][vd] = texcoord_map[face_old][vd];
                    }
                }
            }
        }
    }

    void after_subface_created(Face_index face_new, const CGAL_Mesh& tm){
        faces_new.push_back(face_new);
    }

    Vertex_index edge_v1_old;
    Vertex_index edge_v2_old;
    void before_edge_split(Halfedge_index h, const CGAL_Mesh& tm){
        edge_v1_old = tm.source(h);
        edge_v2_old = tm.target(h);
    }

    void edge_split(Halfedge_index hnew, const CGAL_Mesh& tm){
        printf("e");
        Vertex_index edge_vnew = tm.source(hnew);
        if (edge_vnew == edge_v1_old || edge_vnew == edge_v2_old){
            edge_vnew = tm.target(hnew);
        }
        edge_split_map[{edge_vnew, & tm}] = {edge_v1_old, edge_v2_old};
        vec3 pt = pt3_to_vec3(tm.point(edge_vnew));
        printf("split edge - %d %d split by %d (%f,%f,%f) of %d\n", edge_v1_old,edge_v2_old,edge_vnew,pt.x,pt.y,pt.z, &tm);
    }

    void new_vertex_added(std::size_t i_id, vertex_descriptor v, const CGAL_Mesh& tm){
        printf("new vertex %d of %d - %d\n", v, &tm, i_id);
        new_vertices_src[&tm].push_back(v);
    }

    void add_retriangulation_edge(halfedge_descriptorh, const CGAL_Mesh& tm){
        printf("r - face %d edge %d %d\n", tm.face(h), tm.source(h), tm.target(h));
    }

    void start_triangulating_faces (std::size_t n){
        printf("t - %d\n",n);
    }

    void end_triangulating_faces (){
    }

    void intersection_point_detected(std::size_t i_id,
                                         int 	sdim,
                                         halfedge_descriptor 	h_f, // -> 삼각화 대상
                                         halfedge_descriptor 	h_e,
                                         const CGAL_Mesh& tm_f,
                                         const CGAL_Mesh& tm_e,
                                         bool 	is_target_coplanar,
                                         bool 	is_source_coplanar){
        printf("d - face %d / edge %d %d of %d\n", h_f, tm_e.source(h_e), tm_e.target(h_e), &tm_e);
    }

    void after_face_copy(Face_index f_src, const CGAL_Mesh& tm_src, face_descriptor f_tgt, const CGAL_Mesh& tm_tgt){
        auto texcoord_map_src = tm_src.property_map<Face_index, std::map<Vertex_index, vec2>>("f:texcoords").first;
        auto texcoord_map_tgt = tm_tgt.property_map<Face_index, std::map<Vertex_index, vec2>>("f:texcoords").first;
        for (auto pair : texcoord_map_src[f_src]){
            texcoord_map_tgt[f_tgt][vertex_copy_map[{pair.first, & tm_src}]] = pair.second;
        }
    }

    void after_vertex_copy(Vertex_index v_src, const CGAL_Mesh& tm_src, Vertex_index v_tgt, const CGAL_Mesh& tm_tgt){
        vertex_copy_map[{v_src, & tm_src}] = v_tgt;
    }
};*/

bool Mesh::compute_difference(const Mesh& mesh1, Transform* transform1,
                              const Mesh& mesh2, Transform* transform2, Mesh& result){
    CGAL_Mesh cgal_mesh1 = mesh1.cgal_mesh;
    CGAL_Mesh cgal_mesh2 = mesh2.cgal_mesh;
    CGAL_Mesh cgal_result;

    CGAL::Polygon_mesh_processing::transform(mat4_to_cgal_transform(transform1->get_local_matrix()), cgal_mesh1);
    CGAL::Polygon_mesh_processing::transform(mat4_to_cgal_transform(transform2->get_local_matrix()), cgal_mesh2);

    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh1) && CGAL::Polygon_mesh_processing::does_bound_a_volume(cgal_mesh1)){
        printf("mesh1 self intersects!\n");
        return false;
    }

    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh2) && CGAL::Polygon_mesh_processing::does_bound_a_volume(cgal_mesh2)){
        printf("mesh2 self intersects!\n");
        return false;
    }

    bool result_valid;
    try{
        result_valid = CGAL::Polygon_mesh_processing::corefine_and_compute_difference(cgal_mesh1, cgal_mesh2, cgal_result);
    } catch (std::exception){
        result_valid = false;
    }

    if (!result_valid){
        printf("boolean operation failed!\n");
        return false;
    }

    auto fnormals = cgal_result.add_property_map<Face_index, Kernel::Vector_3>("f:normals", CGAL::NULL_VECTOR).first;
    CGAL::Polygon_mesh_processing::compute_face_normals(cgal_result, fnormals);

    result.cgal_mesh = cgal_result;
    result.name = "[Difference]";
    result.mark_edited();
    return true;
}

bool Mesh::compute_intersection(const Mesh& mesh1, Transform* transform1,
                                const Mesh& mesh2, Transform* transform2, Mesh& result){
    CGAL_Mesh cgal_mesh1 = mesh1.cgal_mesh;
    CGAL_Mesh cgal_mesh2 = mesh2.cgal_mesh;
    CGAL_Mesh cgal_result;

    CGAL::Polygon_mesh_processing::transform(mat4_to_cgal_transform(transform1->get_local_matrix()), cgal_mesh1);
    CGAL::Polygon_mesh_processing::transform(mat4_to_cgal_transform(transform2->get_local_matrix()), cgal_mesh2);

    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh1) && CGAL::Polygon_mesh_processing::does_bound_a_volume(cgal_mesh1)){
        printf("mesh1 self intersects!\n");
        return false;
    }

    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh2) && CGAL::Polygon_mesh_processing::does_bound_a_volume(cgal_mesh2)){
        printf("mesh2 self intersects!\n");
        return false;
    }

    bool result_valid;
    try{
        result_valid = CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(cgal_mesh1, cgal_mesh2, cgal_result);
    } catch (std::exception){
        result_valid = false;
    }

    if (!result_valid){
        printf("boolean operation failed!\n");
        return false;
    }

    auto fnormals = cgal_result.add_property_map<Face_index, Kernel::Vector_3>("f:normals", CGAL::NULL_VECTOR).first;
    CGAL::Polygon_mesh_processing::compute_face_normals(cgal_result, fnormals);

    result.cgal_mesh = cgal_result;
    result.name = "[Intersection]";
    result.mark_edited();
    return true;
}

bool Mesh::compute_union(const Mesh& mesh1, Transform* transform1,
                         const Mesh& mesh2, Transform* transform2, Mesh& result){
    CGAL_Mesh cgal_mesh1 = mesh1.cgal_mesh;
    CGAL_Mesh cgal_mesh2 = mesh2.cgal_mesh;
    CGAL_Mesh cgal_result;

    CGAL::Polygon_mesh_processing::transform(mat4_to_cgal_transform(transform1->get_local_matrix()), cgal_mesh1);
    CGAL::Polygon_mesh_processing::transform(mat4_to_cgal_transform(transform2->get_local_matrix()), cgal_mesh2);

    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh1) && CGAL::Polygon_mesh_processing::does_bound_a_volume(cgal_mesh1)){
        printf("mesh1 self intersects!\n");
        return false;
    }

    if (CGAL::Polygon_mesh_processing::does_self_intersect(cgal_mesh2) && CGAL::Polygon_mesh_processing::does_bound_a_volume(cgal_mesh2)){
        printf("mesh2 self intersects!\n");
        return false;
    }

    // auto is_constrained_map = cgal_result.add_property_map<Edge_index, bool>("e:is_constrained", false).first;
    //auto texcoord_map = cgal_result.add_property_map<Face_index, std::map<Vertex_index, vec2>>("f:texcoords", std::map<Vertex_index, vec2>()).first;

    bool result_valid;
    try{
        result_valid = CGAL::Polygon_mesh_processing::corefine_and_compute_union(cgal_mesh1, cgal_mesh2, cgal_result); //,
                                                                                                                            // CGAL::parameters::visitor(Visitor()),
                                                                                                                            // CGAL::parameters::default_values(),
                                                                                                                            // CGAL::parameters::edge_is_constrained_map(is_constrained_map));
    } catch (std::exception){
        result_valid = false;
    }

    if (!result_valid){
        printf("boolean operation failed!\n");
        // TODO : log창에 연산 결과 추가
        return false;
    }
    auto fnormals = cgal_result.add_property_map<Face_index, Kernel::Vector_3>("f:normals", CGAL::NULL_VECTOR).first;
    CGAL::Polygon_mesh_processing::compute_face_normals(cgal_result, fnormals);

    result.cgal_mesh = cgal_result;
    result.name = "[Union]";
    result.mark_edited();
    return true;
}
