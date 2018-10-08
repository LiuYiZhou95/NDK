//
// Created by admin on 2018/9/11.
//

#ifndef THREAD_CONDITION_H
#define THREAD_CONDITION_H

#include "pthread.h"

//封装一个互斥量和条件变量作为状态
 struct condition{
    pthread_mutex_t pmutex;
    pthread_cond_t pcond;
};

class Condition {

};
//对状态操作的封装
/**
 * 线程的几个状态（1.初始化2.加锁3.解锁4.等待5.时间等待6.信号7.广播8.销毁）
 */
int condition_init(condition *cond);
int condition_lock(condition *cond);
int condition_unlock(condition *cond);
int condition_wait(condition *cond);
int condition_timedwait(condition *cond, const struct timespec *abstime);
int condition_signal(condition* cond);
int condition_broadcast(condition *cond);
int condition_destroy(condition *cond);


#endif //THREAD_CONDITION_H
