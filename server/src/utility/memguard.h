/********************************************************
������:������
����ʱ��:2006-06-05
����:�Զ����ٶ��е��ڴ�

�޶���¼:
    �޶���:
    �޶�ʱ��:
    �޶�˵��:
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

