/*************************************************************************
	> File Name: 002new应用.cc
    > Author: songnan
    > Email: nanjiye123@163.com 
    > Created Time: 2016年05月29日 星期日 12时12分46秒
    > Version: v1.0
 ************************************************************************/

#include <iostream>
#include <cstdlib>
using namespace std;

class Student
{
public:
	Student()
	{
		cout << "student()" << endl;
	}
	~Student()
	{
		cout << "~student()" << endl;
	}
private:
	static void * operator new(size_t sz);//声明系统new函数是私有的
	static void operator delete(void *pRet);//声明系统delete函数是私有的
	                                     //使其不能创建堆对象
#if 0
	static void * operator new(size_t sz)//重载了new函数
	{
		cout << "operator new" << endl;
		void * pRet = malloc(sz);
		return pRet;
	}

	static void operator delete(void *pRet)//重载了delete函数
	{
		cout << "operator delete" << endl;
		free(pRet);
	}
#endif

private:
	int id;
	string name;
};

int main()
{
	Student stu;//只能创建栈对象，不能创建堆对象。
	//因为new被声明为private
	//
	//不能创建堆对象
//	Student *pstu = new Student;
//	delete pstu;
	return 0;
}






