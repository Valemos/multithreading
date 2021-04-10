#ifndef TASKGROUP_H
#define TASKGROUP_H

class ThreadPool;
#include "ThreadPool.hpp"


class TaskGroup  
{
public:

	TaskGroup(ThreadPool* pool);
	~TaskGroup();

	void addTask(std::function<void()> function);
	void waitTasksFinished(); // waits for all tasks in this group

private:
	ThreadPool* pool;
	std::list<std::future<void>> scheduled_futures_;

};
#endif