//
// Created by TaoGe on 2022/5/14.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "pool/manager.h"
#include "struct/thread_pool_t.h"
#include "http/server.h"

void *do_work(void *i) {
    sleep(3);
    printf("do_work init %d ... ... \n", *(int *) i);
}


int running = 1;

int main() {
    atexit(before_exiting);
    /* 线程池初始化，其管理者线程及工作线程都会启动 */
    thread_pool_t *p = create_thread_pool(2, 5, 100);

    for (int i = 0; i < 5; ++i) {
        int *arg = &i;
        /* 接收到任务后添加 */
        thread_pool_add_task(p, do_work, arg);
    }

    while (running) {
    }

    // ... ...
    /* 销毁 */
    destroy_thread_pool(p);
    return 0;
}
