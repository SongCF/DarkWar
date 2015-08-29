#include "LFStrings.h"

#define ROOT_PATH	"config/"
#define FILE_MAIN	"strings.txt"

// 所支持的标签
#define LABLE_KEY		"key"
#define LABLE_VALUE		"value"
#define LABLE_INCLUDE	"include"

map<string,string> LFStrings::data;

// 用于缓存已经解析了的文件
static map<string,int> paresedFile;

bool LFStrings::init()
{
	data.clear();
	paresedFile.clear();
	return parseFile(FILE_MAIN);
}

bool LFStrings::parseFile(string relativePath)
{
	// 看是否已经解析过了
	if (paresedFile.find(relativePath) != paresedFile.end())
	{
		CCLog("Waring : %s --> file[%s] is pared over before or is in parsing now ,do you include it again???",__FUNCTION__,relativePath.c_str());
		return true;
	}
	paresedFile.insert(pair<string, int>(relativePath,0));


	string path = string(ROOT_PATH)+relativePath;
	CCLog("%s parsing file [%s] ....",__FUNCTION__,path.c_str());

	Json::Reader reader;
	Json::Value root;
	unsigned long size;
	const char* doc = LFFileUtiles::getLFFileUtiles()->readResourceFile(path,&size);
	if (size == 0)
	{
		CCLog("%s ----> file [%s] have no content.",__FUNCTION__,path.c_str());
		return true;
	}

	if (doc == NULL)
	{
		CCLog("%s ----> doc is NULL , open Error!",__FUNCTION__,path.c_str());
		return false;
	}

	if (!reader.parse(doc,root))
	{
		CCLog("%s ----> reader.parse failed! [%s]",__FUNCTION__,path.c_str());
		CCLog(doc);
		delete [] doc;
		CCAssert(false,"");
		return false;
	}

	//CCLog(doc);
	Json::Value _strings= root["strings"];
	if (!_strings.isNull() && _strings.isArray())
	{
		int _stringNum = _strings.size();
		for (int i=0;i<_stringNum;i++)
		{
			Json::Value onePair = _strings[i];
			// 校验
			if(!onePair[LABLE_KEY].isNull() && onePair[LABLE_VALUE].isNull())
			{
				CCLog("Error : %s there is no %s for %s [%s]",__FUNCTION__,LABLE_VALUE,LABLE_KEY,onePair[LABLE_KEY].asCString());
				CCAssert(false,"key 和 value 没有配对");
				continue;
			}
			if(!onePair[LABLE_VALUE].isNull() && onePair[LABLE_KEY].isNull())
			{
				CCLog("Error : %s there is no %s for %s [%s]",__FUNCTION__,LABLE_KEY,LABLE_VALUE,onePair[LABLE_VALUE].asCString());
				CCAssert(false,"key 和 value 没有配对");
				continue;
			}


			//有include
			if (!onePair[LABLE_INCLUDE].isNull())
			{
				string file = onePair[LABLE_INCLUDE].asString();
				if (!parseFile(file))
				{
					CCLog("Error : %s parse file %s faild",__FUNCTION__,file.c_str());
					return false;
				}
			}
			// 解析键值对
			if (!onePair[LABLE_KEY].isNull() && !onePair[LABLE_VALUE].isNull())
			{
				string _key = onePair[LABLE_KEY].asString();
				string _value = onePair[LABLE_VALUE].asString();

				if (haveValue(_key))
				{
					CCLog("Waring : %s --> the key [%s] in file[%s] is already used before,we will ignore this key and value[%s]!",__FUNCTION__,_key.c_str(),path.c_str(),_value.c_str());
					continue;
				}
				data.insert(pair<string, string>(_key,_value));
			}
		}
	}

	return true;
}

string LFStrings::getValue(string _key)
{
	map<string, string>::iterator iter;
	//如果map中没有要查找的数据，它返回的迭代器等于end函数返回的迭代器;
	iter = data.find(_key);
	if(iter != data.end())
	{
		return iter->second;
	}
	CCLog("Waring : LFStrings::getValue --> _key[%s] is not found!",_key.c_str());
	return "";
}

bool LFStrings::haveValue(string _key)
{
	map<string, string>::iterator iter;
	//如果map中没有要查找的数据，它返回的迭代器等于end函数返回的迭代器;
	iter = data.find(_key);
	return iter != data.end();
}