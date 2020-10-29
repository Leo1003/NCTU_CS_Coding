#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "hashtable.h"

#define DOWNLOAD_DIR "downloads"
#define EVCNT 4
#define MAX_CONN 256
#define BUFFER_SIZE 1024

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

struct sess_ctx {
    struct sockaddr_in peeraddr;
    int fd;
    size_t total_size;
    size_t recv_size;
    char *filename;
};

void usage(const char *exename)
{
    eprintf("Usage: %s <port>\n", exename);
}

int mkdir_noexist(const char *dirname)
{
    struct stat st;
    if (stat(dirname, &st)) {
        if (errno == ENOENT) {
            return mkdir(dirname, 0755);
        }
    } else {
        if ((st.st_mode & S_IFMT) != S_IFDIR) {
            return 1;
        }
    }
    return 0;
}

int server_loop(int sockfd, hashtable_t *session)
{
    int epollfd;
    struct epoll_event ev, evs[EVCNT];
    unsigned char buf[BUFFER_SIZE];

    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epollfd = epoll_create(64);
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);

    while (1) {
        int nfds = epoll_wait(epollfd, evs, EVCNT, -1);
        if (nfds == -1) {
            eprintf("Failed to epoll on socket\n");
            return -1;
        }

        for (int i = 0; i < nfds; i++) {
            if (evs[i].data.fd == sockfd) {
                struct sockaddr_in peeraddr;
                socklen_t addrlen;
                ssize_t recvlen = recvfrom(sockfd, buf, sizeof(unsigned char) * BUFFER_SIZE, 0, (struct sockaddr *)&peeraddr, &addrlen);
                if (recvlen < 0) {
                    eprintf("Failed to receive data");
                }

                struct sess_ctx *ctx = (struct sess_ctx *)ht_get(session, &peeraddr);
                if (ctx == NULL) {
                    // Indicate that this is a new connection
                } else {
                    // Continue receiving data
                }
            } else {
                eprintf("fd received from epoll_wait() is strange!\n");
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

    if (mkdir_noexist(DOWNLOAD_DIR)) {
        eprintf("Failed to create download directory\n");
        exit(1);
    }

    struct sockaddr_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_port = htons(port);
    bindaddr.sin_addr.s_addr = INADDR_ANY;
    int udpsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bind(udpsock, (struct sockaddr *)&bindaddr, sizeof(bindaddr))) {
        eprintf("Failed to bind on port %hu\n", port);
        exit(1);
    }

    printf("Listening on UDP %hu...\n", port);

    hashtable_t *session = ht_create(MAX_CONN, sizeof(struct sockaddr_in));

    return server_loop(port, session);
}

