#include "ModuleThreading.h"
#include "Globals.h"

ModuleThreading::ModuleThreading(bool start_enabled) : Module(start_enabled)
{
}

ModuleThreading::~ModuleThreading()
{
}

bool ModuleThreading::Init(json file)
{
	concurrentThreads = std::thread::hardware_concurrency() - 1;
	return true;
}

bool ModuleThreading::Start()
{
	for (int i = 0; i < concurrentThreads; i++) {
		threadStatus.push_back(false);
		threadVector.push_back(std::thread(&ModuleThreading::ProcessTasks, this, i, std::ref(stopPool)));
	}

	CONSOLE_LOG("Created %d threads.", concurrentThreads);
	
	poolTerminated = false;
	stopPool = false;

	return true;
}

update_status ModuleThreading::Update(float dt)
{
	bool processing = true;
	while (processing) {
		tQueueMutex.lock();
		//If our task queue is empty we check that our thread pool has finished processing
		if (tasksQueue.empty()) {
			tQueueMutex.unlock();
			std::unique_lock<std::mutex> threadPoolLock(threadPoolMutex);

			bool threadsProcessing = false;
			for (int i = 0; i < concurrentThreads && !threadsProcessing; ++i)
				threadsProcessing = threadsProcessing || threadStatus[i];

			processing = threadsProcessing;
		}
		//Otherwise we process a task ourselves
		else {
			std::function<void()> Task;
			Task = tasksQueue.front();
			tasksQueue.pop();
			tQueueMutex.unlock();
			Task();
		}
	}
	return UPDATE_CONTINUE;
}

bool ModuleThreading::CleanUp()
{
	if (!poolTerminated)
		ShutdownPool();

	return true;
}

void ModuleThreading::AddTask(std::function<void()> newTask)
{
	{
		std::lock_guard<std::mutex> lk(tQueueMutex);
		tasksQueue.push(newTask);
	}
	condition.notify_one(); //Waking up a thread to process the new task
}

void ModuleThreading::ShutdownPool()
{
	{
		std::lock_guard<std::mutex> lk(threadPoolMutex);
		stopPool = true;
	}

	condition.notify_all(); //Waking up all threads

	//Joining all threads
	for (std::thread& currentThread : threadVector)
		currentThread.join();

	threadVector.empty();
	threadStatus.empty();
	poolTerminated = true;
}

void ModuleThreading::ProcessTasks(int threadID, std::atomic<bool>& stop)
{
	while (true)
	{
		{
			std::lock_guard<std::mutex> lk(threadPoolMutex);
			threadStatus[threadID] = false;
		}

		std::function<void()> Task;

		{
			std::unique_lock<std::mutex> lock(tQueueMutex);
			condition.wait(lock, [&] { return !tasksQueue.empty() || stop; });

			if (stop) //If we need to stop the thread, we break the infinite loop
				break;

			{
				std::lock_guard<std::mutex> lk(threadPoolMutex);
				threadStatus[threadID] = true;
			}

			Task = tasksQueue.front();
			tasksQueue.pop();
		}

		Task();
	}
}
