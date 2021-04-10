#include "ThreadPool.hpp"  

#include <iostream>
#include <thread>

ThreadPool::ThreadPool() : 
    ThreadPool(std::thread::hardware_concurrency())
{
}

ThreadPool::ThreadPool(unsigned int size)
{
	executors_ = std::vector<ThreadExecutor*>();
    executors_.reserve(size);
    for (size_t i = 0; i < size; i++)
    {
        executors_.emplace_back(new ThreadExecutor());
    }
}

ThreadPool::~ThreadPool()
{
    for (auto* executor : executors_){
        delete executor;
    }
}

TaskGroup ThreadPool::createTaskGroup(){
    return TaskGroup(this);
}

std::future<void> ThreadPool::addTask(std::function<void()> function) 
{
    // find thread executor with minimal load
    ThreadExecutor* min_executor = executors_[0];
    for (auto* executor : executors_){
        if (min_executor->task_count() > executor->task_count()){
            min_executor = executor;
        }
    }

    if (min_executor->isExecuting()){
        return min_executor->addTask(function);
    }else{
        throw std::exception("thread pool finished execution");
    }
}

template<typename T>
std::future<T> ThreadPool::addTaskWithResult(std::function<T()> function) {
    std::shared_ptr<std::promise<T>> promise_ptr(new std::promise<T>());
    auto result_future = promise_ptr->get_future();
    addTask([promise_ptr, function](){
        promise_ptr->set_value(function());
    });
    return result_future;
}

void ThreadPool::joinAll()
{
    for(auto* executor : executors_){
        executor->join();
    }
}
