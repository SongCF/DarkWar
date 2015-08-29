/********************************************************
������:������
����ʱ��:2006-06-05
����:

�޶���¼:
    �޶���:
    �޶�ʱ��:
    �޶�˵��:
*********************************************************/
#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stlref.h"

namespace Utl
{
    //ȥ��pszStr�ַ���������ߵ����пո�
    //char* LTrim(const char* pszStr);
    void LTrim(string& str);

    //ȫ���ַ���ǰ������пո�
    void Trim(string& str);

    //�쳣�������0
    int AtoI(const char* pszStr);

    //���ָ��Ϊ0���򷵻�""
    char* Null2Empty(char* ptr);

    //�󲹿ո�str�����iWantedLen���ȡ�
    //���str�����������iWantedLen����ά��str����
    char* FillStr_LeftBlank(const char* str, int iWantedLen);

    //
    char* Int64ToStr(long long i64);

    //�滻http�����е�%xx
    char* r_http_code(const char* org_str);

    void r_http_code_threadsafe(const char* str, string& ret_str);

    //����mmapͬ����Ҫ�ĵ�ַ�ʹ�С
    void MmapSyncPointAndSize(void* pPIn, int iSizeIn, void*& pPOut, int& iSizeOut);

    char* UperCase(const char * pszStr);
    char* LowerCase(const char * pszStr);

    //��"2006-11-20 17:50:06" ��ʽ��ʱ�䣬ת��Ϊtime_t
    time_t time_trans(string sStr);
    string fmt_time(time_t t);

    string dump_cid(const char* cid);

    //ȡUTF8�����ַ������Ӵ���start_posΪ��ʼ�ַ�λ��(��0��ʼ)��lenΪϣ����ȡ���Ӵ����ַ���
    std::string utf8_substr(std::string source_str, int start_pos, int len);


    unsigned long long ntohll(unsigned long long val);
    unsigned long long htonll(unsigned long long val);

    unsigned long  auto_random();
    //������̬�ֲ������,miuΪ��ѧ������sigmaΪ����
    double NormalRandom(double miu,double sigma,double min,double max);

    //��ȡ����24:00��time_t
    time_t time_today_end();
};

#endif //_UTILS_H_


