#ifndef UDPFILE_PROTO_H
#define UDPFILE_PROTO_H

#include <inttypes.h>

// Client request
#define UFCODE_SENDFILE         100
#define UFCODE_CONTINUE         102
// Server Response
#define UFCODE_COMPLETED        200
#define UFCODE_PARTIAL          201
#define UFCODE_CREATED          202
#define UFCODE_BAD_REQUEST      300
#define UFCODE_SERVER_ERROR     400

struct uf_msg_packet {
    int32_t code;
    uint32_t datalen;
    unsigned char data[];
};

struct uf_data_sendreq {
    uint64_t filesize;
    uint32_t filename_len;
    char filename[];
};

#endif
