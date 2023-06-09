#include "WorkSpace.h"
#include "Core.h"
#include "Utils.h"
#include "Camera.h"
#include "Model.h"
#include "CSGNode.h"
#include "Renderer.h"
#include "Texture.h"
#include "PointLight.h"

int WorkSpace::id_counter = 1; // 1부터 시작

WorkSpace::WorkSpace(GUI* parent_) : WorkSpace(parent_, std::string("test")){
}

WorkSpace::WorkSpace(GUI* parent_, std::string title_) : parent(parent_), title(title_), actions(WorkSpace_Actions(this)){
    id = id_counter++;

    renderers.push_back(renderer_focused = new Renderer(512, 512));
    renderer_focused->set_parent(this);

    root_model = new Model("Root");
    root_model->clear_components();
    root_model->add_component(new PointLight(this, vec3(12, -10, 22))); // TODO : value 바꿔!
    // light->set_position(vec3(50 * sin(Utils::time_acc()), 0, 50 * sin(Utils::time_acc())));
}

WorkSpace::~WorkSpace(){
    // TODO 
    // -> ImGui::DisposeWindow(Utils::format("View##%1%", workspace.id).c_str());
    // -> ImGui::DisposeWindow(Utils::format("Objects##%1%", workspace.id).c_str());
    // -> ImGui::DisposeWindow(Utils::format("Inspector##%1%", workspace.id).c_str());
    // -> ImGui::DisposeWindow(Utils::format("Logs##%1%", workspace.id).c_str());
    // void ImGui::DisposeWindow(){
    // (windows에서 제거,, windowsbyid에서 제거. frame end에서 실행)
    //}

    for (Renderer* renderer : renderers){
        delete renderer;
    }
    delete root_model;
}

Camera* WorkSpace::get_main_camera(){
    assert(renderer_focused != nullptr);
    return renderer_focused->camera;
}

Model* WorkSpace::find_model(std::string_view name){
    return root_model->find_model(name);
}

std::list<PointLight*>* WorkSpace::get_lights(){
    return &lights;
}

bool is_window_content_hovered(){
    return ImGui::IsWindowHovered() && (ImGui::IsWindowDocked() || !ImGui::GetCurrentWindow()->TitleBarRect().Contains(ImGui::GetMousePos()));
}

