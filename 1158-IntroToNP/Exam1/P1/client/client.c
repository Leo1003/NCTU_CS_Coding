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

#include "uftp_proto.h"

#define BUFFER_SIZE 4096

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a < b ? b : a)

#define DELIM " \t\n"

void usage(const char *exename)
{
    eprintf("Usage: %s <host> <port>\n", exename);
}

ssize_t send_reqp(int sockfd, const struct sockaddr_in *addr, int code, void *data, size_t datalen)
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

void cmd_list(int sockfd, const struct sockaddr_in *addr)
{
    send_reqp(sockfd, addr, UFCODE_LIST, NULL, 0);

    char buf[BUFFER_SIZE];
    ssize_t recvlen = recv(sockfd, buf, BUFFER_SIZE, 0);
    if (recvlen < 0) {
        const char *errmsg = strerror(errno);
        printf("Failed to receive data: %s\n", errmsg);
        return;
    }
    if ((size_t)recvlen < sizeof(struct uf_msg_packet)) {
        printf("Invalid data\n");
        return;
    }

    struct uf_msg_packet *pack = (struct uf_msg_packet *)buf;
    int rcode = ntohl(pack->code);
    size_t rdatalen = ntohl(pack->datalen);

    char *s = pack->data;
    size_t l = 0;
    printf("Files: ");
    while (l < rdatalen) {
        size_t slen = strlen(s);
        printf("%s ", s);
        l += slen + 1;
        s = pack->data + l;
    }
    printf("\n");
}

void cmd_recvfile(int sockfd, const struct sockaddr_in *addr, const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Failed to open %s\n", filename);
        return;
    }

    size_t filename_len = strlen(filename) * sizeof(char);
    size_t datalen = sizeof(struct uf_data_getreq) + filename_len;
    struct uf_data_getreq *data = (struct uf_data_getreq *)malloc(datalen);
    if (data == NULL) {
        abort();
    }
    data->filename_len = htobe32(filename_len);
    memcpy(data->filename, filename, filename_len);

    send_reqp(sockfd, addr, UFCODE_GETFILE, data, datalen);

    unsigned char buf[BUFFER_SIZE];
    bool completed = false;
    while (!completed) {
        ssize_t recvlen = recv(sockfd, buf, BUFFER_SIZE, 0);
        
        if (recvlen < 0) {
            const char *errmsg = strerror(errno);
            printf("Failed to receive data: %s\n", errmsg);
            break;
        }
        if ((size_t)recvlen < sizeof(struct uf_msg_packet)) {
            printf("Invalid data\n");
            break;
        }

        struct uf_msg_packet *pack = (struct uf_msg_packet *)buf;
        int rcode = ntohl(pack->code);
        size_t rdatalen = ntohl(pack->datalen);
        if (rcode == UFCODE_COMPLETED) {
            completed = true;
        } else if (rcode == UFCODE_PARTIAL) {
            completed = false;
            send_reqp(sockfd, addr, UFCODE_CONTINUE, NULL, 0);
        } else {
            printf("Error code received: %d\n", rcode);
            break;
        }

        fwrite(pack->data, 1, rdatalen, fp);
    }

    fflush(fp);
    fclose(fp);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        usage(argv[0]);
        exit(1);
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) == 0) {
        eprintf("Invalid IP address.\n");
        exit(1);
    }

    unsigned short port = atol(argv[2]);
    if (port == 0) {
        eprintf("Invalid peer number.\n");
        exit(1);
    }
    servaddr.sin_port = htons(port);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        eprintf("Failed to create socket.\n");
        exit(1);
    }

    printf("%% ");
    fflush(stdout);

    char *cmd = NULL;
    size_t cmdsz = 0;

    while (getline(&cmd, &cmdsz, stdin) != EOF) {
        size_t cmdlen = strlen(cmd);

        char *arg = strtok(cmd, DELIM);
        if (arg != NULL) {
            if (!strcmp(arg, "get-file-list")) {
                cmd_list(sockfd, &servaddr);
            } else if (!strcmp(arg, "get-file")) {
                arg = strtok(NULL, DELIM);
                while (arg != NULL) {
                    cmd_recvfile(sockfd, &servaddr, arg);
                    arg = strtok(NULL, DELIM);
                }
            } else if (!strcmp(arg, "exit")) {
                break;
            }
        }

        printf("%% ");
        fflush(stdout);
    }

    free(cmd);
    return 0;
}
