#ifndef _CHATROOM_H
#define _CHATROOM_H

#include "buffer.hpp"
#include <arpa/inet.h>
#include <time.h>
#include <string>
#include <queue>
#include <unordered_map>
#include <vector>

struct Session {
    std::string user_id;
    struct sockaddr_in addr;
    Buffer buffer;
};

struct UserInfo {
    UserInfo(const Session &session);
    UserInfo &operator=(const Session &session);

    std::string user_id;
    struct sockaddr_in addr;
};

struct ChatMessage {
    std::string name;
    std::string msg;
    struct tm time;
};

class Chatroom {
public:
    Chatroom();

    void join(int fd, const struct sockaddr_in *addr);
    void leave(int fd);
    Session *get(int fd);
    Session *find_by_name(const char *name);
    std::vector<UserInfo> get_user_infos() const;
    void sendmsg(const char *name, const char *msg);

    void (*message_cb)(const char *fmtstr, const ChatMessage &chatmsg);

private:
    std::queue<ChatMessage> chatmsg;
    std::unordered_map<int, Session> session;
};

#endif

