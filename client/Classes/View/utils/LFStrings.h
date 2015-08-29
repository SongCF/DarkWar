#pragma once
#include <string>
#include "include_json.h"
#include "LFFileUtiles.h"
#include <map>

using namespace std;

/**
 *	字符串工具，是中文字符的键值对
 */
class LFStrings
{
public:
	static bool init();
	static string getValue(string _key);
	static bool haveValue(string _key);
private:

	static bool parseFile(string relativePath);

	static map<string,string> data;
};
