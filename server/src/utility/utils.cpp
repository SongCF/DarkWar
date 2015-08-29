/********************************************************
创建者:吴晓勇
创建时间:2006-06-06
描述:

修订记录:
    修订者:
    修订时间:
    修订说明:
*********************************************************/
#include <stdlib.h>
#include <string>
#include <errno.h>
//#include <asm/page.h>
#include "utils.h"
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <cmath>

/*
char* Utl::LTrim(const char * pszStr)
{
    static char* pszRetStr = 0;

    if(pszRetStr != 0)
    {
        delete pszRetStr;
    }

    int iLen = strlen(pszStr);
    pszRetStr = new char[iLen + 1];

    int i;
    for(i = 0; i < iLen; ++i)
    {
        if(pszStr[i] != ' ')
        {
            break;
        }
    }

    if(i == iLen)
    {
        //pszStr全是空格
        //置为空串
        *pszRetStr = 0;

        return pszRetStr;
    }

    strcpy(pszRetStr, pszStr + i);

    return pszRetStr;
}
*/

void Utl::LTrim(string & str)
{
    //找到第一个不是空格的字符
    string::size_type pos;
    pos = str.find_first_not_of(" ");
    
    if(pos == string::npos)
    {
        //字符串中全是空格
        str = "";
        return;
    }

    str = str.substr(pos);
    return;
}

void Utl::Trim(string& s)
{
	int bpos = s.find_first_not_of(0x20);
	int epos = s.find_last_not_of(0x20);
	if(bpos < 0 || epos < 0)
		s.empty();
	else
		s = string(s, bpos, epos - bpos + 1);
}


int Utl::AtoI(const char * pszStr)
{
    return atoi(pszStr);
}

char* Utl::Null2Empty(char * ptr)
{
    return (ptr == 0) ? (char*)"" : ptr;
}

char* Utl::FillStr_LeftBlank(const char * str, int iWantedLen)
{
    static char* s_NewStr = 0;
    if(s_NewStr != 0)
    {
        delete s_NewStr;
        s_NewStr = 0;
    }

    int iLen = strlen(str);
    s_NewStr = new char[((iLen > iWantedLen) ? iLen : iWantedLen) + 1];

    if(iLen >= iWantedLen)
    {
        strcpy(s_NewStr, str);
    }
    else
    {
        char pszFmtStr[32];
        sprintf(pszFmtStr, "%%%ds", iWantedLen);
        sprintf(s_NewStr, pszFmtStr, str);
    }

    return s_NewStr;
}

char* Utl::Int64ToStr(long long i64)
{
    static char s_str[24];
    sprintf(s_str, "%lld", i64);
    return s_str;
}

char* Utl::r_http_code(const char* str)
{
    static char* pBuf = 0;
    if(pBuf != 0)
    {
        delete[] pBuf;
    }
    
    pBuf = new char[strlen(str) + 1];
    strcpy(pBuf, str);
    
    int i, j;
    int iLen = strlen(pBuf);
    for(i=0, j=0; i<iLen; ++i, ++j)
    {
        if(pBuf[i] == '%')
        {
            char buf[5];
            sprintf(buf, "0x%c%c"
                , pBuf[i+1], pBuf[i+2]);
            i += 2;
            char* x;
            pBuf[j] = strtol(buf, &x, 16);
        }
        else
        {
            pBuf[j] = pBuf[i];
        }
    }
    pBuf[j] = 0;

    return pBuf;
}

void Utl::r_http_code_threadsafe(const char* str, string& ret_str)
{
    char* pBuf = 0;
    
    pBuf = new char[strlen(str) + 1];
    strcpy(pBuf, str);
    
    int i, j;
    int iLen = strlen(pBuf);
    for(i=0, j=0; i<iLen; ++i, ++j)
    {
        if(pBuf[i] == '%')
        {
            char buf[5];
            sprintf(buf, "0x%c%c"
                , pBuf[i+1], pBuf[i+2]);
            i += 2;
            char* x;
            pBuf[j] = strtol(buf, &x, 16);
        }
        else
        {
            pBuf[j] = pBuf[i];
        }
    }
    pBuf[j] = 0;

    ret_str = pBuf;

    delete[] pBuf;
    
    return;
}

