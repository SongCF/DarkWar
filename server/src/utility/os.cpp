/********************************************************
创建者:吴晓勇
创建时间:2006-04-30
描述:对操作系统提供的api进行薄封装。以方便使用

修订记录:
    修订者:
    修订时间:
    修订说明:
*********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "os.h"


int OS::CreateShm(key_t IpcKey, size_t size, int & ret_iShmId, void*& ret_pShm)
{   
    //首先检查共享内存是否已经存在
    int iShmId = shmget(IpcKey, 0, 0600);
    if(iShmId >= 0)
    {
        //共享内存已经存在
        
        return -2;
    }
    else
    {
        if(errno == ENOENT)
        {
            //共享内存不存在，创建之
            
            iShmId = shmget(IpcKey, size, 0600|IPC_CREAT|IPC_EXCL);
            if(iShmId < 0)
            {
                return -1;
            }

            void* pShm = shmat(iShmId, 0, 0);
            if(pShm == (void*)-1)
            {
                shmctl(iShmId, IPC_RMID, 0);

                return -1;
            }

            //成功
            ret_iShmId = iShmId;
            ret_pShm = pShm;

            return 0;
        }
        else
        {
            //其他错误
            
            return -1;
        }
    }
}

int OS::AttachShm(key_t IpcKey
    , size_t & ret_size, int & ret_iShmID, void *& ret_pShm)
{
    int iShmid = shmget(IpcKey, 0, 0600);
    if(iShmid < 0)
    {
        if(errno == ENOENT)
        {
            //共享内存不存在
            return -2;
        }
        
        return -1;
    }
    void* pShm = shmat(iShmid, 0, 0);
    if(pShm == (void*)-1)
    {
        return -1;
    }

    //获取共享内存的大小
    shmid_ds shmds_buf;
    if(shmctl(iShmid, IPC_STAT, &shmds_buf) < 0)
    {
        shmdt(pShm);
        shmctl(iShmid, IPC_RMID, 0);

        return -1;
    }
    
    ret_size = shmds_buf.shm_segsz;
    ret_iShmID = iShmid;
    ret_pShm = pShm;

    return 0;
}

int OS::DisattachShm(void* pShm)
{
    int ret = shmdt(pShm);
    return ret;
}

int OS::DestoryShm(int iShmID)
{
    if(shmctl(iShmID, IPC_RMID, 0) < 0)
    {
        return -1;
    }

    return 0;
}


int OS::InitMutexShared(pthread_mutex_t * pMutex)
{
    assert(pMutex != 0);
    
    pthread_mutexattr_t mattr;
    if(pthread_mutexattr_init(&mattr) != 0)
    {
        return -1;
    }
//#ifdef _POSIX_THREAD_PROCESS_SHARED
    if(pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED) != 0)
    {
        return -1;
    }
//#else
//#error not support _POSIX_THREAD_PROCESS_SHARED
//#endif
    if(pthread_mutex_init(pMutex, &mattr) != 0)
    {
        return -1;
    }

    return 0;
}

int OS::InitMutex(pthread_mutex_t * pMutex)
{
    assert(pMutex != 0);
    
    pthread_mutexattr_t mattr;
    if(pthread_mutexattr_init(&mattr) != 0)
    {
        return -1;
    }

    if(pthread_mutex_init(pMutex, &mattr) != 0)
    {
        return -1;
    }

    return 0;
}


void DefaultRecordHdlr(int iSig)
{
    fprintf(stderr, "signal %d occured\n", iSig);
}

int OS::InitSignal(sigfunc* ExitHdlr)
{
//信号处理方法定义
//维持系统默认
#define SYSDFL 0
//忽略
#define IGNORE 1
//程序退出
#define EXIT 2
//记录发生了此信号。之外不作任何操作
#define RECORD 3

struct SSigOp
{
    int iSigNo;
    int iSigOp;
};

//预定义Linux下所有信号的处理方式
SSigOp SigOpArr[] = 
{
    {SIGHUP    , EXIT}   /* Hangup (POSIX).  */
    ,{SIGINT   , EXIT}   /* Interrupt (ANSI).  */                    
    ,{SIGQUIT  , EXIT}   /* Quit (POSIX).  */                        
    ,{SIGILL   , EXIT}   /* Illegal instruction (ANSI).  */          
    ,{SIGTRAP  , EXIT}   /* Trace trap (POSIX).  */                  
    ,{SIGABRT  , EXIT}   /* Abort (ANSI).  */                        
    ,{SIGIOT   , EXIT}   /* IOT trap (4.2 BSD).  */                  
    ,{SIGBUS   , EXIT}   /* BUS error (4.2 BSD).  */                 
    ,{SIGFPE   , EXIT}   /* Floating-point exception (ANSI).  */     
    ,{SIGKILL  , SYSDFL} /* Kill, unblockable (POSIX).  */           
    ,{SIGUSR1  , IGNORE} /* User-defined signal 1 (POSIX).  */       
    ,{SIGSEGV  , SYSDFL}   /* Segmentation violation (ANSI).  */       
    ,{SIGUSR2  , IGNORE} /* User-defined signal 2 (POSIX).  */       
    ,{SIGPIPE  , IGNORE} /* Broken pipe (POSIX).  */                 
    ,{SIGALRM  , IGNORE} /* Alarm clock (POSIX).  */                 
    ,{SIGTERM  , EXIT}   /* Termination (ANSI).  */                  
    ,{SIGSTKFLT, EXIT}   /* Stack fault.  */                         
    ,{SIGCLD   , IGNORE} /* Same as SIGCHLD (System V).  */          
    ,{SIGCHLD  , IGNORE} /* Child status has changed (POSIX).  */    
    ,{SIGCONT  , SYSDFL} /* Continue (POSIX).  */                    
    ,{SIGSTOP  , SYSDFL} /* Stop, unblockable (POSIX).  */           
    ,{SIGTSTP  , EXIT}   /* Keyboard stop (POSIX).  */               
    ,{SIGTTIN  , IGNORE} /* Background read from tty (POSIX).  */    
    ,{SIGTTOU  , IGNORE} /* Background write to tty (POSIX).  */     
    ,{SIGURG   , IGNORE} /* Urgent condition on socket (4.2 BSD).  */
    ,{SIGXCPU  , IGNORE} /* CPU limit exceeded (4.2 BSD).  */        
    ,{SIGXFSZ  , IGNORE} /* File size limit exceeded (4.2 BSD).  */  
    ,{SIGVTALRM, IGNORE} /* Virtual alarm clock (4.2 BSD).  */       
    ,{SIGPROF  , IGNORE} /* Profiling alarm clock (4.2 BSD).  */     
    ,{SIGWINCH , IGNORE} /* Window size change (4.3 BSD, Sun).  */   
    ,{SIGPOLL  , IGNORE} /* Pollable event occurred (System V).  */  
    ,{SIGIO    , IGNORE} /* I/O now possible (4.2 BSD).  */          
    ,{SIGPWR   , IGNORE} /* Power failure restart (System V).  */    
    ,{SIGSYS   , EXIT}   /* Bad system call.  */                     
    ,{SIGUNUSED, IGNORE} 
    ,{0, 0}
};

