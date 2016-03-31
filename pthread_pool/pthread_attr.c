#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<errno.h>

pthread_mutex_t mutex; //PTHREAD_MUTEX_INITIALIZE
pthread_cond_t cond;	//PTHREAD_COND_INITIALIZE

static void *pthread_func_1(void *);
static void *pthread_func_2(void *);

int main(int argc,char **argv)
{
	pthread_t pt_1=0;
	pthread_t pt_2=0;
	pthread_attr_t attr={0};
	int ret=0;
	
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);
	
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

	ret=pthread_create(&pt_1,&attr,pthread_func_1,NULL);
	if(ret!=0)
	{
		perror("pthread_1_create");
	}
	ret=pthread_create(&pt_2,NULL,pthread_func_2,NULL);
	if(ret!=0)
	{
		perror("pthread_2_create");
	}

	pthread_join(pt_2,NULL);
	sleep(5);
	
	return 0;
}

static void *pthread_func_1(void *arg)
{
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond,&mutex);
	int i=0;
	for(;i<3;i++)
	{
		printf("This is pthread_1.\n");
		if(i==2)
		{
			pthread_exit(0);
		}
	}
	pthread_mutex_unlock(&mutex);

	return ;
}

static void *pthread_func_2(void *arg)
{
	pthread_mutex_lock(&mutex);
	int i=0;
	for(;i<6;i++)
	{
		printf("This is pthread_2.\n");
	}
	pthread_mutex_unlock(&mutex);

	printf("guiqinghai\n");
		pthread_cond_signal(&cond);
	return ;
}
