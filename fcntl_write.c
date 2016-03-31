#include<unistd.h>
#include<sys/file.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>

void lock_set(int fd,int type)
{
	struct flock lock;
	lock.l_whence=SEEK_SET;
	lock.l_start=0;
	lock.l_len=0;
	while(1)
	{
		lock.l_type=type;
		if((fcntl(fd,F_SETLK,&lock))==0)
		{
			if(lock.l_type==F_RDLCK)
			{
				printf("read lock set by %d\n",getpid());
			}
			else if(lock.l_type==F_WRLCK)
			{
				printf("write lock set by %d\n",getpid());
			}
			else if(lock.l_type==F_UNLCK)
			{
				printf("release lock by %d\n",getpid());
			}
			return ;
		}
		//判断文件是否可以上锁 
		fcntl(fd,F_GETLK,&lock);
		if(lock.l_type!=F_UNLCK)
		{
			if(lock.l_type==F_RDLCK)
			{
				printf("read lock alrealy set by %d\n",lock.l_pid);
			}
			else if(lock.l_type==F_WRLCK)
			{
				printf("write lock alrealy set by %d\n",lock.l_pid);
			}
			getchar();
		}
	}
}

int main(void)
{
	int fd;
	fd=open("hello",O_RDWR|O_CREAT,0666);
	if(fd<0)
	{
		perror("open");
		exit(1);
	}
	//给文件上锁
	lock_set(fd,F_WRLCK);
	getchar();
	lock_set(fd,F_UNLCK);
	getchar();
	close(fd);

	exit(0);
}
