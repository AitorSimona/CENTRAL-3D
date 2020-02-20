#ifndef MODULETHREADING_H
#define MODULETHREADING_H

#include "Module.h"
#include <mutex>
#include <queue>
#include <functional>
#include <vector>
#include <thread>
#include <condition_variable>
#include <atomic>

#define ADDTASK(object, function, ...) ModuleThreading::AddTask(std::bind(&function, object, __VA_ARGS__))

class ModuleThreading : public Module {
public:
	ModuleThreading(bool start_enabled = true);
	~ModuleThreading();

	bool Init(json file) override;
	bool Start() override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void AddTask(std::function<void()> newTask);

private:
	void ShutdownPool();
	void ProcessTasks(int threadID, std::atomic<bool>& stop);
	void FinishProcessing();

private:
	std::queue <std::function<void()>> tasksQueue;
	std::vector <std::thread> threadVector;
	//True when thread is executing a task
	std::vector <bool> threadStatus;
	std::mutex tQueueMutex;
	std::mutex threadPoolMutex;
	std::condition_variable condition;

	unsigned int concurrentThreads = 0;

	//Flags
	std::atomic<bool> stopPool{ false };
	bool poolTerminated = false;
};

#endif