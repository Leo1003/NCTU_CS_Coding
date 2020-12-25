#ifndef _BUFFER_H
#define _BUFFER_H

#include <memory>
#include <string>

#define BUFFER_SIZE 4096

class Buffer {
public:
    Buffer();

    size_t feed(const char *data, size_t len);
    char *getline();
    bool getline(std::string &s);

private:
    std::unique_ptr<char []> buf;
    size_t pos;
};

#endif