//计算mmap同步需要的地址和大小
void Utl::MmapSyncPointAndSize(void* pPIn, int iSizeIn, void*& pPOut, int& iSizeOut)
{
    /* PAGE_SIZE在__x86_64__定义的情况下找不到定义
    pPOut = 
        (void*)((long)(pPIn) / PAGE_SIZE * PAGE_SIZE);
    
    void* pEnd = (void*)(((long)(pPIn)+iSizeIn) / PAGE_SIZE * PAGE_SIZE);
    
    iSizeOut = (((long)pEnd-(long)pPOut)+1) * PAGE_SIZE;
    */

    return;
}

char* Utl::UperCase(const char * pszStr)
{
    static char* pStr = 0;

    if(pStr != 0)
    {
        delete[] pStr;
    }

    int iStrLen = strlen(pszStr);

    pStr = new char[iStrLen + 1];

    int i;
    for(i=0; i<iStrLen; ++i)
    {
        pStr[i] = toupper(pszStr[i]);
    }
    pStr[iStrLen] = 0;

    return pStr;
}

char* Utl::LowerCase(const char * pszStr)
{
    static char* pStr = 0;

    if(pStr != 0)
    {
        delete[] pStr;
    }

    int iStrLen = strlen(pszStr);

    pStr = new char[iStrLen + 1];

    int i;
    for(i=0; i<iStrLen; ++i)
    {
        pStr[i] = tolower(pszStr[i]);
    }
    pStr[iStrLen] = 0;

    return pStr;
}

int SplitStr(string& str, string sDelimiter, vector<string>& vResult)
{
	if( str.empty() )
	{
		vResult.clear();
		return 0;
	}
	unsigned int iIndex=0, iOldIndex;
	while (iIndex != string::npos)
	{
		iOldIndex = iIndex;
		iIndex = str.find(sDelimiter, iOldIndex+1);
		if (iOldIndex == 0)
		{
			vResult.push_back(str.substr(0, iIndex));
			continue;
		}
		
		iOldIndex += sDelimiter.length();
		if (iIndex == string::npos)
		{
			vResult.push_back(str.substr(iOldIndex));
			continue;
		}
		vResult.push_back(str.substr(iOldIndex, iIndex-iOldIndex));
	}

	return (int)vResult.size();
}

//从"2006-11-20 17:50:06" 格式的时间，转换为time_t
time_t Utl::time_trans(string sStr)
{
	struct tm t;
	bzero(&t, sizeof(tm));
	
	
	string sYear;
	string sMon = "1";
	string sDay = "1";
	string sHour;
	string sMin;
	string sSec;
	
	string sStr_1;
	string sStr_2;
	
	int iBlank = sStr.find(" ");
	if(iBlank == string::npos)
	{
		sStr_1 = sStr;
	}
	else
	{
		sStr_1 = sStr.substr(0, iBlank);
		sStr_2 = sStr.substr(iBlank+1);
	}

	vector<string> vec;
	SplitStr(sStr_1, "-", vec);
	
	if(vec.size() == 3)
	{
		sYear = vec[0];
		sMon = vec[1];
		sDay = vec[2];
	}
	else if(vec.size() == 2)
	{
		sYear = vec[0];
		sMon = vec[1];
	}
	else if(vec.size() == 1)
	{
		sYear = vec[0];
	}
	
	vec.clear();
	SplitStr(sStr_2, ":", vec);
	if(vec.size() == 3)
	{
		sHour = vec[0];
		sMin = vec[1];
		sSec = vec[2];
	}
	else if(vec.size() == 2)
	{
		sHour = vec[0];
		sMin = vec[1];
	}
	else if(vec.size() == 1)
	{
		sHour = vec[0];
	}

	t.tm_year = atoi(sYear.c_str()) - 1900;
	t.tm_mon = atoi(sMon.c_str()) - 1;
	t.tm_mday = atoi(sDay.c_str());
	t.tm_hour = atoi(sHour.c_str());
	t.tm_min = atoi(sMin.c_str());
	t.tm_sec = atoi(sSec.c_str());

	return mktime(&t);
}

string Utl::fmt_time(time_t t)
{
    char szTime[32];
    struct tm stCurrTime;
    
	localtime_r(&t, &stCurrTime);
	sprintf(szTime, "%d%02d%02d %02d:%02d:%02d",
		stCurrTime.tm_year+1900, stCurrTime.tm_mon+1, stCurrTime.tm_mday,
		stCurrTime.tm_hour, stCurrTime.tm_min, stCurrTime.tm_sec);

    return string(szTime);
}