//
    int i;
    for(i = 0; SigOpArr[i].iSigNo != 0; ++i)
    {
        struct sigaction SigAct;
        
        switch(SigOpArr[i].iSigOp)
        {
            case SYSDFL:
                break;
            case IGNORE:
                SigAct.sa_handler = SIG_IGN;
                sigemptyset(&(SigAct.sa_mask));
                SigAct.sa_flags = 0;
                if(sigaction(SigOpArr[i].iSigNo, &SigAct, 0) < 0)
                {
                    return -1;
                }
                break;
            case EXIT:
                SigAct.sa_handler = ExitHdlr;
                sigemptyset(&(SigAct.sa_mask));
                SigAct.sa_flags = 0;
                if(sigaction(SigOpArr[i].iSigNo, &SigAct, 0) < 0)
                {
                    return -1;
                }
                break;
            case RECORD:
                SigAct.sa_handler = DefaultRecordHdlr;
                sigemptyset(&(SigAct.sa_mask));
                SigAct.sa_flags = 0;
                if(sigaction(SigOpArr[i].iSigNo, &SigAct, 0) < 0)
                {
                    return -1;
                }
                break;
            default :
                fprintf(stderr, "信号处理方式设置错误\n");
                break;
        }
    }

    return 0;

}

void OS::Daemonize()
{
    pid_t iPID;
    
    if((iPID = fork()) < 0) /* -1是创建失败的情况 */
    {
    	fprintf(stderr, "fork1 failed!\n");
        return ;
    }
    else if(iPID > 0)       /* 大于0是将子进程的PID传回给父进程 */
    {
        exit(0);
    }

    /* 创建新的session和process group，成为其leader，并脱离控制终端 */
    setsid();

    /*
    * 工作目录更改为根目录。这是为了保证我们的进程不使用任何目录。否则我们的守护进程将一直
    * 占用某个目录，这可能会造成超级用户不能卸载一个文件系统。 
    */
    chdir("/");
    
    /*
     * 将文件方式创建屏蔽字设置为"0"。这是因为由继承得来的文件创建方式屏蔽字可能会禁止某些许可权。
     * 例如我们的守护进程需要创建一组可读可写的文件，而此守护进程从父进程那里继承来的文件创建方式
     * 屏蔽字却有可能屏蔽掉了这两种许可权，则新创建的一组文件其读或写操作就不能生效。因此要将文件
     * 方式创建屏蔽字设置为"0"。
     */ 
    umask(0);
    

    if((iPID = fork()) < 0) 
    {
    	fprintf(stderr, "fork1 failed!\n");
        return;
    }
    else if(iPID > 0) 
    {
        exit(0);
    }

    return;
}

