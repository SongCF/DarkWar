
#include <stdlib.h>

#include "DoubleLinkedList.h"


DoubleLinkedList::DoubleLinkedList()
{
    m_head.next = &m_tail;
    m_head.prev = NULL;
    m_head.data = NULL;
    m_tail.next = NULL;
    m_tail.prev = &m_head;
    m_tail.data = NULL;
    
    m_count = 0;
}

DoubleLinkedList::~DoubleLinkedList()
{
    DoubleLinkedListItem* p = m_head.next;
    while(p != &m_tail)
    {
        Unlink(p);
        delete p;

        p = m_head.next;
    }
}

void DoubleLinkedList::AddHead(DoubleLinkedListItem * item)
{
    item->next = m_head.next;
    item->prev = &m_head;

    m_head.next = item;

    item->next->prev = item;
}

void DoubleLinkedList::AddTail(DoubleLinkedListItem * item)
{
    item->next = &m_tail;
    item->prev = m_tail.prev;

    m_tail.prev = item;

    item->prev->next = item;
}

void DoubleLinkedList::Unlink(DoubleLinkedListItem * item)
{
    if((item == &m_head) || (item == &m_tail))
    {
        return;
    }

    item->prev->next = item->next;
    item->next->prev = item->prev;
}





