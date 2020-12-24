#ifndef UDPFILE_PROTO_H
#define UDPFILE_PROTO_H

#include <inttypes.h>

// Client request
#define UFCODE_GETFILE          100
#define UFCODE_LIST             101
#define UFCODE_CONTINUE         102
#define UFCODE_CANCEL           104
// Server Response
#define UFCODE_COMPLETED        200
#define UFCODE_PARTIAL          201
#define UFCODE_BAD_REQUEST      300
#define UFCODE_NOT_FOUND        304
#define UFCODE_SERVER_ERROR     400

struct uf_msg_packet {
    int32_t code;
    uint32_t datalen;
    unsigned char data[];
};

struct uf_data_getreq {
    uint32_t filename_len;
    char filename[];
};

#endif