int OS::CreateTCPSock()
{
    int iSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if(iSockFd < 0)
    {
        fprintf(stderr, "CreateTCPSock failed.%s\n", strerror(errno));
        return -1;
    }

    //设置地址重用
    int iOptVal = 1;
    socklen_t iOptLen = 4;
    if(setsockopt(iSockFd, SOL_SOCKET, SO_REUSEADDR, &iOptVal, iOptLen) < 0)
    {
        fprintf(stderr, "setsockopt failed.%s\n", strerror(errno));
        close(iSockFd);
        
        return -1;
    }

    return iSockFd;
}

int OS::Bind(int iSockFd, unsigned int shPort, const char * pszIP)
{
    sockaddr_in addr;

    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(shPort);
    //设置IP地址
    if((pszIP == 0) || (*pszIP == '*'))
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        int iNetIP = inet_addr(pszIP);
        if(iNetIP == INADDR_NONE)
        {
            fprintf(stderr, "inet_addr failed!ipaddr=[%s]!%s\n"
                , pszIP, strerror(errno));

            return -1;
        }

        addr.sin_addr.s_addr = iNetIP;
    }

    if(bind(iSockFd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "bind failed!%s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int OS::Listen(int iSockFd, int iBackLog)
{
    return listen(iSockFd, iBackLog);
}

int OS::Accept(int iListenFd)
{
    //sockaddr_in ClientAddr;
    //socklen_t ClientAddrLen;

    int iConnFd;
    while(1)
    {

        //iConnFd = accept(iListenFd, (sockaddr*)&ClientAddr, &ClientAddrLen);
        iConnFd = accept(iListenFd, 0, 0);
        if(iConnFd < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }

            fprintf(stderr, "accept failed!errno = %d, %s\n", errno, strerror(errno));
            return -1;
        }
        break;
    }

    return iConnFd;
}

int OS::AcceptTOut(int iListenFd, int iTimeOut_ms)
{
    if(iTimeOut_ms == -1)
    {
        return Accept(iListenFd);
    }
    
    int iTimePass_ms = -10;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(iListenFd, &fdset);

    struct timeval timev;
    timev.tv_sec = 0;
    timev.tv_usec = 0;

    ////等待socket可读
    while(1)
    {
        int ret = select(iListenFd + 1, &fdset, 0, 0, &timev);
        if(ret < 0)
        {
            if(errno == EINTR)
            {
                FD_ZERO(&fdset);
                FD_SET(iListenFd, &fdset);
                timev.tv_sec = 0;
                timev.tv_usec = 100000;
                continue;
            }

            return -1;
        }
        if(ret == 0)
        {
            //当前无数据可读
            
            iTimePass_ms += 10;
            if(iTimePass_ms < iTimeOut_ms)
            {
                FD_ZERO(&fdset);
                FD_SET(iListenFd, &fdset);
                timev.tv_sec = 0;
                timev.tv_usec = 100000;
    
                continue;
            }

            //超时
            return -2;
        }

        //有数据可读了
        break;
    }

    int iConnFd;
    iConnFd = accept(iListenFd, 0, 0);

    return iConnFd;
}

int OS::GetPeerInfor(int iSockFd, char * & pszPeerIP, int & iPeerPort)
{
    sockaddr_in addr;
    socklen_t len;

    len = sizeof(addr);
    if(getpeername(iSockFd, (sockaddr*)&addr, &len) < 0)
    {
        fprintf(stderr, "getpeername失败");
        return -1;
    }

    static char pszIP[16];
    strcpy(pszIP, inet_ntoa(addr.sin_addr));
    pszPeerIP = pszIP;
    
    iPeerPort = ntohs(addr.sin_port);

    return 0;
}

int OS::Connect(int iSockFd, char * pszServIP, unsigned int shServPort)
{
    char IP[64];
    if(strcmp(pszServIP, "*") == 0)
    {
        strcpy(IP, "127.0.0.1");
    }
    else
    {
        strcpy(IP, pszServIP);
    }
    sockaddr_in ServAddr;

    memset(&ServAddr, 0, sizeof(sockaddr_in));
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_port = htons(shServPort);
    int iNetIP = inet_addr(IP);
    if(iNetIP == INADDR_NONE)
    {
        fprintf(stderr, "inet_addr failed!ServIP=[%s]!%s\n"
            , IP, strerror(errno));

        return -1;
    }

    ServAddr.sin_addr.s_addr = iNetIP;

    return connect(iSockFd, (sockaddr*)&ServAddr, sizeof(sockaddr_in));
}

int OS::Send(int iSockFd, char * pszSendBuf, int iSendBytes)
{
    int iBytes=0, this_write;

	while(iBytes < iSendBytes)
	{
		do
		{
			this_write = send(iSockFd, pszSendBuf, iSendBytes - iBytes, 0);
		}
		while((this_write<0)&&(errno==EINTR));

		if(this_write<=0)   //如果出错
			return this_write;
		
		iBytes += this_write;
		pszSendBuf += this_write;
	}

	return iBytes;
}

int OS::Recv(int iSockFd, char* pszRecvBuf, int iRecvBytes)
{
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(iSockFd, &fdset);

    ////等待socket可读
    while(1)
    {
        int ret = select(iSockFd + 1, &fdset, 0, 0, 0);
        if(ret < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }

            return -1;
        }

        //有数据可读了
        break;
    }

    //能读多少算多少
    int iBytes = recv(iSockFd, pszRecvBuf, iRecvBytes, 0);
    
    return iBytes;
}

