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

#include "hashtable.h"
#include "udpfile_proto.h"

#define DOWNLOAD_DIR "downloads"
#define EVCNT 4
#define MAX_CONN 256
#define BUFFER_SIZE 1032

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a < b ? b : a)

struct sess_ctx {
    struct sockaddr_in peeraddr;
    FILE *fp;
    size_t total_size;
    size_t recv_size;
    char *filename;
};

void sess_ctx_free(struct sess_ctx *ctx)
{
    if (ctx->fp) {
        fclose(ctx->fp);
    }
    free(ctx->filename);
    free(ctx);
}

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

ssize_t send_resp(int sockfd, const struct sockaddr_in *addr, int code, void *data, size_t datalen)
{
    struct msghdr mhdr;
    struct iovec iov[2];
    struct uf_msg_packet phdr;
    memset(&mhdr, 0, sizeof(struct msghdr));

    mhdr.msg_name = (void *)addr;
    mhdr.msg_namelen = sizeof(struct sockaddr_in);
    mhdr.msg_iov = iov;
    mhdr.msg_iovlen = (data ? 2 : 1);

    iov[0].iov_base = &phdr;
    iov[0].iov_len = sizeof(struct uf_msg_packet);

    if (data) {
        iov[1].iov_base = data;
        iov[1].iov_len = datalen;
    }

    phdr.code = htonl(code);
    phdr.datalen = htonl(data ? datalen : 0);
    
    return sendmsg(sockfd, &mhdr, 0);
}

void handle_msg(int sockfd, hashtable_t *sessions)
{
    struct sockaddr_in peeraddr;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    unsigned char buf[BUFFER_SIZE];
    struct uf_msg_packet *reqp = (struct uf_msg_packet *)buf;

    ssize_t recvlen = recvfrom(sockfd, buf, sizeof(unsigned char) * BUFFER_SIZE, 0, (struct sockaddr *)&peeraddr, &addrlen);
    if (recvlen < 0) {
        const char *errmsg = strerror(errno);
        eprintf("Failed to receive data: %s\n", errmsg);
        return;
    }
    if ((size_t)recvlen < sizeof(struct uf_msg_packet)) {
        send_resp(sockfd, &peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
        return;
    }
    struct sess_ctx *ctx = (struct sess_ctx *)ht_get(sessions, &peeraddr);

    int code = ntohl(reqp->code);
    size_t datalen = ntohl(reqp->datalen);
    if (datalen != recvlen - sizeof(struct uf_msg_packet)) {
        send_resp(sockfd, &peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
        return;
    }
    switch (code) {
        case UFCODE_REQUEST: {
            if (ctx != NULL || datalen < sizeof(struct uf_data_sendreq)) {
                send_resp(sockfd, &peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
                return;
            }

            struct uf_data_sendreq *reqdata = (struct uf_data_sendreq *)&reqp->data;
            size_t filesize = be64toh(reqdata->filesize);
            size_t namelen = ntohl(reqdata->filename_len);
            if (namelen != datalen - sizeof(struct uf_data_sendreq)) {
                send_resp(sockfd, &peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
                return;
            }

            ctx = (struct sess_ctx *)malloc(sizeof(struct sess_ctx));
            if (ctx == NULL) {
                send_resp(sockfd, &peeraddr, UFCODE_SERVER_ERROR, NULL, 0);
                return;
            }
            char *filename = (char *)malloc(namelen + 1);
            if (filename == NULL) {
                free(ctx);
                send_resp(sockfd, &peeraddr, UFCODE_SERVER_ERROR, NULL, 0);
                return;
            }
            memcpy(filename, reqdata->filename, namelen);
            filename[namelen] = '\0';
            bool altered = false;
            for (size_t i = 0; i < namelen; i++) {
                if (filename[namelen] == '/') {
                    filename[namelen] = '_';
                    altered = true;
                }
            }

            struct stat st;
            if (stat(filename, &st) == 0 || errno != ENOENT) {
                send_resp(sockfd, &peeraddr, UFCODE_EXISTED, NULL, 0);
                free(filename);
                free(ctx);
                return;
            }

            ctx->peeraddr = peeraddr;
            ctx->total_size = filesize;
            ctx->recv_size = 0;
            ctx->filename = filename;
            ctx->fp = fopen(filename, "wbx");
            if (ctx->fp == NULL) {
                send_resp(sockfd, &peeraddr, UFCODE_SERVER_ERROR, NULL, 0);
                free(filename);
                free(ctx);
                return;
            }
            ht_put(sessions, &peeraddr, ctx);

            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &peeraddr.sin_addr, ipstr, INET_ADDRSTRLEN);
            eprintf("Create new session from: %s:%hu\n", ipstr, ntohs(peeraddr.sin_port));
            eprintf("\tFilename: %s\n", filename);
            eprintf("\tSize: %zu\n", filesize);
            if (altered) {
                size_t filename_len = strlen(filename);
                size_t reslen = sizeof(struct uf_data_alterres) + filename_len;
                struct uf_data_alterres *resdata = (struct uf_data_alterres *)malloc(reslen);
                if (resdata == NULL) {
                    send_resp(sockfd, &peeraddr, UFCODE_ALTERED, NULL, 0);
                }
                resdata->filename_len = htonl(filename_len);
                memcpy(resdata->filename, filename, filename_len);
                send_resp(sockfd, &peeraddr, UFCODE_ALTERED, resdata, reslen);
                free(resdata);
            } else {
                send_resp(sockfd, &peeraddr, UFCODE_CREATED, NULL, 0);
            }

            break;
        }
        case UFCODE_DATA: {
            if (ctx == NULL) {
                send_resp(sockfd, &peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
                return;
            }

            size_t left_size = ctx->total_size - ctx->recv_size;
            size_t wrotelen = fwrite(reqp->data, 1, min(datalen, left_size), ctx->fp);
            ctx->recv_size += wrotelen;

            if (ctx->recv_size >= ctx->total_size) {
                send_resp(sockfd, &peeraddr, UFCODE_COMPLETED, NULL, 0);
                eprintf("File %s received.\n", ctx->filename);
                ht_remove(sessions, &peeraddr);
                sess_ctx_free(ctx);
            } else {
                send_resp(sockfd, &peeraddr, UFCODE_CONTINUE, NULL, 0);
            }

            break;
        }
        case UFCODE_CANCEL: {
            // Not implemented
            send_resp(sockfd, &peeraddr, UFCODE_SERVER_ERROR, NULL, 0);
            break;
        }
        default: {
            send_resp(sockfd, &peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
            break;
        }
    }
}

int server_loop(int sockfd, hashtable_t *sessions)
{
    int epollfd;
    struct epoll_event ev, evs[EVCNT];
    memset(&ev, 0, sizeof(struct epoll_event));

    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epollfd = epoll_create(64);
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
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
            if (evs[i].data.fd == sockfd) {
                handle_msg(sockfd, sessions);
            } else {
                eprintf("Warning: fd received from epoll_wait() is strange!\n");
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

    printf("Listening on UDP port %hu...\n", port);

    if (chdir(DOWNLOAD_DIR)) {
        eprintf("Failed to chdir() into download directory\n");
        exit(1);
    }

    hashtable_t *session = ht_create(MAX_CONN, sizeof(struct sockaddr_in));

    return server_loop(udpsock, session);
}

