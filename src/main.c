//
// Created by TaoGe on 2022/5/14.
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "http/server.h"
#include "utils/helper.h"
#include "pool/manager.h"

int running = 1;

void *do_work2(void *i) {
    sleep(1);
    printf("do_work init %d ... ... \n", *(int *) i);
}


int main(int argc, char *argv[]) {
    atexit(before_exiting);
    unsigned int port = 8080;
    if (argc > 1) {
        char *ptr;
        port = strtol(argv[1], &ptr, 10);
        if (NULL != ptr) {
            free(ptr);
            //避免指针问题（野指针）
            ptr = NULL;
        }
    }

    int server_fd, conn_fd;
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);

    server_fd = create_server(port);
    LOG("Server started server_fd %d, listen port %d", server_fd, port);
    LOG("gateway pid %d", getppid());
    LOG("system cpu num is %ld, enable num is %ld", sysconf(_SC_NPROCESSORS_CONF), sysconf(_SC_NPROCESSORS_ONLN));

    /* 线程池初始化，其管理者线程及工作线程都会启动 */
    thread_pool_t *p = create_thread_pool(2, 5, 100);

    while (running) {
        // 等待客户端连接请求到达
        conn_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);
        LOG("conn_fd %d", conn_fd);
        if (conn_fd < 0) {
            EXIT("accept client socket failed");
        }

        pthread_t tid;
        int res = pthread_create(&tid, NULL, process, &conn_fd);
        if (res == 0) {
            LOG("thread_id 0x%x", (unsigned int) tid);
            unsigned char *ip = (unsigned char *) &client.sin_addr.s_addr;
            unsigned short sinPort = client.sin_port;
            LOG("request %u.%u.%u.%u:%5u", ip[0], ip[1], ip[2], ip[3], sinPort);
        } else {
            EXIT("create thread fail");
        }

//        /* 接收到任务后添加 */
//        int res = thread_pool_add_task(p, do_work2, &conn_fd);
//        if (res == 0) {
//            unsigned char *ip = (unsigned char *) &client.sin_addr.s_addr;
//            unsigned short sinPort = client.sin_port;
//            LOG("request %u.%u.%u.%u:%5u", ip[0], ip[1], ip[2], ip[3], sinPort);
//        } else {
//            EXIT("add task fail");
//        }
    }
    /* 销毁线程池 */
    destroy_thread_pool(p);
    return 0;
}