int OS::RecvTOut(int iSockFd, char * pszRecvBuf, int iRecvBytes, int iTimeOut_ms)
{
    if(iTimeOut_ms == -1)
    {
        return Recv(iSockFd, pszRecvBuf, iRecvBytes);
    }
    
    int iTimePass_ms = -10;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(iSockFd, &fdset);

    struct timeval timev;
    timev.tv_sec = 0;
    timev.tv_usec = 0;

    ////等待socket可读
    while(1)
    {
        int ret = select(iSockFd + 1, &fdset, 0, 0, &timev);
        if(ret < 0)
        {
            if(errno == EINTR)
            {
                FD_ZERO(&fdset);
                FD_SET(iSockFd, &fdset);
                timev.tv_sec = 0;
                timev.tv_usec = 10000;
                continue;
            }

            return -1;
        }
        if(ret == 0)
        {
            //当前无数据可读
            
            iTimePass_ms += 10;
            if(iTimePass_ms < iTimeOut_ms)
            {
                FD_ZERO(&fdset);
                FD_SET(iSockFd, &fdset);
                timev.tv_sec = 0;
                timev.tv_usec = 10000;
    
                continue;
            }

            //超时
            return -2;
        }

        //有数据可读了
        break;
    }

    //能读多少算多少
    int iBytes = recv(iSockFd, pszRecvBuf, iRecvBytes, 0);
    
    return iBytes;
/*
    int iTimePass_ms = 0;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(iSockFd, &fdset);

    struct timeval timev;
    timev.tv_sec = 0;
    timev.tv_usec = 0;

    ////等待socket可读
    while(1)
    {
        int ret = select(iSockFd + 1, &fdset, 0, 0, &timev);
        if(ret < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }

            return -1;
        }
        if(ret == 0)
        {
            //当前无数据可读
            
            if(iTimeOut_ms == 0)
            {
                //不等待
                return -2;
            }
            
            //sleep一毫秒
            usleep(1000);
            iTimePass_ms++;
            if((iTimeOut_ms == -1) || (iTimePass_ms < iTimeOut_ms))
            {
                FD_ZERO(&fdset);
                FD_SET(iSockFd, &fdset);
    
                continue;
            }

            //超时
            return -2;
        }

        //有数据可读了
        break;
    }

    //能读多少算多少
    int iBytes = recv(iSockFd, pszRecvBuf, iRecvBytes, 0);
    
    return iBytes;
*/
}


