#include "ThreadPool.hpp"  

#include <iostream>
#include <thread>

ThreadPool::ThreadPool() : 
    ThreadPool(std::thread::hardware_concurrency() - 1)
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
    ThreadExecutor* min_executor = findBestExecutor();

    if (min_executor->isExecuting()){
        return min_executor->addTask(function);
    }else{
        throw std::exception("thread pool finished execution");
    }
}

ThreadExecutor* ThreadPool::findBestExecutor(){
    ThreadExecutor* min_executor = executors_[0];
    for (auto* executor : executors_){
        if (min_executor->task_count() > executor->task_count()){
            min_executor = executor;
        }
    }
    return min_executor;
}

void ThreadPool::stopAll()
{
    for(auto* executor : executors_){
        executor->stop();
    }
}
