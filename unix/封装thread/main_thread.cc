//主函数
//封装thread，面向对象的方式
//编译 g++ *.cc -lpthread

#include "RunThread.h"
#include <iostream>

int main(void)
{
	wd::Thread * pThread = new wd::RunThread;
	pThread->start();
	pThread->join();

	return 0;
}


