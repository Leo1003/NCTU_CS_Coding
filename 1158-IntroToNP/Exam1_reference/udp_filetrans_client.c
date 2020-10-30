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

#include "udpfile_proto.h"

#define BUFFER_SIZE 1032

#define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a < b ? b : a)

void usage(const char *exename)
{
    eprintf("Usage: %s <host> <port> <filename>\n", exename);
}

void print_rescode(int code)
{
    switch (code) {
        case UFCODE_BAD_REQUEST:
            eprintf("Bad request\n");
            break;
        case UFCODE_EXISTED:
            eprintf("File existed\n");
            break;
        case UFCODE_QUOTA_EXCEED:
            eprintf("Quota exceed\n");
            break;
        case UFCODE_SERVER_ERROR:
            eprintf("Server error\n");
            break;
    }
}

int send_reqpack(int sockfd, const char *filepath, const struct sockaddr_in *servaddr)
{
    unsigned char buf[BUFFER_SIZE];
    struct stat st;
    if (stat(filepath, &st) || (st.st_mode & S_IFMT) == S_IFDIR) {
        eprintf("Failed to open the file.\n");
        return 1;
    }

    const char *filename = strrchr(filepath, '/');
    if (filename == NULL) {
        filename = filepath;
    } else {
        filename++;
    }

    size_t filename_len = strlen(filename) * sizeof(char);
    size_t reqpack_len = sizeof(struct uf_msg_packet) + sizeof(struct uf_data_sendreq) + filename_len;
    struct uf_msg_packet *reqpack = (struct uf_msg_packet *)malloc(reqpack_len);
    if (reqpack == NULL) {
        abort();
    }
    reqpack->code = htonl(UFCODE_REQUEST);
    reqpack->datalen = htonl(sizeof(struct uf_data_sendreq) + filename_len);
    
    struct uf_data_sendreq *data = (struct uf_data_sendreq *)&reqpack->data;
    data->filesize = htobe64(st.st_size);
    data->filename_len = htobe32(filename_len);
    memcpy(data->filename, filename, filename_len);

    if (sendto(sockfd, reqpack, reqpack_len, 0, (const struct sockaddr *)servaddr, sizeof(struct sockaddr_in)) < 0) {
        eprintf("Failed to send request\n");
        return 1;
    }
    
    if (recv(sockfd, buf, BUFFER_SIZE, 0) < 0) {
        eprintf("Failed to recv()\n");
        return 1;
    }

    struct uf_msg_packet *resp = (struct uf_msg_packet *)buf;
    int rescode = ntohl(resp->code);
    if (rescode == UFCODE_CREATED) {
        return 0;
    } else if (rescode == UFCODE_ALTERED) {
        char altered_filename[1024];
        struct uf_data_alterres *resdata = (struct uf_data_alterres *)&resp->data;
        size_t altered_len = ntohl(resdata->filename_len);
        memcpy(altered_filename, resdata->filename, min(altered_len, 1023));
        altered_filename[min(altered_len, 1023)] = '\0';
        eprintf("Warning: Server would receive as: %s\n", altered_filename);
        return 0;
    } else if (rescode >= UFCODE_BAD_REQUEST) {
        print_rescode(rescode);
        return 1;
    } else {
        eprintf("Unknown response code.\n");
        return 1;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
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
        eprintf("Invalid poer number.\n");
        exit(1);
    }
    servaddr.sin_port = htons(port);


    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        eprintf("Failed to create socket.\n");
        exit(1);
    }

    if (send_reqpack(sockfd, argv[3], &servaddr)) {
        eprintf("Failed to send request packet\n");
        exit(1);
    }

    return 0;
}

