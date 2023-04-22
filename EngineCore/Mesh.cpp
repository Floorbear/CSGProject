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

Mesh Mesh::Triangle = Mesh({Vertex(0.5f, -0.5f, 0.0f),
                                 Vertex(-0.5f, -0.5f, 0.0f),
                                 Vertex(0.0f,  0.5f, 0.0f)},
                                 {0, 1, 2});

Mesh Mesh::Square = Mesh({Vertex(-0.5f, -0.5f, 0.0f), // left down  
                               Vertex(0.5f, -0.5f, 0.0f), // right down
                               Vertex(-0.5f,  0.5f, 0.0f), // left top
                               Vertex(0.5f,  0.5f, 0.0f)}, // right top
                               {0, 1, 2, 2, 3, 1});

//Mesh Mesh::Cube = Mesh({
//                                Vertex(-1, -1, -1   ),
//                                Vertex(1, -1, -1    ),
//                                Vertex(1, 1, -1     ),
//                                Vertex(-1, 1, -1    ),
//                                Vertex(-1, -1, 1    ),
//                                Vertex(1, -1, 1     ),
//                                Vertex(1, 1, 1      ),
//                                Vertex(-1, 1, 1     )
//                                                     },
//                               {0, 1, 3, 3, 1, 2,
//                                1, 5, 2, 2, 5, 6,
//                                5, 4, 6, 6, 4, 7,
//                                4, 0, 7, 7, 0, 3,
//                                3, 2, 7, 7, 2, 6,
//                                4, 5, 0, 0, 5, 1});

Mesh Mesh::Cube = Mesh({
                                Vertex(-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f ),
                                Vertex( 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f ),
                                Vertex( 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f ),
                                Vertex( 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f ),
                                Vertex(-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f ),
                                Vertex(-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f ),
                                Vertex(-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f  ),
                                Vertex( 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f  ),
                                Vertex( 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f  ),
                                Vertex( 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f  ),
                                Vertex(-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f  ),
                                Vertex(-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f  ),
                                Vertex(-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f ),
                                Vertex(-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f ),
                                Vertex(-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f ),
                                Vertex(-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f ),
                                Vertex(-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f ),
                                Vertex(-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f ),
                                Vertex(0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f  ),
                                Vertex(0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f  ),
                                Vertex(0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f  ),
                                Vertex(0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f  ),
                                Vertex(0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f  ),
                                Vertex(0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f  ),
                                Vertex(-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f ),
                                Vertex( 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f ),
                                Vertex( 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f ),
                                Vertex( 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f ),
                                Vertex(-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f ),
                                Vertex(-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f ),
                                Vertex(-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f ),
                                Vertex( 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f ),
                                Vertex( 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f ),
                                Vertex( 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f ),
                                Vertex(-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f ),
                                Vertex(-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f )

    },
    { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35 });

/*
*   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
*/
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
        //glEnableVertexAttribArray(1); //Vertex Color Data
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
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