#include <arpa/inet.h>
#include <endian.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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

static bool server_run_signal = true;

struct sess_ctx {
    char *user_id;
    struct sockaddr_in addr;
    char *buffer;
    size_t buffer_pos;
};
struct sess_ctx session_table[256];

void usage(const char *exename)
{
    eprintf("Usage: %s <port>\n", exename);
}

static void handle_shutdown_signal(int _sig)
{
    eprintf("Shutdown server...\n");
    server_run_signal = false;
}

static int user_sort_cmp(const void *a, const void *b)
{
    const struct sess_ctx *lhs = (const struct sess_ctx *)a;
    const struct sess_ctx *rhs = (const struct sess_ctx *)b;
    return strcmp(lhs->user_id, rhs->user_id);
}

int handle_command(int sockfd, const char *arg, struct sess_ctx *ctx)
{
    //eprintf("Command: \"%s\"\n", arg);
    char sendbuf[BUFFER_SIZE];

    if (!strcmp(arg, "list-users")) {
        char *ptr = sendbuf;
        for (int i = 0; i < 256; i++) {
            if (session_table[i].user_id != NULL) {
                char ipstr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &session_table[i].addr.sin_addr, ipstr, INET_ADDRSTRLEN);

                int l = sprintf(ptr, "%s %s:%hu\n", session_table[i].user_id, ipstr, ntohs(session_table[i].addr.sin_port));
                ptr += l;
            }
        }
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
    } else if (!strcmp(arg, "sort-users")) {
        struct sess_ctx sort_buf[256];
        size_t user_cnt = 0;

        for (int i = 0; i < 256; i++) {
            if (session_table[i].user_id != NULL) {
                sort_buf[user_cnt] = session_table[i];
                user_cnt++;
            }
        }
        qsort(sort_buf, user_cnt, sizeof(struct sess_ctx), user_sort_cmp);

        char *ptr = sendbuf;
        for (int i = 0; i < user_cnt; i++) {
            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &sort_buf[i].addr.sin_addr, ipstr, INET_ADDRSTRLEN);

            int l = sprintf(ptr, "%s %s:%hu\n", sort_buf[i].user_id, ipstr, ntohs(sort_buf[i].addr.sin_port));
            ptr += l;
        }
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
    } else if (!strcmp(arg, "get-ip")) {
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ctx->addr.sin_addr, ipstr, INET_ADDRSTRLEN);
        sprintf(sendbuf, "IP: %s:%hu\n", ipstr, ntohs(ctx->addr.sin_port));
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
    } else if (!strcmp(arg, "exit")) {
        sprintf(sendbuf, "Bye, %s.\n", ctx->user_id);
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
        shutdown(sockfd, SHUT_RDWR);
    } else {
        sprintf(sendbuf, "Unknown command\n");
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
    }

    return 0;
}

int handle_setname(int sockfd, const char *name, struct sess_ctx *ctx)
{
    char sendbuf[BUFFER_SIZE];

    size_t namelen = strlen(ctx->buffer);
    if (namelen == 0) {
        return 0;
    }

    // Check if name being used
    bool name_used = false;
    for (int i = 0; i < 256; i++) {
        if (session_table[i].user_id != NULL) {
            if (!strcmp(session_table[i].user_id, name)) {
                name_used = true;
                break;
            }
        }
    }
    if (name_used) {
        sprintf(sendbuf, "The username is already used!\n");
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
        return 0;
    }

    ctx->user_id = (char *)malloc(sizeof(char) * (namelen + 1));
    if (ctx->user_id == NULL) {
        // malloc error
        return -1;
    }
    strcpy(ctx->user_id, ctx->buffer);

    sprintf(sendbuf, "Welcome, %s.\n", ctx->user_id);
    send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
    return 0;
}