ImVec4 ImVec4_add(ImVec4 a, ImVec4 b){
    return ImVec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

std::list<TransformEntity*> to_transform_entities(std::list<Model*>& models){
    std::list<TransformEntity*> ret;
    for (Model* model : models){
        ret.push_back(model);
    }
    return ret;
}

std::list<TransformEntity*> to_transform_entities(std::list<CSGNode*>& meshes){
    std::list<TransformEntity*> ret;
    for (CSGNode* mesh : meshes){
        ret.push_back(mesh);
    }
    return ret;
}

void WorkSpace::render_view(Renderer* renderer){
    main_renderer = renderer;
    // https://stackoverflow.com/questions/60955993/how-to-use-opengl-glfw3-render-in-a-imgui-window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(renderer->get_viewport_size().x, renderer->get_viewport_size().y), ImGuiCond_FirstUseEver);
    ImGui::Begin(Utils::format("View##%1%", renderer->get_id()).c_str(), 0, ImGuiWindowFlags_NoCollapse);
    vec2 p_min = vec2(ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x,
                      ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y);
    vec2 p_max = vec2(ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x,
                      ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMax().y);
    renderer->resize(p_max - p_min);
    renderer->render(root_model->get_children(), &lights);

    // view 마우스 좌표 측정
    mouse_pos_left_current_view = vec2(ImGui::GetMousePos().x - p_min.x, ImGui::GetMousePos().y - p_min.y);
    if (is_window_content_hovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
        renderer_focused = renderer;
        mouse_pos_left_press_view = mouse_pos_left_current_view;
    }

    // 오브젝트 피킹 (클릭시 기즈모, 모델,메쉬 선택, 호버링시 기즈모 하이라이트)]
    if (is_window_content_hovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
        SelectionPixelObjectInfo info;
        if (selection_mode == SelectionMode::Model){
            info = renderer->find_selection_gizmo(to_transform_entities(selected_models), mouse_pos_left_current_view);
        } else if (selection_mode == SelectionMode::Mesh){
            info = renderer->find_selection_gizmo(to_transform_entities(selected_meshes), mouse_pos_left_current_view);
        }

        if (info.object_type == SelectionPixelInfo::object_type_gizmo){
            is_gizmo_pressed = true;
            std::list<TransformEntity*> selected_entities; // TODO : selected_enties로 바꾸기
            if (selection_mode == SelectionMode::Model){
                for (Model* selected_model : selected_models){
                    bool contains_parent = false;
                    for (Model* possible_parent : selected_models){
                        if (possible_parent != selected_model && selected_model->is_descendant_of(possible_parent)){
                            contains_parent = true;
                            break;
                        }
                    }
                    if (!contains_parent){
                        selected_entities.push_back(selected_model);
                    }
                }
            } else if (selection_mode == SelectionMode::Mesh){
                for (CSGNode* selected_mesh : selected_meshes){
                    bool contains_parent = false;
                    for (CSGNode* possible_parent : selected_meshes){
                        if (possible_parent != selected_mesh && selected_mesh->is_descendant_of(possible_parent)){
                            contains_parent = true;
                            break;
                        }
                    }
                    if (!contains_parent){
                        selected_entities.push_back(selected_mesh);
                    }
                }
            }
            renderer->gizmo->on_click(selected_entities, info.gizmo_axis);
            gizmo_on_move = [=, this](Camera* camera, vec2 cursor_click_position, vec2 cursor_position){
                renderer->gizmo->on_move(camera, cursor_click_position, cursor_position, info.gizmo_axis);
            };
        } else{
            SelectionPixelObjectInfo info = renderer->find_selection_objects(root_model->get_children(), mouse_pos_left_current_view);

            if (info.empty()){
                is_background_pressed = true;
                selected_models.clear();
                selected_meshes.clear();

            } else if (info.object_type == SelectionPixelInfo::object_type_object){
                if (selection_mode == SelectionMode::Model){
                    if (ImGui::GetIO().KeyCtrl){
                        if (Utils::contains(selected_models, info.model)){
                            selected_models.remove(info.model);
                        } else{
                            selected_models.push_back(info.model);
                        }
                    } else{
                        selected_models.clear();
                        selected_models.push_back(info.model);
                    }
                } else if (selection_mode == SelectionMode::Mesh){
                    if (ImGui::GetIO().KeyCtrl){
                        if (Utils::contains(selected_meshes, info.mesh)){
                            selected_meshes.remove(info.mesh);
                        } else{
                            selected_meshes.push_back(info.mesh);
                        }
                    } else{
                        selected_meshes.clear();
                        selected_meshes.push_back(info.mesh);
                    }
                }
            }
        }

    } else if (is_window_content_hovered() && !ImGui::IsMouseDown(ImGuiButtonFlags_MouseButtonLeft)){
        if (!is_gizmo_pressed){
            SelectionPixelObjectInfo info;
            if (selection_mode == SelectionMode::Model){
                info = renderer->find_selection_gizmo(to_transform_entities(selected_models), mouse_pos_left_current_view);
            } else if (selection_mode == SelectionMode::Mesh){
                info = renderer->find_selection_gizmo(to_transform_entities(selected_meshes), mouse_pos_left_current_view);
            }
            renderer->gizmo->set_selected_axis(info.gizmo_axis); // None까지 같이 처리 (info.object_type 검사 불필요)
        }
    }

    if (selection_mode == SelectionMode::Model){
        renderer->render_outline(selected_models);
    } else if (selection_mode == SelectionMode::Mesh){
        renderer->render_mesh_overlay_and_outline(root_model->get_children(), selected_meshes, &lights);
    }

    // Gui 렌더링
    #pragma warning(disable: 4312)
    ImGui::GetWindowDrawList()->AddImage((void*)renderer->framebuffer_screen->
                                         get_texture()->get_handle(),
                                         ImVec2(p_min.x, p_min.y), ImVec2(p_max.x, p_max.y),
                                         ImVec2(0, 0), ImVec2(1, 1));
    ImGui::PopStyleVar();
    render_popup_menu_view(renderer);
    ImGui::End();

    {
        ImGui::PushID(1);
        ImVec4* colors = ImGui::GetStyle().Colors;
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.06f, 0.06f, 0.06f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.22f, 0.48f, 0.80f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.26f, 0.59f, 0.98f, 1.0f));

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(colors[ImGuiCol_WindowBg].x, colors[ImGuiCol_WindowBg].y, colors[ImGuiCol_WindowBg].z, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));

        ImGui::SetNextWindowPos(ImVec2(p_min.x + 10, p_min.y + 10));
        ImGui::Begin(Utils::format("View_Floating##%1%", renderer->get_id()).c_str(), 0,
                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        static Texture* button_image_model_mode = nullptr;
        static Texture* button_image_mesh_mode = nullptr;
        static Texture* button_image_gizmo_translate = nullptr;
        static Texture* button_image_gizmo_rotate = nullptr;
        static Texture* button_image_gizmo_scale = nullptr;
        if (button_image_model_mode == nullptr){
            button_image_model_mode = Texture::create_texture(EnginePath::get_texture_path().move("button_image_model_mode.png"), GL_RGBA8, GL_RGBA, false);
            button_image_mesh_mode = Texture::create_texture(EnginePath::get_texture_path().move("button_image_mesh_mode.png"), GL_RGBA8, GL_RGBA, false);
            button_image_gizmo_translate = Texture::create_texture(EnginePath::get_texture_path().move("button_image_gizmo_translate.png"), GL_RGBA8, GL_RGBA, false);
            button_image_gizmo_rotate = Texture::create_texture(EnginePath::get_texture_path().move("button_image_gizmo_rotate.png"), GL_RGBA8, GL_RGBA, false);
            button_image_gizmo_scale = Texture::create_texture(EnginePath::get_texture_path().move("button_image_gizmo_scale.png"), GL_RGBA8, GL_RGBA, false);
        }
        if (ImGui::ImageButton("Selection_Mode_Model", (ImTextureID)button_image_model_mode->get_handle(), selection_mode == SelectionMode::Model, ImVec2(25, 25))){
            selected_meshes.clear();
            selection_mode = SelectionMode::Model;
        }
        ImGui::SameLine();
        if (ImGui::ImageButton("Selection_Mode_Mesh", (ImTextureID)button_image_mesh_mode->get_handle(), selection_mode == SelectionMode::Mesh, ImVec2(25, 25))){
            selected_models.clear();
            selection_mode = SelectionMode::Mesh;
        }
        ImGui::SameLine();
        ImGui::Text("  ");
        ImGui::SameLine();
        if (ImGui::ImageButton("Gizmo_Mode_Translate", (ImTextureID)button_image_gizmo_translate->get_handle(), renderer->gizmo->get_gizmo_mode() == GizmoMode::Translation, ImVec2(25, 25))){
            renderer->gizmo->set_gizmo_mode(GizmoMode::Translation);
        }
        ImGui::SameLine();
        if (ImGui::ImageButton("Gizmo_Mode_Rotation", (ImTextureID)button_image_gizmo_rotate->get_handle(), renderer->gizmo->get_gizmo_mode() == GizmoMode::Rotation, ImVec2(25, 25))){
            renderer->gizmo->set_gizmo_mode(GizmoMode::Rotation);
        }
        ImGui::SameLine();
        if (ImGui::ImageButton("Gizmo_Mode_Scale", (ImTextureID)button_image_gizmo_scale->get_handle(), renderer->gizmo->get_gizmo_mode() == GizmoMode::Scale, ImVec2(25, 25))){
            renderer->gizmo->set_gizmo_mode(GizmoMode::Scale);
        }
        ImGui::End();
        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(4);
        ImGui::PopID();
    }
}

