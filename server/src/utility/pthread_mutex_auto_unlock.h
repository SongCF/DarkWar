/********************************************************
创建者:吴晓勇
创建时间:2006-11-09

修订记录:
    修订者:
    修订时间:
    修订说明:
*********************************************************/
#ifndef _PTHREAD_MUTEX_AUTO_UNLOCK_H_
#define _PTHREAD_MUTEX_AUTO_UNLOCK_H_
#include <pthread.h>
#include "logs.h"

class CPthreadMutexAutoUnlock
{
public:
    CPthreadMutexAutoUnlock(pthread_mutex_t* pMutex)
    {
        m_pMutex = pMutex;
    };
    ~CPthreadMutexAutoUnlock()
    {
        if(pthread_mutex_unlock(m_pMutex) != 0)
        {
            //
            LogMsg("%s, line %d, pthread_mutex_unlock failed.%s"
                , __FILE__, __LINE__, strerror(errno));
        }
    };

    pthread_mutex_t* m_pMutex;
};

class CPthreadMutexAutoLockAutoUnlock
{
public:
    CPthreadMutexAutoLockAutoUnlock(pthread_mutex_t* pMutex)
    {
        m_pMutex = pMutex;

        if(pthread_mutex_lock(m_pMutex) != 0)
        {
            //
            LogMsg("%s, line %d, pthread_mutex_lock failed.%s"
                , __FILE__, __LINE__, strerror(errno));
        }
    };
    ~CPthreadMutexAutoLockAutoUnlock()
    {
        if(pthread_mutex_unlock(m_pMutex) != 0)
        {
            //
            LogMsg("%s, line %d, pthread_mutex_unlock failed.%s"
                , __FILE__, __LINE__, strerror(errno));
        }
    };

    pthread_mutex_t* m_pMutex;
};


#endif //_PTHREAD_MUTEX_AUTO_UNLOCK_H_


