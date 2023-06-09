#include "Gizmo.h"
#include "Shader.h"
#include "Camera.h"
#include "FileSystem.h"
#include "SelectionPixelInfo.h"

#include <glad/glad.h>

#include <algorithm>

Gizmo::EntityInfo::EntityInfo(TransformEntity* entity_, ComponentSnapshot* captured_component_, Transform captured_transform_) :
    entity(entity_), captured_component(captured_component_), captured_transform(captured_transform_){
}

bool Gizmo::gizmo_mesh_created = false;

std::map<GizmoMode, std::vector<Mesh*>> Gizmo::gizmo_mesh;
std::vector<vec3> Gizmo::stick_zsort_position;

std::vector<vec3> Gizmo::colors;
const vec3 Gizmo::color_selected = {1.f,1.f,0.f};

Shader* Gizmo::shader_screen = nullptr;
Shader* Gizmo::shader_selection = nullptr;

void Gizmo::make_gizmo_mesh(){
    Mesh cone = Mesh::cone(0.5, 1, 8);
    Mesh cube = Mesh::cube(1);
    Mesh torus = Mesh::torus(1, 0.03f, 20, 20);

    float value_big, value_small;
    float dot_scale = 0.13f;

    Transform stick_x, stick_y, stick_z;
    Transform arrow_cone_x, arrow_cone_y, arrow_cone_z;
    Transform arrow_cube_x, arrow_cube_y, arrow_cube_z;
    Transform torus_x, torus_y, torus_z;

    // 메쉬 제작때 활용할 Transform 추가
    {
        value_big = 0.63f;
        stick_x.set_position({value_big, 0, 0});
        stick_y.set_position({0, value_big, 0});
        stick_z.set_position({0, 0, value_big});

        value_big = 1.2f;
        value_small = 0.13f;
        stick_x.set_scale({value_big, value_small, value_small});
        stick_y.set_scale({value_small, value_big, value_small});
        stick_z.set_scale({value_small, value_small, value_big});

        value_big = 1.2f;
        arrow_cone_x.set_position({value_big, 0, 0});
        arrow_cone_y.set_position({0, value_big, 0});
        arrow_cone_z.set_position({0, 0, value_big});

        value_big = 0.5f;
        value_small = 0.3f;
        arrow_cone_x.set_scale({value_small, value_big, value_small});
        arrow_cone_y.set_scale({value_small, value_big, value_small});
        arrow_cone_z.set_scale({value_small, value_big, value_small});

        arrow_cone_x.set_rotation({0, 0, -90});
        arrow_cone_y.set_rotation({0, 0, 0});
        arrow_cone_z.set_rotation({90, 0, 0});

        value_big = 1.2f;
        arrow_cube_x.set_position({value_big, 0, 0});
        arrow_cube_y.set_position({0, value_big, 0});
        arrow_cube_z.set_position({0, 0, value_big});

        value_big = 0.3f;
        value_small = 0.3f;
        arrow_cube_x.set_scale({value_big, value_small, value_small});
        arrow_cube_y.set_scale({value_small, value_big, value_small});
        arrow_cube_z.set_scale({value_small, value_small, value_big});

        torus_x.set_rotation({0, 0, 90});
        torus_y.set_rotation({0, 0, 0});
        torus_z.set_rotation({90, 0, 0});
    }

    Mesh dot;
    Mesh translate_x, translate_y, translate_z;
    Mesh rotation_x, rotation_y, rotation_z;
    Mesh scale_x, scale_y, scale_z;

    // 메쉬 제작
    {
        dot = Mesh::cube(dot_scale);

        Mesh::compute_union(cube, &stick_x, cone, &arrow_cone_x, translate_x);
        Mesh::compute_union(cube, &stick_y, cone, &arrow_cone_y, translate_y);
        Mesh::compute_union(cube, &stick_z, cone, &arrow_cone_z, translate_z);

        Mesh::compute_union(cube, &stick_x, cube, &arrow_cube_x, scale_x);
        Mesh::compute_union(cube, &stick_y, cube, &arrow_cube_y, scale_y);
        Mesh::compute_union(cube, &stick_z, cube, &arrow_cube_z, scale_z);

        Mesh::transform(torus, &torus_x, rotation_x);
        Mesh::transform(torus, &torus_y, rotation_y);
        Mesh::transform(torus, &torus_z, rotation_z);
    }

    // 메쉬 추가
    {
        gizmo_mesh[GizmoMode::Translation].push_back(new Mesh(translate_x));
        gizmo_mesh[GizmoMode::Translation].push_back(new Mesh(translate_y));
        gizmo_mesh[GizmoMode::Translation].push_back(new Mesh(translate_z));
        gizmo_mesh[GizmoMode::Translation].push_back(new Mesh(dot));

        gizmo_mesh[GizmoMode::Scale].push_back(new Mesh(scale_x));
        gizmo_mesh[GizmoMode::Scale].push_back(new Mesh(scale_y));
        gizmo_mesh[GizmoMode::Scale].push_back(new Mesh(scale_z));
        gizmo_mesh[GizmoMode::Scale].push_back(new Mesh(dot));

        gizmo_mesh[GizmoMode::Rotation].push_back(new Mesh(rotation_x));
        gizmo_mesh[GizmoMode::Rotation].push_back(new Mesh(rotation_y));
        gizmo_mesh[GizmoMode::Rotation].push_back(new Mesh(rotation_z));
        gizmo_mesh[GizmoMode::Rotation].push_back(new Mesh());
    }

    // 색 추가
    {
        colors.push_back({1, 0, 0});
        colors.push_back({0, 1, 0});
        colors.push_back({0, 0, 1});
        colors.push_back({1, 1, 1});
    }

    // z sort를 위한 데이터 추가
    stick_zsort_position.push_back(vec3(1, 0, 0));
    stick_zsort_position.push_back(vec3(0, 1, 0));
    stick_zsort_position.push_back(vec3(0, 0, 1));

    gizmo_mesh_created = true;
}