void WorkSpace::render_hierarchy(){
    ImGui::Begin(Utils::format("Hierarchy##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    CSGNode* mesh_clicked = nullptr;
    Model* model_clicked = nullptr;

    static std::function<void(CSGNode*)> draw_mesh_tree = [&](CSGNode* node){
        // 노드 생성
        ImGuiTreeNodeFlags node_flags = base_flags;
        if (Utils::contains(selected_meshes, node)){
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }
        if (node->is_leaf_node()){
            node_flags |= ImGuiTreeNodeFlags_Leaf;
        }
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        bool node_open = ImGui::TreeNodeEx((void*)node, node_flags, node->get_name().c_str(), 0);
        if ((ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)) &&
            ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && !ImGui::IsItemToggledOpen()){
            mesh_clicked = node;
            if (ImGui::GetIO().KeyCtrl){
                if (selected_models.empty()){
                    this->selection_mode = SelectionMode::Mesh;
                    if (Utils::contains(selected_meshes, mesh_clicked)){
                        selected_meshes.remove(mesh_clicked);
                    } else{
                        selected_meshes.push_back(mesh_clicked);
                    }
                }
            } else{
                if (!Utils::contains(selected_meshes, mesh_clicked)){
                    this->selection_mode = SelectionMode::Mesh;
                    selected_meshes.clear();
                    selected_models.clear();
                    selected_meshes.push_back(mesh_clicked);
                }
            }
        }

        // 재배열 메뉴
        if (ImGui::BeginPopupContextItem()){
            if (ImGui::MenuItem("Cut", "CTRL+X")){}
            if (ImGui::MenuItem("Copy", "CTRL+C")){}
            if (ImGui::MenuItem("Paste", "CTRL+V")){}
            if (ImGui::MenuItem("Delete Selection And Subtree", "Del", false, !selected_meshes.empty())){
                actions.delete_selected_meshes();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Move Up", 0, false, node->get_parent() != nullptr && node->get_parent()->get_children().front() != node)){
                actions.reorder_mesh_up(node);
            }
            if (ImGui::MenuItem("Move Down", 0, false, node->get_parent() != nullptr && node->get_parent()->get_children().back() != node)){
                actions.reorder_mesh_down(node);
            }
            ImGui::Separator();
            if (!node->is_root_node()){
                if (ImGui::MenuItem("Unpack Operation", 0, false, node->get_type() == node->get_parent()->get_type() &&
                                    (node->get_type() == CSGNode::Type::Union || node->get_type() == CSGNode::Type::Intersection))){
                    transaction_manager.add(new TreeEntityModifyTask("Reparent Model", node->get_parent(), [=](){
                        return node->unpack_to_parent();
                    }));
                }
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Add Basic Model As Child")){
                if (ImGui::MenuItem("Cube")){
                    actions.add_mesh_new(node, Mesh::cube(1.0f));
                }
                if (ImGui::MenuItem("Pyramid")){
                    actions.add_mesh_new(node, Mesh::pyramid(0.5f, 1.0f));
                }
                if (ImGui::MenuItem("Sphere")){
                    actions.add_mesh_new(node, Mesh::sphere(0.5f, 2));
                }
                if (ImGui::MenuItem("Cylinder")){
                    actions.add_mesh_new(node, Mesh::cylinder(0.5f, 1.0f, 32));
                }
                if (ImGui::MenuItem("Cone")){
                    actions.add_mesh_new(node, Mesh::cone(0.5f, 1.0f, 32));
                }
                if (ImGui::MenuItem("Torus")){
                    actions.add_mesh_new(node, Mesh::torus(0.5f, 0.25f, 32, 16));
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Load Model As Child")){
                Core::get()->task_manager.add([this](){
                    Transform load_default_transform;
                    // load_default_transform.set_scale(vec3(0.1f, 0.1f, 0.1f));
                    // load_default_transform.set_rotation(vec3(-90, 0, 0));
                    actions.add_model_new(Mesh::load(FileSystem::select_file_load().get_path()), load_default_transform);
                });
            }
            ImGui::EndPopup();
        }

        // 드래그 드롭
        if (ImGui::BeginDragDropSource()){
            ImGui::SetDragDropPayload("DND_Payload_Mesh", &node, sizeof(CSGNode*), ImGuiCond_Once);
            ImGui::Text(node->get_name().c_str());
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget()){
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Payload_Mesh")){
                bool ignore = false;
                if (node->get_type() == CSGNode::Type::Operand && selected_meshes.size() > 1){
                    // TODO : Log 추가
                    ignore = true;
                }
                if (node->get_type() == CSGNode::Type::Difference && node->children_size() + selected_meshes.size() > 2){
                    // TODO : Log 추가
                    ignore = true;
                }

                if (!ignore){
                    for (CSGNode* selected_node : selected_meshes){
                        Model* node_model = node->model; // 추후 작업으로 model값이 변해있을수도 있기때문에 필요.
                        Model* selected_node_model = selected_node->model;

                        transaction_manager.add((new TreeEntityModifyTask<CSGNode>("Reparent Mesh", [=](){
                            return node->reparent_child(selected_node);
                        }, node_model->get_csg_mesh(),
                            [=](CSGNode* root){node_model->set_csg_mesh(root, true);
                        }, selected_node_model->get_csg_mesh(),
                            [=](CSGNode* root){selected_node_model->set_csg_mesh(root, true);

                        }))->link(new TreeEntityModifyTask("Delete Empty Model", root_model, [=](){
                            if (selected_node_model->get_csg_mesh() == nullptr){ // empty model
                                selected_node_model->remove_self();
                            }
                            return true;
                        })));
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        // 하위 노드
        if (node_open){
            for (CSGNode* child : node->get_children()){
                draw_mesh_tree(child);
            }
            ImGui::TreePop();
        }
    };

    static std::function<void(Model*)> draw_model_tree = [&](Model* model){
        // 노드 생성
        ImGuiTreeNodeFlags node_flags = base_flags | ImGuiTreeNodeFlags_AllowItemOverlap;
        if (Utils::contains(selected_models, model)){
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        bool node_open = ImGui::TreeNodeEx((void*)model, node_flags, model->name.c_str(), 0);
        if ((ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)) &&
            ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && !ImGui::IsItemToggledOpen()){
            model_clicked = model;
            if (ImGui::GetIO().KeyCtrl){
                if (selected_meshes.empty()){
                    this->selection_mode = SelectionMode::Model;
                    if (Utils::contains(selected_models, model_clicked)){
                        selected_models.remove(model_clicked);
                    } else{
                        selected_models.push_back(model_clicked);
                    }
                }
            } else{
                if (!Utils::contains(selected_models, model_clicked)){
                    this->selection_mode = SelectionMode::Model;
                    selected_meshes.clear();
                    selected_models.clear();
                    selected_models.push_back(model_clicked);
                }
            }
        }

        // 메뉴
        if (ImGui::BeginPopupContextItem()){
            if (ImGui::MenuItem("Cut", "CTRL+X")){}
            if (ImGui::MenuItem("Copy", "CTRL+C")){}
            if (ImGui::MenuItem("Paste", "CTRL+V")){}
            if (ImGui::MenuItem("Delete Selection", "Del", false, !selected_models.empty())){
                actions.delete_selected_models();
            }
            ImGui::Separator();

            if (ImGui::MenuItem("Move Up", 0, false, model->get_parent()->get_children().front() != model)){
                actions.reorder_model_up(model);
            }
            if (ImGui::MenuItem("Move Down", 0, false, model->get_parent()->get_children().back() != model)){
                actions.reorder_model_down(model);
            }
            if (ImGui::MenuItem("Move To Parent", 0, false, model->get_parent() != root_model)){
                actions.move_model_to_parent(model);
            }
            ImGui::EndPopup();
        }

        // 드래그 드롭
        if (ImGui::BeginDragDropSource()){
            ImGui::SetDragDropPayload("DND_Payload_Model", &model, sizeof(Model*), ImGuiCond_Once);
            std::string selected_names;
            for (Model* selected_model : selected_models){
                if (selected_models.front() != selected_model){
                    selected_names.append(", ");
                }
                selected_names.append(selected_model->name);
            }
            ImGui::Text(selected_names.c_str());
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget()){
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Payload_Model")){
                MultiTransactionTask* task_multi = new MultiTransactionTask(Utils::format("Reparent %1% Model(s)", (int)selected_models.size()));
                for (Model* selected_model : selected_models){
                    task_multi->add_task(new TreeEntityModifyTask("Reparent Model", root_model, [=](){
                        return model->reparent_child(selected_model);
                    }));
                }
                transaction_manager.add(task_multi);
            }
            ImGui::EndDragDropTarget();
        }

        // 하위 노드
        if (node_open){
            if (model->get_csg_mesh() != nullptr){
                draw_mesh_tree(model->get_csg_mesh());
            }
            for (Model* child : model->get_children()){
                draw_model_tree(child);
            }
            ImGui::TreePop();
        }
    };

    for (Model* model : root_model->get_children()){
        draw_model_tree(model);
    }

    if (is_window_content_hovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)
        && mesh_clicked == nullptr && model_clicked == nullptr){ // 빈칸 선택
        selected_meshes.clear();
        selected_models.clear();

    }
    ImGui::End();
}

void WorkSpace::render_inspector(){
    ImGui::Begin(Utils::format("Inspector##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);

    if (selected_models.size() == 1){
        ImGui::Text(("Model : " + selected_models.back()->name + "\n\n").c_str());

        for (Component* component : selected_models.back()->get_components()){
            component->render();
        }
    } else if (selected_meshes.size() == 1){
        ImGui::Text(("Mesh : " + selected_meshes.back()->get_mesh()->get_name() + "\n").c_str());
        ImGui::Text((selected_meshes.back()->get_mesh()->get_mesh_info() + "\n").c_str());
        for (Component* component : selected_meshes.back()->get_components()){
            component->render();
        }
    } else if (selected_models.empty() && selected_meshes.empty()){
        ImGui::Text("World Properties\n\n");
        for (Component* component : root_model->get_components()){
            component->render();
        }
    }

    render_popup_menu_inspector();
    ImGui::End();
}

void WorkSpace::render_logs(){
    if (gui_logs){
        ImGui::Begin(Utils::format("Logs##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
        if (logs.size() > 100){
            logs.pop_front();
        }
        for (std::string line : logs){
            ImGui::Text(line.c_str());
        }
        ImGui::ScrollToItem();
        ImGui::End();
    }
}

void WorkSpace::render(){
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)){ // release 없이 클릭될수도 있음!
        is_background_pressed = false;
        is_gizmo_pressed = false;
    }
    for (Renderer* renderer : renderers){
        render_view(renderer);
    }

    if (gui_hierarchy){
        render_hierarchy();
    }
    if (gui_inspector){
        render_inspector();
    }
    if (gui_logs){
        render_logs();
    }
}

void WorkSpace::check_init_dockspace(int dockspace_id_, const ImGuiViewport* viewport){
    if (dockspace_id == -1){ // not initialized
        // TODO : 도킹도 FirstUseEver일때만 수행
        dockspace_id = dockspace_id_;
        ImGui::DockBuilderRemoveNode(dockspace_id);
        if (ImGui::DockBuilderGetNode(dockspace_id) == NULL){
            ImGui::DockBuilderAddNode(dockspace_id, 0);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            ImGuiID dock_main_id = dockspace_id;
            ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);
            ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
            ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);

            ImGui::DockBuilderDockWindow(Utils::format("View##%1%", renderer_focused->get_id()).c_str(), dock_main_id);
            ImGui::DockBuilderDockWindow(Utils::format("Hierarchy##%1%", id).c_str(), dock_id_left);
            ImGui::DockBuilderDockWindow(Utils::format("Inspector##%1%", id).c_str(), dock_id_right);
            ImGui::DockBuilderDockWindow(Utils::format("Logs##%1%", id).c_str(), dock_id_bottom);
            ImGui::DockBuilderFinish(dockspace_id);
        }
    }
}

void WorkSpace::process_input(){
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
        mouse_pos_left_press_raw = vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
        on_mouse_press_left();
    }
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)){
        mouse_pos_left_current_raw = vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
        on_mouse_drag_left();
    }
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
        mouse_pos_left_current_raw = vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
        on_mouse_release_left();
    }
}

