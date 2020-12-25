#define _POSIX_C_SOURCE 200809L
#include "chatroom.hpp"

#include <algorithm>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHAT_HISTORY_LEN    64
#define CHAT_MSG_MAXLEN     1024
#define TIME_MAXSTRLEN      256
using namespace std;

UserInfo::UserInfo(const Session &session)
    :user_id(session.user_id), addr(session.addr)
{
}

UserInfo &UserInfo::operator=(const Session &session)
{
    this->user_id = session.user_id;
    this->addr = session.addr;

    return *this;
}


Chatroom::Chatroom()
    :message_cb(NULL)
{
}

void Chatroom::join(int fd, const struct sockaddr_in *addr)
{
    Session s;
    s.addr = *addr;

    this->session.emplace(fd, move(s));
}

void Chatroom::leave(int fd)
{
    this->session.erase(fd);
}

Session *Chatroom::get(int fd)
{
    try {
        return &this->session.at(fd);
    } catch (const out_of_range &e) {
        return NULL;
    }
}

vector<UserInfo> Chatroom::get_user_infos() const
{
    vector<UserInfo> l;

    for (auto &it: this->session) {
        const Session &s = it.second;

        if (!s.user_id.empty()) {
            l.emplace_back(s);
        }
    }

    return l;
}

Session *Chatroom::find_by_name(const char *name)
{
    auto it = find_if(this->session.begin(), this->session.end(), [name](const auto &p) { return p.second.user_id == name; });
    if (it == this->session.end()) {
        return NULL;
    }

    return &it->second;
}

void Chatroom::sendmsg(const char *name, const char *msg)
{
    ChatMessage chatmsg;
    time_t msgtime = time(NULL);
    localtime_r(&msgtime, &chatmsg.time);
    char msgbuf[CHAT_MSG_MAXLEN];
    char timebuf[TIME_MAXSTRLEN];

    chatmsg.name = name;
    chatmsg.msg = msg;

    strftime(timebuf, TIME_MAXSTRLEN, "%H:%M", &chatmsg.time);
    snprintf(msgbuf, CHAT_MSG_MAXLEN, "[%s] %s: %s\n", timebuf, name, msg);
    msgbuf[CHAT_MSG_MAXLEN - 1] = '\0';
    msgbuf[CHAT_MSG_MAXLEN - 2] = '\n';

    if (this->message_cb) {
        this->message_cb(msgbuf, chatmsg);
    }

    for (auto &it: this->session) {
        int fd = it.first;
        Session &s = it.second;

        if (!s.user_id.empty() && s.user_id != name) {
            send(fd, msgbuf, strlen(msgbuf) + 1, 0);
        }
    }
}

