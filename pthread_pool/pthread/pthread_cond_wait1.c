#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

static pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

struct node
{
	int n_number;
	struct node *n_next;
}*head=NULL;

static void cleanup_handler(void *arg)
{
	printf("cleanup handler of second thread./\n");
	free(arg);
	(void)pthread_mutex_unlock(&mtx);
}

static void *thread_func(void *arg)
{
	struct node *p=NULL;
	
	pthread_cleanup_push(cleanup_handler,p);
	while(1)
	{
		pthread_mutex_lock(&mtx);
		/*pthread_cond_wait()可能被意外唤醒,意外被唤醒不是我们想要的，应该让线程进入pthread_cond_wait*/
		while(head==NULL)
		{
			/*pthread_cond_wait会解除之前的pthread_mutex_lock锁定的锁，然后阻塞在等待队列里休眠，直到再次被唤醒（大多数情况是等待的条件成立而被唤醒，唤醒后该进程会先锁定pthread_mutex_lock(&mutx)） 再读取资源 流程block---》unlock---》wait---》return ----》lock*/
			pthread_cond_wait(&cond,&mtx);
		}
		
		p=head;
		head=head->n_next;
		printf("Got %d from front of queue/n\n",p->n_number);
		free(p);
		pthread_mutex_unlock(&mtx);
	}
	pthread_cleanup_pop(0);

	return 0;
}

int main(int argc,char **argv)
{
	pthread_t tid;
	int i;
	struct node *p;
	pthread_create(&tid,NULL,thread_func,NULL);
	
	for(i=0;i<10;i++)
	{
		p=malloc(sizeof(struct node));
		p->n_number=i;
		pthread_mutex_lock(&mtx);
		p->n_next=head;
		head=p;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mtx);
		sleep(1);
	}
	printf("thread 1 wanna end the line.So cancel thread 2./n\n");
	pthread_cancel(tid);/*从最近的取消点退出*/
	pthread_join(tid,NULL);
	printf("all done --exiting/n\n");

	return 0;
}
