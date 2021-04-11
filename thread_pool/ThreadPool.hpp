#ifndef THREADPOOL_H
#define THREADPOOL_H

class TaskGroup;

#include <vector>
#include <list>
#include <future>
#include "ThreadExecutor.hpp"


class ThreadPool
{
public:

	ThreadPool();
	ThreadPool(unsigned int size);
	~ThreadPool();

	std::future<void> addTask(std::function<void()> function);
	void addTaskToGroup(std::function<void()> function);
	void waitGroupFinished();
	void stopAll();

private:
	std::list<std::future<void>> scheduled_futures_;

	std::vector<ThreadExecutor*> executors_;
	ThreadExecutor* findBestExecutor();
};

#endif