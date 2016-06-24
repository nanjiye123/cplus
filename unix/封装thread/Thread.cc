#include "Thread.h"
#include <iostream>

namespace wd
{


Thread::Thread():_pthid(0), _isRunning(false)
{
}

Thread::~Thread()
{
	if(_isRunning)
	{
		pthread_detach(_pthid);
	}
}

void Thread::start()
{
	pthread_create(&_pthid, NULL, &Thread::threadFunc, this);
	_isRunning = true;
}

void Thread::join()
{
	if(_isRunning)
	{
		pthread_join(_pthid, NULL);
	}
}

void * Thread::threadFunc(void * arg)
{
	Thread * pThread = static_cast<Thread *>(arg);//变成指向该类的指针
	if(pThread)
	{
		pThread->run();
		//由于threadFunc是static，没有this，找不到this->run，需要传进this
	}
}



}//end namespace
