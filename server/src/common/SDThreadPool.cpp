
#include "SDThreadPool.h"

using namespace std;


SDThreadPool::SDThreadPool(int pool_size) : m_pool_size(pool_size)
{
	m_thread_ids = NULL;
	m_active_count = 0;
	pthread_mutex_init(&m_count_lock, NULL);
}

SDThreadPool::~SDThreadPool()
{
	if(m_thread_ids)
	{
		delete[] m_thread_ids;
		m_thread_ids = NULL;
	}
	pthread_mutex_destroy(&m_count_lock);
}

bool SDThreadPool::startThreads()
{
	bool ret = true;
	if(m_pool_size > 0)
	{		
		pthread_attr_t attr;
		pthread_attr_init(&attr);

		//set stack size
		pthread_attr_setstacksize(&attr, STACK_SIZE);

		m_thread_ids = new pthread_t[m_pool_size];
		for(int i = 0; i < m_pool_size; i++)
		{
			if(pthread_create((m_thread_ids + i), &attr, threadProc, (void*)this))
			{
				ret = false;
			}
		}

		pthread_attr_destroy(&attr);
	}
	return ret;
}

void SDThreadPool::waitThreadsTermination()
{
	if(m_thread_ids)
	{
		for(int i = 0; i < m_pool_size; i++)
		{
			pthread_join(m_thread_ids[i], NULL);
		}

		delete[] m_thread_ids;
		m_thread_ids = NULL;
	}
}

void* SDThreadPool::threadProc(void* para)
{
	SDThreadPool* me = (SDThreadPool*) para;
    
	me->doIt();
    
	return NULL;
}


SDTwinThreadPool::SDTwinThreadPool(unsigned poolsize1,unsigned poolsize2)
	: m_pool_size1(poolsize1),m_pool_size2(poolsize2),m_thread_ids1(NULL),m_thread_ids2(NULL),
		m_active_count1(0),m_active_count2(0)
{
	pthread_mutex_init(&m_count_lock, NULL);
}

SDTwinThreadPool::~SDTwinThreadPool()
{
	waitThreadsTermination();
	pthread_mutex_destroy(&m_count_lock);
}

bool SDTwinThreadPool::startThreads()
{
	bool ret = true;
	if(m_pool_size1 > 0)
	{		
		pthread_attr_t attr;
		pthread_attr_init(&attr);

		//set stack size
		pthread_attr_setstacksize(&attr, STACK_SIZE);

		m_thread_ids1 = new pthread_t[m_pool_size1];
		for(unsigned i = 0; i < m_pool_size1; i++)
		{			
			if(pthread_create(m_thread_ids1+i, &attr, threadProc, (void*)this))
			{
				ret = false;
			}
		}

		pthread_attr_destroy(&attr);
	}

	if(m_pool_size2 > 0)
	{		
		pthread_attr_t attr;
		pthread_attr_init(&attr);

		//set stack size
		pthread_attr_setstacksize(&attr, STACK_SIZE);

		m_thread_ids2 = new pthread_t[m_pool_size2];
		for(unsigned i = 0; i < m_pool_size2; i++)
		{
			if(pthread_create(m_thread_ids2+i, &attr, threadProc2, (void*)this))
			{
				ret = false;
			}
			
		}

		pthread_attr_destroy(&attr);
	}

	return ret;
}

void SDTwinThreadPool::waitThreadsTermination()
{
	if(NULL != m_thread_ids1)
	{
		for(unsigned i = 0; i < m_pool_size1; i++)
		{
			
			pthread_join(m_thread_ids1[i], NULL);
		}

		delete[] m_thread_ids1;
		m_thread_ids1 = NULL;
	}
	if(NULL != m_thread_ids2)
	{
		for(unsigned i = 0; i < m_pool_size2; i++)
		{
			pthread_join(m_thread_ids2[i], NULL);
		}

		delete[] m_thread_ids2;
		m_thread_ids2 = NULL;
	}
}

void* SDTwinThreadPool::threadProc(void* para)
{
	SDTwinThreadPool* me = (SDTwinThreadPool*) para;

	me->doIt();
    
	return NULL;
}

