//
// Created by TaoGe on 2022/5/15.
//
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "server.h"
#include "../utils/helper.h"

int parse_request(int sock_fd, char *method, char *uri) {
    char buff[MAX_REQUEST_LEN] = {0};
    ssize_t len = recv(sock_fd, buff, sizeof(buff), 0);
    if (len <= 0) {
        LOG("call recv error, ret %d", (int) len);
        return -1;
    }

    char *cur = buff;
    int i = 0;
    while (i < MAX_METHOD_LEN && !isspace(*cur)) {
        method[i++] = *cur++;
    }
    method[i] = '\0';

    while (isspace(*cur)) cur++;
    i = 0;
    while (i < MAX_URI_LEN && !isspace(*cur)) {
        uri[i++] = *cur++;
    }
    uri[i] = '\0';
    return 0;
}

void unimplemented(int client) {
    char buff[] =
            "HTTP/1.0 501 Method Not Implemented\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "Method Not Implemented";
    send(client, buff, sizeof(buff), 0);
}

void not_found(int client) {
    char buff[] =
            "HTTP/1.0 404 NOT FOUND\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "The resource specified is unavailable.\r\n";
    send(client, buff, strlen(buff), 0);
}

void url_decode(const char *src, char *dest) {
    const char *p = src;
    char code[3] = {0};
    while (*p && *p != '?') {
        if (*p == '%') {
            memcpy(code, ++p, 2);
            *dest++ = (char) strtoul(code, NULL, 16);
            p += 2;
        } else {
            *dest++ = *p++;
        }
    }
    *dest = '\0';
}

void do_get(int sock_fd, const char *uri) {
    char filename[MAX_URI_LEN] = {0};
    const char *cur = uri + 1;
    size_t len = strlen(cur);
    if (len == 0) {
        strcpy(filename, "index.html");
    } else {
        url_decode(cur, filename);
    }
    printf("%s\n", filename);

    FILE *f = fopen(filename, "r");
    if (NULL == f) {
        not_found(sock_fd);
        return;
    }

    char header[] =
            "HTTP/1.0 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "\r\n";
    send(sock_fd, header, sizeof(header), 0);

    char line[128] = {0};
    while (fgets(line, sizeof(line), f) != NULL) {
        send(sock_fd, line, strlen(line), 0);
        memset(line, 0, sizeof(line));
    }
    char end[] = "\r\n";
    send(sock_fd, end, 2, 0);
    fclose(f);
}

void *process(void *p_sock_fd) {
    int sock_fd = *(int *) p_sock_fd;
    LOG("sock_fd %d", sock_fd);
    char method[MAX_METHOD_LEN] = {0};
    char uri[MAX_URI_LEN] = {0};

    if (parse_request(sock_fd, method, uri) != 0) {
        goto final;
    }
    if (strcmp(method, "GET") == 0) {
        do_get(sock_fd, uri);
    } else {
        unimplemented(sock_fd);
    }

    final:
    LOG("method %s", method);
    LOG("uri %s", uri);
    close(sock_fd);
    return NULL;
}

int create_server(unsigned int port) {
    // 建立服务器端socket
    int server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == -1) {
        EXIT("create socket fail");
    }
    // 设置套接字选项避免地址使用错误
    int opt_val = 1;
    if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val))) < 0) {
        EXIT("setsockopt failed");
    }
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    // 将套接字绑定到服务器的网络地址上
    if (bind(server_fd, (struct sockaddr *) &server, sizeof(server)) == -1) {
        EXIT("bind fail");
    }
    // 建立监听队列
    if (listen(server_fd, 10) == -1) {
        EXIT("listen fail");
    }
    return server_fd;
}

extern int running;

void before_exiting(void) {
    // 函数内声明变量 running 为外部变量
    // 给外部变量（全局变量）赋值
    running = 0;
    LOG("Server exiting");
}