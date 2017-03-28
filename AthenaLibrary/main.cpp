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

#include <algorithm>

class test
{
public:
	void operator()()
	{
		std::cout << "999\n";
	}
};

int main()
{
	myThread thr;
	
	test c;

	while (true)
	{
		_getch();
		thr.start(c);
	}

	return 0;
}
