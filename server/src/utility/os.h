/********************************************************
������:������
����ʱ��:2006-04-30
����:�Բ���ϵͳ�ṩ��api���б���װ���Է���ʹ��

�޶���¼:
    �޶���:
    �޶�ʱ��:
    �޶�˵��:
*********************************************************/
#ifndef _OS_H_
#define _OS_H_

#include <pthread.h>
#include <sys/types.h>

namespace OS
{
    //��������ڴ�ԭ�������ڣ��򴴽�֮�������ɹ������� 0��
    //��������ڴ��Ѿ����ڣ�ֱ�ӷ���-2
    //�������д��󷵻�-1��������Ϣ���Դ�errno���
    int CreateShm(key_t ipckey, size_t size, int& ret_iShmID, void*& ret_pShm);

    //���ٹ����ڴ�
    int DestoryShm(int iShmID);

    //attach���Ѿ������Ĺ����ڴ档
    //�ɹ������� 0
    //�����ڴ治���ڣ�����-2
    //�������д��󣬷��� -1��������Ϣ���Դ�errno���
    int AttachShm(key_t ipckey, size_t& ret_size, int& ret_iShmID, void*& ret_pShm);

    //disattach�����ڴ�
    int DisattachShm(void* pShm);

    //��ʼ��mutex��Ϊ���̼乲��
    int InitMutexShared(pthread_mutex_t* pMutex);
    //��ʼ��mutex��
    int InitMutex(pthread_mutex_t * pMutex);

    //��ʼ���źŲ������
    typedef void sigfunc(int);
    int InitSignal(sigfunc* ExitHdlr);

    //����ǰ���̾��黯
    void Daemonize();

    //socket����---------------------------------------------------------------
    
    //����һ��TCP Socket
    //�ɹ����ز�С��0��sockfd
    //ʧ�ܷ���-1
    int CreateTCPSock();

    //��socket��ip�Ͷ˿�
    //���pszIPΪ0���򴴽�INADDR_ANY
    //�ɹ�����0��ʧ�ܷ���-1
    int Bind(int iSockFd, unsigned int shPort, const char* pszIP = 0);

    //�ɹ�����0��ʧ�ܷ���-1
    int Listen(int iSockFd, int iBackLog = 30);


    //out_pClientPort��out_pszClientIPBuf���������ϵĿͻ�����Ϣ��
    //��������Ŀͻ�����Ϣ�����Խ�������Ϊ0
    //�ɹ����ز�С��0��ConnSockFd�����󷵻�-1
    int Accept(int iListenFd);

    //�ɹ����ز�С��0��ConnSockFd�����󷵻�-1
    //����-2��ʾ��ʱ
    //iTimeOut_ms��λ:����
    //iTimeOut_msΪ-1��ʾ��Զ�ȴ�
    //iTimeOut_msΪ0��ʾ������
    int AcceptTOut(int iListenFd, int iTimeOut_ms);

    //��ȡ���ӶԷ���IP�Ͷ˿�
    int GetPeerInfor(int iSockFd, char*& pszPeerIP, int& iPeerPort);

    //�ɹ�����0��ʧ�ܷ���-1
    int Connect(int iSockFd, char* pszServIP, unsigned int shServPort);

    //����ʵ�ʷ��ͳɹ����ֽ�
    //����-1��ʾ����
    int Send(int iSockFd, char* pSendBuf, int iSendBytes);

    //����ʵ�ʽ��յ����ֽ���
    //����0��ʾ�Է��Ѿ��ر�����
    //����-1��ʾ��������
    //ԭ��:һֱ�ȴ�iSockFd�����ݿɶ���
    //�����ݿɶ���ֻ��ȡһ�Ρ�
    //�����ǰ�ɶ��ֽڴﵽiRecvBytes�����ȡiRecvBytes
    //�����ǰ�ɶ��ֽڲ���iRecvBytes�����ȡ�ܶ�ȡ�������ֽڣ�Ȼ�󷵻ض�ȡ���ֽ���
    int Recv(int iSockFd, char* pRecvBuf, int iRecvBytes);

    //����ʵ�ʽ��յ����ֽ���
    //����0��ʾ�Է��Ѿ��ر�����
    //����-2��ʾ��ʱ
    //����-1��ʾ��������
    //iTimeOut_ms��λ:����
    //iTimeOut_msΪ-1��ʾ��Զ�ȴ�
    //iTimeOut_msΪ0��ʾ������
    //ԭ��:һֱ�ȴ�iSockFd�����ݿɶ�����ʱ
    //�����ݿɶ���ֻ��ȡһ�Ρ�
    //�����ǰ�ɶ��ֽڴﵽiRecvBytes�����ȡiRecvBytes
    //�����ǰ�ɶ��ֽڲ���iRecvBytes�����ȡ�ܶ�ȡ�������ֽڣ�Ȼ�󷵻ض�ȡ���ֽ���
    int RecvTOut(int iSockFd, char* pRecvBuf, int iRecvBytes
        , int iTimeOut_ms = -1);

    //����ʵ�ʽ��յ����ֽ���
    //����0��ʾ�Է��Ѿ��ر�����
    //����-2��ʾ��ʱ
    //����-1��ʾ��������
    //iTimeOut_ms��λ:����
    //iTimeOut_msΪ-1��ʾ��Զ�ȴ�
    //iTimeOut_msΪ0��ʾ������
    //ԭ��:һֱ�ȴ�����ȡ��iRecvBytes���ֽڣ���ʱ�����������
    int RecvBytesTOut(int iSockFd, char* pRecvBuf, int iRecvBytes
        , int iTimeOut_ms = -1);

    //�ر�socket
    int CloseSock(int iSockFd);

    //֧�ִ����༶Ŀ¼������0��ʾ�ɹ�
    int CreatePath(const char *path);
    void RemovePath(const char* path);

    unsigned long GetFileSize(char* filepath);
};

#endif //_OS_H_
