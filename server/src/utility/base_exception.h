/********************************************************
������:������
����ʱ��:2006-06-05
����:�����쳣��

�޶���¼:
    �޶���:
    �޶�ʱ��:
    �޶�˵��:
*********************************************************/
#ifndef _BASE_EXCEPTION_H_
#define _BASE_EXCEPTION_H_

class CBaseException
{
public:
    CBaseException(int iExceptionCode, char* pszMsg = "")
        :m_iExceptionCode(iExceptionCode), m_msg(pszMsg)
    {
    };

public:
    int m_iExceptionCode;
    string m_msg;
};

#endif //_BASE_EXCEPTION_H_

