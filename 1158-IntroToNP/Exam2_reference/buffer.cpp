#include "buffer.hpp"
#include <algorithm>
#include <string.h>
using namespace std;

Buffer::Buffer()
    :buf(new char[BUFFER_SIZE]), pos(0)
{
}

size_t Buffer::feed(const char *data, size_t len)
{
    size_t cpylen = sizeof(char) * min(len, BUFFER_SIZE - this->pos);
    memcpy(&this->buf[this->pos], data, cpylen);
    this->pos += cpylen;
    return cpylen;
}

char *Buffer::getline()
{
    for (size_t p = 0; p < this->pos; p++) {
        if (this->buf[p] == '\n') {
            this->buf[p] = '\0';

            char *s = strdup(this->buf.get());

            // Move buffer & Recalculate p
            size_t rmlen = p + 1;
            memmove(this->buf.get(), &this->buf[rmlen], sizeof(char) * (this->pos - rmlen));
            p -= rmlen;
            this->pos -= rmlen;

            return s;
        }
    }

    return NULL;
}

bool Buffer::getline(string &s)
{
    for (size_t p = 0; p < this->pos; p++) {
        if (this->buf[p] == '\n') {
            this->buf[p] = '\0';

            s = this->buf.get();

            // Move buffer & Recalculate p
            size_t rmlen = p + 1;
            memmove(this->buf.get(), &this->buf[rmlen], sizeof(char) * (this->pos - rmlen));
            p -= rmlen;
            this->pos -= rmlen;

            return true;
        }
    }

    return false;
}

