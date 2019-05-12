#ifndef BITMAP_H
#define BITMAP_H

#include <inttypes.h>
#include <iostream>
#include <vector>
using namespace std;

struct bmp_file_header {
    char signature[2];
    uint32_t size;
    uint32_t _reserve;
    uint32_t offset;

    friend istream &operator >>(istream &stream, struct bmp_file_header &data)
    {
        stream.read((char *)&data.signature, 2);
        stream.read((char *)&data.size, 4);
        stream.read((char *)&data._reserve, 4);
        stream.read((char *)&data.offset, 4);
        return stream;
    }

    friend ostream &operator <<(ostream &stream, const struct bmp_file_header &data)
    {
        stream.write((char *)&data.signature, 2);
        stream.write((char *)&data.size, 4);
        stream.write((char *)&data._reserve, 4);
        stream.write((char *)&data.offset, 4);
        return stream;
    }
};

struct bmp_info_header {
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t color_planes;
    uint16_t color_depth;
    uint32_t compression;
    uint32_t image_size;
    uint32_t hori_resolu;
    uint32_t vert_resolu;
    uint32_t colors;
    uint32_t important_colors;
    unsigned char unused[84];
    
    friend istream &operator >>(istream &stream, struct bmp_info_header &data)
    {
        stream.read((char *)&data.size, 4);
        stream.read((char *)&data.width, 4);
        stream.read((char *)&data.height, 4);
        stream.read((char *)&data.color_planes, 2);
        stream.read((char *)&data.color_depth, 2);
        stream.read((char *)&data.compression, 4);
        stream.read((char *)&data.image_size, 4);
        stream.read((char *)&data.hori_resolu, 4);
        stream.read((char *)&data.vert_resolu, 4);
        stream.read((char *)&data.colors, 4);
        stream.read((char *)&data.important_colors, 4);
        int remain = data.size - 40;
        stream.read((char *)data.unused, remain);
        return stream;
    }
    friend ostream &operator <<(ostream &stream, const struct bmp_info_header &data)
    {
        stream.write((char *)&data.size, 4);
        stream.write((char *)&data.width, 4);
        stream.write((char *)&data.height, 4);
        stream.write((char *)&data.color_planes, 2);
        stream.write((char *)&data.color_depth, 2);
        stream.write((char *)&data.compression, 4);
        stream.write((char *)&data.image_size, 4);
        stream.write((char *)&data.hori_resolu, 4);
        stream.write((char *)&data.vert_resolu, 4);
        stream.write((char *)&data.colors, 4);
        stream.write((char *)&data.important_colors, 4);
        int remain = data.size - 40;
        stream.write((char *)data.unused, remain);
        return stream;
    }
};

struct pixel_24bit {
    uint8_t B;
    uint8_t G;
    uint8_t R;

    friend istream &operator >>(istream &stream, struct pixel_24bit &data)
    {
        stream.read((char *)&data.B, 1);
        stream.read((char *)&data.G, 1);
        stream.read((char *)&data.R, 1);
        return stream;
    }

    friend ostream &operator <<(ostream &stream, const struct pixel_24bit &data)
    {
        stream.write((char *)&data.B, 1);
        stream.write((char *)&data.G, 1);
        stream.write((char *)&data.R, 1);
        return stream;
    }
};

struct pixel_32bit {
    uint8_t B;
    uint8_t G;
    uint8_t R;
    uint8_t A;

    friend istream &operator >>(istream &stream, struct pixel_32bit &data)
    {
        stream.read((char *)&data.B, 1);
        stream.read((char *)&data.G, 1);
        stream.read((char *)&data.R, 1);
        stream.read((char *)&data.A, 1);
        return stream;
    }

    friend ostream &operator <<(ostream &stream, const struct pixel_32bit &data)
    {
        stream.write((char *)&data.B, 1);
        stream.write((char *)&data.G, 1);
        stream.write((char *)&data.R, 1);
        stream.write((char *)&data.A, 1);
        return stream;
    }
};

class Bitmap24 {
public:
    unsigned long height() const;
    unsigned long width() const;

    struct pixel_24bit &at(size_t x, size_t y);
    const struct pixel_24bit &at_const(size_t x, size_t y) const;

    static Bitmap24 load_file(string path);

    void write_file(string path) const;
private:
    friend class Bitmap;
    struct bmp_file_header fh;
    struct bmp_info_header ih;
    vector<pixel_24bit> pixels;
};

class Bitmap32 {
public:
    unsigned long height() const;
    unsigned long width() const;

    struct pixel_32bit &at(size_t x, size_t y);
    const struct pixel_32bit &at_const(size_t x, size_t y) const;

    static Bitmap32 load_file(string path);

    void write_file(string path) const;
private:
    friend class Bitmap;
    struct bmp_file_header fh;
    struct bmp_info_header ih;
    vector<pixel_32bit> pixels;
};

class Bitmap {
public:
    unsigned long height() const;
    unsigned long width() const;
    int color_bits() const;

    Bitmap24 into_24bit() const;
    Bitmap32 into_32bit() const;

    static Bitmap load_file(string path);
private:
    struct bmp_file_header fh;
    struct bmp_info_header ih;
    vector<unsigned char> data;
};

#endif
