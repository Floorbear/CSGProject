#pragma once

#include "Utils.h"
#include "Mesh.h"
#include "Transform.h"

enum class GizmoMode{
    Translation,
    Scale,
    Rotation,
    Max
};

enum class GizmoAxis{
    None = -1, X = 0, Y = 1, Z = 2, XYZ = 3, XY = 4, YZ = 5, ZX = 6,
};

class Shader;
class Camera;
class Gizmo{
    static bool gizmo_mesh_created;
    static void make_gizmo_mesh();

    // (0 : x), (1 : y), (2 : z), (3 : dot)
    static std::map<GizmoMode, std::vector<Mesh*>> gizmo_mesh;
    static std::vector<vec3> stick_zsort_position;

    static std::vector<vec3> colors;
    const static vec3 color_selected;

    static Shader* shader_screen;
    static Shader* shader_selection;

    GizmoMode gizmo_mode = GizmoMode::Translation;
    GizmoAxis selected_axis = GizmoAxis::None;

public:
    Gizmo();

    void render(Camera* camera, std::list<TransformEntity*> transform_entities);
    void render_selection(Camera* camera, std::list<TransformEntity*> transform_entities);

    GizmoMode get_gizmo_mode();
    void set_gizmo_mode(GizmoMode gizmo_mode_);
    void set_selected_axis(GizmoAxis axis);

    void move(Camera* _camera, std::list<TransformEntity*> transform_entities, vec2 _curPos, vec2 _prevPos, GizmoAxis axis);
private:
    void move_dot(Camera* _camera, std::list<TransformEntity*> transform_entities, glm::vec2& _curPos, glm::vec2& _prevPos);

    struct zSortStruct{
        int renderOrder = 0;
        float length = 0;
    };

    std::vector<int> zSort(std::vector<zSortStruct> _vector);
    std::vector<int> get_renderOrder(Camera* _camera, std::vector<vec3> _positionVector);
};

