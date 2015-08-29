/********************************************************
������:������
����ʱ��:2006-04-30
����:�Բ���ϵͳ�ṩ��api���б���װ���Է���ʹ��

�޶���¼:
    �޶���:
    �޶�ʱ��:
    �޶�˵��:
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
    //���ȼ�鹲���ڴ��Ƿ��Ѿ�����
    int iShmId = shmget(IpcKey, 0, 0600);
    if(iShmId >= 0)
    {
        //�����ڴ��Ѿ�����
        
        return -2;
    }
    else
    {
        if(errno == ENOENT)
        {
            //�����ڴ治���ڣ�����֮
            
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

            //�ɹ�
            ret_iShmId = iShmId;
            ret_pShm = pShm;

            return 0;
        }
        else
        {
            //��������
            
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
            //�����ڴ治����
            return -2;
        }
        
        return -1;
    }
    void* pShm = shmat(iShmid, 0, 0);
    if(pShm == (void*)-1)
    {
        return -1;
    }

    //��ȡ�����ڴ�Ĵ�С
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
//�źŴ���������
//ά��ϵͳĬ��
#define SYSDFL 0
//����
#define IGNORE 1
//�����˳�
#define EXIT 2
//��¼�����˴��źš�֮�ⲻ���κβ���
#define RECORD 3

struct SSigOp
{
    int iSigNo;
    int iSigOp;
};

//Ԥ����Linux�������źŵĴ���ʽ
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
                fprintf(stderr, "�źŴ���ʽ���ô���\n");
                break;
        }
    }

    return 0;

}

void OS::Daemonize()
{
    pid_t iPID;
    
    if((iPID = fork()) < 0) /* -1�Ǵ���ʧ�ܵ���� */
    {
    	fprintf(stderr, "fork1 failed!\n");
        return ;
    }
    else if(iPID > 0)       /* ����0�ǽ��ӽ��̵�PID���ظ������� */
    {
        exit(0);
    }

    /* �����µ�session��process group����Ϊ��leader������������ն� */
    setsid();

    /*
    * ����Ŀ¼����Ϊ��Ŀ¼������Ϊ�˱�֤���ǵĽ��̲�ʹ���κ�Ŀ¼���������ǵ��ػ����̽�һֱ
    * ռ��ĳ��Ŀ¼������ܻ���ɳ����û�����ж��һ���ļ�ϵͳ�� 
    */
    chdir("/");
    
    /*
     * ���ļ���ʽ��������������Ϊ"0"��������Ϊ�ɼ̳е������ļ�������ʽ�����ֿ��ܻ��ֹĳЩ���Ȩ��
     * �������ǵ��ػ�������Ҫ����һ��ɶ���д���ļ��������ػ����̴Ӹ���������̳������ļ�������ʽ
     * ������ȴ�п������ε������������Ȩ�����´�����һ���ļ������д�����Ͳ�����Ч�����Ҫ���ļ�
     * ��ʽ��������������Ϊ"0"��
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

    //���õ�ַ����
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
    //����IP��ַ
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

    ////�ȴ�socket�ɶ�
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
            //��ǰ�����ݿɶ�
            
            iTimePass_ms += 10;
            if(iTimePass_ms < iTimeOut_ms)
            {
                FD_ZERO(&fdset);
                FD_SET(iListenFd, &fdset);
                timev.tv_sec = 0;
                timev.tv_usec = 100000;
    
                continue;
            }

            //��ʱ
            return -2;
        }

        //�����ݿɶ���
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
        fprintf(stderr, "getpeernameʧ��");
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

		if(this_write<=0)   //�������
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

    ////�ȴ�socket�ɶ�
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

        //�����ݿɶ���
        break;
    }

    //�ܶ����������
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

    ////�ȴ�socket�ɶ�
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
            //��ǰ�����ݿɶ�
            
            iTimePass_ms += 10;
            if(iTimePass_ms < iTimeOut_ms)
            {
                FD_ZERO(&fdset);
                FD_SET(iSockFd, &fdset);
                timev.tv_sec = 0;
                timev.tv_usec = 10000;
    
                continue;
            }

            //��ʱ
            return -2;
        }

        //�����ݿɶ���
        break;
    }

    //�ܶ����������
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

    ////�ȴ�socket�ɶ�
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
            //��ǰ�����ݿɶ�
            
            if(iTimeOut_ms == 0)
            {
                //���ȴ�
                return -2;
            }
            
            //sleepһ����
            usleep(1000);
            iTimePass_ms++;
            if((iTimeOut_ms == -1) || (iTimePass_ms < iTimeOut_ms))
            {
                FD_ZERO(&fdset);
                FD_SET(iSockFd, &fdset);
    
                continue;
            }

            //��ʱ
            return -2;
        }

        //�����ݿɶ���
        break;
    }

    //�ܶ����������
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
            //��ǰ�����ݿɶ�
            
            iTimePass_ms += 10;
            if((iTimeOut_ms == -1) || (iTimePass_ms < iTimeOut_ms))
            {
                FD_ZERO(&fdset);
                FD_SET(iSockFd, &fdset);
                timev.tv_sec = 0;
                timev.tv_usec = 10000;
    
                continue;
            }

            //��ʱ
            return -2;
        }

        //�����ݿɶ�

        ret = recv(iSockFd, pszRecvBuf + iRecvedBytes
            , iRecvBytes - iRecvedBytes, 0);
        if(ret <= 0)
        {
            //�������
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
            /*������ ��: ȷ���ļ��ķ���Ȩ�ޣ����ĳ���ļ��Ĵ�ȡ��ʽ������˵��ֻ����ʽ��ֻд��ʽ�ȡ����ָ���Ĵ�ȡ��ʽ��Ч����������0��
            * ����������-1
            �� ��: int access(const char *filename, int amode); */
            if (access(DirName, 0) != 0) {
                /*int mkdir(const char *path)mkdir�����ĵ�һ��������Ŀ¼�����ڶ���������ָ�����ļ���ģʽ
                * ���������ֵ������λ�˽��ƣ�ÿһ������ָ����ͬ�û��Ը��ļ���Ȩ�ޣ���һ�����ִ����ļ������ߵ�Ȩ�ޣ�
                * �ڶ������ִ���ָ����(group)��Ȩ�ޣ�����������ָ�����������˵�Ȩ��(public����
                ���У�ÿһ�����ְ�������Ȩ�ޣ�read,write,exec,����Ȩ��ֵΪ4��2��1��
                ����ֵ��Ӿ���ĳ���û���ĳ���ļ���Ȩ�ޣ����磺��������Լ���read,write ,exec��
                ������Ա��read,exec,������Ա����read Ȩ�ޣ�����԰�mode�趨Ϊ751*/
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

