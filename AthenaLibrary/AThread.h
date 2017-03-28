/*

Athena Classes Library For Microsoft Visual C++

Copyright @2016 CDU INnoVation Studio
All rights reserved.

文件名称：ADBConnector.h
摘要：
1.本文件含有MySQL数据库操作类的声明，实现在ADBConnector.cpp中。
2.ADBConnector类的对象能实现连接MySQL数据库，并执行相关的操作。
3.所有的功能均是对MySQL提供C API的封装。
4.通过ADBConnector来访问数据库并不会比纯C API慢，因为数据库的性能瓶颈在于数据库本身以及硬盘。

当前版本：V1.0
作者：余悦
e-mail：1397011447@qq.com
创建日期：2016年12月03日
更新日期：2016年12月26日

修正日志：

*/

#pragma once

#include <memory>

#include "ACore.h"

ATHENA_BEGIN

struct threadData;

class AThread
{
public:
	AThread();
	virtual ~AThread();
	void start();
	bool isRunning();
	bool isFinished();
protected:
	virtual void exec();
	virtual void run();
protected:
	std::unique_ptr<threadData> data;
};

ATHENA_END