void WorkSpace::on_mouse_press_left(){ // TODO : 여기서 is gizmo pressed 시 transform 저장
    prevPos = mouse_pos_left_press_raw;
    // camera_transform_saved = camera.transform; -> 이것 캡쳐 추가 (파라미터변경 step 에서)
}

void WorkSpace::on_mouse_drag_left(){
    float sensitivity = 22.f;
    vec2 moveDir = mouse_pos_left_current_raw - prevPos;

    if (is_background_pressed){
        if (abs(length(mouse_pos_left_current_raw - prevPos)) > 0.01f){
            get_main_camera()->get_transform()->rotate(vec3(-moveDir.y * Utils::time_delta() * sensitivity, moveDir.x * Utils::time_delta() * sensitivity, 0));
        }
    } else if (is_gizmo_pressed){
        if (gizmo_on_move != nullptr){
            gizmo_on_move(get_main_camera(), mouse_pos_left_press_view, mouse_pos_left_current_view);

        }
    }
    prevPos = mouse_pos_left_current_raw;

    // camera.transform.set_rotation(Transform(camera_transform_saved).rotate(C * vec3(0, mouse_pos_left_current_raw - mouse_pos_left_press_raw, 0)));
}

void WorkSpace::on_mouse_release_left(){
    if(is_gizmo_pressed){
        transaction_manager.add(renderer_focused->gizmo->on_release_task_new());
    }
    gizmo_on_move = nullptr;

    is_background_pressed = false;
    is_gizmo_pressed = false;
}

