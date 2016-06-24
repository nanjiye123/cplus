//本头文件实现禁止复制功能
//当其他类继承自该类时，将不能实现对象的复制

#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

namespace wd
{


class Noncopyable
{
protected:
	Noncopyable(){}

private:
	Noncopyable(const Noncopyable & rhs);//拷贝构造函数
	Noncopyable & operator=(const Noncopyable & rhs);//重载=
	//只是声明两个函数，没有实现
};


}//end namespace


#endif

