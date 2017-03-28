#include "AThread.h"

#include <thread>
#include <mutex>

ATHENA_BEGIN

struct threadData
{
	std::thread thread;
	std::mutex locker;
	std::condition_variable signal;
	bool running;
};

AThread::AThread()
{
	data = std::make_unique<threadData>();
	data->thread = std::thread(&AThread::exec, this);
	data->thread.detach();
}

AThread::~AThread()
{

}

void AThread::start()
{
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
		run();
		data->running = false;
	}
}

void AThread::run()
{
}

ATHENA_END