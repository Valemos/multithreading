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

	// MUST NOT add tasks, that temselves will add tasks to this ThreadPool. 
	// This will create a deadlock
	std::future<void> addTask(std::function<void()> task);

	// waits for all tasks to finish, than returns
	void stop();
	bool isExecuting() const noexcept;
	int task_count() const noexcept;

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
	bool finish_execution_;

	// endless loop running tasks
	void execute();
	// stops thread execution if no tasks available
	void waitForTask();
};

#endif