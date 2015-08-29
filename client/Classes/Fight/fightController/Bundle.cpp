#include "Bundle.h"

// 从map中获取数据，
// MAP_K			: map定义时的key
// MAP_V			: map定义时的value
// MAP				: map
// KEY_TO_FOUND		: 要在Map中查找的key
// DEFAULT_VALUE	: 没找打返回的数据
#define GET_FROM_MAP(MAP_K,MAP_V,MAP,KEY_TO_FOUND,DEFAULT_VALUE) 	\
						map<MAP_K,MAP_V>::iterator l_it;			\
						l_it=MAP.find(KEY_TO_FOUND);				\
						if(l_it==MAP.end())							\
						{											\
							return DEFAULT_VALUE;					\
						}											\
						return l_it->second; 


Bundle::Bundle(void)
{
}

void Bundle::putInt(string key,int value)
{
	data_int.insert(pair<string,int>(key,value));
}
void Bundle::putDouble(string key,double value)
{
	data_double.insert(pair<string,double>(key,value));
}
void Bundle::putString(string key,string value)
{
	data_string.insert(pair<string,string>(key,value));
}
void Bundle::putBool(string key,bool value)
{
	data_bool.insert(pair<string,bool>(key,value));
}
void Bundle::putChar(string key,char value)
{
	data_char.insert(pair<string,char>(key,value));
}

void Bundle::putPointer(string key,void* value)
{
	data_pointer.insert(pair<string,void*>(key,value));
}

int Bundle::getInt(string key,int defaultValue /*= -1*/)
{
	//map<string,int>::iterator l_it;; 
	//l_it=data_int.find(key);
	//if(l_it==data_int.end())
	//{
	//	return defaultValue;
	//}
	//return l_it->second;
	GET_FROM_MAP(string,int,data_int,key,defaultValue);
}
double Bundle::getDouble(string key,double defaultValue /*= -1*/)
{
	GET_FROM_MAP(string,double,data_double,key,defaultValue);
}
string Bundle::getString(string key,string defaultValue /*= ""*/)
{
	GET_FROM_MAP(string,string,data_string,key,defaultValue);
}
bool Bundle::getBool(string key,bool defaultValue /*= false*/)
{
	GET_FROM_MAP(string,bool,data_bool,key,defaultValue);
}
char Bundle::getChar(string key,char defaultValue /*= 0*/)
{
	GET_FROM_MAP(string,char,data_char,key,defaultValue);
}
void* Bundle::getPointer(string key,void* defaultvalue /*= NULL*/)
{
	GET_FROM_MAP(string,void*,data_pointer,key,defaultvalue);
	/*map<string,void*>::iterator l_it;; 
	l_it=data_pointer.find(key);
	if(l_it==data_pointer.end())
	{
	return defaultvalue;
	}
	return l_it->second;*/
}

void Bundle::clear()
{
	data_int.clear();
	data_double.clear();
	data_string.clear();
	data_bool.clear();
	data_char.clear();
	data_pointer.clear();
}

Bundle::~Bundle(void)
{

}
