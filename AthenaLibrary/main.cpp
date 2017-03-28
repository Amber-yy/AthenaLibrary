#include <iostream>
#include <conio.h>

#include "AThread.h"

class myThread :public athena::AThread
{
protected:
	virtual void run() override;
};

void myThread::run()
{
	std::cout << "666\n";
}

int main()
{
	myThread thr;

	while (true)
	{
		_getch();
		thr.start();
	}

	return 0;
}
