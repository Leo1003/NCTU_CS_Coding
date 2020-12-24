#include <arpa/inet.h>
#include <endian.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define EVCNT 4
#define BUFFER_SIZE 4096
#define DELIM " \t\r\n"

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a < b ? b : a)

int user_cnt = 0;

struct sess_ctx {
    int user_id;
    struct sockaddr_in addr;
    char *buffer;
    size_t buffer_pos;
};
struct sess_ctx session_table[256];

void usage(const char *exename)
{
    eprintf("Usage: %s <port>\n", exename);
}

int command(int sockfd, const char *arg, struct sess_ctx *ctx)
{
    eprintf("Command: \"%s\"\n", arg);
    char sendbuf[BUFFER_SIZE];

    if (!strcmp(arg, "list-users")) {
        for (int i = 0; i < 256; i++) {
            char *ptr = sendbuf;
            if (session_table[i].user_id != 0) {
                int l = sprintf(ptr, "user%d\n", session_table[i].user_id);
                ptr += l;
            }
        }
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
    } else if (!strcmp(arg, "get-ip")) {
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ctx->addr.sin_addr, ipstr, INET_ADDRSTRLEN);
        sprintf(sendbuf, "IP: %s:%hu\n", ipstr, ntohs(ctx->addr.sin_port));
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
    } else if (!strcmp(arg, "exit")) {
        sprintf(sendbuf, "Bye, user%d.\n", ctx->user_id);
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
        shutdown(sockfd, SHUT_RDWR);
        free(ctx->buffer);
        ctx->buffer = NULL;
        close(sockfd);
    }

    return 0;
}

int server_loop(int tcpsock)
{
    int epollfd;
    struct epoll_event ev, evs[EVCNT];
    memset(&ev, 0, sizeof(struct epoll_event));

    ev.events = EPOLLIN;
    ev.data.fd = tcpsock;
    epollfd = epoll_create(64);
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, tcpsock, &ev) < 0) {
        eprintf("Failed to epoll_ctl(EPOLL_CTL_ADD)\n");
        return -1;
    }

    while (1) {
        int nfds = epoll_wait(epollfd, evs, EVCNT, -1);
        if (nfds == -1) {
            if (errno == EINTR) {
                continue;
            }
            eprintf("Failed to epoll on socket\n");
            return -1;
        }

        for (int i = 0; i < nfds; i++) {
            if (evs[i].data.fd == tcpsock) {
                struct sockaddr_in peeraddr;
                socklen_t addrlen = sizeof(peeraddr);
                int conn_sock = accept(tcpsock, (struct sockaddr *) &peeraddr, &addrlen);
                if (conn_sock == -1) {
                    eprintf("Failed to accept()");
                    return -1;
                }

                int user_id = ++user_cnt;

                char ipstr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &peeraddr.sin_addr, ipstr, INET_ADDRSTRLEN);
                printf("New connection from %s:%hu user%d\n", ipstr, ntohs(peeraddr.sin_port), user_id);

                session_table[conn_sock].user_id = user_id;
                session_table[conn_sock].addr = peeraddr;
                session_table[conn_sock].buffer_pos = 0;
                session_table[conn_sock].buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
                if (session_table[conn_sock].buffer == NULL) {
                    eprintf("Buffer malloc error\n");
                    close(conn_sock);
                    continue;
                }

                ev.events = EPOLLIN | EPOLLHUP;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    eprintf("Failed to epoll on socket\n");
                    return -1;
                }
            } else {
                if (evs[i].events & EPOLLHUP) {
                    struct sess_ctx *ctx = &session_table[evs[i].data.fd];
                    char ipstr[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &ctx->addr.sin_addr, ipstr, INET_ADDRSTRLEN);
                    printf("user%d %s:%hu disconnected\n", ctx->user_id, ipstr, ntohs(ctx->addr.sin_port));
                    ctx->user_id = 0;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, evs[i].data.fd, NULL);
                } else if (evs[i].events & EPOLLIN) {
                    int sockfd = evs[i].data.fd;
                    struct sess_ctx *ctx = &session_table[sockfd];

                    ssize_t recvlen = recv(sockfd, &ctx->buffer[ctx->buffer_pos], sizeof(char) * BUFFER_SIZE - ctx->buffer_pos, 0);
                    if (recvlen < 0) {
                        eprintf("Failed to recv()");
                        continue;
                    }
                    ssize_t end_pos = ctx->buffer_pos + recvlen;
                    for (ssize_t p = ctx->buffer_pos; p < end_pos; p++) {
                        if (ctx->buffer[p] == '\n') {
                            ctx->buffer[p] = '\0';

                            char *arg = strtok(ctx->buffer, DELIM);
                            if (!arg) {
                                continue;
                            }

                            command(sockfd, arg, ctx);

                            // Move buffer & Recalculate p
                            size_t rmlen = p + 1;
                            memmove(ctx->buffer, ctx->buffer + rmlen, sizeof(char) * (end_pos - rmlen));
                            p -= rmlen;
                            end_pos -= rmlen;
                        }
                    }
                    ctx->buffer_pos = end_pos;

                    if (ctx->buffer) {
                        ctx->buffer[ctx->buffer_pos] = '\0';
                        eprintf("Buffer: %s\n", ctx->buffer);
                    }
                }
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv[0]);
        exit(1);
    }

    unsigned short port = atoi(argv[1]);
    if (port == 0) {
        eprintf("Invalid port number\n");
        exit(1);
    }

    struct sockaddr_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_port = htons(port);
    bindaddr.sin_addr.s_addr = INADDR_ANY;
    int tcpsock = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(tcpsock, (struct sockaddr *)&bindaddr, sizeof(bindaddr))) {
        eprintf("Failed to bind on port %hu\n", port);
        exit(1);
    }
    if (listen(tcpsock, 64)) {
        eprintf("Failed to listen on port %hu\n", port);
        exit(1);
    }

    printf("Listening on TCP port %hu...\n", port);

    return server_loop(tcpsock);
}
