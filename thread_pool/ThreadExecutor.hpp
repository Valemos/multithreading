#ifndef THREADEXECUTOR_H
#define THREADEXECUTOR_H
	
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <functional>
#include <future>


class ThreadExecutor  
{
public:

	ThreadExecutor();
	~ThreadExecutor();

	// adds new task to queue, resumes execution for first task added
	std::future<void> addTask(std::function<void()> task);
	// waits for all tasks to finish, than returns
	void stop();
	bool isExecuting() const;
	int task_count() const;

private:
	struct ScheduledTask{
		std::function<void()> function;
		std::promise<void> promise;
	};

	std::thread thread_;
	std::deque<ScheduledTask> task_queue_;
	std::mutex m_add_task_;
	std::mutex m_no_tasks_;
	std::condition_variable cond_no_tasks_;
	bool finish_execution_; // when set to true will finish executing all tasks and stop execution

	// endless loop running tasks
	void execute();
	// stops thread execution if no tasks available
	void waitForTask();
};

#endif