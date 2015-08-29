
#ifndef _MSGQUEUE_H_
#define _MSGQUEUE_H_

#include <sys/types.h>
#include <string.h>

class CMsgQueue
{
public:
    //�ɹ�����0
    //ʧ�ܷ���-1
    //���EXCLΪtrue��������Ϣ�����Ѿ������򷵻� -2
    int Create(key_t IpcKey, bool EXCL = false);

    //Destroy�����ж����е�����ȫ��������
    void Destroy();

    //�ɹ�����0
    //�쳣����-1
    //���Ϊ������(bBlock==false)������-2��ʾ��������
    int Push(char* pMsg, int iMsgSize, bool bBlock = true);

    //�ɹ�������Ϣ���ȣ�����������Ԫ�ؿ�����Buf��������Ԫ��ɾ��
    //�쳣����-1
    //���Ϊ������(bBlock==false)������-2��ʾ����Ϊ��
    int Pop(char* pMsgBuf, int iBufSize, bool bBlock = true);
    
    //��ȡ��ǰ��������Ϣ����
    int GetMsgNum();

public:
    int m_MsgQId;
};

#endif //_MSGQUEUE_H_

