#include <algorithm>
#include <string.h>
#include "SDConfiguration.h"
#include <stdio.h>

using namespace std;

SDConfiguration::SDConfiguration(const char* file_name)
{
	strcpy(m_file_name, file_name);
}

SDConfiguration::SDConfiguration()
{
}

SDConfiguration::~SDConfiguration()
{
	m_content.clear();
}

bool SDConfiguration::load(const char* file_name)
{
	if(file_name != NULL)
	{
		strcpy(m_file_name, file_name);
	}

	char buffer[max_line_length], *p_ret;
	FILE* fp = fopen(m_file_name, "r");

	if(!fp)
	{
		return false;
	}

	m_content.clear();
	while(true) 
	{
		p_ret = fgets(buffer, max_line_length, fp);
		if(!p_ret)
		{
			break;
		}
		loadLine(buffer);
	}
	fclose(fp);
	return true;
}

bool SDConfiguration::loadLine(char* buffer)
{
	char *p_eq = strchr(buffer, '=');
	if(p_eq)
	{
		int pos = p_eq - buffer;
		char *p_LF = strrchr(buffer, '\n');
		string key = string(buffer, 0, pos);
		string value = string(buffer, pos + 1, p_LF - p_eq - 1);
		trim(key);
		trim(value);
		if(key[0] != '#' || key[0] != '/')
		{
			insertItem(SDItemData(key, value));
		}
	}
	else
	{
		return false;
	}
	return true;
}

void SDConfiguration::trim(string& s)
{
	int bpos = s.find_first_not_of(0x20);
	int epos = s.find_last_not_of(0x20);
	if(bpos < 0 || epos < 0)
		s.empty();
	else
		s = string(s, bpos, epos - bpos + 1);
}

void SDConfiguration::insertItem(const SDItemData& item)
{
	vector<SDItemData>::iterator it_find 
		= lower_bound(m_content.begin(), m_content.end(), item);

	if(it_find == m_content.end() || it_find->m_key != item.m_key)
	{
		m_content.insert(it_find, item);
	}
}

string SDConfiguration::getString(const char* key,const char* strdefault)
{
	vector<SDItemData>::iterator it_find 
		= lower_bound(m_content.begin(), m_content.end(), SDItemData(key, ""));

	if(it_find != m_content.end() && it_find->m_key == key)
	{
		return it_find->m_value;
	}
	return strdefault;
}

int SDConfiguration::getInt(const char* key,int ndefault)
{
	vector<SDItemData>::iterator it_find 
		= lower_bound(m_content.begin(), m_content.end(), SDItemData(key, ""));

	if(it_find != m_content.end() && it_find->m_key == key)
	{
		return atoi(it_find->m_value.c_str());
	}
	return ndefault;
}
