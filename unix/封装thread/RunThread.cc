//线程的任务执行函数

#include "Thread.h"
#include "RunThread.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>

using std::cout;
using std::endl;

namespace wd
{

void RunThread::run()
{
	srand(time(NULL));
	while(true)
	{
		int num = rand() % 100;
		cout << num << endl;
		sleep(1);
	}
}




}//end namespace


