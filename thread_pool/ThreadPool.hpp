#ifndef THREADPOOL_H
#define THREADPOOL_H

class TaskGroup;

#include <vector>
#include <list>
#include <future>
#include "ThreadExecutor.hpp"
#include "TaskGroup.hpp"


class ThreadPool
{
public:

	ThreadPool();
	ThreadPool(unsigned int size);
	~ThreadPool();

	std::future<void> addTask(std::function<void()> function);
	TaskGroup createTaskGroup();
	void joinAll();
	
	template<typename T>
	std::future<T> addTaskWithResult(std::function<T()> function);

private:
	std::vector<ThreadExecutor*> executors_;
};

template std::future<std::vector<int>> ThreadPool::addTaskWithResult(std::function<std::vector<int>()>);

#endif