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
	void destory()
	{
		delete this;
	}
private:
	~Student()
	{
		cout << "~student()" << endl;
	}

private:
	int id;
	string name;
};

int main()
{//只能创建堆对象，不能创建栈对象
//即将析构函数放入private区域
//对于堆对象的销毁，需要写一个public函数
	Student *pstu = new Student;
	pstu->destory();

//	Student stu;//不能创建栈对象，只能创建堆对象。
	return 0;
}






