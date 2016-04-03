#include<pthread.h>
#include<stdio.h>

int main(void)
{
	pthread_spinlock_t lock;
	printf("pthread_spinlock_t size is %d\n",sizeof(pthread_spinlock_t));
	lock=-1;
	
	pthread_spin_init(&lock,0);
	printf("after init:%d\n",(int)lock);

//	pthread_spin_lock(&lock);
//	printf("after lock:%d\n",(int)lock);

	pthread_spin_lock(&lock);
	printf("after luck:%d\n",(int)lock);

	pthread_spin_unlock(&lock);
	printf("after unlock:%d\n",(int)lock);

	pthread_spin_unlock(&lock);
	printf("after unlock:%d\n",(int)lock);

	return 0;
}
