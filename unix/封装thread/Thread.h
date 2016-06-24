//封装thread

#ifndef _THREAD_H_
#define _THREAD_H_

#include "Noncopyable.h"
#include <pthread.h>

namespace wd
{


class Thread : private Noncopyable //私有继承，成为实现继承。
{
public:
	Thread();
	~Thread();

	void start();//开始创建线程并执行，是入口
	void join();

private:
	virtual void run() = 0; //做接口，线程中真正的任务
	static void * threadFunc(void *);
	//定义成static，参数第一个没有this指针

private:
	pthread_t _pthid;
	bool      _isRunning;
};


}//end namespace

#endif

