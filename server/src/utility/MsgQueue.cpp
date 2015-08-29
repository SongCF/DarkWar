
#include <sys/msg.h>
#include <string>
#include <errno.h>
#include "MsgQueue.h"

int CMsgQueue::Create(key_t IpcKey, bool EXCL)
{
    int iFlag = IPC_CREAT|0600;
    if(EXCL)
    {
        iFlag |= IPC_EXCL;
    }
    
    m_MsgQId = msgget(IpcKey, iFlag);
    if(m_MsgQId < 0)
    {
        if(errno == EEXIST)
        {
            return -2;
        }
        
        return -1;
    }

    return 0;
}

void CMsgQueue::Destroy()
{
    msgctl(m_MsgQId, IPC_RMID, 0);
}

int CMsgQueue::Push(char * pMsg, int iMsgSize, bool bBlock)
{
    msgbuf* pBuf = (msgbuf*)new char[iMsgSize+sizeof(long)];
    pBuf->mtype = 1;
    memcpy(pBuf->mtext, pMsg, iMsgSize);

    int iFlag = 0;
    if(!bBlock)
    {
        iFlag = IPC_NOWAIT;
    }
    
    int ret = msgsnd(m_MsgQId, pBuf, iMsgSize, iFlag);
    delete[] pBuf;
    
    if(ret < 0)
    {
        if(!bBlock)
        {
            if(errno == EAGAIN)
            {
                //队列满
                return -2;
            }
        }
        return -1;
    }

    return 0;
}

int CMsgQueue::Pop(char * pMsgBuf, int iBufSize, bool bBlock)
{
    msgbuf* pBuf = (msgbuf*)new char[iBufSize+sizeof(long)];

    int iFlag = 0;
    if(!bBlock)
    {
        iFlag = IPC_NOWAIT;
    }

    int ret;
    while(1)
    {
        ret = msgrcv(m_MsgQId, pBuf, iBufSize+sizeof(long), 0, iFlag);
        if(ret < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            
            delete[] pBuf;

            if(errno == ENOMSG)
            {
                //队列满
                return -2;
            }
        
            return -1;
        }

        memcpy(pMsgBuf, pBuf->mtext, ret);
        delete[] pBuf;
        break;
    }

    return ret;
}

int CMsgQueue::GetMsgNum()
{
    msqid_ds ds;
    msgctl(m_MsgQId, IPC_STAT, &ds);

    return ds.msg_qnum;
}


