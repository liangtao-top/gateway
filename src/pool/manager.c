//
// Created by TaoGe on 2022/5/15.
//
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include "manager.h"
#include "../utils/helper.h"
#include "../struct/thread_pool_t.h"

// 管理间隔
#define DEFAULT_TIME 3
// 最小正在等待的任务数量，排队数量超过则创建新线程
#define MIN_WAIT_TASK_NUM 1
// Manager线程动态创建、销毁的线程基数，也就是1次最低创建多少线程或者销毁多少线程
#define DEFAULT_THREAD_NUM 1

/**
 * 工作线程
 * @param thread_pool 线程池结构体指针
 * @return
 */
void *worker_thread(void *thread_pool) {
    thread_pool_t *pool = (thread_pool_t *) thread_pool;
    while (true) {
        pthread_mutex_lock(&(pool->lock));
        /* 无任务则阻塞在 “任务队列不为空” 上，有任务则跳出 */
        while ((pool->queue_size == 0) && (!pool->shutdown)) {
            LOG("worker 线程 0x%x 睡眠，等待唤醒", (unsigned int) pthread_self());
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock));
            /* 判断是否需要清除线程,自杀功能 */
            if (pool->wait_exit_thr_num > 0) {
                pool->wait_exit_thr_num--;
                /* 判断线程池中的线程数是否大于最小线程数，是则结束当前线程 */
                if (pool->live_thr_num > pool->min_thr_num) {
                    LOG("worker 线程 0x%x 闲置，且存活线程数 %d > 最小线程数 %d，自我销毁", (unsigned int) pthread_self(),
                        pool->live_thr_num, pool->min_thr_num);
                    pool->live_thr_num--;
                    pthread_mutex_unlock(&(pool->lock));
                    pthread_exit(NULL);//结束线程
                }
            }
        }
        /* 线程池开关状态 */
        if (pool->shutdown) {
            // 关闭线程池
            pthread_mutex_unlock(&(pool->lock));
            LOG("线程池已关闭，worker 线程 0x%x 自我销毁", (unsigned int) pthread_self());
            pthread_exit(NULL); //线程自己结束自己
        }
        thread_pool_task_t task;
        // 否则该线程可以拿出任务
        task.function = pool->task_queue[pool->queue_front].function; // 出队操作
        task.arg = pool->task_queue[pool->queue_front].arg;
        pool->queue_front = (pool->queue_front + 1) % pool->queue_max_size; // 环型结构
        pool->queue_size--;
        // 通知可以添加新任务
        pthread_cond_broadcast(&(pool->queue_not_full));
        // 释放线程锁
        pthread_mutex_unlock(&(pool->lock));
        // 执行刚才取出的任务
        LOG("worker 线程 0x%x 开始工作", (unsigned int) pthread_self());
        pthread_mutex_lock(&(pool->thread_counter)); // 锁住忙线程变量
        pool->busy_thr_num++;
        pthread_mutex_unlock(&(pool->thread_counter));
        (*(task.function))(task.arg); // 执行任务
        // 任务结束处理
        LOG("worker 线程 0x%x 工作结束", (unsigned int) pthread_self());
        pthread_mutex_lock(&(pool->thread_counter));
        pool->busy_thr_num--;
        pthread_mutex_unlock(&(pool->thread_counter));
    }
    pthread_exit(NULL);
}

/**
 * 线程是否存活
 * @param tid 线程ID
 * @return
 */
bool is_thread_alive(pthread_t tid) {
    int kill_rc = pthread_kill(tid, 0); // 发送0号信号，测试是否存活
    if (kill_rc == ESRCH) { // 线程不存在
        return false;
    }
    return true;
}

/**
 * 管理线程
 * @param thread_pool 线程池结构体指针
 * @return
 */
