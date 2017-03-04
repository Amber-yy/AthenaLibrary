/*

Athena Classes Library For Microsoft Visual C++

Copyright @2016 CDU INnoVation Studio
All rights reserved.

文件名称：ASetting.h
摘要：
1.本文件含有ini读写类的声明，实现在ASetting.cpp中。
2.ini读写类能实现通过键和值来读写ini文件中的内容。
3.所有功能的实现均不依赖于Windows API，所以理论上讲本文件可以实现跨平台。
4.ini文件一行最多只能有1024个字符，否则会出现错误。
5.支持ANSI格式，UTF-8仅支持无BOM格式。

当前版本：V1.1
作者：余悦
e-mail：1397011447@qq.com
创建日期：2016年11月25日
更新日期：2016年12月02日

修正日志：
v1.1，修正底层数据表示的一个逻辑错误

*/

#pragma once

#include <string>
#include <memory>
#include "ACore.h"

ATHENA_BEGIN

struct ASettingData;

class ASetting final
{
public:
	ASetting();
	ASetting(const ASetting &);
	ASetting(ASetting &&);
	explicit ASetting(const char *);
	explicit ASetting(const std::string &);
	const std::string &getValue(const std::string&, const std::string &) const;
	bool setValue(const std::string&, const std::string &, const std::string&);
	bool saveToFile();
	~ASetting();
private:
	void readFromFile();
private:	
	std::unique_ptr<ASettingData> data;
};

ATHENA_END
