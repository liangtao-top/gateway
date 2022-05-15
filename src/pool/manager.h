//
// Created by TaoGe on 2022/5/15.
//
#include "../struct/thread_pool_t.h"

#ifndef GW_MANAGER_H
#define GW_MANAGER_H

#endif //GW_MANAGER_H

/**
 * 创建线程池
 * @param min_thr_num  最小线程数
 * @param max_thr_num  最大线程数
 * @param queue_max_size  最大任务数
 * @return
 */
thread_pool_t *create_thread_pool(int min_thr_num, int max_thr_num, int queue_max_size);

/**
 * 向线程池的任务队列中添加一个任务
 * @param pool 线程池结构体指针
 * @param function 任务函数指针
 * @param arg
 * @return
 */
int thread_pool_add_task(thread_pool_t *pool, void *(*function)(void *arg), void *arg);

/**
 * 销毁线程池
 * @param pool 线程池结构体指针
 * @return
 */
int destroy_thread_pool(thread_pool_t *pool);