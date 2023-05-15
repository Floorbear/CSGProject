#include "WorkSpace.h"
#include "Utils.h"
#include "Camera.h"
#include "Model.h"
#include "CSGNode.h"
#include "Renderer.h"
#include "FrameBuffer.h"
#include "Texture.h"
#include "PointLight.h"

int WorkSpace::id_counter = 1; // 1부터 시작

WorkSpace::WorkSpace(GUI* parent_) : WorkSpace(parent_, std::string("test")){
}

WorkSpace::WorkSpace(GUI* parent_, std::string title_) : parent(parent_), title(title_), actions(WorkSpace_Actions(this)){
    id = id_counter++;
    //TODO : 여기서 도킹 설정?

    renderers.push_back(renderer_focused = new Renderer(512, 512));
    renderer_focused->set_parent(parent);
    renderer_focused->init();

    root_model = new Model("Root");
    root_model->add_component(new PointLight(this, vec3(42, 0, 42))); // TODO : value 바꿔!
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

void WorkSpace::render_view(Renderer* renderer){
    // https://stackoverflow.com/questions/60955993/how-to-use-opengl-glfw3-render-in-a-imgui-window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(Utils::format("View##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);
    vec2 p_min = vec2(ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y);
    vec2 p_max = vec2(ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMax().y);
    renderer->resize(p_max - p_min);

    // view 마우스 좌표 측정
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
        renderer_focused = renderer;
        mouse_pos_left_press_view = vec2(ImGui::GetMousePos().x - p_min.x, ImGui::GetMousePos().y - p_min.y);
        mouse_pos_left_current_view = mouse_pos_left_press_view;
        is_view_pressed = true;
    }

    if (renderer_focused == renderer && ImGui::IsMouseDragging(ImGuiMouseButton_Left)){
        mouse_pos_left_current_view = vec2(ImGui::GetMousePos().x - p_min.x, ImGui::GetMousePos().y - p_min.y);
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
        mouse_pos_left_current_view = vec2(ImGui::GetMousePos().x - p_min.x, ImGui::GetMousePos().y - p_min.y);

        // selection 처리
        SelectionPixelObjectInfo info = renderer->find_selection(root_model->get_children(), mouse_pos_left_current_view);
        if (!info.empty()){
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

    // Gui 렌더링
    #pragma warning(disable: 4312)
    ImGui::GetWindowDrawList()->AddImage((void*)renderer->framebuffer_screen->get_framebufferTexutre()->get_textureHandle(), ImVec2(p_min.x, p_min.y), ImVec2(p_max.x, p_max.y), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::PopStyleVar();

    render_popup_menu_view();
    ImGui::End();
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
        bool node_open = ImGui::TreeNodeEx((void*)node, node_flags, "<mesh>", 0);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()){
            mesh_clicked = node;
        }

        // 재배열 메뉴
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)){
            ImGui::OpenPopup(Utils::format("View_Popup_Inspector_%1%", (int)node).c_str());
        }
        if (ImGui::BeginPopup(Utils::format("View_Popup_Inspector_%1%", (int)node).c_str())){
            if (ImGui::MenuItem("Move Up", 0, false, node->get_parent() != nullptr && node->get_parent()->get_children().front() != node)){
                actions.reorder_mesh_up(node);
            }
            if (ImGui::MenuItem("Move Down", 0, false, node->get_parent() != nullptr && node->get_parent()->get_children().back() != node)){
                actions.reorder_mesh_down(node);
            }
            ImGui::EndPopup();
        }

        // 드래그 드롭
        if (ImGui::BeginDragDropSource()){
            ImGui::SetDragDropPayload("DND_Payload_Mesh", &node, sizeof(CSGNode*), ImGuiCond_Once);
            ImGui::Text("<mesh>");
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget()){
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Payload_Mesh")){
                IM_ASSERT(payload->DataSize == sizeof(CSGNode*));
                CSGNode* payload_node = *(CSGNode**)payload->Data;
                // printf("mesh to mesh\n"); // TODO : undo추가
                node->set_child(payload_node);
                // TODO : 빈 모델 삭제 (reversable)
            }
            // if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Payload_Model")){ empty }
            ImGui::EndDragDropTarget();
        }

        // 추가 ui
        if (!node->is_leaf_node()){
            ImGui::SameLine();
            ImGui::Checkbox("##Selection Group", &node->selection_group); // TODO : 체크박스 안쪽에서는 트리노드 클릭 안되게하기...
        }

        // 하위 노드
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (node_open){
            for (CSGNode* child : node->get_children()){
                draw_mesh_tree(child);
            }
            ImGui::TreePop();
        }
    };

    static std::function<void(Model*)> draw_model_tree = [&](Model* model){
        // 노드 생성
        ImGuiTreeNodeFlags node_flags = base_flags;
        if (Utils::contains(selected_models, model)){
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }
        bool node_open = ImGui::TreeNodeEx((void*)model, node_flags, model->name.c_str(), 0);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()){
            model_clicked = model;
        }

        // 재배열 메뉴
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)){
            ImGui::OpenPopup(Utils::format("View_Popup_Inspector_%1%", (int)model).c_str());
        }
        if (ImGui::BeginPopup(Utils::format("View_Popup_Inspector_%1%", (int)model).c_str())){
            if (ImGui::MenuItem("Move Up", 0, false, model->get_parent()->get_children().front() != model)){
                actions.reorder_model_up(model);
            }
            if (ImGui::MenuItem("Move Down", 0, false, model->get_parent()->get_children().back() != model)){
                actions.reorder_model_down(model);
            }
            if (ImGui::MenuItem("Move To Parent", 0, false, model->get_parent() != root_model)){
                model->get_parent()->get_parent()->set_child(model, model->get_parent()); // TODO : undo
            }
            ImGui::EndPopup();
        }

        WorkSpace* ws = this;
        // 드래그 드롭
        if (ImGui::BeginDragDropSource()){
            ImGui::SetDragDropPayload("DND_Payload_Model", &model, sizeof(Model*), ImGuiCond_Once);
            ImGui::Text(model->name.c_str());
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget()){
            // if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Payload_Mesh")){ empty } 
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Payload_Model")){
                IM_ASSERT(payload->DataSize == sizeof(Model*));
                Model* payload_model = *(Model**)payload->Data;
                transaction_manager.add("Nodel Tree Edit", [=](){
                    // TreeNode<Model>::TreeSnapShot snapshot = root_model->make_snapshot();
                    model->set_child(payload_model);// TODO : undo작업 만들기
                }, [=](){
                });
            }
            ImGui::EndDragDropTarget();
        }

        // 하위 노드
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (node_open){
            draw_mesh_tree(model->get_csg_mesh());
            for (Model* child : model->get_children()){
                draw_model_tree(child);
            }
            ImGui::TreePop();
        }
    };

    for (Model* model : root_model->get_children()){
        draw_model_tree(model);
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered(ImGuiHoveredFlags_None)
        && mesh_clicked == nullptr && model_clicked == nullptr){ // 빈칸 선택
        selected_meshes.clear();
        selected_models.clear();

    } else if (mesh_clicked != nullptr){ // 메쉬 선택
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
            this->selection_mode = SelectionMode::Mesh;
            selected_meshes.clear();
            selected_models.clear();
            selected_meshes.push_back(mesh_clicked);
        }

    } else if (model_clicked != nullptr){ // 모델 선택
        if (ImGui::GetIO().KeyCtrl){
            if (selected_meshes.empty()){
                this->selection_mode = SelectionMode::Model;
                if (Utils::contains(selected_models, model_clicked)){
                    selected_models.remove(model_clicked);
                } else{
                    selected_models.push_back(model_clicked);
                    printf("%s", model_clicked->name.c_str());
                }
            }
        } else{
            this->selection_mode = SelectionMode::Model;
            selected_meshes.clear();
            selected_models.clear();
            selected_models.push_back(model_clicked);
        }
    }
    ImGui::End();
}

