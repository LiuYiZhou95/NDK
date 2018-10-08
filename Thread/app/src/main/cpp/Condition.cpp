//
// Created by admin on 2018/9/11.
//

#include "Condition.h"
//初始化
int condition_init(condition *cond) {

    int status;
    if((status=pthread_mutex_init(&((*cond).pmutex),NULL)))
        return status;
    if((status = pthread_cond_init(&cond->pcond, NULL)))
        return status;
    return 0;
}
/**
 * 加锁
 * @param cond
 * @return
 */
int condition_lock(condition *cond){
    return pthread_mutex_lock(&cond->pmutex);
}
/**
 * 解锁
 * @param cond
 * @return
 */
int condition_unlock(condition *cond){
    return pthread_mutex_unlock(&cond->pmutex);
}
/**
 * 等待
 * @param cond
 * @return
 */
int condition_wait(condition *cond){
    return pthread_cond_wait(&cond->pcond,&cond->pmutex);
}
/**
 * 固定时间等待
 * @param cond
 * @param abstime
 * @return
 */
int condition_timedwait(condition *cond, const struct timespec *abstime){
    return pthread_cond_timedwait(&cond->pcond,&cond->pmutex,abstime);
}
/**
 *
 * @param cond
 * @return
 */
int condition_signal(condition* cond){
    return pthread_cond_signal(&cond->pcond);
}
/**
 *
 * @param cond
 * @return
 */
int condition_broadcast(condition *cond){
    return pthread_cond_broadcast(&cond->pcond);
}
/**
 *
 * @param cond
 * @return
 */
int condition_destroy(condition *cond){
    int status;
    if((status=pthread_mutex_destroy(&cond->pmutex))){
        return status;
    }
    if((status=pthread_cond_destroy(&cond->pcond))){
        return status;
    }
}
