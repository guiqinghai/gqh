#include<pthread.h>
#include<stdio.h>
#include<unistd.h>

pthread_spinlock_t lock;

void *th_handler(void *arg)
{
	printf("thread %lu start.............\n",pthread_self());
	pthread_spin_lock(&lock);
	printf("thread(%lu) get lock success!\n",pthread_self());
	pthread_spin_unlock(&lock);

	return NULL;
}

int create_test_thread()
{
	pthread_t tid;
	if(pthread_create(&tid,NULL,(void *)th_handler,NULL))
	{
		printf("create thread failed!\n");
		return -1;
	}
	sleep(1);
	printf("after create thread %lu,lock's value is %d\n",tid,(int)lock);
}

int main(void)
{
	int i;
	pthread_spin_init(&lock,0);
	pthread_spin_lock(&lock);
	for(i=0;i<5;i++)
	{
		create_test_thread();
	}
	pthread_spin_unlock(&lock);
	
	sleep(2);

	pthread_spin_destroy(&lock);

	return 0;
}
