/********************************************************************
	created:		2005/08/15
	created:		15:8:2005   12:32
	filename: 		d:\work\newcvs\chub\impl\src\common\sdqueue.h
	file path:		d:\work\newcvs\chub\impl\src\common
	file base:		sdqueue
	file ext:		h
	author:			lifeng
	description:	this is a fixed size queue; class T had better be a pointer
*********************************************************************/

#ifndef SANDAI_C_QUEUE_H_200508151110
#define SANDAI_C_QUEUE_H_200508151110

#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include "SDException.h"

template <class T> class SDQueue {

	T*  m_buffer;

	int m_capacity;
	int m_start;
	int m_size;

	pthread_mutex_t m_lock;
	pthread_cond_t	m_cond_not_full;
	pthread_cond_t	m_cond_not_empty;

public:

	SDQueue()
	{
		m_buffer = NULL;
		m_start = 0;
		m_size = 0;
		m_capacity = 0;

		pthread_mutex_init(&m_lock, NULL);
		if(pthread_cond_init(&m_cond_not_full, NULL))
			throw SDInitException("Init thread_cond_full error: "+ errno);
		if(pthread_cond_init(&m_cond_not_empty, NULL))
			throw SDInitException("Init thread_cond_empty error: "+ errno);
	}

	virtual ~SDQueue()
	{
		pthread_mutex_destroy(&m_lock);
		pthread_cond_destroy(&m_cond_not_full);
		pthread_cond_destroy(&m_cond_not_empty);
	}

	void init(int capacity)
	{
		try
		{
			m_buffer = allocBuffer(capacity);
			m_capacity = capacity;
		}
		catch(std::exception&){}
	}

	void uninit()
	{
		freeBuffer(m_buffer);
	}

	bool pop(T& retVal, int waitMs)
	{
		if(m_buffer == NULL)
		{
			return false;
		}

		bool ret_val = false;
		pthread_mutex_lock(&m_lock);

		try
		{
			if(m_size == 0)
			{//empty
				if(waitMs == 0)
				{
					pthread_mutex_unlock(&m_lock);
					return false;
				}
				else if(waitMs < 0)
				{//wait infinitely
					while(m_size == 0)
						pthread_cond_wait(&m_cond_not_empty, &m_lock);					
				}
				else
				{//wait for a given timeout
					struct timespec timespot;
					getAbsTimeout(waitMs, timespot);					
					pthread_cond_timedwait(&m_cond_not_empty, &m_lock, &timespot);
				}
			}

			if(m_size > 0)
			{
				retVal = m_buffer[m_start];
				ret_val = true;

				if(++m_start >= m_capacity)
					m_start -= m_capacity;

				m_size --;
				pthread_cond_signal(&m_cond_not_full);
			}
		}
		catch (std::exception& ) {
			//exception
			ret_val = false;
		}

		pthread_mutex_unlock(&m_lock);
		return ret_val;
	}

	bool push(const T& element, int waitMs)
	{
		if(m_buffer == NULL)
		{
			return false;
		}

		bool retVal = false;
		pthread_mutex_lock(&m_lock);

		try	
		{
			if(m_size >= m_capacity)
			{//full
				if(waitMs == 0)
				{					
					pthread_mutex_unlock(&m_lock);
					return false;
				}
				else if(waitMs < 0)
				{//wait infinitely
					while(m_size >= m_capacity)
						pthread_cond_wait(&m_cond_not_full, &m_lock);
				}
				else
				{//wait for a given timeout
					struct timespec timespot;

					getAbsTimeout(waitMs, timespot);
					
					pthread_cond_timedwait(&m_cond_not_full, &m_lock, &timespot);
				}
			}

			if(m_size < m_capacity)
			{
				int newpos = m_start + m_size;
				if(newpos >= m_capacity)
					newpos -= m_capacity;

				m_buffer[newpos] = element;
				m_size ++;
				pthread_cond_signal(&m_cond_not_empty);
				retVal = true;
			}
		}
		catch (std::exception&) {
			//exception
			retVal = false;
		}

		pthread_mutex_unlock(&m_lock);
		return retVal;
	}

	bool pop(T& retVal)
	{
		return pop(retVal, -1);
	}

	bool push(T element)
	{
		return push(element, -1);
	}

	int getCapacity()
	{
		return m_capacity;
	}

	int getSize()
	{
		pthread_mutex_lock(&m_lock);
		int size = m_size;
		pthread_mutex_unlock(&m_lock);
		return size;
	}

	bool	empty()
	{
		pthread_mutex_lock(&m_lock);
		bool ret = (0 == m_size);
		pthread_mutex_unlock(&m_lock);
		return ret;
	}

protected:
	virtual T* allocBuffer(int capacity) = 0;
	virtual void freeBuffer(T* pBuffer) = 0;

private:
	void getAbsTimeout(int timeout, struct timespec& abstimeout)
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		timeout += (now.tv_usec / 1000);

		abstimeout.tv_sec = now.tv_sec + timeout / 1000;
		abstimeout.tv_nsec = (timeout % 1000) * 1000 * 1000;
	}

};

#endif
