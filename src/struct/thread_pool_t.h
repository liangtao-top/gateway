//
// Created by TaoGe on 2022/5/15.
//
#include <pthread.h>
#include "thread_pool_task_t.h"

#ifndef GW_THREAD_POOL_T_H
#define GW_THREAD_POOL_T_H
// 线程池结构体
typedef struct {
    pthread_mutex_t lock; /* 线程池结构体的锁 */
    pthread_mutex_t thread_counter; /* 正在工作的线程数的锁 */

    pthread_cond_t queue_not_full; /* 条件变量，任务队列不为满 */
    pthread_cond_t queue_not_empty; /* 任务队列不为空 */

    pthread_t *threads; /* 存放线程的tid,实际上就是管理了线 数组 */
    pthread_t admin_tid; /* 管理者线程tid */
    thread_pool_task_t *task_queue; /* 任务队列 */

    /*线程池信息*/
    int min_thr_num; /* 线程池中最小线程数 */
    int max_thr_num; /* 线程池中最大线程数 */
    int live_thr_num; /* 线程池中存活的线程数 */
    int busy_thr_num; /* 正在工作的线程数 */
    int wait_exit_thr_num; /* 需要销毁的线程数 */
    /*任务队列信息*/
    int queue_front; /* 队头 */
    int queue_rear; /* 队尾 */
    int queue_size;
    /* 存在的任务数 */
    int queue_max_size; /* 队列能容纳的最大任务数 */
    /*线程池状态*/
    int shutdown; /* true为关闭 */
} thread_pool_t;
#endif //GW_THREAD_POOL_T_H
