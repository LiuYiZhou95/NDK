#include <jni.h>
#include <string>
#include "pthread.h"
#include "WlAndroidLog.h"
#include "queue"
#include "unistd.h"
#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>

pthread_t pthread;//声明线程对象
void *threadDoThings(void *data){
    LOGD("jni thread do something");
    pthread_exit(&pthread);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lyz_thread_JniThread_normalThread(
        JNIEnv *env,
        jobject /* this */) {
    // TODO
    LOGD("normal thread");
    //创建线程
    pthread_create(&pthread, NULL, threadDoThings, NULL);
}


std::queue<int> queue; //产品队列，里面是int的队列
pthread_t pthread_produce;//生产者线程
pthread_t pthread_customer;//消费者线程
pthread_mutex_t mutex;//线程锁
pthread_cond_t cond;//条件对象

void *produceThread(void *data){
    while (queue.size()<50){
        LOGD("生产者生成一个产品");
        pthread_mutex_lock(&mutex);
        queue.push(1);
        if(queue.size()>0){
            LOGD("生产者通知消费者有产品产生，产品数量为：%d", queue.size());
            pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&mutex);
        sleep(4);
    }
    pthread_exit(&pthread_produce);
}

void *customerThread(void *data){
    char *prod=(char *)data;
    LOGD("%", prod);
    while(1) //这里用死循环，时间情况应该给一个变量来控制跳出循环
    {
        pthread_mutex_lock(&mutex); //操作队列前先加锁
        if(queue.size() > 0)
        {
            queue.pop();
            LOGE("消费者消费一个产品，产品数量为：%d", queue.size());
        } else{
            LOGE("产品消费完了，等待生产者生产......");
            pthread_cond_wait(&cond, &mutex); //阻塞线程等待生产者的通知
        }
        pthread_mutex_unlock(&mutex);//解锁线程
        usleep(500 * 1000); //休息0.5秒，usleep单位是微妙
    }
    pthread_exit(&pthread_customer);
}

void initMutex(){
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);
    pthread_create(&pthread_produce,NULL,produceThread,(void *)"product");
    pthread_create(&pthread_customer,NULL,customerThread,NULL);

}

extern "C"
JNIEXPORT void JNICALL
        Java_com_lyz_thread_JniThread_mutexThread(JNIEnv *env, jobject instance){
    //TODO
    //初始化时，先往队列中添加10个产品
    for(int i=0;i<10;i++){
        queue.push(i);
    }
    initMutex();
}


void* mytask(void *arg)
{
    LOGD("thread %d is working on task %d\n", (int)pthread_self(), *(int*)arg);
    sleep(1);
    free(arg);
    return NULL;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_lyz_thread_JniThread_threadPool(JNIEnv *env, jobject instance){
    //TODO
    threadpool pool;
    //初始化线程池，最多三个线程
    threadpool_init(&pool, 3);
    int i;
    //创建十个任务
    for(i=0; i < 10; i++)
    {
        int *arg =(int *)malloc(sizeof(int));
        *arg = i;
        threadpool_add_task(&pool, mytask, arg);

    }
    threadpool_destory(&pool);
}
