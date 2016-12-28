/*

Yuyue's Classes Library For Microsoft Visual C++

Copyright @2016 CDU INnoVation Studio
All rights reserved.

文件名称：ABigNum.h
摘要：
1.本文件含大数类的声明，实现在ABigNum.cpp中

当前版本：V2.0
作者：余悦
e-mail：1397011447@qq.com
创建日期：2016年07月18日
更新日期：2016年12月28日

修正日志：
V1.1，修复了metaDivi中的一个bug，修复了operator/=中的巨大隐患。
V2.0，调整了类的声明，现在私有成员对外不可见。

*/

#pragma once

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib,"ABigNum64d")
#else
#pragma comment(lib,"ABigNum64")
#endif

#else

#ifdef _DEBUG
#pragma comment(lib,"ABigNumd")
#else
#pragma comment(lib,"ABigNum")
#endif

#endif

#include "ACore.h"

#include <iostream>
#include <memory>

ATHENA_BEGIN

struct bigNumData;

class ABigNum final
{
public:
	ABigNum();
	explicit ABigNum(long long);
	ABigNum(ABigNum &&);
	ABigNum(const ABigNum &);
	friend std::ostream & operator<<(std::ostream &,const ABigNum &);
	friend std::istream & operator>>(std::istream &,  ABigNum &);
	ABigNum & operator+=(const ABigNum &);
	ABigNum & operator-=(const ABigNum &);
	ABigNum & operator*=(const ABigNum &);
	ABigNum & operator/=(const ABigNum &);
	ABigNum &operator=(const ABigNum &);
	ABigNum &operator=(ABigNum &&);
	ABigNum operator-(const ABigNum &) const;
	ABigNum operator+(const ABigNum &) const;
	ABigNum operator*(const ABigNum &) const;
	ABigNum operator/(const ABigNum &) const;
	bool operator>(const ABigNum&) const;
	bool isOk() const;
	int getLength() const;
	const char *getBase() const;
	~ABigNum();
protected:
	void metaAdd(ABigNum *, const ABigNum *, const ABigNum *) const;
	void metaSub(ABigNum *, const ABigNum *, const ABigNum *) const;
	void metaMulti(ABigNum *, const ABigNum *, const ABigNum *) const;
	void metaDivi(ABigNum *, const ABigNum *, const ABigNum *) const;
	void resetSize(int);
	std::unique_ptr<bigNumData> data;
	const static int baseSize = 2000;
};

ATHENA_END