#if !defined _CEXCEPTION_H
#define _CEXCEPTION_H

#include <stdio.h>
#include <string>


using namespace std;

class CCommonException
{
public:
	CCommonException(string sErrMsg) {m_str= sErrMsg;};
	CCommonException(string sErrMsg, string sUrl) {m_str=sErrMsg; m_url=sUrl;};

	const char* GetErrMsg() {return m_str.c_str();};
	const char* GetUrl() {return m_url.c_str();};
public:
	string m_str;
	string m_url;
};

class CMysqlException: public CCommonException
{
public:
	CMysqlException(const char* sErrMsg):CCommonException(sErrMsg) {};
	CMysqlException(const char* sErrMsg, const char* sUrl):CCommonException(sErrMsg, sUrl) {};
};

#endif
