/********************************************************
创建者:吴晓勇
创建时间:2006-06-05
描述:

修订记录:
    修订者:
    修订时间:
    修订说明:
*********************************************************/
#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stlref.h"

namespace Utl
{
    //去掉pszStr字符串中最左边的所有空格
    //char* LTrim(const char* pszStr);
    void LTrim(string& str);

    //全掉字符串前后的所有空格
    void Trim(string& str);

    //异常情况返回0
    int AtoI(const char* pszStr);

    //如果指针为0，则返回""
    char* Null2Empty(char* ptr);

    //左补空格将str填充至iWantedLen长度。
    //如果str本身超过或等于iWantedLen，则维持str不变
    char* FillStr_LeftBlank(const char* str, int iWantedLen);

    //
    char* Int64ToStr(long long i64);

    //替换http编码中的%xx
    char* r_http_code(const char* org_str);

    void r_http_code_threadsafe(const char* str, string& ret_str);

    //计算mmap同步需要的地址和大小
    void MmapSyncPointAndSize(void* pPIn, int iSizeIn, void*& pPOut, int& iSizeOut);

    char* UperCase(const char * pszStr);
    char* LowerCase(const char * pszStr);

    //从"2006-11-20 17:50:06" 格式的时间，转换为time_t
    time_t time_trans(string sStr);
    string fmt_time(time_t t);

    string dump_cid(const char* cid);

    //取UTF8编码字符串的子串。start_pos为起始字符位置(从0开始)，len为希望获取的子串的字符数
    std::string utf8_substr(std::string source_str, int start_pos, int len);


    unsigned long long ntohll(unsigned long long val);
    unsigned long long htonll(unsigned long long val);

    unsigned long  auto_random();
    //产生正态分布随机数,miu为数学期望，sigma为方差
    double NormalRandom(double miu,double sigma,double min,double max);

    //获取今日24:00的time_t
    time_t time_today_end();
};

#endif //_UTILS_H_


