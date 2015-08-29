/********************************************************
创建者:吴晓勇
创建时间:2006-06-05
描述:自动销毁堆中的内存

修订记录:
    修订者:
    修订时间:
    修订说明:
*********************************************************/
#ifndef _MEMGUARD_H_
#define _MEMGUARD_H_

class CMemGuard
{
public:
    CMemGuard(char* p)
        :m_p(p)
    {
    };
    
    ~CMemGuard()
    {
        if(m_p != 0)
        {
            delete m_p;
        }
    };
private:
    char* m_p;
};

#endif //_MEMGUARD_H_

