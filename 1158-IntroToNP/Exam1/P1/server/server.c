#include <arpa/inet.h>
#include <dirent.h>
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
#include "uftp_proto.h"

#define EVCNT 4
#define MAX_CONN 256
#define FBUF_SIZE 1024
#define LBUF_SIZE 4096
#define BUFFER_SIZE 1032

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a < b ? b : a)

hashtable_t *session_table;

struct sess_ctx {
    struct sockaddr_in peeraddr;
    FILE *fp;
    size_t total_size;
    size_t sent_size;
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

static ssize_t send_resp(int sockfd, const struct sockaddr_in *addr, int code, void *data, size_t datalen)
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

void cmd_list(int sockfd, const struct sockaddr_in *peeraddr, void *data, size_t datalen)
{
    char listbuf[LBUF_SIZE];
    char *ptr = listbuf;
    size_t listlen = 0;

    DIR *pwd = opendir("./");
    struct dirent *ent;
    while ((ent = readdir(pwd)) != NULL) {
        if (ent->d_type == DT_REG) {
            size_t slen = strlen(ent->d_name);
            strncpy(ptr, ent->d_name, min(LBUF_SIZE - listlen, slen + 1));
            ptr += slen + 1;
            listlen += slen + 1;
        }
    }

    send_resp(sockfd, peeraddr, UFCODE_COMPLETED, listbuf, listlen);
}

static ssize_t send_file(int sockfd, const struct sockaddr_in *peeraddr, struct sess_ctx *ctx)
{
    //eprintf("send_file(): %s, %zu/%zu\n", ctx->filename, ctx->sent_size, ctx->total_size);

    unsigned char filebuf[FBUF_SIZE];
    size_t readsz = fread(filebuf, 1, FBUF_SIZE, ctx->fp);
    ctx->sent_size += readsz;
    int code;
    if (ctx->sent_size >= ctx->total_size) {
        code = UFCODE_COMPLETED;
    } else {
        code = UFCODE_PARTIAL;
    }
    send_resp(sockfd, peeraddr, code, filebuf, readsz);

    if (code == UFCODE_COMPLETED) {
        ht_remove(session_table, peeraddr);
        sess_ctx_free(ctx);
    }

    //eprintf("return %zu/%zu\n", ctx->sent_size, ctx->total_size);
}

void cmd_continue(int sockfd, const struct sockaddr_in *peeraddr, void *data, size_t datalen)
{
    struct sess_ctx *ctx = (struct sess_ctx *)ht_get(session_table, peeraddr);

    if (ctx == NULL) {
        send_resp(sockfd, peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
        return;
    }

    send_file(sockfd, peeraddr, ctx);
}

void cmd_getfile(int sockfd, const struct sockaddr_in *peeraddr, void *data, size_t datalen)
{
    struct uf_data_getreq *req = (struct uf_data_getreq *)data;
    struct sess_ctx *ctx = (struct sess_ctx *)ht_get(session_table, peeraddr);

    if (ctx != NULL || datalen < sizeof(struct uf_data_getreq)) {
        send_resp(sockfd, peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
        return;
    }

    size_t namelen = ntohl(req->filename_len);
    if (namelen != datalen - sizeof(struct uf_data_getreq)) {
        send_resp(sockfd, peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
        return;
    }

    ctx = (struct sess_ctx *)malloc(sizeof(struct sess_ctx));
    if (ctx == NULL) {
        send_resp(sockfd, peeraddr, UFCODE_SERVER_ERROR, NULL, 0);
        return;
    }
    char *filename = (char *)malloc(namelen + 1);
    if (filename == NULL) {
        free(ctx);
        send_resp(sockfd, peeraddr, UFCODE_SERVER_ERROR, NULL, 0);
        return;
    }
    memcpy(filename, req->filename, namelen);
    filename[namelen] = '\0';
    for (size_t i = 0; i < namelen; i++) {
        if (filename[namelen] == '/') {
            filename[namelen] = '_';
        }
    }

    struct stat st;
    if (stat(filename, &st)) {
        send_resp(sockfd, peeraddr, UFCODE_NOT_FOUND, NULL, 0);
        free(filename);
        free(ctx);
        return;
    }

    ctx->peeraddr = *peeraddr;
    ctx->total_size = st.st_size;
    ctx->sent_size = 0;
    ctx->filename = filename;
    ctx->fp = fopen(filename, "rb");
    if (ctx->fp == NULL) {
        send_resp(sockfd, peeraddr, UFCODE_SERVER_ERROR, NULL, 0);
        free(filename);
        free(ctx);
        return;
    }
    ht_put(session_table, peeraddr, ctx);

    send_file(sockfd, peeraddr, ctx);
}

int server_loop(int sockfd)
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
                struct sockaddr_in peeraddr;
                socklen_t addrlen = sizeof(struct sockaddr_in);
                unsigned char buf[BUFFER_SIZE];
                struct uf_msg_packet *reqp = (struct uf_msg_packet *)buf;

                ssize_t recvlen = recvfrom(sockfd, buf, sizeof(unsigned char) * BUFFER_SIZE, 0, (struct sockaddr *)&peeraddr, &addrlen);
                if (recvlen < 0) {
                    const char *errmsg = strerror(errno);
                    eprintf("Failed to receive data: %s\n", errmsg);
                    continue;
                }
                if ((size_t)recvlen < sizeof(struct uf_msg_packet)) {
                    send_resp(sockfd, &peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
                    continue;
                }

                int code = ntohl(reqp->code);
                size_t datalen = ntohl(reqp->datalen);
                if (datalen != recvlen - sizeof(struct uf_msg_packet)) {
                    send_resp(sockfd, &peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
                    continue;
                }

                // Handle commands
                switch (code) {
                    case UFCODE_GETFILE:
                        cmd_getfile(sockfd, &peeraddr, reqp->data, datalen);
                        break;
                    case UFCODE_LIST:
                        cmd_list(sockfd, &peeraddr, reqp->data, datalen);
                        break;
                    case UFCODE_CONTINUE:
                        cmd_continue(sockfd, &peeraddr, reqp->data, datalen);
                        break;
                    default:
                        send_resp(sockfd, &peeraddr, UFCODE_BAD_REQUEST, NULL, 0);
                        break;
                }
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

    session_table = ht_create(MAX_CONN, sizeof(struct sockaddr_in));

    return server_loop(udpsock);
}