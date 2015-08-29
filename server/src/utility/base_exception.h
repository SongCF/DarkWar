/********************************************************
创建者:吴晓勇
创建时间:2006-06-05
描述:基础异常类

修订记录:
    修订者:
    修订时间:
    修订说明:
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

