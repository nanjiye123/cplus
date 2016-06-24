/*************************************************************************
	> File Name: 004IO流.cc
    > Author: songnan
    > Email: nanjiye123@163.com 
    > Created Time: 2016年05月29日 星期日 15时38分07秒
    > Version: v1.0
 ************************************************************************/

#include <iostream>
#include <limits>
using namespace std;

void printCinState()
{
	cout << endl;
	cout << "bad  = " << cin.bad() << endl;
	cout << "fail = " << cin.fail() << endl;
	cout << "eof  = " << cin.eof() << endl;
	cout << "good = " << cin.good() << endl;
	cout << endl;
}

int main()
{
	//==========示例1===================================================
	printCinState();//第一次查询输入流的状态，goodbit=1

	int inum;
	while(cin >> inum)//当输入不是int型的时候出错
	{
		cout << inum << endl;
	}
	printCinState();//第二次查询，不是int的输入会出错，failbit=1

	cin.clear();//failbit是可恢复的错误，用clear恢复
	cin.ignore(numeric_limits<streamsize>::max(), '\n');//清除缓冲区，否则刚才的错误输入将会打印出来
	printCinState();//goodbit=1

	string a;
	cin >> a;
	cout << a << endl;
	//=================================================================
	//
	//
	//
	//=========示例2===================================================
	int ival;
    while(cin >> ival, !cin.eof())//到文件末尾 或 输入停止，退出
    { //按ctrl+C退出
        if(cin.bad())//系统级故障，不可恢复
        {
            cout << "IO stream corrupted! " << endl;
            return -1;
        }
        if(cin.fail())//输入的不是int型
        {
            cout << "data illegal, try again!" << endl;
            cin.clear();
            cin.ignore(1024, '\n');
            continue;
        }
        cout << "ival = " << ival << endl;
    }
	//================================================================

	return 0;
}







