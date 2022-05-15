//
// Created by TaoGe on 2022/5/15.
//

#ifndef GW_SERVER_H
#define GW_SERVER_H

#endif //GW_SERVER_H


#define MAX_REQUEST_LEN 10240
#define MAX_METHOD_LEN  32
#define MAX_URI_LEN     256


int parse_request(int sock_fd, char *method, char *uri);

void unimplemented(int client);

void not_found(int client);

void url_decode(const char *src, char *dest);

void do_get(int sock_fd, const char *uri);

void *process(void *p_sock_fd);

int create_server(unsigned int port);

void before_exiting(void);