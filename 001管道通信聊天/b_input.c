/*************************************************************************
	> File Name: a.c
    > Author: songnan
    > Email: nanjiye123@163.com 
    > Created Time: 2016年06月05日 星期日 12时04分56秒
    > Version: v1.0
 ************************************************************************/

//实现两个进程管道通信，使用到共享内存和信号量
//
//b进程输入端
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
	int ret1 = 0, ret2 = 0;
	if(!check_file("fifo1"))//查找是否已经存在管道文件
	{
		ret1 = mkfifo("fifo1", 0777);//定义管道1
	}
	if(!check_file("fifo2"))//查找是否已经存在管道文件
	{
		ret2 = mkfifo("fifo2", 0777);//定义管道2
	}
	if(ret1 < 0 || ret2 < 0)
	{
		perror("mkfifo error");
		exit(1);
	}

	int fdr, fdw;
	fdw = open("fifo1", O_WRONLY);//打开管道1
	fdr = open("fifo2", O_RDONLY);//打开管道2
	if(fdw < 0 || fdr < 0)
	{
		perror("open error");
		exit(1);
	}


//信号量部分======================================================================
	if(!check_file("memory_b.dat"))//创建文件
	{
		creat("memory_b.dat", 0777);
	}
	key_t key = ftok("memory_b.dat", 1);//获得key（信号量）
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
//output:pbuf

//====================================
	char buf[128];
	fd_set readset;
	struct timeval mytimer;
	mytimer.tv_sec = 0;
	mytimer.tv_usec = 0;
	int ret;
	int i=0;
	pshm->num = 10;//任意初值，不为0
	while(1)
	{
		FD_ZERO(&readset);//
		FD_SET(STDIN_FILENO, &readset);
		FD_SET(fdr, &readset);//将读和输入进行监测
		ret = select(fdr+1, &readset, NULL, NULL, &mytimer);
		if(ret > 0)
		{
			if(FD_ISSET(fdr, &readset))//判断fdr是否有效
			{
				bzero(buf, sizeof(buf));
				bzero(pshm->buf, sizeof(pshm->buf));
				if(read(fdr, buf, sizeof(buf)) > 0)
				{
					//printf("%s\n", buf);//===============需要修改
					
					if(sem_p(semid))
					{
						perror("sem_p error");
						exit(1);
					}
					pshm->num = i;
					i++;
					strcpy(pshm->buf, buf);
					if(sem_v(semid))//释放信号量
					{
						printf("sem_v error");
						exit(1);
					}
					if(!strcmp(buf, "bye")) break;//对方已经退出
				}
			}
			if(FD_ISSET(STDIN_FILENO, &readset))//判断stdin是否有效
			{
				bzero(buf, sizeof(buf));
				bzero(pshm->buf, sizeof(pshm->buf));
				if(read(STDIN_FILENO, buf, sizeof(buf)) > 0)
				{
					write(fdw, buf, strlen(buf));
					if(sem_p(semid))
					{
						perror("sem_p error");
						exit(1);
					}
					pshm->num = i;
					i++;
					strcpy(pshm->buf, buf);
					if(sem_v(semid))//释放信号量
					{
						printf("sem_v error");
						exit(1);
					}
				}
				else//本方输入ctrl+D
				{
					write(fdw, "bye", 3);
					if(sem_p(semid))
					{
						perror("sem_p error");
						exit(1);
					}
					strcpy(pshm->buf, "bye");
					if(sem_v(semid))//释放信号量
					{
						printf("sem_v error");
						exit(1);
					}
					break;
				}
			}
		}
	}//while(1)

	if(shmdt(pmap) < 0)//进程和共享内存脱离
	{
		perror("shmdt error");
		exit(1);
	}
	close(fdr);
	close(fdw);

	return 0;
}


