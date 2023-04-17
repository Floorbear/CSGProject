#include "Task.h"

// ===== Job System ===== //

Task::Task(std::function<void()> work_) :
    work(work_){
}

void Task::execute() const{
    work();
}

TransactionTask::TransactionTask(std::string detail_, std::function<void()> work_, std::function<void()> work_undo_) :
    detail(detail_),
    work(work_),
    work_undo(work_undo_){
}

void TransactionTask::execute() const{
    if (work != NULL){
        work();
    }
}

TransactionTask TransactionTask::undo_task() const{
    return TransactionTask(detail, [=](){ work_undo(); },
                           [=](){ work(); });
}

void TaskManager::add(std::function<void()> work_){
    work_queue.push(Task(work_));
}

void TaskManager::add(const Task task_){
    work_queue.push(task_);
}

void TaskManager::execute_all(){
    while (!work_queue.empty()){
        work_queue.front().execute();
        work_queue.pop();
    }
}

TransactionTaskManager::TransactionTaskManager(){
    option_undo_max_cnt = 10;
}

void TransactionTaskManager::add(std::string detail_, std::function<void()> work_, std::function<void()> work_undo_){
    work_queue.push(TransactionTask(detail_, work_, work_undo_));
    redo_stack.clear();
}

void TransactionTaskManager::add(const TransactionTask task_){
    work_queue.push(task_);
    redo_stack.clear();
}

void TransactionTaskManager::execute_all(){
    while (!work_queue.empty()){
        work_queue.front().execute();
        history_stack.push_back(work_queue.front());
        if (history_stack.size() > option_undo_max_cnt){
            history_stack.pop_front();
        }
        work_queue.pop();
    }
    while (!work_queue_no_history.empty()){
        work_queue_no_history.front().execute();
        work_queue_no_history.pop();
    }
}

void TransactionTaskManager::undo(){
    if (history_stack.empty()){
        return;
    }

    TransactionTask task_to_undo = history_stack.back();
    history_stack.pop_back();

    work_queue_no_history.push(Task([=](){
        task_to_undo.undo_task().execute();
        redo_stack.push_back(task_to_undo);
    }));
}

void TransactionTaskManager::redo(){
    if (redo_stack.empty()){
        return;
    }

    work_queue.push(redo_stack.back());
    redo_stack.pop_back();
}

bool TransactionTaskManager::can_undo(){
    return !history_stack.empty();
}

bool TransactionTaskManager::can_redo(){
    return !redo_stack.empty();
}

std::string TransactionTaskManager::undo_detail(){
    if (history_stack.empty()){
        return std::string();
    }
    return history_stack.back().detail;
}

std::string TransactionTaskManager::redo_detail(){
    if (redo_stack.empty()){
        return std::string();
    }
    return redo_stack.back().detail;
}