int OS::RecvBytesTOut(int iSockFd, char * pszRecvBuf, int iRecvBytes
    , int iTimeOut_ms)
{
    int iTimePass_ms = -10;

    int iRecvedBytes = 0;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(iSockFd, &fdset);
        
    struct timeval timev;
    timev.tv_sec = 0;
    timev.tv_usec = 0;
    
    while(1)
    {
        int ret = select(iSockFd + 1, &fdset, 0, 0, &timev);
        if(ret < 0)
        {
            if(errno == EINTR)
            {
                FD_ZERO(&fdset);
                FD_SET(iSockFd, &fdset);
                timev.tv_sec = 0;
                timev.tv_usec = 10000;
                continue;
            }

            return -1;
        }
        else if(ret == 0)
        {
            //当前无数据可读
            
            iTimePass_ms += 10;
            if((iTimeOut_ms == -1) || (iTimePass_ms < iTimeOut_ms))
            {
                FD_ZERO(&fdset);
                FD_SET(iSockFd, &fdset);
                timev.tv_sec = 0;
                timev.tv_usec = 10000;
    
                continue;
            }

            //超时
            return -2;
        }

        //有数据可读

        ret = recv(iSockFd, pszRecvBuf + iRecvedBytes
            , iRecvBytes - iRecvedBytes, 0);
        if(ret <= 0)
        {
            //网络故障
            return ret;
        }
        iRecvedBytes += ret;
        if(iRecvedBytes < iRecvBytes)
        {
            FD_ZERO(&fdset);
            FD_SET(iSockFd, &fdset);
            timev.tv_sec = 0;
            timev.tv_usec = 10000;
                
            continue;
        }

        return iRecvedBytes;
    }
    
}

int OS::CloseSock(int iSockFd)
{
    return close(iSockFd);
}

int OS::CreatePath(const char * path)
{
    char DirName[256];
    strcpy(DirName, path);
    int i, len = strlen(DirName);
    if (DirName[len - 1] != '/')
    strcat(DirName, "/");

    len = strlen(DirName);

    for (i = 1; i < len; i++) {
        if (DirName[i] == '/') {
            DirName[i] = 0;
            /*　　功 能: 确定文件的访问权限，检查某个文件的存取方式，比如说是只读方式、只写方式等。如果指定的存取方式有效，则函数返回0，
            * 否则函数返回-1
            用 法: int access(const char *filename, int amode); */
            if (access(DirName, 0) != 0) {
                /*int mkdir(const char *path)mkdir函数的第一个参数是目录名，第二个参数则指定了文件的模式
                * 后面参数的值包括三位八进制，每一个数字指定不同用户对该文件的权限，第一个数字代表文件所有者的权限，
                * 第二个数字代表指定组(group)的权限，第三个数字指定其他所有人的权限(public）。
                其中，每一个数字包含三种权限：read,write,exec,他们权限值为4，2，1，
                三个值想加就是某个用户对某个文件的权限！例如：如果允许自己有read,write ,exec，
                本组人员有read,exec,其他人员仅有read 权限，则可以把mode设定为751*/
                if (mkdir(DirName, 0751) == -1) {
                    return -1;
                }
            }
            DirName[i] = '/';
        }
    }

    return 0;
}

void OS::RemovePath(const char* path)
{
    std::string dir(path);
    struct stat stat_buf;
    if(lstat(dir.c_str(), &stat_buf) < 0){
        return;
    }
    if (S_ISDIR(stat_buf.st_mode)) {
        DIR *dp = opendir(dir.c_str());
        struct dirent* d;
        while ((d =readdir(dp)) != NULL ) {
            if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..")==0){
                continue;
            }
            
            std::string new_path = dir + "/" + d->d_name;
            RemovePath(new_path.c_str());
        }
        
        rmdir(dir.c_str());
    } else {
        unlink(dir.c_str());
    }
}


unsigned long OS::GetFileSize(char * filepath)
{
    struct stat buf;  
    if(stat(filepath, &buf)<0)  
    {  
        return 0;
    }  
    
    return (unsigned long)buf.st_size;  
}