Gizmo::Gizmo(){
    if (!gizmo_mesh_created){
        make_gizmo_mesh();
    }

    if (shader_screen == nullptr){
        shader_screen = new Shader("DefaultVertexShader.glsl", "GizmoFragmentShader.glsl");
        shader_selection = new Shader("DefaultVertexShader.glsl", "SelectionFragmentShader.glsl");
    }
}

void Gizmo::render(Camera* camera, std::list<TransformEntity*> transform_entities){
    shader_screen->use();

    std::vector<int> render_order;
    if (gizmo_mode == GizmoMode::Rotation){
        glEnable(GL_DEPTH_TEST);
        render_order = {0, 1, 2, 3};
    } else{
        glDisable(GL_DEPTH_TEST);
        render_order = get_renderOrder(camera, Gizmo::stick_zsort_position);
    }

    for (TransformEntity* transform_entity : transform_entities){
        for (int i = 0; i < render_order.size(); i++){
            transform_entity->get_transform()->calculate_matrix();

            Transform newTransform = transform_entity->get_transform()->get_world_transform();
            // ortho gizmo
            // float depth = glm::dot(camera->get_transform()->get_forward_dir(), transform_entity->get_transform()->get_position() - camera->get_transform()->get_position()) / 15.0; // TODO : 상수 추출
            float depth = 1.0f;
            newTransform.set_scale(vec3(depth, depth, depth));

            shader_screen->set_mat4("world", newTransform.get_world_matrix());
            shader_screen->set_mat4("view", camera->get_view());
            shader_screen->set_mat4("projection", camera->get_projection());

            if (render_order[i] == (int)selected_axis){
                shader_screen->set_vec3("gizmoColor", Gizmo::color_selected);
            } else{
                shader_screen->set_vec3("gizmoColor", Gizmo::colors[render_order[i]]);
            }

            gizmo_mesh[gizmo_mode][render_order[i]]->render();
        }
    }
    glEnable(GL_DEPTH_TEST);
}

