#ifndef UDPFILE_PROTO_H
#define UDPFILE_PROTO_H

#include <inttypes.h>

#define UFCODE_REQUEST          100
#define UFCODE_DATA             101
#define UFCODE_CANCEL           104
#define UFCODE_COMPLETED        200
#define UFCODE_CREATED          201
#define UFCODE_ALTERED          202
#define UFCODE_CONTINUE         203
#define UFCODE_BAD_REQUEST      300
#define UFCODE_EXISTED          302
#define UFCODE_QUOTA_EXCEED     303
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

struct uf_data_alterres {
    uint32_t filename_len;
    char filename[];
};

#endif
