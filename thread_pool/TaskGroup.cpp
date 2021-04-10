#include "TaskGroup.hpp"  
	
TaskGroup::TaskGroup(ThreadPool* pool) :
    pool(pool)
{}

TaskGroup::~TaskGroup()
{
    waitTasksFinished();
}

void TaskGroup::addTask(std::function<void()> function){
    auto task_future = pool->addTask(function);
    scheduled_futures_.emplace_back(std::move(task_future));
}

void TaskGroup::waitTasksFinished(){
    for (auto& future : scheduled_futures_){
        future.get();
    }
    scheduled_futures_.clear();
}
