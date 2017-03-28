/*

Athena Classes Library For Microsoft Visual C++

Copyright @2016 CDU INnoVation Studio
All rights reserved.

文件名称：ADBConnector.h
摘要：
1.本文件含有多线程类的声明，实现在AThread.cpp中。
2.AThread类提供了一种线程复用方案，一个线程被创建后，不会只运行一次就失效，而可以被多次使用。
3.多线程类实际上是对标准库中的thread类的封装，但线程默认与主线程分离。
4.调用start函数来启动目标线程，线程启动时，掉用的函数为run，run函数可被重写。

当前版本：V1.1
作者：余悦
e-mail：1397011447@qq.com
创建日期：2017年03月28日
更新日期：2017年03月28日

修正日志：
V1.1，现在线程start的时候，可以绑定任意callable，而不只是绑定在run函数上。

*/

#pragma once

#include <memory>
#include <functional>

#include "ACore.h"

ATHENA_BEGIN

struct threadData;

class AThread
{
public:
	AThread();
	virtual ~AThread();
	void start();
	void start(std::function<void()> fun);
	bool isRunning();
	bool isFinished();
protected:
	virtual void exec();
	virtual void run();
protected:
	std::unique_ptr<threadData> data;
};

ATHENA_END