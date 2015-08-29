
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
    //只负责清除DoubleLinkedListItem本身。
    //DoubleLinkedListItem::data需要使用者自己负责清除
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


