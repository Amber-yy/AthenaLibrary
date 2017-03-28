#include "AThread.h"

#include <thread>
#include <mutex>
#include <atomic>

ATHENA_BEGIN

struct threadData
{
	std::thread thread;
	std::mutex locker;
	std::condition_variable signal;
	std::function<void()> fun;
	std::atomic_bool running;
	std::atomic_bool useFun;
};

AThread::AThread()
{
	data = std::make_unique<threadData>();
	data->thread = std::thread(&AThread::exec, this);
	data->thread.detach();
	data->running = false;
}

AThread::~AThread()
{

}

void AThread::start()
{
	if (data->running)
	{
		return;
	}
	data->useFun = false;
	data->signal.notify_one();
}

void AThread::start(std::function<void()> fun)
{
	if (data->running)
	{
		return;
	}
	data->fun = fun;
	data->useFun = true;
	data->signal.notify_one();
}

bool AThread::isRunning()
{
	return data->running;
}

bool AThread::isFinished()
{
	return !data->running;
}

void AThread::exec()
{
	std::unique_lock<std::mutex> lock(data->locker);
	while (true)
	{
		data->signal.wait(lock);
		data->running = true;
		if (data->useFun)
		{
			data->fun();
		}
		else
		{
			run();
		}
		data->running = false;
	}

}

void AThread::run()
{
}

ATHENA_END