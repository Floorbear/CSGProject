#pragma once

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
        };
        Node* root;

        TreeSnapShot(T* root_) : root(new Node(root_)){}
        ~TreeSnapShot(){
            delete root;
        }

        void recover(){
            root->ptr->recover_from_snapshot_(root);
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

    std::list<T*> get_children(){
        return children;
    }

    virtual void add_child(T* item){
        if (is_descendant_of(item)){
            return;
        }
        item->parent = static_cast<T*>(this);
        children.push_back(item);
    }

    virtual void set_child(T* item, T* after = nullptr){
        if (is_descendant_of(item)){
            return;
        }
        if (item->parent != nullptr){
            item->parent->children.remove(item);
        }
        item->parent = static_cast<T*>(this);
        if (after != nullptr){
            children.insert(std::next(std::find(children.begin(), children.end(), after)), item);
        } else{
            children.push_back(item);
        }
    }

    virtual void swap_child(T* child1, T* child2){
        auto it1 = std::find(children.begin(), children.end(), child1);
        auto it2 = std::find(children.begin(), children.end(), child2);
        std::swap(*it1, *it2);
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

    TreeSnapShot make_snapshot(){
        TreeSnapShot ret(static_cast<T*>(this));
        make_snapshot_(ret.root);
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

    void recover_from_snapshot_(TreeSnapShot::Node* node){
        children.clear();
        for (auto node_child : node->ptr->get_children()){
            node_child->ptr->parent = static_cast<T*>(this);
            children.push_back(node_child->ptr);
            node_child->recover_from_snapshot_();
        }
    }
};