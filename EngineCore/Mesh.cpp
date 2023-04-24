#include "Mesh.h"
#include "Shader.h"

// ===== Vertex ===== //

Vertex::Vertex(const vec3& position_) : position(position_),normal() {
}

Vertex::Vertex(float x, float y, float z) : position(vec3(x, y, z)){
}

Vertex::Vertex(float x, float y, float z, float normal_x_, float normal_y_, float normal_z_)
    : position(vec3(x, y, z)),
    normal(vec3(normal_x_, normal_y_, normal_z_))

{
}


// ===== Mesh ===== //
cgal_Mesh Mesh::_cgal_Cube = Mesh::create_cgal_cube(2.0f);
cgal_Mesh Mesh::_cgal_Cube2 = Mesh::create_cgal_cube(4.0f);
cgal_Mesh Mesh::_cgal_Sphere = Mesh::create_cgal_sphere(2,10,30);
Mesh Mesh::Triangle = Mesh({Vertex(0.5f, -0.5f, 0.0f),
                                 Vertex(-0.5f, -0.5f, 0.0f),
                                 Vertex(0.0f,  0.5f, 0.0f)},
                                 {0, 1, 2});

Mesh Mesh::Square = Mesh({Vertex(-0.5f, -0.5f, 0.0f), // left down  
                               Vertex(0.5f, -0.5f, 0.0f), // right down
                               Vertex(-0.5f,  0.5f, 0.0f), // left top
                               Vertex(0.5f,  0.5f, 0.0f)}, // right top
                               {0, 1, 2, 2, 3, 1});



Mesh Mesh::Cube = Mesh::cgal_mesh_to_mesh( Mesh::mesh_to_cgal_mesh( Mesh::cgal_mesh_to_mesh(Mesh::_cgal_Cube) ) );
Mesh Mesh::Cube2 = Mesh::cgal_mesh_to_mesh(Mesh::mesh_to_cgal_mesh(Mesh::cgal_mesh_to_mesh(Mesh::_cgal_Cube2)));
Mesh Mesh::Sphere = Mesh::cgal_mesh_to_mesh(Mesh::mesh_to_cgal_mesh(Mesh::cgal_mesh_to_mesh(Mesh::_cgal_Sphere)));

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

Mesh::Mesh(){
    is_buffer_created = false;
}

Mesh::Mesh(const Mesh& ref) : vertices(ref.vertices), indices(ref.indices){
    is_buffer_created = false;
}

Mesh::Mesh(const Mesh* mesh) : vertices(mesh->vertices), indices(mesh->indices){
    is_buffer_created = false;
}

Mesh::Mesh(std::vector<Vertex> vertices_, std::vector<unsigned int> indices_) : vertices(vertices_), indices(indices_){
    is_buffer_created = false;
}

Mesh::~Mesh(){
    if(VAO != 0){
        glDeleteVertexArrays(1, &VAO);
    }
    if(VBO != 0){
        glDeleteBuffers(1, &VBO);
    }
    if(EBO != 0){
        glDeleteBuffers(1, &EBO);
    }
}
// TODO : vertex, index 하나만 변경되어도 is_buffer_valid = false 해야한다!

Mesh* Mesh::clone_new() const{
    return new Mesh(this);
}

void Mesh::render(){
    // 버퍼 지연 초기화
    if (!is_buffer_created){
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        is_buffer_created = true;

        buffers_bind();

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

        // Linking Vertex Attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1); //Vertex normal Data
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Vertex::normal)));
    }

    //transform.AddLocalRotation(glm::vec3(0.05f, 0.f, 0.f)); 
    //transform.SetLocalPosition(glm::vec3(sin(glfwGetTime()), 0, 0)); //
    //transform.SetLocalScale(glm::vec3(cos(glfwGetTime()), cos(glfwGetTime()), 0)); //

    //int transformLocation = glGetUniformLocation(shader->GetShaderProgram(),"transform");
    //glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(*transform.GetTransformMat()));

    //int uniformLocation = glGetUniformLocation(shaderProgram, "ourColor");
    //float timeValue = (sin(glfwGetTime()) / 0.5f) + 0.5f;
    //glUniform4f(uniformLocation, timeValue, 0.1f, 0.1f, 1.0f);

    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
}
cgal_Mesh Mesh ::create_cgal_cube(float size)
{
    cgal_Mesh cgal_Cube;
    std::vector<vertex_descriptor> vd;

    vd.push_back(cgal_Cube.add_vertex(K::Point_3(0.0f, 0.0f, 0.0f)));
    vd.push_back(cgal_Cube.add_vertex(K::Point_3(0.0f, size, 0.0f)));
    vd.push_back(cgal_Cube.add_vertex(K::Point_3(size, size, 0.0f)));
    vd.push_back(cgal_Cube.add_vertex(K::Point_3(size, 0.0f, 0.0f)));
    vd.push_back(cgal_Cube.add_vertex(K::Point_3(0.0f, 0.0f, size)));
    vd.push_back(cgal_Cube.add_vertex(K::Point_3(0.0f, size, size)));
    vd.push_back(cgal_Cube.add_vertex(K::Point_3(size, size, size)));
    vd.push_back(cgal_Cube.add_vertex(K::Point_3(size, 0.0f, size)));


    face_descriptor f1 = cgal_Cube.add_face(vd[0], vd[3], vd[2]);
    face_descriptor f2 = cgal_Cube.add_face(vd[1], vd[0], vd[2]);
    face_descriptor f3 = cgal_Cube.add_face(vd[3], vd[0], vd[4]);
    face_descriptor f4 = cgal_Cube.add_face(vd[3], vd[4], vd[7]);
    face_descriptor f5 = cgal_Cube.add_face(vd[1], vd[2], vd[5]);
    face_descriptor f6 = cgal_Cube.add_face(vd[5], vd[2], vd[6]);
    face_descriptor f7 = cgal_Cube.add_face(vd[5], vd[6], vd[7]);
    face_descriptor f8 = cgal_Cube.add_face(vd[5], vd[7], vd[4]);
    face_descriptor f9 = cgal_Cube.add_face(vd[2], vd[3], vd[7]);
    face_descriptor f10 = cgal_Cube.add_face(vd[2], vd[7], vd[6]);
    face_descriptor f11 = cgal_Cube.add_face(vd[0], vd[1], vd[4]);
    face_descriptor f12 = cgal_Cube.add_face(vd[4], vd[1], vd[5]);   

    return cgal_Cube;
}

