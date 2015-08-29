#pragma once
#include <map>
#include <string>
using namespace std;

/**
 *	数据容器，模拟java的Bundle
 */
class Bundle
{
protected:
	map<string,int> data_int;
	map<string,double> data_double;
	map<string,string> data_string;
	map<string,bool> data_bool;
	map<string,char> data_char;
	map<string,void*> data_pointer;
public:
	Bundle(void);

	void putInt(string key,int value);
	void putDouble(string key,double value);
	void putString(string key,string value);
	void putBool(string key,bool value);
	void putChar(string key,char value);

	int getInt(string key,int defaultValue = -1);
	double getDouble(string key,double defaultValue = -1);
	string getString(string key,string defaultValue = "");
	bool getBool(string key,bool defaultValue = false);
	char getChar(string key,char defaultValue = 0);

	void putPointer(string key,void* value);
	void* getPointer(string key,void* defaultvalue = NULL);

	void clear();

	~Bundle(void);
};