void *manager_thread(void *thread_pool) {
    int i;
    thread_pool_t *pool = (thread_pool_t *) thread_pool;
    while (!pool->shutdown) {
        sleep(DEFAULT_TIME); /*隔一段时间再管理*/
//        LOG("manager 线程定时巡查，间隔 %d 秒", DEFAULT_TIME);
        pthread_mutex_lock(&(pool->lock)); /*加锁*/
        int queue_size = pool->queue_size; /*任务数*/
        int live_thr_num = pool->live_thr_num; /*存活的线程数*/
        pthread_mutex_unlock(&(pool->lock)); /*解锁*/
        pthread_mutex_lock(&(pool->thread_counter));
        int busy_thr_num = pool->busy_thr_num; /*正在工作线程数*/
        pthread_mutex_unlock(&(pool->thread_counter));
        LOG("manager 线程巡查（间隔 %d 秒），当前工作线程数 %d 存活线程数 %d", DEFAULT_TIME, busy_thr_num, live_thr_num);
        /*创建新线程 实际任务数量大于 最小正在等待的任务数量，存活线程数小于最大线程数*/
        if (queue_size >= MIN_WAIT_TASK_NUM && live_thr_num <= pool->max_thr_num) {
            LOG("排队任务数 %d >= 最小正在等待任务数 %d and 存活线程数 %d <= 最大线程数 %d，自动创建线程", queue_size, MIN_WAIT_TASK_NUM, live_thr_num,
                pool->max_thr_num);
            pthread_mutex_lock(&(pool->lock));
            int add = 0;
            /*一次增加 DEFAULT_THREAD_NUM 个线程*/
            for (i = 0;
                 i < pool->max_thr_num &&
                 add < DEFAULT_THREAD_NUM &&
                 pool->live_thr_num < pool->max_thr_num;
                 i++) {
                if (pool->threads[i] == 0 || !is_thread_alive(pool->threads[i])) {
                    pthread_create(&(pool->threads[i]), NULL, worker_thread, (void *) pool);
                    add++;
                    pool->live_thr_num++;
                    LOG("manager 动态启动 worker-%d 线程 0x%x", i + 1, (unsigned int) pool->threads[i]);
                }
            }
            pthread_mutex_unlock(&(pool->lock));
        }
        /*销毁多余的线程 忙线程x2 都小于 存活线程，并且存活的大于最小线程数*/
        if ((busy_thr_num * 2) < live_thr_num && live_thr_num > pool->min_thr_num) {
            // printf("admin busy --%d--%d----\n", busy_thr_num, live_thr_num);
            /*一次销毁DEFAULT_THREAD_NUM个线程*/
            pthread_mutex_lock(&(pool->lock));
            pool->wait_exit_thr_num = DEFAULT_THREAD_NUM;
            pthread_mutex_unlock(&(pool->lock));
            for (i = 0; i < DEFAULT_THREAD_NUM; i++) {
                // 通知正在处于空闲的线程，自杀
                pthread_cond_signal(&(pool->queue_not_empty));
                LOG("唤醒正在处于空闲的线程，进行自我销毁");
            }
        }
    }
    return NULL;
}

/**
 * 释放工作线程池和任务队列
 * @param pool 线程池结构体指针
 * @return
 */
int free_worker_thread_pool_and_task_queue(thread_pool_t *pool) {
    if (pool == NULL)
        return -1;
    if (pool->task_queue)
        free(pool->task_queue);
    if (pool->threads) {
        free(pool->threads);
        pthread_mutex_lock(&(pool->lock)); /*先锁住再销毁*/
        pthread_mutex_destroy(&(pool->lock));
        pthread_mutex_lock(&(pool->thread_counter));
        pthread_mutex_destroy(&(pool->thread_counter));
        pthread_cond_destroy(&(pool->queue_not_empty));
        pthread_cond_destroy(&(pool->queue_not_full));
    }
    free(pool);
    pool = NULL;
    return 0;
}

/**
 * 销毁线程池
 * @param pool 线程池结构体指针
 * @return
 */
