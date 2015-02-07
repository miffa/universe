#ifndef CLConditionVariable_H
#define CLConditionVariable_H

#include <pthread.h>
#include "CLStatus.h"

using namespace std;

class CLMutex;

class CLConditionVariable
{
public:
    	CLConditionVariable();
	explicit CLConditionVariable(pthread_cond_t *pCond);
	virtual ~CLConditionVariable();

	CLStatus Wait(CLMutex *pMutex);
	CLStatus Wakeup();
	CLStatus WakeupAll();

private:
	CLConditionVariable(const CLConditionVariable&);
	CLConditionVariable& operator=(const CLConditionVariable&);

private:
	pthread_cond_t *m_pConditionVariable;
	bool m_bNeededDestroy;
};

#endif