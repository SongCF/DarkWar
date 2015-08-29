
#include "PacketQueue.h"

PacketQueue::PacketQueue(int capacity)
{
	init(capacity);
}

PacketQueue::~PacketQueue()
{
	uninit();
}

Packet** PacketQueue::allocBuffer(int capacity)
{
	return new Packet*[capacity];
}

void PacketQueue::freeBuffer(Packet** buffer)
{
	delete[] buffer;
}