int destroy_thread_pool(thread_pool_t *pool) {
    int i;
    if (pool == NULL) {
        return -1;
    }
    pool->shutdown = true;
    /*销毁管理者线程*/
    pthread_join(pool->admin_tid, NULL);
    //通知所有线程去自杀(在自己领任务的过程中)
    for (i = 0; i < pool->live_thr_num; i++) {
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }
    /*等待线程结束 先是pthread_exit 然后等待其结束*/
    for (i = 0; i < pool->live_thr_num; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    free_worker_thread_pool_and_task_queue(pool);
    return 0;
}

/**
 * 创建线程池
 * @param min_thr_num  最小线程数
 * @param max_thr_num  最大线程数
 * @param queue_max_size  最大任务数
 * @return
 */
thread_pool_t *create_thread_pool(int min_thr_num, int max_thr_num, int queue_max_size) {
    int i;
    thread_pool_t *pool = NULL;
    do {
        /* 线程池空间开辟 */
        if ((pool = (thread_pool_t *) malloc(sizeof(thread_pool_t))) == NULL) {
            LOG("线程池空间分配内存失败");
            break;
        } else {
            LOG("线程池空间分配内存，地址 %p 大小 %ld 字节", pool, sizeof(thread_pool_t));
        }
        /*线程池信息初始化*/
        pool->min_thr_num = min_thr_num;/* 线程池中最小线程数 */
        pool->max_thr_num = max_thr_num;/* 线程池中最大线程数 */
        pool->busy_thr_num = 0;/* 正在工作的线程数 */
        pool->live_thr_num = min_thr_num;/* 线程池中存活的线程数 */
        pool->wait_exit_thr_num = 0;/* 需要销毁的线程数 */
        /*任务队列信息*/
        pool->queue_front = 0; /* 队头 */
        pool->queue_rear = 0;/* 队尾 */
        pool->queue_size = 0;
        /* 存在的任务数 */
        pool->queue_max_size = queue_max_size;/* 队列能容纳的最大任务数 */
        /*线程池状态*/
        pool->shutdown = false;/* true为关闭 */
        /* 根据最大线程数，给工作线程数组开空间，清0 */
        pool->threads = (pthread_t *) malloc(sizeof(pthread_t) * max_thr_num);/* 存放线程的tid,实际上就是管理了线 数组 */
        if (pool->threads == NULL) {
            LOG("工作线程ID池空间分配内存失败");
            break;
        } else {
            LOG("工作线程ID池空间分配内存，地址 %p 大小 %ld 字节，池子长度 %d", pool->threads, sizeof(pthread_t) * max_thr_num, max_thr_num);
        }
        memset(pool->threads, 0, sizeof(pthread_t) * max_thr_num);
        /* 队列开空间 */
        pool->task_queue = (thread_pool_task_t *) malloc(sizeof(thread_pool_task_t) * queue_max_size);/* 任务队列 */
        if (pool->task_queue == NULL) {
            LOG("任务队列池空间分配内存失败");
            break;
        } else {
            LOG("任务队列池空间分配内存，地址 %p 大小 %ld 字节，队列长度 %d", pool->task_queue, sizeof(thread_pool_task_t) * queue_max_size,
                queue_max_size);
        }
        /* 初始化互斥锁和条件变量 */
        if (pthread_mutex_init(&(pool->lock), NULL) != 0 ||
            pthread_mutex_init(&(pool->thread_counter), NULL) != 0 ||
            pthread_cond_init(&(pool->queue_not_empty), NULL) != 0 ||
            pthread_cond_init(&(pool->queue_not_full), NULL) != 0) {
            LOG("初始化互斥锁和条件变量失败");
            break;
        }
        /* 启动min_thr_num个工作线程 */
        for (i = 0; i < min_thr_num; i++) {
            /* pool指向当前线程池 thread_pool_thread函数在后面讲解 */
            pthread_create(&(pool->threads[i]), NULL, worker_thread, (void *) pool);
            LOG("启动 worker-%d 线程 0x%x", i + 1, (unsigned int) pool->threads[i]);
        }
        /* 管理者线程 admin_thread函数在后面讲解 */
        pthread_create(&(pool->admin_tid), NULL, manager_thread, (void *) pool);
        LOG("启动 manager 线程 0x%x", (unsigned int) pool->admin_tid);
        return pool;
    } while (0);
    /* 释放pool的空间 */
    free_worker_thread_pool_and_task_queue(pool);
    return NULL;
}

/**
 * 向线程池的任务队列中添加一个任务
 * @param pool 线程池结构体指针
 * @param function 任务函数指针
 * @param arg
 * @return
 */
int thread_pool_add_task(thread_pool_t *pool, void *(*function)(void *arg), void *arg) {
    pthread_mutex_lock(&(pool->lock));
    /*如果队列满了,调用wait阻塞*/
    while ((pool->queue_size == pool->queue_max_size) && (!pool->shutdown))
        pthread_cond_wait(&(pool->queue_not_full), &(pool->lock));
    /*如果线程池处于关闭状态*/
    if (pool->shutdown) {
        pthread_mutex_unlock(&(pool->lock));
        return -1;
    }
    /*清空工作线程的回调函数的参数arg*/
    if (pool->task_queue[pool->queue_rear].arg != NULL) {
        free(pool->task_queue[pool->queue_rear].arg);
        pool->task_queue[pool->queue_rear].arg = NULL;
    }
    /*添加任务到任务队列*/
    pool->task_queue[pool->queue_rear].function = function;
    pool->task_queue[pool->queue_rear].arg = arg;
    pool->queue_rear = (pool->queue_rear + 1) % pool->queue_max_size; /* 逻辑环 */
    pool->queue_size++;
    /*添加完任务后,队列就不为空了,唤醒线程池中的一个线程*/
    pthread_cond_signal(&(pool->queue_not_empty));
    pthread_mutex_unlock(&(pool->lock));
    return 0;
}