bool WorkSpace::check_model_selected_exact(Model* model){
    return Utils::contains(selected_models, model);
}

bool WorkSpace::check_model_selected(Model* model){
    Model* current_model = model;
    while (current_model != nullptr){
        if (check_model_selected_exact(current_model)){
            return true; // 자신 혹은 부모가 선택됨
        }
        current_model = current_model->get_parent();
    }
    return false;
}

bool WorkSpace::check_mesh_selected_exact(CSGNode* mesh){
    return Utils::contains(selected_meshes, mesh);
}

bool WorkSpace::check_mesh_selected(CSGNode* mesh){
    CSGNode* current_mesh = mesh;
    while (current_mesh != nullptr){
        if (check_mesh_selected_exact(current_mesh)){
            return true;
        }
        current_mesh = current_mesh->get_parent();
    }
    return false;
}

WorkSpace* WorkSpace::create_new(GUI* parent_, const char* filename){
    return new WorkSpace(parent_, filename);
}

void WorkSpace::add_view_new(){
    renderers.push_back(renderer_focused = new Renderer(256, 256));
    renderer_focused->set_parent(this);
}

void WorkSpace::render_popup_menu_view(Renderer* renderer){
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && ImGui::GetIO().MouseReleased[ImGuiMouseButton_Right]){
        ImGui::OpenPopup("View_Popup_Edit");
    }
    if (ImGui::BeginPopup("View_Popup_Edit")){
        if (ImGui::MenuItem("Reset Camera")){
            renderer->camera->get_transform()->set_position(vec3(0, 0, Camera::default_pos_z));
            renderer->camera->get_transform()->set_rotation({0,-90,0});
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Select")){
            if (ImGui::MenuItem("Switch Last Selected Object To Parent")){ // TODO : 메쉬랑 분리
                if (!selected_models.empty()){
                    Model* last_item = selected_models.back();
                    if (last_item->get_parent() != nullptr){
                        selected_models.pop_back();
                        if (!Utils::contains(selected_models, last_item->get_parent())){
                            selected_models.push_back(last_item->get_parent());
                        }
                    }
                } else if (!selected_meshes.empty()){
                    CSGNode* last_item = selected_meshes.back();
                    if (last_item->get_parent() != nullptr){
                        selected_meshes.pop_back();
                        if (!Utils::contains(selected_meshes, last_item->get_parent())){
                            selected_meshes.push_back(last_item->get_parent());
                        }
                    }
                }
            }
            if (ImGui::MenuItem("Reverse Selection")){
                if (!selected_models.empty()){
                    std::list<Model*> selected_models_reversed;
                    for (Model* model : selected_models){
                        Model* parent = model->get_parent();
                        for (Model* child : parent->get_children()){
                            if (!Utils::contains(selected_models_reversed, child) && !Utils::contains(selected_models, child)){
                                selected_models_reversed.push_back(child);
                            }
                        }
                    }
                    selected_models.clear();
                    selected_models.splice(selected_models.end(), selected_models_reversed);
                } else if (!selected_meshes.empty()){
                    if (!selected_meshes.empty()){
                        std::list<CSGNode*> selected_meshes_reversed;
                        for (CSGNode* model : selected_meshes){
                            CSGNode* parent = model->get_parent();
                            for (CSGNode* child : parent->get_children()){
                                if (!Utils::contains(selected_meshes_reversed, child) && !Utils::contains(selected_meshes, child)){
                                    selected_meshes_reversed.push_back(child);
                                }
                            }
                        }
                        selected_meshes.clear();
                        selected_meshes.splice(selected_meshes.end(), selected_meshes_reversed);
                    }
                }
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Add Basic Model")){
            if (ImGui::MenuItem("Cube")){
                actions.add_model_new(Mesh::cube(1.0f));
            }
            if (ImGui::MenuItem("Pyramid")){
                actions.add_model_new(Mesh::pyramid(0.5f, 1.0f));
            }
            if (ImGui::MenuItem("Sphere")){
                actions.add_model_new(Mesh::sphere(0.5f, 2));
            }
            if (ImGui::MenuItem("Cylinder")){
                actions.add_model_new(Mesh::cylinder(0.5f, 1.0f, 32));
            }
            if (ImGui::MenuItem("Cone")){
                actions.add_model_new(Mesh::cone(0.5f, 1.0f, 32));
            }
            if (ImGui::MenuItem("Torus")){
                actions.add_model_new(Mesh::torus(0.5f, 0.25f, 32, 16));
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Load Model")){
            Core::get()->task_manager.add([this](){
                Transform load_default_transform;
                // load_default_transform.set_scale(vec3(0.1f, 0.1f, 0.1f));
                // load_default_transform.set_rotation(vec3(-90, 0, 0));
                actions.add_model_new(Mesh::load(FileSystem::select_file_load().get_path()), load_default_transform);
            });
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Boolean Operation", selection_mode == SelectionMode::Model)){
            if (ImGui::MenuItem("Create Union Of Selected Models", NULL, false, selected_models.size() >= 2)){
                actions.create_boolean_opertation_of_selected_models(CSGNode::Type::Union);
            }
            if (ImGui::MenuItem("Create Intersection Of Selected Models", NULL, false, selected_models.size() >= 2)){
                actions.create_boolean_opertation_of_selected_models(CSGNode::Type::Intersection);
            }
            std::string selected_model1_name = "A";
            std::string selected_model2_name = "B";
            if (selected_models.size() == 2){
                selected_model1_name = selected_models.front()->name;
                selected_model2_name = selected_models.back()->name;
            }
            if (ImGui::MenuItem(("Create Difference Of Model " + selected_model1_name + " - " + selected_model2_name).c_str(), NULL, false, selected_models.size() == 2)){
                actions.create_boolean_opertation_of_selected_models(CSGNode::Type::Difference);
            }
            if (ImGui::MenuItem(("Create Difference Of Model " + selected_model2_name + " - " + selected_model1_name).c_str(), NULL, false, selected_models.size() == 2)){
                Model* model2 = selected_models.front();
                selected_models.pop_front();
                selected_models.push_back(model2);
                actions.create_boolean_opertation_of_selected_models(CSGNode::Type::Difference);
            }
            ImGui::EndMenu();
        }

        // TODO : 메쉬 정제(csg가능하게) 연산들 - cgal 라이브러리 내에 다양한 알고리즘들 제공
        ImGui::EndPopup();
    }
}

void WorkSpace::render_popup_menu_inspector(){
    return; // TODO : 컴포넌트 추가 삭제 관련 기능이 구현되면 해제 
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && ImGui::GetIO().MouseReleased[ImGuiMouseButton_Right]){
        ImGui::OpenPopup("View_Popup_Inspector");
    }
    if (ImGui::BeginPopup("View_Popup_Inspector")){
        if (ImGui::MenuItem("Add Component", 0, false, selected_models.size() <= 1 && selected_meshes.size() <= 1)){}
        if (ImGui::MenuItem("Show Remove Button", 0, &Component::show_remove_button)){}
        ImGui::EndPopup();
    }
}
