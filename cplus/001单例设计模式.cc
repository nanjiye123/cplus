/*************************************************************************
	> File Name: 单例设计模式.cc
    > Author: songnan
    > Email: nanjiye123@163.com 
    > Created Time: 2016年05月28日 星期六 16时22分48秒
    > Version: v1.0
 ************************************************************************/

#include <iostream>
#include <cstdio>
using namespace std;

class Singleton
{
public:
	static Singleton *getInstance();
	static void destory();
private:
	Singleton();
private:
	static Singleton *_pInstance;
};

Singleton* Singleton::_pInstance = NULL;

Singleton::Singleton()
{
	cout << "singleton" << endl;
}

Singleton *Singleton::getInstance()
{
	if(NULL == _pInstance)	
	{
		_pInstance = new Singleton;	
	}
	return _pInstance;
}

void Singleton::destory()
{
	delete _pInstance;
}

int main() //内存中只有一个类的对象
{
	Singleton *p = Singleton::getInstance();
	printf("p=%p\n", p);
	Singleton::destory();

}


