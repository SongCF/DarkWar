/********************************************************************
	created:		2005/08/21
	created:		21:8:2005   17:41
	filename: 		d:\work\newcvs\chub\impl\src\common\sdthreadpool.h
	file path:		d:\work\newcvs\chub\impl\src\common
	file base:		sdthreadpool
	file ext:		h
	author:			lifeng
	description:	a simple thread pool
*********************************************************************/

#ifndef SANDAI_C_THREADPOOL_H_200508211741
#define SANDAI_C_THREADPOOL_H_200508211741

#include <pthread.h>

class SDThreadPool
{
	const static int STACK_SIZE = (256 * 1024);

public:
	SDThreadPool(int pool_size);
	virtual ~SDThreadPool();

	bool startThreads();
	void waitThreadsTermination();

	inline int get_pool_size()
	{
		return m_pool_size;
	}

	inline int get_active_thread_count()
	{
		pthread_mutex_lock(&m_count_lock);
		int active_count = m_active_count;
		pthread_mutex_unlock(&m_count_lock);
		return active_count;
	}

protected:
	virtual void doIt() = 0;

	inline void setActive()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count++;
		pthread_mutex_unlock(&m_count_lock);
	}

	inline void setInactive()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count--;
		pthread_mutex_unlock(&m_count_lock);
	}

private:

	static void* threadProc(void* para);

	int m_pool_size;
	pthread_t *m_thread_ids;
	pthread_mutex_t m_count_lock;

	int m_active_count;
};

class SDTwinThreadPool
{
	const static int STACK_SIZE = (256 * 1024);

public:
	SDTwinThreadPool(unsigned poolsize1 = 1,unsigned poolsize2 = 1);
	virtual ~SDTwinThreadPool();

	bool startThreads();
	void waitThreadsTermination();

	inline int get_pool_size()
	{
		return m_pool_size1;
	}

	inline int get_pool_size2()
	{
		return m_pool_size2;
	}

	inline int get_active_thread_count()
	{		
		pthread_mutex_lock(&m_count_lock);
		int native =  m_active_count1;
		pthread_mutex_unlock(&m_count_lock);
		return native;
	}

	inline int get_active_thread_count2()
	{		
		pthread_mutex_lock(&m_count_lock);
		int native =  m_active_count2;
		pthread_mutex_unlock(&m_count_lock);
		return native;
	}

protected:
	virtual void doIt() = 0;

	virtual void doIt2() = 0;

	inline void setActive()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count1++;
		pthread_mutex_unlock(&m_count_lock);
	}

	inline void setInactive()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count1--;
		pthread_mutex_unlock(&m_count_lock);
	}

	inline void setActive2()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count2++;
		pthread_mutex_unlock(&m_count_lock);
	}

	inline void setInactive2()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count2--;
		pthread_mutex_unlock(&m_count_lock);
	}

private:

	static void* threadProc(void* para);
	
	static void* threadProc2(void* para);

	unsigned		m_pool_size1;

	unsigned		m_pool_size2;
	
	pthread_t*	m_thread_ids1;
	pthread_t*	m_thread_ids2;
	
	pthread_mutex_t m_count_lock;

	int m_active_count1;
	int m_active_count2;
};

class SDTripleThreadPool
{
	const static int STACK_SIZE = (256 * 1024);

public:
	SDTripleThreadPool(unsigned poolsize1 = 1,unsigned poolsize2 = 1,unsigned poolsize3 = 1);
	virtual ~SDTripleThreadPool();

	bool startThreads();
	void waitThreadsTermination();

	inline int get_pool_size()
	{
		return m_pool_size1;
	}

	inline int get_pool_size2()
	{
		return m_pool_size2;
	}

    inline int get_pool_size3()
	{
		return m_pool_size2;
	}

	inline int get_active_thread_count()
	{		
		pthread_mutex_lock(&m_count_lock);
		int native =  m_active_count1;
		pthread_mutex_unlock(&m_count_lock);
		return native;
	}

	inline int get_active_thread_count2()
	{		
		pthread_mutex_lock(&m_count_lock);
		int native =  m_active_count2;
		pthread_mutex_unlock(&m_count_lock);
		return native;
	}

    inline int get_active_thread_count3()
	{		
		pthread_mutex_lock(&m_count_lock);
		int native =  m_active_count3;
		pthread_mutex_unlock(&m_count_lock);
		return native;
	}

protected:
	virtual void doIt() = 0;

	virtual void doIt2() = 0;

    virtual void doIt3() = 0;

	inline void setActive()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count1++;
		pthread_mutex_unlock(&m_count_lock);
	}

	inline void setInactive()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count1--;
		pthread_mutex_unlock(&m_count_lock);
	}

	inline void setActive2()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count2++;
		pthread_mutex_unlock(&m_count_lock);
	}

	inline void setInactive2()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count2--;
		pthread_mutex_unlock(&m_count_lock);
	}

    inline void setActive3()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count3++;
		pthread_mutex_unlock(&m_count_lock);
	}

	inline void setInactive3()
	{
		pthread_mutex_lock(&m_count_lock);
		m_active_count3--;
		pthread_mutex_unlock(&m_count_lock);
	}

private:

	static void* threadProc(void* para);
	
	static void* threadProc2(void* para);

    static void* threadProc3(void* para);

	unsigned		m_pool_size1;

	unsigned		m_pool_size2;

    unsigned		m_pool_size3;
	
	pthread_t*	m_thread_ids1;
	pthread_t*	m_thread_ids2;
    pthread_t*	m_thread_ids3;
	
	pthread_mutex_t m_count_lock;

	int m_active_count1;
	int m_active_count2;
    int m_active_count3;
};


#endif
