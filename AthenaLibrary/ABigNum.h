/*

AthenaClasses Library For Microsoft Visual C++

Copyright @2016 CDU INnoVation Studio
All rights reserved.

文件名称：ABigNum.h
摘要：
1.本文件含大数类的声明，实现在ABigNum.cpp中

当前版本：V3.0
作者：余悦
e-mail：1397011447@qq.com
创建日期：2016年07月18日
更新日期：2016年12月28日

修正日志：
V1.1，修复了metaDivi中的一个bug，修复了operator/=中的巨大隐患。
V2.0，调整了类的声明，现在私有成员对外不可见。
V3.0，增加了各种比较运算符。

*/

#pragma once

#include <iostream>
#include <memory>

#include "ACore.h"

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
	bool operator<(const ABigNum&) const;
	bool operator==(const ABigNum&) const;
	bool operator<=(const ABigNum&) const;
	bool operator>=(const ABigNum&) const;
	bool operator!=(const ABigNum&) const;
	bool less(const ABigNum &) const;
	bool greater(const ABigNum &) const;
	bool equal(const ABigNum &) const;
	bool lessEqual(const ABigNum &) const;
	bool greaterEqual(const ABigNum &) const;
	bool notEqual(const ABigNum &) const;
	bool isOk() const;
	int getLength() const;
	const char *getBase() const;
	~ABigNum();
protected:
	bool compare(const bool *, const ABigNum &) const;
	void metaAdd(ABigNum *, const ABigNum *, const ABigNum *) const;
	void metaSub(ABigNum *, const ABigNum *, const ABigNum *) const;
	void metaMulti(ABigNum *, const ABigNum *, const ABigNum *) const;
	void metaDivi(ABigNum *, const ABigNum *, const ABigNum *) const;
	void resetSize(int);
protected:
	std::unique_ptr<bigNumData> data;
	const static int baseSize = 2000;
};

ATHENA_END