
#ifndef _MSGQUEUE_H_
#define _MSGQUEUE_H_

#include <sys/types.h>
#include <string.h>

class CMsgQueue
{
public:
    //成功返回0
    //失败返回-1
    //如果EXCL为true，并且消息队列已经存在则返回 -2
    int Create(key_t IpcKey, bool EXCL = false);

    //Destroy后，所有队列中的数据全部被丢弃
    void Destroy();

    //成功返回0
    //异常返回-1
    //如果为非阻塞(bBlock==false)，返回-2表示队列已满
    int Push(char* pMsg, int iMsgSize, bool bBlock = true);

    //成功返回消息长度，并将队列首元素拷贝入Buf，并将首元素删除
    //异常返回-1
    //如果为非阻塞(bBlock==false)，返回-2表示队列为空
    int Pop(char* pMsgBuf, int iBufSize, bool bBlock = true);
    
    //获取当前队列中消息个数
    int GetMsgNum();

public:
    int m_MsgQId;
};

#endif //_MSGQUEUE_H_