cgal_Mesh Mesh::create_cgal_sphere(int radius, int stackCount, int sectorCount)
{
    cgal_Mesh m;
    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    std::vector<vertex_descriptor> vd;
    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)          
            vd.push_back(m.add_vertex(K::Point_3(x, y, z)));
         
        }

    }


    int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
                m.add_face(vd[k1], vd[k2], vd[k1 + 1]);

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
                m.add_face(vd[k1 + 1], vd[k2], vd[k2 + 1]);

            /*cout << "k1: " << vd[k1] << "k2: " << vd[k2] << "vd[k1+1]: " << vd[k1 + 1] << "\n";
            cout << "k1+1: " << vd[k1+1] << "k2: " << vd[k2] << "vd[k2+1]: " << vd[k2 + 1] << "\n";*/
        }
        std::cout << "\n\n";
    }

    return m;
}

Mesh Mesh::cgal_mesh_to_mesh(cgal_Mesh cg_Mesh)
{
    std::vector<Vertex> vertices;
    for (cgal_Mesh::Vertex_index vi : cg_Mesh.vertices())
    {
        //uint32_t index = vd; //via size_t
        float v_x = (float)(cg_Mesh).point(vi).x();
        float v_y = (float)(cg_Mesh).point(vi).y();
        float v_z = (float)(cg_Mesh).point(vi).z();       
        vertices.push_back({ v_x, v_y, v_z });
    }

    std::vector<unsigned int> indices;
    for (cgal_Mesh::Face_index face_index : cg_Mesh.faces()) {
        CGAL::Vertex_around_face_circulator<cgal_Mesh> vcirc(cg_Mesh.halfedge(face_index), cg_Mesh), done(vcirc);
        do
        {            
            indices.push_back(*vcirc++);
        } while (vcirc != done);
    }
   
    return Mesh(vertices,indices);
}


cgal_Mesh Mesh::mesh_to_cgal_mesh(Mesh m)
{
    cgal_Mesh cg_Mesh;
    int vd_size = m.vertices.size();
    int fd_size = m.indices.size() / 3;
    std::vector<vertex_descriptor> vd;
    std::vector<face_descriptor> fd;
  
    for (int i = 0; i < vd_size; i++)
    {      
        vd.push_back(cg_Mesh.add_vertex(K::Point_3(m.vertices[i].position.x, m.vertices[i].position.y, m.vertices[i].position.z)));        
    }

    std::cout << "----------------------" << "\n";
    for (cgal_Mesh::Vertex_index vi : cg_Mesh.vertices())
    {
        //uint32_t index = vd; //via size_t
        float v_x = (float)(cg_Mesh).point(vi).x();
        float v_y = (float)(cg_Mesh).point(vi).y();
        float v_z = (float)(cg_Mesh).point(vi).z();

        std::cout << v_x << "      " <<  v_y << "      " << v_z << "\n";
    }
    std::cout << "----------------------" << "\n";

    int j = 0;
    for (int i = 0; i < fd_size; i++)
    {
        int v1_idx = m.indices[j];
        int v2_idx = m.indices[j+1];
        int v3_idx = m.indices[j+2];
        fd.push_back(cg_Mesh.add_face(vd[v1_idx], vd[v2_idx], vd[v3_idx]));
        j = j + 3;
    }

    return cg_Mesh;
}

Mesh Mesh::compute_intersection(Mesh m1, Mesh m2)
{
    bool check;
    cgal_Mesh m1_m2_intersection;
    cgal_Mesh cg_m1 = mesh_to_cgal_mesh(m1);
    cgal_Mesh cg_m2 = mesh_to_cgal_mesh(m2);
    check = CGAL::Polygon_mesh_processing::does_self_intersect(cg_m1);
    if (check == true) std::cout << "true" << "\n";
    else std::cout << "false" << "\n";
    check = CGAL::Polygon_mesh_processing::does_self_intersect(cg_m2);
    if (check == true) std::cout << "true" << "\n";
    else std::cout << "false" << "\n";


    std::cout << "---------------------test1------------------" << "\n";
    CGAL::Polygon_mesh_processing::corefine_and_compute_difference(cg_m1, cg_m2, m1_m2_intersection);
    std::cout << "---------------------test1------------------" << "\n";

    return Mesh::cgal_mesh_to_mesh(Mesh::mesh_to_cgal_mesh(Mesh::cgal_mesh_to_mesh(m1_m2_intersection)));;
}

