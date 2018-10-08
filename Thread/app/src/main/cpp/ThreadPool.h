//
// Created by admin on 2018/9/12.
//

#ifndef THREAD_THREADPOOL_H
#define THREAD_THREADPOOL_H
//线程池头文件
#include "Condition.h"
//封装线程池中的对象需要执行的任务对象
 struct task{
    void *(*run) (void *args);//函数指针，需要执行的任务
    void *arg;                //参数
    struct task *next;        //任务队列中的下一个任务
};

//线程池结构体
typedef struct _threadpool{
    condition ready;           //状态量
    task *frist;                //任务队列第一个任务
    task *last;                 //任务队列最后一个任务
    int counter;                 //线程池中已有线程数
    int idle;                   //空线程数
    int max_threads;            //最大线程数
    int quit;                   //是否退出标志
}threadpool;

//线程的初始化
void threadpool_init(threadpool *pool,int threads);
//往线程池中加入任务
void threadpool_add_task(threadpool *pool,void *(*run)(void *arg), void *arg);
//销毁线程池
void threadpool_destory(threadpool *pool);

#endif //THREAD_THREADPOOL_H