void Gizmo::render_selection(Camera* camera, std::list<TransformEntity*> transform_entities){
    shader_selection->use();

    std::vector<int> render_order;
    if (gizmo_mode == GizmoMode::Rotation){
        glEnable(GL_DEPTH_TEST);
        render_order = {0, 1, 2, 3};
    } else{
        glDisable(GL_DEPTH_TEST);
        render_order = get_renderOrder(camera, Gizmo::stick_zsort_position);
    }

    for (TransformEntity* transform_entity : transform_entities){
        for (int i = 0; i < render_order.size(); i++){
            transform_entity->get_transform()->calculate_matrix();

            Transform newTransform = transform_entity->get_transform()->get_world_transform();
            // ortho gizmo
            // float depth = glm::dot(camera->get_transform()->get_forward_dir(), transform_entity->get_transform()->get_position() - camera->get_transform()->get_position()) / 15.0; // TODO : 상수 추출
            float depth = 1.0f;
            newTransform.set_scale(vec3(depth, depth, depth));

            shader_selection->set_mat4("world", newTransform.get_world_matrix());
            shader_selection->set_mat4("view", camera->get_view());
            shader_selection->set_mat4("projection", camera->get_projection());

            shader_selection->set_uint("objectType", SelectionPixelInfo::object_type_gizmo);
            shader_selection->set_uint("modelID", render_order[i]);
            shader_selection->set_uint("meshID", 0);

            gizmo_mesh[gizmo_mode][render_order[i]]->render();
        }
    }
    glEnable(GL_DEPTH_TEST);
}

GizmoMode Gizmo::get_gizmo_mode(){
    return gizmo_mode;
}

void Gizmo::set_gizmo_mode(GizmoMode gizmo_mode_){
    gizmo_mode = gizmo_mode_;
}

void Gizmo::set_selected_axis(GizmoAxis axis){
    selected_axis = axis;
}

void Gizmo::on_click(std::list<TransformEntity*> transform_entities, GizmoAxis axis){
    clicked_axis = axis;
    clicked_entities.clear();
    for (TransformEntity* transform_entity : transform_entities){
        clicked_entities.push_back(EntityInfo(transform_entity,
                                              transform_entity->get_transform()->make_snapshot_new(),
                                              transform_entity->get_transform()->get_value()));
    }
}


