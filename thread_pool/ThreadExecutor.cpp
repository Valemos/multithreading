#include "ThreadExecutor.hpp"  

#include <iostream>

ThreadExecutor::ThreadExecutor() : 
    finish_execution_(false)
{
	thread_ = std::thread(&ThreadExecutor::execute, this);
}

ThreadExecutor::~ThreadExecutor()
{
    join();
}

int ThreadExecutor::task_count() const{
    return task_queue_.size();
}

void ThreadExecutor::execute(){
    while (true){
        if (task_queue_.size() > 0){
            auto& task = task_queue_.front();

            // complete task
            task.function();
            task.promise.set_value();

            // must delete after task finished or will get an error
            task_queue_.pop_front();

        }else{
            if (finish_execution_) { break; }
            else { waitForTask(); }
        }
    }
}

void ThreadExecutor::waitForTask(){
    std::unique_lock<std::mutex> lock(m_no_tasks_);
    cond_no_tasks_.wait(lock);
}

bool ThreadExecutor::isExecuting() const{
    return !finish_execution_;
}

std::future<void> ThreadExecutor::addTask(std::function<void()> function) {
    // must not be called after join command. check before adding task with isExecuting()
    if (finish_execution_) throw std::exception("executor is being joined!");

    std::lock_guard<std::mutex> lock(m_add_task_);

    std::promise<void> promise_result;
    auto future_task = promise_result.get_future(); // must create future before promise moved
    task_queue_.push_back({function, std::move(promise_result)});
    
    // resume execution for the first task added to queue
    if (task_queue_.size() == 1) {
        cond_no_tasks_.notify_one();
    }

    return future_task;
}

void ThreadExecutor::join(){
    finish_execution_ = true; // must be set to finish executing all tasks or else will not stop
    cond_no_tasks_.notify_one(); // if thread was waiting for tasks continue execution
	if (thread_.joinable()){
        thread_.join();
    }
}
