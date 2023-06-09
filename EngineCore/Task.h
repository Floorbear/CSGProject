#pragma once

#include <string>
#include <list>
#include <deque>
#include <queue>
#include <functional>

class Task{
    const std::function<void()> work;
public:
    Task(std::function<void()> work_);
    void execute() const;
};

class TransactionTask{
public:
    const std::function<bool()> work;
    const std::function<void()> work_undo;
    
    const std::string detail;

    TransactionTask(std::string detail_, std::function<bool()> work_, std::function<void()> work_undo_);
    bool execute() const;
    TransactionTask undo_task() const;
    virtual TransactionTask* link(TransactionTask* task);
};

class MultiTransactionTask : public TransactionTask{
    std::list<TransactionTask*> tasks;
public:
    MultiTransactionTask(std::string detail_);
    ~MultiTransactionTask();
    void add_task(TransactionTask* task);
    TransactionTask* link(TransactionTask* task) override;
};

class TaskManager{
    // TODO : 멀티스레드, 우선순위 등등...
    std::queue<Task> work_queue;
public:
    void add(std::function<void()> work_);
    void add(const Task task_);
    void execute_all();
};

class TransactionTaskManager{
    std::queue<Task> work_queue_no_history;
    std::queue<TransactionTask*> work_queue;
    std::deque<TransactionTask*> history_stack;
    std::deque<TransactionTask*> redo_stack;

public:
    int option_undo_max_cnt;

    TransactionTaskManager();
    void add(std::string detail_, std::function<bool()> work_, std::function<void()> work_undo_);
    void add(TransactionTask* task_);
    void execute_all();
    void undo();
    void redo();
    bool can_undo();
    bool can_redo();
    std::string undo_detail();
    std::string redo_detail();
};


