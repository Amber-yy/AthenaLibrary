#include "ASetting.h"
#include <fstream>
#include <map>
#include <vector>

ATHENA_BEGIN

struct line
{
	std::string data;
	int id;
};

struct origin
{
	origin():isSection(false){}
	std::string data;
	bool isSection;
};

using keyValue=std::map<std::string,line>;
using iniValue= std::map<std::string, keyValue>;
using stringVector = std::vector<std::string>;

struct ASettingData
{
	void operator=(const ASettingData& asd)
	{
		isOk = asd.isOk;
		fileName = asd.fileName;
		values = asd.values;
		addData = asd.addData;
		addValue = asd.addValue;
	}
	bool isOk;
	std::string fileName;
	std::string emptyValue;
	std::vector<origin> originData;//原始数据
	iniValue addData;//原始数据中有的section增加的键值对
	iniValue addValue;//增加的section
	iniValue values;
};

ASetting::ASetting()
{
	data = std::make_unique<ASettingData>();
}

ASetting::~ASetting()
{
	saveToFile();
}

ASetting::ASetting(const ASetting &as)
{
	data = std::make_unique<ASettingData>();
	*data = *as.data;
}

ASetting::ASetting(ASetting &&as)
{
	as.data.swap(data);
}

void ASetting::readFromFile()
{
	if (data->fileName.empty())
	{
		data->isOk = false;
		return;
	}

	std::ifstream inputFile(data->fileName);
	if (!inputFile.is_open())
	{
		data->isOk = false;
		return;
	}

	char buffer[1024];
	std::string currentSection;
	data->values.insert(std::pair<std::string, keyValue>(currentSection, keyValue()));

	for (int l=0;!inputFile.eof();l++)
	{
		int equalIndex=-1,len;
		inputFile.getline(buffer, sizeof(buffer));


		origin metaData;
		metaData.data = buffer;
		data->originData.push_back(std::move(metaData));

		if (buffer[0] == ';')
		{
			continue;
		}

		for (len= 0;buffer[len]!=0; len++)
		{
			if (buffer[len] == '='&&equalIndex==-1)
			{
				equalIndex = len;
			}
		}

		if (buffer[0] == '['&&buffer[len - 1] == ']')
		{
			buffer[len - 1] = 0;
			currentSection=buffer + 1;
			if (data->values.find(currentSection) == data->values.end())
			{
				data->values.insert(std::pair<std::string,keyValue>(currentSection,keyValue()));
			}
			data->originData[l].data = currentSection;
			data->originData[l].isSection = true;
			continue;
		}

		if (equalIndex == -1)
		{
			continue;
		}

		line value;
		value.data=buffer + equalIndex + 1;
		value.id = l;
		buffer[equalIndex] = 0;
		std::string key(buffer);
		
		auto target = data->values.find(currentSection);
		target->second[key] = std::move(value);
	}

	data->isOk = true;
}

ASetting::ASetting(const char *file)
{
	data = std::make_unique<ASettingData>();
	data->fileName = file;
	readFromFile();
}

ASetting::ASetting(const std::string &file):ASetting(file.c_str())
{

}

const std::string & ASetting::getValue(const std::string &section, const std::string &key) const
{
	auto it = data->values.find(section);
	if (it == data->values.end())
	{
		return data->emptyValue;
	}

	auto result = it->second.find(key);
	if (result == it->second.end())
	{
		return data->emptyValue;
	}

	return result->second.data;
}

bool ASetting::setValue(const std::string &section, const std::string &key, const std::string &value)
{
	auto it = data->values.find(section);
	if (it == data->values.end())//如果是原始数据中不存在的section
	{
		it = data->addValue.find(section);
		if (it == data->addValue.end())//如果在新增section中也不存在
		{
			auto &map = data->addValue[section];
			map[key].data = std::move(value);
			return true;
		}
		it->second[key].data = std::move(value);
		return true;
	}

	auto result = it->second.find(key);

	if (result == it->second.end())//如果是原始数据中存在的section，但key不存在
	{
		auto da=data->addData.find(section);

		if (da == data->addData.end())
		{
			std::pair<std::string, line> oneData;
			oneData.first = std::move(key);
			oneData.second.data = std::move(value);
			keyValue kv;
			kv.insert(std::move(oneData));

			std::pair<std::string, keyValue> result;
			result.first = std::move(section);
			result.second = std::move(kv);

			data->addData.insert(std::move(result));
			return true;
		}

		std::pair<std::string, line> oneData;
		oneData.first = std::move(key);
		oneData.second.data = std::move(value);

		da->second.insert(oneData);
		return true;
	}

	data->originData[result->second.id].data = key+'='+value;
	result->second.data = key + '=' + value;
	return true;
}

bool ASetting::saveToFile()
{
	std::ofstream outFile(data->fileName);
	if (!outFile.is_open())
	{
		return false;
	}

	std::string temp;

	bool flag = false;

	for (auto &oneLine : data->originData)
	{
		if (flag)
		{
			temp = '\n';
		}
		else
		{
			flag = true;
			temp.clear();
		}
		if (oneLine.isSection)//如果这一行是section
		{
			temp += '[';
			temp += oneLine.data;//写入一行
			temp += ']';
			outFile << temp;

			auto it = data->addData.find(oneLine.data);//检查有木有额外插入的数据
			if (it == data->addData.end())
			{
				continue;
			}
			auto &map = it->second;
			for (auto &record : map)//有则逐行写入文件
			{
				temp = '\n';
				temp +=record.first;
				temp += '=';
				temp+=record.second.data;
				outFile << temp;
			}
		}
		else
		{
			temp += oneLine.data;//写入一行
			outFile << temp;
		}
	}

	for (auto &newSection : data->addValue)
	{
		temp = '\n';
		temp += '\n';
		temp += '[';//写入section
		temp += newSection.first;
		temp.push_back(']');
		
		outFile << temp;

		for (auto &record : newSection.second)//写入section的每一行
		{
			temp = '\n';
			temp += record.first;
			temp.push_back('=');
			temp += record.second.data;
			outFile << temp;
		}
	}

	return true;
}

ATHENA_END
