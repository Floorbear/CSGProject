#include "Renderer.h"
#include "Core.h"
#include "Utils.h"
#include "Camera.h"
#include "Material.h"
#include "Model.h"
#include "WorkSpace.h"
#include "Texture.h"
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma warning(disable : 4717)

vec3 Renderer::lightPos = vec3(30, -100, -50);

Renderer::Renderer(int viewport_width, int viewport_height){
    parent = nullptr;
    camera = nullptr;

    texture_size = vec2(512, 512); // default
    viewport_size = vec2(viewport_width, viewport_height);
    camera = new Camera((float)viewport_width, (float)viewport_height);
}

Renderer::~Renderer(){
    delete camera;
}

void Renderer::set_parent(GUI* parent_){
    parent = parent_;
}

void Renderer::init(){
}

void Renderer::set_bind_fbo(int texture_width, int texture_height){ // TODO : 리팩토링
    if (fbo == 0){ // 지연 초기화
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenTextures(1, &frame_texture);
        glBindTexture(GL_TEXTURE_2D, frame_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, frame_texture);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_texture, 0);

        GLuint depthrenderbuffer;
        glGenRenderbuffers(1, &depthrenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texture_width, texture_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Renderer::render(const std::list<Model*>& models, RenderSpace space_){
    glViewport(0, 0, (GLsizei)texture_size.x, (GLsizei)texture_size.y);



    //픽킹텍스처
    static int pickingTest = 0;
    static unsigned int fbo = 0;
    static unsigned int texture = 0;
    static unsigned int depthTexture = 0;
    //===== init ======
    if (pickingTest == 0)    {
        pickingTest = 1;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        //glGenTextures(1, &texture);
        //glBindTexture(GL_TEXTURE_2D, texture);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, (int)texture_size.x, (int)texture_size.y, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        Texture* newTexture = Texture::create_frameTexture(ivec2((int)texture_size.x, (int)texture_size.y), GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT);
        Texture* newdepthTexture = Texture::create_depthTexture(ivec2((int)texture_size.x, (int)texture_size.y));

        //glGenTextures(1, &depthTexture);
        //glBindTexture(GL_TEXTURE_2D, depthTexture);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)texture_size.x, (int)texture_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

        auto Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        assert(Status == GL_FRAMEBUFFER_COMPLETE); //버퍼가 정상적으로 생성됬는지 체크

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    //===== if Pressed ======
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

       // glClearColor(0, 0, 0, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        //WorldTrans& worldTransform = pMesh->GetWorldTransform();
        //Matrix4f View = m_pGameCamera->GetMatrix();
        //Matrix4f Projection = m_pGameCamera->GetProjectionMat();

        //for (uint i = 0; i < (int)ARRAY_SIZE_IN_ELEMENTS(m_worldPos); i++) {
        //    worldTransform.SetPosition(m_worldPos[i]);
        //    // Background is zero, the real objects start at 1
        //    m_pickingEffect.SetObjectIndex(i + 1);
        //    Matrix4f World = worldTransform.GetMatrix();
        //    Matrix4f WVP = Projection * View * World;
        //    m_pickingEffect.SetWVP(WVP);
        //    pMesh->Render(&m_pickingEffect);
        //}
        for (Model* model : models){
            model->get_material()->set_uniform_transform(model->get_transform());
            model->get_material()->set_uniform_camera(camera);
            model->get_material()->set_uniform_selection_id(5);
            model->get_material()->apply_object_selection();
            model->render();
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    }

    // ===== Read Pixel ======
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

    glReadBuffer(GL_COLOR_ATTACHMENT0);

    PixelInfo Pixel;
    glReadPixels(256, 256, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &Pixel);

    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    if (Pixel.objectID == 5)    {
        //printf("%오브젝트 감지 \n", Pixel.objectID);

    }


    //===================================================

    set_bind_fbo((int)texture_size.x, (int)texture_size.y);

    ImVec4 clear_color = ImVec4(0.03f, 0.30f, 0.70f, 1.00f);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera->calculate_view();


    static Model* newModel = NULL;
    if (newModel == NULL){
        newModel = new Model("MyModel");

        newModel->set_new(Mesh::Cube);
        //newModel->set_new(Mesh::compute_intersection(Mesh::Cube2,Mesh::Cube));
        //newModel->set_new(Mesh::Sphere);

        parent->active_workspace->models.push_back(newModel);
        camera->get_transform()->set_position(vec3(0.0f, 0.0f, 20.0f));
        camera->get_transform()->set_rotation({0,-90,0});
    }


    Model* model = parent->active_workspace->find_model("MyModel");
    if (model != NULL){
        //CSGMesh* newMesh = 
        Transform* newMesh = model->get_transform();
        newMesh->set_position(vec3(0, 0, 2));
        newMesh->set_scale(vec3(1.5f, 1.0f, 0.5f));
    }

    lightPos.x = 50 * sin(Utils::time_acc());
    lightPos.z = 50 * sin(Utils::time_acc());

    for (Model* model : models){
        model->get_material()->set_uniform_transform(model->get_transform());
        model->get_material()->set_uniform_camera(camera);
        model->get_material()->set_uniform_lights(lightPos);
        model->get_material()->apply();
        model->render();
    }
}

void Renderer::dispose(){
}

void Renderer::resize(int width, int height){
    viewport_size = vec2(width, height);
    camera->resize((float)width, (float)height);
}

vec2 Renderer::get_mouse_position(){
    return vec2();
}
