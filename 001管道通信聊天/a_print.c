/*************************************************************************
	> File Name: a.c
    > Author: songnan
    > Email: nanjiye123@163.com 
    > Created Time: 2016年06月05日 星期日 12时04分56秒
    > Version: v1.0
 ************************************************************************/

//实现两个进程管道通信，使用到共享内存和信号量
//
//a进程输入端
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <dirent.h>
#include <strings.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_SIZE 1024

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};

//sembuf不用自己创建
typedef struct tag_shm
{
    char buf[SHM_SIZE];
    unsigned short num;
}shm_buf;

int check_file(char* fifo)
{
	DIR *dir;
	dir = opendir(".");
	if(NULL == dir)
	{
		perror("error opendir");
		exit(1);
	}

	struct dirent* pt;
	while(pt = readdir(dir))
	{
		if(!strcmp(pt->d_name, fifo))//找到相同fifo
		{
			return 1;
		}
	}
	return 0;//没找到相同fifo
}


int sem_p(int semid)//占用资源，执行p操作（获取信号量）
{
	struct sembuf sem_arg;
	sem_arg.sem_num = 0;
	sem_arg.sem_op = -1;
	sem_arg.sem_flg = SEM_UNDO;

	if(semop(semid, & sem_arg, 1) == -1)
	{
		printf("%s : can't do the sem_p!\n",__func__);
		return -1;
	}
	return 0;
}

int sem_v(int semid)//释放资源，执行v操作（释放信号量）
{
	struct sembuf sem_arg;
	sem_arg.sem_num = 0;
	sem_arg.sem_op = 1;
	sem_arg.sem_flg = SEM_UNDO;

	if(semop(semid, & sem_arg, 1) == -1)
	{
		printf("%s : can't do the sem_v!\n",__func__);
		return -1;
	}
	return 0;
}

int del_sem(int semid)//删除信号量
{
    if(semctl(semid, 0, IPC_RMID) == -1)
    {
        printf("%s : can't rm the sem!\n",__func__);
        return -1;
    }
    return 0;
}


int main()
{
//信号量部分======================================================================
	if(!check_file("memory_a.dat"))//创建文件
	{
		creat("memory_a.dat", 0777);
	}
	key_t key = ftok("memory_a.dat", 1);//获得key（信号量）
	if(key < 0)
	{
		perror("ftok error");
		exit(1);
	}

	int semid = semget(key, 1, IPC_CREAT|0666);//获取信号量标识符（信号量）
	if(semid < 0)
	{
		perror("semget error");
		exit(1);
	}

	//初始化信号量（信号量）
	union semun sem_arg;
	sem_arg.val = 1; //==============信号量val=1
	if(semctl(semid, 0, SETVAL, sem_arg) == -1)
	{
		perror("semctl error");
		exit(1);
	}

//====================================================================================
//共享内存部分
	int shmid = shmget(key, sizeof(shm_buf), 0666|IPC_CREAT);//创建共享内存段(共享内存)
	if(shmid < 0)
	{
		perror("shmget error");
		exit(1);
	}

	void* pmap = (void *)shmat(shmid, NULL, 0);//获得共享内存的首地址(共享内存)
	if((void*)-1 == pmap)
	{
		perror("shmat error");
		exit(1);
	}
	shm_buf* pshm = NULL;
	pshm = pmap;
//output:pshm
//======================================================================================
	

	int before_num = 0;
	pshm->num = 10;
	while(1)
	{

		if(sem_p(semid))
		{
			perror("sem_p error");
			exit(1);
		}
		//printf("%d, %d\n", before_num,pshm->num);
		if(!strcmp(pshm->buf, "bye")) break;
		if(before_num == pshm->num)
		{
			printf("%s", pshm->buf);
			//printf("%d, %d", before_num,pshm->num);
			before_num++;
		}

		if(sem_v(semid))//释放信号量
		{
			printf("sem_v error");
			exit(1);
		}

	}//while(1)


	if(del_sem(semid))
    {
        printf("%s : del_sem failed!\n", __func__);
        return -1;
    }
	if(shmdt(pmap) < 0)//进程和共享内存脱离
	{
		perror("shmdt error");
		exit(1);
	}
	/*删除共享内存*/
    if(shmctl(shmid, IPC_RMID, 0) == -1)
    {
        printf("%s : shmctl failed!\n",__func__);
        return -1;
    }
	return 0;
}


