#pragma once
#include "Task.h"

#include <list>

// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern

template <typename T>// requires std::derived_from<T, TreeNode<T>>
class TreeNode{
public:
    class TreeSnapShot{
    public:
        class Node : public TreeNode<Node>{
        public:
            T* ptr;
            Node(T* ptr_) : ptr(ptr_){}

            void recover(){
                ptr->children.clear();
                for (Node* node_child : Node::get_children()){
                    node_child->ptr->parent = ptr;
                    ptr->children.push_back(node_child->ptr);
                    node_child->recover();
                }
            }
        };
        Node* root;

        TreeSnapShot(T* root_) : root(new Node(root_)){}
        ~TreeSnapShot(){
            delete root;
        }

        void recover(){
            root->recover();
        }
    };

protected:
    T* parent;
    std::list<T*> children;

public:
    ~TreeNode(){
        for (T* child : children){
            delete child;
        }
    }

    T* get_parent(){
        return parent;
    }

    virtual void set_parent(T* parent_){
        parent = parent_;
    }

    std::list<T*> get_children(){
        return children;
    }

    size_t children_size(){
        return children.size();
    }

    virtual bool add_child(T* item){
        if (is_descendant_of(item)){
            return false;
        }
        item->set_parent(static_cast<T*>(this));
        children.push_back(item);
        return true;
    }

    virtual bool reparent_child(T* item, T* after = nullptr){
        if (is_descendant_of(item)){
            return false;
        }
        if (item->parent != nullptr){
            item->parent->children.remove(item);
        }
        item->set_parent(static_cast<T*>(this));
        if (after != nullptr){
            children.insert(std::next(std::find(children.begin(), children.end(), after)), item);
        } else{
            children.push_back(item);
        }
        return true;
    }

    virtual void swap_child(T* child1, T* child2){
        auto it1 = std::find(children.begin(), children.end(), child1);
        auto it2 = std::find(children.begin(), children.end(), child2);
        std::swap(*it1, *it2);
    }

    virtual void remove_self(){
        assert(parent != nullptr); // root는 있어야한다
        auto it = std::next(std::find(parent->children.begin(), parent->children.end(), static_cast<T*>(this)));
        for (T* child : children){
            parent->children.insert(it, child);
        }
        parent->children.remove(static_cast<T*>(this));
    }

    // void remove_child(T* item){
    //    children.remove(item);
    // }

    bool is_leaf_node(){
        return children.empty();
    }

    bool is_descendant_of(T* parent_){
        for (T* child : parent_->children){
            if (is_descendant_of(child)){
                return true;
            }
        }
        return parent == parent_;
    }

    TreeSnapShot* make_snapshot_new(){
        TreeSnapShot* ret = new TreeSnapShot(static_cast<T*>(this));
        make_snapshot_(ret->root);
        return ret;
    }

private:
    void make_snapshot_(TreeSnapShot::Node* node_this){
        for (T* child : children){
            auto node_child = new TreeSnapShot::Node(child);
            node_this->add_child(node_child);
            child->make_snapshot_(node_child);
        }
    }
};

template <typename T>
class TreeModifyTask : public TransactionTask{
    TreeNode<T>::TreeSnapShot* snapshot1 = nullptr;
    TreeNode<T>::TreeSnapShot* snapshot2 = nullptr;
public:
    TreeModifyTask(std::string detail_, TreeNode<T>* recovery_root, std::function<bool()> work_) : TransactionTask(detail_, [=, this](){
        snapshot1 = recovery_root->make_snapshot_new();
        return work_();
    }, [this](){
        snapshot1->recover();
    }){
    }

    TreeModifyTask(std::string detail_, std::function<bool()> work_with_set_root,
                   TreeNode<T>* root_old1, std::function<void(T*)> set_root1,
                   TreeNode<T>* root_old2, std::function<void(T*)> set_root2) : TransactionTask(detail_, [=, this](){ // root가 수정되는 작업
        if (root_old1 != nullptr){
            snapshot1 = root_old1->make_snapshot_new();
        }
        if (root_old2 != nullptr){
            snapshot2 = root_old2->make_snapshot_new();
        }
        return work_with_set_root(); // set root 작업이 있어도 되고 없어도 됨.
    }, [=, this](){
        if (snapshot1 != nullptr){
            snapshot1->recover();
        }
        if (snapshot2 != nullptr){
            snapshot2->recover();
        }
        if (root_old1 != nullptr){
            root_old1->set_parent(nullptr);
        }
        if (root_old2 != nullptr){
            root_old2->set_parent(nullptr);
        }
        set_root1(static_cast<T*>(root_old1));
        set_root2(static_cast<T*>(root_old2));
    }){
    }

    ~TreeModifyTask(){
        if (snapshot1 != nullptr){
            delete snapshot1;
        }
        if (snapshot2 != nullptr){
            delete snapshot2;
        }
    }
};
