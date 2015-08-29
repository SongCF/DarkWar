/********************************************************************
	created:		2005/08/27
	created:		27:8:2005   9:49
	filename: 		d:\work\newcvs\chub\impl\src\common\sdconfiguration.h
	file path:		d:\work\newcvs\chub\impl\src\common
	file base:		sdconfiguration
	file ext:		h
	author:			lifeng
	description:	a simple configuartion (only read now)
					key=value
*********************************************************************/

#ifndef SANDAI_C_SDCONFIGURATION_H_200508270949
#define SANDAI_C_SDCONFIGURATION_H_200508270949

#include <vector>
#include <string>

class SDItemData
{
public:
	SDItemData(const std::string& s1, const std::string& s2)
	{
		m_key = s1;
		m_value = s2;
	}

	std::string m_key;
	std::string m_value;

	friend bool operator< (const SDItemData& s1, const SDItemData& s2)
	{
		return s1.m_key < s2.m_key;
	}

protected:
private:
};

class SDConfiguration
{
	static const int max_path = 256;
	static const int max_line_length = 256;
public:
	SDConfiguration(const char* file_name);
	SDConfiguration();
	virtual ~SDConfiguration();

	bool load(const char* file_name = NULL);
	char*	getConfigurationName() {return m_file_name;}
public:
	std::string getString(const char* key,const char* strdefault = "");
	int getInt(const char* key,int ndefault = 0);

protected:
private:
	char m_file_name[max_path];
	std::vector<SDItemData> m_content;

	bool loadLine(char* buffer);
	void trim(std::string&);
	void insertItem(const SDItemData&);
};

#endif