string Utl::dump_cid(const char * cid)
{
	char buf[100] = {0};
	int i=0;
	unsigned nstr=0;
	for(i = 0 ;i < 20 && nstr < 100; i++)
	{
		sprintf(buf+nstr,"%02X",(unsigned char)cid[i]);
		nstr+=2;
	}	
	

	buf[nstr] = '\0';
	return string(buf);
}

/*
   UCS-4 range (hex.)    UTF-8 octet sequence (binary)
   0000 0000-0000 007F   0xxxxxxx
   0000 0080-0000 07FF   110xxxxx 10xxxxxx
   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

*/
//取UTF8编码字符串的子串。start_pos为起始字符位置(从0开始)，len为希望获取的子串的字符数
std::string Utl::utf8_substr(std::string source_str, int start_pos, int len)
{
    unsigned char* p = (unsigned char*)source_str.c_str();
    int start_delta = 0;
    int find_delta = 0;

    //先跳过start_pos个字符
    for(int i=0; i<start_pos; ++i)
    {
        //超长
        if(start_delta >= source_str.length())
        {
            return "";
        }

        if(*(p+start_delta) <= 0x7f)
        {
            start_delta += 1;
        }
        else if(*(p+start_delta) <= 0xdf)
        {
            start_delta += 2;
        }
        else if(*(p+start_delta) <= 0xef)
        {
            start_delta += 3;
        }
        else if(*(p+start_delta) <= 0xf7)
        {
            start_delta += 4;
        }
        else if(*(p+start_delta) <= 0xfb)
        {
            start_delta += 5;
        }
        else
        {
            return "";
        }
    }
    if(start_delta >= source_str.length())
    {
        return "";
    }
    

    //找len字符
    find_delta = start_delta;
    for(int i=0; i<len; ++i)
    {
        //超长
        if(find_delta >= source_str.length())
        {
            break;
        }

        if(*(p+find_delta) <= 0x7f)
        {
            find_delta += 1;
        }
        else if(*(p+find_delta) <= 0xdf)
        {
            find_delta += 2;
        }
        else if(*(p+find_delta) <= 0xef)
        {
            find_delta += 3;
        }
        else if(*(p+find_delta) <= 0xf7)
        {
            find_delta += 4;
        }
        else if(*(p+find_delta) <= 0xfb)
        {
            find_delta += 5;
        }
        else
        {
            return "";
        }
    }
    

    return source_str.substr(start_delta, find_delta - start_delta);
    
}


unsigned long long Utl::ntohll(unsigned long long val)
{

    if (__BYTE_ORDER == __LITTLE_ENDIAN)
    {
        return (((unsigned long long )htonl((int)((val << 32) >> 32))) << 32) | (unsigned int)htonl((int)(val >> 32));

    }else if (__BYTE_ORDER == __BIG_ENDIAN)
    {
        return val;
    }
}

unsigned long long Utl::htonll(unsigned long long val)
{
    return ntohll(val);
}

unsigned long Utl::auto_random()
{
    static time_t t = time(0);
    srandom(++t);
    
    return random();
}

#define PI 3.1415926
double AverageRandom(double min,double max)
{
    long int minInteger = (long int)(min*10000);
    long int maxInteger = (long int)(max*10000);
    long int randInteger = Utl::auto_random();
    long int diffInteger = maxInteger - minInteger;
    long int resultInteger = randInteger % diffInteger + minInteger;
    return ((double)resultInteger)/10000.0;
}
double Normal(double x,double miu,double sigma) //概率密度函数
{
    return 1.0/sqrt(2*PI*sigma) * exp(-1*(x-miu)*(x-miu)/(2*sigma*sigma));
}
double Utl::NormalRandom(double miu,double sigma,double min,double max)//产生正态分布随机数
{
    double x;
    double dScope;
    double y;
    do
    {
        x = AverageRandom(min,max);
        y = Normal(x, miu, sigma);
        dScope = AverageRandom(0, Normal(miu,miu,sigma));
    }while( dScope > y);
    return x;
}



time_t Utl::time_today_end()
{
    time_t t = time(0);

    tm* p_tm = localtime(&t);
    int hour = p_tm->tm_hour;
    int minutes = p_tm->tm_min;
    int sec = p_tm->tm_sec;

    t += (23 - hour) * 3600;
    t += (59 - minutes) * 60;
    t += 60 - sec;

    return t;
}