void WorkSpace::render_inspector(){
    ImGui::Begin(Utils::format("Inspector##%1%", id).c_str(), 0, ImGuiWindowFlags_NoCollapse);

    // 마지막으로 선택된 것 (기즈모가 그려지는 것)
    if (!selected_models.empty()){
        ImGui::Text(("Model : " + selected_models.back()->name + "\n\n").c_str());
        for (Component* component : selected_models.back()->get_components()){
            component->render();
        }
    } else if (!selected_meshes.empty()){
        ImGui::Text("Mesh\n\n");
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
        ImGui::Text(Utils::format("Text%1%", id).c_str()); // TODO
        ImGui::End();
    }
}

void WorkSpace::render(){
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
        is_view_pressed = false;
    }
    for (Renderer* renderer : renderers){
        renderer->render(root_model->get_children(), &lights);
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

void WorkSpace::on_mouse_press_left(){
    prevPos = mouse_pos_left_press_raw;
    // camera_transform_saved = camera.transform;
}

void WorkSpace::on_mouse_drag_left(){
    float sensitivity = 15.f;
    vec2 moveDir = mouse_pos_left_current_raw - prevPos;
    // printf("%lf \n", abs(length(mouse_pos_left_current_raw) - length(prevPos)));
    if (abs(length(mouse_pos_left_current_raw) - length(prevPos)) > 0.01f){
        prevPos = mouse_pos_left_current_raw;
        get_main_camera()->get_transform()->rotate(vec3(-moveDir.y * Utils::time_delta() * sensitivity, moveDir.x * Utils::time_delta() * sensitivity, 0));
    }
    // camera.transform.set_rotation(Transform(camera_transform_saved).rotate(C * vec3(0, mouse_pos_left_current_raw - mouse_pos_left_press_raw, 0)));
}

void WorkSpace::on_mouse_release_left(){
    // camera.transform.set_rotation(Transform(camera_transform_saved).rotate(C * vec3(0, mouse_pos_left_current_raw - mouse_pos_left_press_raw, 0))); 그냥 똑같은짓 하고 끝
    // 아니면 관성 추가!
    // vec2 mouse_inertia = mouse_pos_left_current_raw - mouse_pos_left_press_raw;
    //
}

//  WorkSpace::update (){
//  ...
//  while(mouse_inertia.magnitude() < 0.01f){
//    camera.transform.rotate(vec3(0, mouse_inertia.x * C, 0))); 여기는 누적방식
//    mouse_inertia *= 0.9;
//  ...

WorkSpace* WorkSpace::create_new(GUI* parent_, const char* filename){
    return new WorkSpace(parent_, filename);
}

void WorkSpace::render_popup_menu_view(){
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && ImGui::GetIO().MouseReleased[ImGuiMouseButton_Right]){
        ImGui::OpenPopup("View_Popup_Edit");
    }
    if (ImGui::BeginPopup("View_Popup_Edit")){
        if (ImGui::MenuItem("Cut", "CTRL+X")){}
        if (ImGui::MenuItem("Copy", "CTRL+C")){}
        if (ImGui::MenuItem("Paste", "CTRL+V")){}
        if (ImGui::MenuItem("Delete", "Del", false, !selected_models.empty() || !selected_meshes.empty())){
            actions.delete_selected();
        }
        ImGui::Separator();

        if (ImGui::BeginMenu("Select")){
            if (ImGui::MenuItem("Switch Last Selected Object To Parent")){
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
            // if (ImGui::MenuItem("Filter Selection")){}
            // TODO : 옵션 추가
            ImGui::EndMenu();
        }
        ImGui::Separator();
        // 우클릭도 선택가능.
        // TODO : if 선택된 모델이 있는경우 클릭하면 add child model
        // TODO : if 선택된 메쉬가 있는경우 add mesh_union, add mesh intersention, add mesh difference(제한적으로 활성화)
        if (ImGui::BeginMenu("Add Model")){
            if (ImGui::MenuItem("Cube")){
                transaction_manager.add("add cube",
                    [this](){ actions.add_cube_new(); },
                    [this](){ /*actions.delete_model();*/ });
            }
            /*if (ImGui::MenuItem("Sphere")){} TODO : 주석해제
            if (ImGui::MenuItem("Cylinder")){}
            if (ImGui::MenuItem("Cone")){}
            if (ImGui::MenuItem("Tetrahedron")){}
            if (ImGui::MenuItem("Pyramid")){}
            if (ImGui::MenuItem("Torus")){}*/
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Load Model")){}
        ImGui::Separator();

        // TODO : 메쉬 정제(csg가능하게) 연산들 - cgal 라이브러리 내에 다양한 알고리즘들 제공
        ImGui::EndPopup();
    }
}

void WorkSpace::render_popup_menu_inspector(){
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && ImGui::GetIO().MouseReleased[ImGuiMouseButton_Right]){
        ImGui::OpenPopup("View_Popup_Hierarchy");
    }
    if (ImGui::BeginPopup("View_Popup_Hierarchy")){
        if (ImGui::MenuItem("Add Component", 0, false, selected_models.size() <= 1 && selected_meshes.size() <= 1)){} // TODO : 구현
        if (ImGui::MenuItem("Show Remove Button", 0, &Component::show_remove_button)){}
        ImGui::EndPopup();
    }
}