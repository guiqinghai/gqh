#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>


struct job
{
	void *(*callback_function)(void *arg);
	void *arg;
	struct job *next;
};

struct threadpool
{
	int thread_num;
	int queue_max_num;
	struct job *head;
	struct job *tail;
	pthread_t *pthreads;
	pthread_mutex_t mutex;
	pthread_cond_t queue_empty;	//队列为空的条件变量
	pthread_cond_t queue_not_empty; //队列不为空的条件变量
	pthread_cond_t queue_not_full;  //队列不为满的条件变量
	int queue_cur_num;
	int queue_close;		//队列是否已关闭
	int pool_close;			//线程池是否已关闭
};

struct threadpool *threadpool_init(int thread_num,int queue_max_num);
int threadpool_add_job(struct threadpool *pool,void *(*callback_function)(void *arg),void *arg);
int threadpool_destroy(struct threadpool *pool);
void *threadpool_function(void *arg);

#endif
