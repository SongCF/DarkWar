/********************************************************
创建者:吴晓勇
创建时间:2006-04-30
描述:对操作系统提供的api进行薄封装。以方便使用

修订记录:
    修订者:
    修订时间:
    修订说明:
*********************************************************/
#ifndef _OS_H_
#define _OS_H_

#include <pthread.h>
#include <sys/types.h>

namespace OS
{
    //如果共享内存原本不存在，则创建之。创建成功，返回 0；
    //如果共享内存已经存在，直接返回-2
    //其他所有错误返回-1，错误信息可以从errno获得
    int CreateShm(key_t ipckey, size_t size, int& ret_iShmID, void*& ret_pShm);

    //销毁共享内存
    int DestoryShm(int iShmID);

    //attach到已经创建的共享内存。
    //成功，返回 0
    //共享内存不存在，返回-2
    //其他所有错误，返回 -1，错误信息可以从errno获得
    int AttachShm(key_t ipckey, size_t& ret_size, int& ret_iShmID, void*& ret_pShm);

    //disattach共享内存
    int DisattachShm(void* pShm);

    //初始化mutex锁为进程间共享
    int InitMutexShared(pthread_mutex_t* pMutex);
    //初始化mutex锁
    int InitMutex(pthread_mutex_t * pMutex);

    //初始化信号捕获机制
    typedef void sigfunc(int);
    int InitSignal(sigfunc* ExitHdlr);

    //将当前进程精灵化
    void Daemonize();

    //socket操作---------------------------------------------------------------
    
    //创建一个TCP Socket
    //成功返回不小于0的sockfd
    //失败返回-1
    int CreateTCPSock();

    //给socket绑定ip和端口
    //如果pszIP为0，则创建INADDR_ANY
    //成功返回0，失败返回-1
    int Bind(int iSockFd, unsigned int shPort, const char* pszIP = 0);

    //成功返回0，失败返回-1
    int Listen(int iSockFd, int iBackLog = 30);


    //out_pClientPort和out_pszClientIPBuf返回链接上的客户端信息。
    //如果不关心客户端信息，可以将其设置为0
    //成功返回不小于0的ConnSockFd，错误返回-1
    int Accept(int iListenFd);

    //成功返回不小于0的ConnSockFd，错误返回-1
    //返回-2表示超时
    //iTimeOut_ms单位:毫秒
    //iTimeOut_ms为-1表示永远等待
    //iTimeOut_ms为0表示非阻塞
    int AcceptTOut(int iListenFd, int iTimeOut_ms);

    //获取连接对方的IP和端口
    int GetPeerInfor(int iSockFd, char*& pszPeerIP, int& iPeerPort);

    //成功返回0，失败返回-1
    int Connect(int iSockFd, char* pszServIP, unsigned int shServPort);

    //返回实际发送成功的字节
    //返回-1表示错误
    int Send(int iSockFd, char* pSendBuf, int iSendBytes);

    //返回实际接收到的字节数
    //返回0表示对方已经关闭链接
    //返回-1表示其他错误
    //原则:一直等待iSockFd有数据可读。
    //有数据可读后，只读取一次。
    //如果当前可读字节达到iRecvBytes，则读取iRecvBytes
    //如果当前可读字节不够iRecvBytes，则读取能读取的所有字节，然后返回读取的字节数
    int Recv(int iSockFd, char* pRecvBuf, int iRecvBytes);

    //返回实际接收到的字节数
    //返回0表示对方已经关闭链接
    //返回-2表示超时
    //返回-1表示其他错误
    //iTimeOut_ms单位:毫秒
    //iTimeOut_ms为-1表示永远等待
    //iTimeOut_ms为0表示非阻塞
    //原则:一直等待iSockFd有数据可读，或超时
    //有数据可读后，只读取一次。
    //如果当前可读字节达到iRecvBytes，则读取iRecvBytes
    //如果当前可读字节不够iRecvBytes，则读取能读取的所有字节，然后返回读取的字节数
    int RecvTOut(int iSockFd, char* pRecvBuf, int iRecvBytes
        , int iTimeOut_ms = -1);

    //返回实际接收到的字节数
    //返回0表示对方已经关闭链接
    //返回-2表示超时
    //返回-1表示其他错误
    //iTimeOut_ms单位:毫秒
    //iTimeOut_ms为-1表示永远等待
    //iTimeOut_ms为0表示非阻塞
    //原则:一直等待至读取到iRecvBytes个字节，或超时，或网络故障
    int RecvBytesTOut(int iSockFd, char* pRecvBuf, int iRecvBytes
        , int iTimeOut_ms = -1);

    //关闭socket
    int CloseSock(int iSockFd);

    //支持创建多级目录，返回0表示成功
    int CreatePath(const char *path);
    void RemovePath(const char* path);

    unsigned long GetFileSize(char* filepath);
};

#endif //_OS_H_
