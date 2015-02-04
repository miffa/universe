#ifndef CLMutex_H
#define CLMutex_H

#include <pthread.h>
#include "CLStatus.h"

class CLMutex
{
public:
	friend class CLConditionVariable;

	CLMutex();
	virtual ~CLMutex();

	CLStatus Lock();
	CLStatus Unlock();

private:
	CLMutex(const CLMutex&);
	CLMutex& operator=(const CLMutex&);

private:
	pthread_mutex_t m_Mutex;
};

#endif