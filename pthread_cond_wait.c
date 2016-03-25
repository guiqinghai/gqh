#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

void *thread1(void *);
void *thread2(void *);

int i=1;

int main(int argc,char **argv)
{
	pthread_t t_a;
	pthread_t t_b;

	pthread_create(&t_a,NULL,thread1,(void *)NULL);
	pthread_create(&t_b,NULL,thread2,(void *)NULL);

	pthread_join(t_b,NULL);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

	exit(0);	
}

void *thread1(void *junk)
{
	for(i=1;i<=9;i++)
	{
		pthread_mutex_lock(&mutex);
		if(i%3==0)
		{
			pthread_cond_signal(&cond); //条件开变，发送信号，通知头进程
		}
		else
		{
			printf("thread1:%d\n",i);
		}
		pthread_mutex_unlock(&mutex);
		printf("Up Unlock Mutex\n");
		sleep(1);
	}
}

void *thread2(void *junk)
{
	while(i<9)
	{
		pthread_mutex_lock(&mutex);
		
		if(i%3!=0)
		{
			pthread_cond_wait(&cond,&mutex);
		}
		printf("thread2:%d\n",i);
		pthread_mutex_unlock(&mutex);
		printf("Down Ulock Mutex\n");

		sleep(1);
	}
}
