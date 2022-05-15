//
// Created by TaoGe on 2022/5/15.
//

#ifndef GW_THREAD_POOL_TASK_T_H
#define GW_THREAD_POOL_TASK_T_H
// 线程池任务结构体
typedef struct {
    void *(*function)(void *);

    void *arg;
} thread_pool_task_t;
#endif //GW_THREAD_POOL_TASK_T_H