void Gizmo::on_move(Camera* camera, vec2 cursor_click_position, vec2 cursor_position, GizmoAxis axis){
    set_selected_axis(axis);
    vec2 cursor_delta = cursor_position - cursor_click_position;
    if (abs(length(cursor_delta)) < 0.01f){
        return;
    }

    if (axis == GizmoAxis::XYZ){
        move_dot(camera, cursor_delta);
        return;
    }

    for (EntityInfo entity_info : clicked_entities){
        TransformEntity* entity = entity_info.entity;

        /*vec2 parentScreenPositon = camera->world_position_to_screen_position(entity->get_transform()->get_position());
        vec2 axisScreenPosition = camera->world_position_to_screen_position(entity->get_transform()->get_position() + stick_zsort_position[(int)axis]);
        vec2 screenVector = axisScreenPosition - parentScreenPositon;
        if (length(screenVector) < 0.01f){
            return;
        }
        screenVector = normalize(screenVector);
        vec2 mouseVector = cursor_delta;
        mouseVector.y = -mouseVector.y;

        float moveForce = dot(screenVector, mouseVector);*/

        vec2 screen_position_start = camera->world_position_to_screen_position(entity_info.captured_transform.get_position());
        vec2 screen_position_end = camera->world_position_to_screen_position(entity_info.captured_transform.get_position() + stick_zsort_position[(int)axis]);
        vec2 screen_vector = screen_position_end - screen_position_start;

        float projected_cursor_delta_length = dot(cursor_delta, normalize(screen_vector));

        float factor = projected_cursor_delta_length / length(screen_vector);

        switch (gizmo_mode){
            case GizmoMode::Translation:
                vec3 entity_position_next = entity_info.captured_transform.get_position() + stick_zsort_position[(int)axis] * factor; // TODO : z축 각도 고려해서 구하거나 알고리즘 바꾸기
                entity->get_transform()->set_position(entity_position_next);
                break;
            case GizmoMode::Scale:
                vec3 entity_scale_next = entity_info.captured_transform.get_scale() + stick_zsort_position[(int)axis] * factor; // TODO : z축 각도 고려해서 구하거나 알고리즘 바꾸기
                entity->get_transform()->set_scale(entity_scale_next);
                break;
            case GizmoMode::Rotation: // TODO : 정확한 각도로 계산!
                vec2 click_direction_vector = normalize(cursor_click_position - screen_position_start);
                vec2 direction_vector = normalize(cursor_position - screen_position_start);

                float click_direction = 360 / (M_PI * 2) * atan2(-click_direction_vector.y, click_direction_vector.x);
                float direction = 360 / (M_PI * 2) * atan2(-direction_vector.y, direction_vector.x);

                vec3 entity_rotation_next = entity_info.captured_transform.get_rotation() + stick_zsort_position[(int)axis] * (direction - click_direction);
                entity->get_transform()->set_rotation(entity_rotation_next);
        }
    }
}

TransactionTask* Gizmo::on_release_task_new(){
    MultiTransactionTask* multi_task = new MultiTransactionTask("Transform Edit");
    for (EntityInfo entity_info : clicked_entities){
        multi_task->add_task(new ComponentModifyTask("Transform Edit", entity_info.captured_component, entity_info.entity->get_transform()->make_snapshot_new()));
    }
    clicked_entities.clear();
    return multi_task;
}

void Gizmo::move_dot(Camera* _camera, glm::vec2& cursor_delta){
    vec3 moveRightVector = _camera->get_transform()->get_right_dir();
    vec3 moveUpVecotr = _camera->get_transform()->get_up_dir();

    vec2 mouseVector = cursor_delta;
    mouseVector.y = -mouseVector.y;

    vec3 moveDirVector = moveRightVector * mouseVector.x + moveUpVecotr * mouseVector.y;

    //카메라가 멀어지면 이동값이 크고 카메라와 오브젝트가 가까우면 이동값이 작아야한다.
    //이때 이동값이 일정하기 위해서는 내적을 해야한다.!
    for (EntityInfo entity_info : clicked_entities){
        float speed = 0.8f * dot(entity_info.entity->get_transform()->get_position() - _camera->get_transform()->get_position(), _camera->get_transform()->get_forward_dir());
        entity_info.entity->get_transform()->add_position(Utils::time_delta() * moveDirVector * speed);
    }

}

std::vector<int> Gizmo::zSort(std::vector<zSortStruct> _vector){
    std::sort(_vector.begin(), _vector.end(), [](zSortStruct _a, zSortStruct _b){
        return _a.length > _b.length;
    });

    std::vector<int> result;
    for (int i = 0; i < 3; i++){
        result.push_back(_vector[i].renderOrder);
    }
    return result;
}

std::vector<int> Gizmo::get_renderOrder(Camera* _camera, std::vector<vec3> _positionVector){
    //Set ZOrder
    std::vector<zSortStruct> newZSortStructVector;
    for (int i = 0; i < 3; i++){
        zSortStruct newZSortStruct;
        newZSortStruct.length = length(_positionVector[i] - _camera->get_transform()->get_position());
        newZSortStruct.renderOrder = i;
        newZSortStructVector.push_back(newZSortStruct);
    }
    std::vector<int> renderOrder = zSort(newZSortStructVector);
    renderOrder.push_back(3);
    return renderOrder;
}
