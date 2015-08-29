

#ifndef _PACKETQUEUE_H_
#define _PACKETQUEUE_H_

#include "common/SDQueue.h"
#include "Packet.h"

class PacketQueue : public SDQueue<Packet*>{
public:
	PacketQueue(int capacity);
	virtual ~PacketQueue();

protected:
	virtual Packet** allocBuffer(int capacity);
	virtual void freeBuffer(Packet**);

private:
};


#endif //_PACKETQUEUE_H_


