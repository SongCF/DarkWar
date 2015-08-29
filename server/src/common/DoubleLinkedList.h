
#ifndef _DOUBLELINKEDLIST_H_
#define _DOUBLELINKEDLIST_H_

struct DoubleLinkedListItem
{
    DoubleLinkedListItem* next;
    DoubleLinkedListItem* prev;

    void* data;
};

class DoubleLinkedList
{
public:
    DoubleLinkedList();
    //ֻ�������DoubleLinkedListItem����
    //DoubleLinkedListItem::data��Ҫʹ�����Լ��������
    ~DoubleLinkedList(); 

    void AddTail(DoubleLinkedListItem* item);
    void AddHead(DoubleLinkedListItem* item);
    void Unlink(DoubleLinkedListItem* item);

public:
    DoubleLinkedListItem m_head;
    DoubleLinkedListItem m_tail;
    

    int m_count;
};


#endif //_DOUBLELINKEDLIST_H_


