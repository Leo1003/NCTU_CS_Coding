#include "chatroom.hpp"

#include <algorithm>
#include <arpa/inet.h>
#include <endian.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

#define EVCNT 4
#define BUFFER_SIZE 4096
#define DELIM " \t\r\n"
#define SYSTEM_NAME "sys"

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a < b ? b : a)

static bool server_run_signal = true;

Chatroom chatroom;

void usage(const char *exename)
{
    eprintf("Usage: %s <port>\n", exename);
}

static void handle_shutdown_signal(__attribute__((unused)) int sig)
{
    eprintf("Shutdown server...\n");
    server_run_signal = false;
}

int handle_command(int sockfd, const char *cmdline, struct Session *ctx)
{
    //eprintf("Command: \"%s\"\n", cmdline);
    char sendbuf[BUFFER_SIZE];
    char *cmdbuf = strdup(cmdline);
    char *arg = strtok(cmdbuf, DELIM);
    if (arg == NULL) {
        free(cmdbuf);
        return 0;
    }

    if (!strcmp(arg, "list-users")) {
        vector<UserInfo> infos = chatroom.get_user_infos();

        char *ptr = sendbuf;
        for (const UserInfo &info: infos) {
            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &info.addr.sin_addr, ipstr, INET_ADDRSTRLEN);

            int l = sprintf(ptr, "%s %s:%hu\n", info.user_id.c_str(), ipstr, ntohs(info.addr.sin_port));
            ptr += l;
        }
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
    } else if (!strcmp(arg, "sort-users")) {
        vector<UserInfo> infos = chatroom.get_user_infos();
        sort(infos.begin(), infos.end(), [](const UserInfo &lhs, const UserInfo &rhs) { return lhs.user_id < rhs.user_id; });

        char *ptr = sendbuf;
        for (const UserInfo &info: infos) {
            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &info.addr.sin_addr, ipstr, INET_ADDRSTRLEN);

            int l = sprintf(ptr, "%s %s:%hu\n", info.user_id.c_str(), ipstr, ntohs(info.addr.sin_port));
            ptr += l;
        }
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
    } else if (!strcmp(arg, "get-ip")) {
        char ipstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ctx->addr.sin_addr, ipstr, INET_ADDRSTRLEN);
        sprintf(sendbuf, "IP: %s:%hu\n", ipstr, ntohs(ctx->addr.sin_port));
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
    } else if (!strcmp(arg, "exit")) {
        sprintf(sendbuf, "Bye, %s.\n", ctx->user_id.c_str());
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
        shutdown(sockfd, SHUT_RDWR);
    } else {
        // Chat message
        chatroom.sendmsg(ctx->user_id.c_str(), cmdline);
        /*
        sprintf(sendbuf, "Unknown command\n");
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
        */
    }

    free(cmdbuf);
    return 0;
}

int handle_setname(int sockfd, const char *name, struct Session *ctx)
{
    char sendbuf[BUFFER_SIZE];

    size_t namelen = strlen(name);
    if (namelen == 0) {
        return 0;
    }

    // Check if name being used
    const Session *user = chatroom.find_by_name(name);
    if (user != NULL) {
        sprintf(sendbuf, "The username is already used!\n");
        send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
        return 0;
    }

    sprintf(sendbuf, "User %s join!", name);
    chatroom.sendmsg(SYSTEM_NAME, sendbuf);

    ctx->user_id = name;

    sprintf(sendbuf, "Welcome, %s.\n", ctx->user_id.c_str());
    send(sockfd, sendbuf, strlen(sendbuf) + 1, 0);
    return 0;
}

void handle_data(int sockfd)
{
    char buf[BUFFER_SIZE];
    struct Session *ctx = chatroom.get(sockfd);

    ssize_t recvlen = recv(sockfd, buf, sizeof(char) * BUFFER_SIZE, 0);
    if (recvlen < 0) {
        eprintf("Failed to recv()");
        return;
    }
    ctx->buffer.feed(buf, recvlen);

    char *cmdline = NULL;
    while ((cmdline = ctx->buffer.getline()) != NULL) {
        if (ctx->user_id.empty()) {
            // Not set username yet
            char *arg = strtok(cmdline, "\r\n");
            if (arg) {
                handle_setname(sockfd, arg, ctx);
            }
        } else {
            char *arg = strtok(cmdline, "\r\n");
            if (arg) {
                handle_command(sockfd, arg, ctx);
            }
        }
        free(cmdline);

        // Command line prompt
        sprintf(buf, "%% ");
        send(sockfd, buf, strlen(buf) + 1, 0);
    }

    /*
    if (ctx->buffer) {
        ctx->buffer[ctx->buffer_pos] = '\0';
        eprintf("Buffer: %s\n", ctx->buffer);
    }
    */
}

void handle_disconnect(int epollfd, int sockfd)
{
    struct Session *ctx = chatroom.get(sockfd);
    char ipstr[INET_ADDRSTRLEN], buf[BUFFER_SIZE];

    inet_ntop(AF_INET, &ctx->addr.sin_addr, ipstr, INET_ADDRSTRLEN);
    printf("%s %s:%hu disconnected\n", ctx->user_id.c_str(), ipstr,
           ntohs(ctx->addr.sin_port));

    sprintf(buf, "User %s leave!", ctx->user_id.c_str());
    chatroom.leave(sockfd);
    // ctx is invalid from here!!
    chatroom.sendmsg(SYSTEM_NAME, buf);

    epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, NULL);
    close(sockfd);
}

void handle_connect(int epollfd, int tcpsock)
{
    char sendbuf[BUFFER_SIZE];
    struct sockaddr_in peeraddr;
    socklen_t addrlen = sizeof(peeraddr);

    int conn_sock = accept(tcpsock, (struct sockaddr *)&peeraddr, &addrlen);
    if (conn_sock == -1) {
        eprintf("Failed to accept()");
        return;
    }

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &peeraddr.sin_addr, ipstr, INET_ADDRSTRLEN);
    printf("New connection from %s:%hu\n", ipstr, ntohs(peeraddr.sin_port));

    chatroom.join(conn_sock, &peeraddr);

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
                //eprintf("epoll event: Connect\n");
                handle_connect(epollfd, tcpsock);
            } else {
                if (evs[i].events & EPOLLHUP) {
                    //eprintf("epoll event: Disconnect\n");
                    handle_disconnect(epollfd, evs[i].data.fd);
                } else if (evs[i].events & EPOLLIN) {
                    //eprintf("epoll event: Data\n");
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
        eprintf("Failed to bind on port %hu: %s\n", port, strerror(errno));
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