void* SDTwinThreadPool::threadProc2(void* para)
{
	SDTwinThreadPool* me = (SDTwinThreadPool*) para;

	me->doIt2();

	return NULL;
}


SDTripleThreadPool::SDTripleThreadPool(unsigned poolsize1,unsigned poolsize2,unsigned poolsize3)
	: m_pool_size1(poolsize1),m_pool_size2(poolsize2),m_pool_size3(poolsize3)
	    ,m_thread_ids1(NULL),m_thread_ids2(NULL),m_thread_ids3(NULL)
		,m_active_count1(0),m_active_count2(0),m_active_count3(0)
{
	pthread_mutex_init(&m_count_lock, NULL);
}

SDTripleThreadPool::~SDTripleThreadPool()
{
	waitThreadsTermination();
	pthread_mutex_destroy(&m_count_lock);
}

bool SDTripleThreadPool::startThreads()
{
	bool ret = true;
	if(m_pool_size1 > 0)
	{		
		pthread_attr_t attr;
		pthread_attr_init(&attr);

		//set stack size
		pthread_attr_setstacksize(&attr, STACK_SIZE);

		m_thread_ids1 = new pthread_t[m_pool_size1];
		for(unsigned i = 0; i < m_pool_size1; i++)
		{			
			if(pthread_create(m_thread_ids1+i, &attr, threadProc, (void*)this))
			{
				ret = false;
			}
		}

		pthread_attr_destroy(&attr);
	}

	if(m_pool_size2 > 0)
	{		
		pthread_attr_t attr;
		pthread_attr_init(&attr);

		//set stack size
		pthread_attr_setstacksize(&attr, STACK_SIZE);

		m_thread_ids2 = new pthread_t[m_pool_size2];
		for(unsigned i = 0; i < m_pool_size2; i++)
		{
			if(pthread_create(m_thread_ids2+i, &attr, threadProc2, (void*)this))
			{
				ret = false;
			}
			
		}

		pthread_attr_destroy(&attr);
	}

    if(m_pool_size3 > 0)
	{		
		pthread_attr_t attr;
		pthread_attr_init(&attr);

		//set stack size
		pthread_attr_setstacksize(&attr, STACK_SIZE);

		m_thread_ids3 = new pthread_t[m_pool_size3];
		for(unsigned i = 0; i < m_pool_size3; i++)
		{
			if(pthread_create(m_thread_ids3+i, &attr, threadProc3, (void*)this))
			{
				ret = false;
			}
			
		}

		pthread_attr_destroy(&attr);
	}

	return ret;
}

void SDTripleThreadPool::waitThreadsTermination()
{
	if(NULL != m_thread_ids1)
	{
		for(unsigned i = 0; i < m_pool_size1; i++)
		{
			
			pthread_join(m_thread_ids1[i], NULL);
		}

		delete[] m_thread_ids1;
		m_thread_ids1 = NULL;
	}
	if(NULL != m_thread_ids2)
	{
		for(unsigned i = 0; i < m_pool_size2; i++)
		{
			pthread_join(m_thread_ids2[i], NULL);
		}

		delete[] m_thread_ids2;
		m_thread_ids2 = NULL;
	}
    if(NULL != m_thread_ids3)
	{
		for(unsigned i = 0; i < m_pool_size3; i++)
		{
			pthread_join(m_thread_ids3[i], NULL);
		}

		delete[] m_thread_ids3;
		m_thread_ids3 = NULL;
	}
}

void* SDTripleThreadPool::threadProc(void* para)
{
	SDTripleThreadPool* me = (SDTripleThreadPool*) para;

	me->doIt();
    
	return NULL;
}

void* SDTripleThreadPool::threadProc2(void* para)
{
	SDTripleThreadPool* me = (SDTripleThreadPool*) para;

	me->doIt2();

	return NULL;
}

void* SDTripleThreadPool::threadProc3(void* para)
{
	SDTripleThreadPool* me = (SDTripleThreadPool*) para;

	me->doIt3();

	return NULL;
}



