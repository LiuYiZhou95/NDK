//
// Created by admin on 2018/9/12.
//

#include "ThreadPool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <linux/time.h>
#include "WlAndroidLog.h"

//创建的线程执行
void *thread_routine(void *arg){
    struct timespec abstime;
    int timeout;
    LOGD("thread %d is starting\n", (int)pthread_self());
    threadpool *pool=(threadpool *)arg;//!!!!!
    while (1){
        timeout=0;
        //访问线程池之前需要加锁
        condition_lock(&pool->ready);
        //空闲
        pool->idle++;
        //等待队列有任务到来或者收到线程池销毁通知
        while(pool->frist==NULL&&!pool->quit){
            //否则线程阻塞等待
            LOGD("thread %d is waiting\n", (int)pthread_self());
            //获取当前时间，并加上等待时间，设置进程的超时睡眠时间
            clock_gettime(CLOCK_REALTIME,&abstime);
            abstime.tv_sec+=2;
            int status;
            status=condition_timedwait(&pool->ready,&abstime);
            if(status==ETIMEDOUT){
                LOGD("thread %d wait timed out\n", (int)pthread_self());
                timeout = 1;
                break;
            }
        }
        pool->idle--;
        if(pool->frist!=NULL){
            //去除等待队列中的第一个任务，移除并执行
            task *t=pool->frist;
            pool->frist=t->next;
            //由于执行任务需要消耗时间，先解锁，让其他线程访问线程池
            condition_unlock(&pool->ready);
            //执行任务
            t->run(t->arg);
            //执行完毕任务释放内存
            free(t);
            //重新加锁
            condition_lock(&pool->ready);
        }
        //退出线程池
        if(pool->quit&&pool->frist==NULL){
            pool->counter--;//当前工作的线程数-1
            //若线程池中没有线程，通知等待线程（主线程）全部任务已经完成
            if(pool->counter==0){
                condition_signal(&pool->ready);

            }
            condition_unlock(&pool->ready);
            break;
        }
        //超时，跳出销毁线程
        if(timeout==1){
            pool->counter--;//当前工作的线程数-1
            condition_unlock(&pool->ready);
            break;
        }
        condition_unlock(&pool->ready);
    }
    LOGD("thread %d is exiting\n", (int)pthread_self());
    return NULL;
}

//线程的初始化
void threadpool_init(threadpool *pool,int threads){
    condition_init(&pool->ready);
    pool->frist=NULL;
    pool->last=NULL;
    pool->counter=0;
    pool->idle=0;
    pool->max_threads=threads;
    pool->quit=0;
}
//往线程池中加入任务
void threadpool_add_task(threadpool *pool,void *(*run)(void *arg), void *arg){
    //产生一个新的任务
    task *next_task=(task *)malloc(sizeof(task));
    next_task->run=run;
    next_task->arg=arg;
    next_task->next=NULL;//新加的任务放在队列尾

    //线程池的状态被多个线程共享，操作前需要加锁
    condition_lock(&pool->ready);

    //第一个任务加入
    if(pool->frist==NULL){
        pool->frist=next_task;
    } else{
        pool->last->next=next_task;
    }
    pool->last=next_task;//队列尾指向新加入的线程

    //线程池中有线程空闲，唤醒
    if(pool->idle>0){
        condition_signal(&pool->ready);
    }
    //当线程池中的线程个数没有达到设定的最大值，创建一个新的线程
    else if(pool->counter<pool->max_threads){
        pthread_t tid;
        pthread_create(&tid,NULL,thread_routine,pool);
        pool->counter++;
    }
    //结束访问
    condition_unlock(&pool->ready);
}
//销毁线程池
void threadpool_destory(threadpool *pool){
    //如果已经调用销毁，直接返回
    if(pool->quit){
        return;
    }
    //加锁
    condition_lock(&pool->ready);
    //设置销毁标记为1
    pool->quit=1;
    //线程池中线程个数大于0
    if(pool->counter>0){
        //对于等等的线程，发送唤醒信号
        if(pool->idle>0){
            condition_broadcast(&pool->ready);
        }
        //正在执行任务的线程，等待任务结束
        while(pool->counter){
            condition_wait(&pool->ready);
        }
    }
    condition_unlock(&pool->ready);
    condition_destroy(&pool->ready);
}