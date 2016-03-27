#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<pthread.h>
#include<assert.h>

/*
*线程池里所有运行和等待的任务都是一个CThread_worker
×由于所有任务都在链表里，所以是一个链表结构
*/
typedef struct worker
{
	/*回调函数，任务运行时会调用此函数*/
	void *(*process)(void *arg);

	/* 回调函数的参数*/
	void *arg;

	struct worker *next;
}CThread_worker;

/* 线程池结构*/
typedef struct
{
	pthread_mutex_t queue_lock;
	pthread_cond_t queue_ready;
	
	/*链表结构：线程池中所有的任务*/
	CThread_worker *queue_head;
	
	/*是否销毁线程池*/
	int shutdown;
	pthread_t *threadid;

	/*线程池中允许的活动线程数目*/
	int max_thread_num;

	/*当前等待队列的任务数目*/
	int cur_queue_size;
}CThread_pool;

//share resource
static CThread_pool *pool=NULL;

void *thread_routine(void *arg);
int pool_add_worker(void *(*process)(void *arg),void *arg);
int pool_destroy();
void *myprocess(void *arg);

void pool_init(int max_thread_num)
{
	pool=(CThread_pool *)malloc(sizeof(CThread_pool));
	
	pthread_mutex_init(&(pool->queue_lock),NULL);
	pthread_cond_init(&(pool->queue_ready),NULL);

	pool->queue_head=NULL;
	
	pool->max_thread_num=max_thread_num;
	pool->cur_queue_size=0;

	pool->shutdown=0;

	pool->threadid=(pthread_t *)malloc(max_thread_num*sizeof(pthread_t));
	int i=0;
	for(i=0;i<max_thread_num;i++)
	{
		pthread_create(&(pool->threadid[i]),NULL,thread_routine,NULL);
	}
}

void *myprocess(void *arg)
{
	printf("threadid is 0x%x,working on task %d\n",(int)pthread_self(),*(int *)arg);
	/*休息一秒，延长任务的执行时间*/
	sleep(1); 

	return NULL;
}

void *thread_routine(void *arg)
{
	printf("starting thread 0x%x\n",(int)pthread_self());
	while(1)
	{
		/*如果等待队列为0且不销毁线程，则处于阻塞状态
		pthread_cond_wait()是一个原子操作，等待前会解锁，唤醒后会加锁*/
		pthread_mutex_lock(&(pool->queue_lock));
		while(pool->cur_queue_size==0 && !pool->shutdown)
		{
			printf("thread 0x%x is waiting\n",(int)pthread_self());
			pthread_cond_wait(&(pool->queue_ready),&(pool->queue_lock));
		}
		
		/*线程要销毁*/
		if(pool->shutdown)
		{
			/*遇到break，continue，return等跳转语句，千万不要忘记先解锁*/
			pthread_mutex_unlock(&(pool->queue_lock));
			printf("thread 0x%x will exit\n",(int)pthread_self());
			pthread_exit(NULL);
		}

		printf("thread 0x%x is starting to work\n",(int)pthread_self());
		
		/*assert是调试的好助手*/
		assert(pool->cur_queue_size!=0);
		assert(pool->queue_head!=NULL);
		
		/*等待队列长度减一，并取出链表中的`头元素*/
		pool->cur_queue_size--;
		CThread_worker *worker=pool->queue_head;
		pool->queue_head=worker->next;
		pthread_mutex_unlock(&(pool->queue_lock));

		/*调用回调函数，执行任务*/
		(*(worker->process))(worker->arg);

		free(worker);
		worker=NULL;
	}
	
	/*这句因该是不可达的*/
	pthread_exit(NULL);
}

/*向线程池中添加任务*/
int pool_add_worker(void *(*process)(void *arg),void *arg)
{
	/*构造一个新任务*/
	CThread_worker *newworker=(CThread_worker *)malloc(sizeof(CThread_worker));
	newworker->process=process;
	newworker->arg=arg;
	/*别忘记置空*/
	newworker->next=NULL;

	pthread_mutex_lock(&(pool->queue_lock));
	
	/*将任务加到等待队列中*/
	CThread_worker *member=pool->queue_head;
	if(member!=NULL)
	{
		while(member->next!=NULL)
		{
			member=member->next;
		}
		member->next=newworker;
	}
	else
	{
		pool->queue_head=newworker;
	}

	assert(pool->queue_head!=NULL);

	pool->cur_queue_size++;
	pthread_mutex_unlock(&(pool->queue_lock));
	
	/*等待队列中有任务了，唤醒一个等待线程;注意如果所有的线程都在忙碌，则这句话不起作用*/
	pthread_cond_signal(&(pool->queue_ready));

	return 0;
}

int pool_destroy()
{
	if(pool->shutdown)
	{
		/*防止销毁两次*/
		return -1;
	}
	pool->shutdown=1;
	
	/*唤醒所有等待线程，线程池要销毁了*/
	pthread_cond_broadcast(&(pool->queue_ready));
	
	/*阻塞等待线程退出，否则就成僵尸了*/
	int i;
	for(i=0;i<pool->max_thread_num;i++)
	{
		pthread_join(pool->threadid[i],NULL);
	}
	free(pool->threadid);
	
	/*销毁等待队列*/
	CThread_worker *head=NULL;
	while(pool->queue_head!=NULL)
	{
		head=pool->queue_head;
		pool->queue_head=pool->queue_head->next;
		free(head);
	}
	
	/*销毁互斥锁和条件变量*/
	pthread_mutex_destroy(&(pool->queue_lock));
	pthread_cond_destroy(&(pool->queue_ready));
	
	free(pool);
	/*销毁指针后置空*/
	pool=NULL;
	
	return 0;
}

int main(int argc,char **argv)
{
	/*线程池中最多3个活动线程*/
	pool_init(3);

	int *workingnum=(int *)malloc(sizeof(int)*10);

	/*连续向线程池中投入10个任务*/
	int i;
	for(i=0;i<10;i++)
	{
		workingnum[i]=i;
		pool_add_worker(myprocess,&workingnum[i]);
	}
	
	/*等待所有任务完成*/	
	sleep(5);
	
	/*销毁线程池*/
	pool_destroy();

	free(workingnum);

	return 0;
}