void handle_data(int sockfd)
{
    char sendbuf[BUFFER_SIZE];
    struct sess_ctx *ctx = &session_table[sockfd];

    ssize_t recvlen = recv(sockfd, &ctx->buffer[ctx->buffer_pos], sizeof(char) * BUFFER_SIZE - ctx->buffer_pos, 0);
    if (recvlen < 0) {
        eprintf("Failed to recv()");
        return;
    }
    ssize_t end_pos = ctx->buffer_pos + recvlen;
    for (ssize_t p = ctx->buffer_pos; p < end_pos; p++) {
        if (ctx->buffer[p] == '\n') {
            ctx->buffer[p] = '\0';

            if (ctx->user_id == NULL) {
                // Not set username yet
                char *arg = strtok(ctx->buffer, "\r\n");
                if (arg) {
                    handle_setname(sockfd, ctx->buffer, ctx);
                }
            } else {
                char *arg = strtok(ctx->buffer, DELIM);
                if (arg) {
                    handle_command(sockfd, arg, ctx);
                }
            }

            // Move buffer & Recalculate p
            size_t rmlen = p + 1;
            memmove(ctx->buffer, ctx->buffer + rmlen, sizeof(char) * (end_pos - rmlen));
            p -= rmlen;
            end_pos -= rmlen;

            // Command line prompt
            sprintf(sendbuf, "%% ");
            send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
        }
    }
    ctx->buffer_pos = end_pos;

    /*
    if (ctx->buffer) {
        ctx->buffer[ctx->buffer_pos] = '\0';
        eprintf("Buffer: %s\n", ctx->buffer);
    }
    */
}

void handle_disconnect(int epollfd, int sockfd)
{
    struct sess_ctx *ctx = &session_table[sockfd];
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ctx->addr.sin_addr, ipstr, INET_ADDRSTRLEN);
    printf("%s %s:%hu disconnected\n", ctx->user_id, ipstr, ntohs(ctx->addr.sin_port));

    free(ctx->buffer);
    ctx->buffer = NULL;
    free(ctx->user_id);
    ctx->user_id = NULL;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, NULL);
    close(sockfd);
}

void handle_connect(int epollfd, int tcpsock)
{
    char sendbuf[BUFFER_SIZE];
    struct sockaddr_in peeraddr;
    socklen_t addrlen = sizeof(peeraddr);

    int conn_sock = accept(tcpsock, (struct sockaddr *) &peeraddr, &addrlen);
    if (conn_sock == -1) {
        eprintf("Failed to accept()");
        return;
    }

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &peeraddr.sin_addr, ipstr, INET_ADDRSTRLEN);
    printf("New connection from %s:%hu\n", ipstr, ntohs(peeraddr.sin_port));

    session_table[conn_sock].user_id = NULL;
    session_table[conn_sock].addr = peeraddr;
    session_table[conn_sock].buffer_pos = 0;
    session_table[conn_sock].buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    if (session_table[conn_sock].buffer == NULL) {
        eprintf("Buffer malloc error\n");
        close(conn_sock);
        return;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLHUP;
    ev.data.fd = conn_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
        eprintf("Failed to epoll on socket\n");
        close(conn_sock);
        return;
    }

    sprintf(sendbuf, "Hello, please assign your username: \n");
    send(conn_sock, sendbuf, strlen(sendbuf) + 1, 0);
    // Command line prompt
    sprintf(sendbuf, "%% ");
    send(conn_sock, sendbuf, strlen(sendbuf) + 1, 0);
}

int server_loop(int tcpsock)
{
    int epollfd;
    struct epoll_event ev, evs[EVCNT];
    memset(&ev, 0, sizeof(struct epoll_event));

    ev.events = EPOLLIN | EPOLLHUP;
    ev.data.fd = tcpsock;
    epollfd = epoll_create(64);
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, tcpsock, &ev) < 0) {
        eprintf("Failed to epoll_ctl(EPOLL_CTL_ADD)\n");
        return -1;
    }

    while (server_run_signal) {
        int nfds = epoll_wait(epollfd, evs, EVCNT, -1);
        if (nfds == -1) {
            if (errno == EINTR) {
                continue;
            }
            eprintf("Failed to epoll on socket\n");
            close(epollfd);
            return -1;
        }

        for (int i = 0; i < nfds; i++) {
            if (evs[i].data.fd == tcpsock) {
                handle_connect(epollfd, tcpsock);
            } else {
                if (evs[i].events & EPOLLHUP) {
                    handle_disconnect(epollfd, evs[i].data.fd);
                } else if (evs[i].events & EPOLLIN) {
                    handle_data(evs[i].data.fd);
                }
            }
        }
    }

    close(epollfd);
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

    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, handle_shutdown_signal);
    signal(SIGINT, handle_shutdown_signal);
    signal(SIGTERM, handle_shutdown_signal);

    int ret = server_loop(tcpsock);

    close(tcpsock);
    return ret;
}
