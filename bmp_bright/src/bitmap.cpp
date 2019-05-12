#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "bitmap.h"
using namespace std;

Bitmap24 Bitmap24::load_file(string path)
{
    Bitmap24 bmp;
    ifstream f;
    f.open(path, ios::binary);
    f >> bmp.fh >> bmp.ih;
    f.seekg(bmp.fh.offset);
    size_t rowsize = (3 * bmp.width() + 3) / 4 * 4;
    bmp.pixels.resize(bmp.height() * rowsize);
    for (int r = bmp.height() - 1; r >= 0; r--) {
        size_t readed = 0;
        for (int x = 0; x < (int)bmp.width(); x++) {
            f >> bmp.at(x, r);
            readed += 3;
        }
        f.read(NULL, rowsize - readed);
    }

    return bmp;
}

void Bitmap24::write_file(string path) const
{
    ofstream f;
    f.open(path, ios::binary);
    
    size_t rowsize = (3 * this->width() + 3) / 4 * 4;
    struct bmp_file_header fh_c = this->fh;
    fh_c.offset = 14 + this->ih.size;
    fh_c.size = 14 + this->ih.size + (rowsize * this->height());

    f << fh_c;
    f << this->ih;
    
    for (int r = this->height() - 1; r >= 0; r--) {
        size_t wrote = 0;
        for (int x = 0; x < (int)this->width(); x++) {
            f << this->at_const(x, r);
            wrote += 3;
        }
        for (; wrote < rowsize; wrote++) {
            f << '\0';
        }
    }
    f.flush();
    f.close();
}

unsigned long Bitmap24::height() const
{
    return this->ih.height;
}

unsigned long Bitmap24::width() const 
{
    return this->ih.width;
}

struct pixel_24bit &Bitmap24::at(size_t x, size_t y)
{
    if (x < this->width() && y < this->height()) {
        return this->pixels[y * this->width() + x];
    }
    throw "Out of bound!";
}

const struct pixel_24bit &Bitmap24::at_const(size_t x, size_t y) const
{
    if (x < this->width() && y < this->height()) {
        return this->pixels[y * this->width() + x];
    }
    throw "Out of bound!";
}

Bitmap32 Bitmap32::load_file(string path)
{
    Bitmap32 bmp;
    ifstream f;
    f.open(path, ios::binary);
    f >> bmp.fh >> bmp.ih;
    f.seekg(bmp.fh.offset);
    size_t rowsize = bmp.width() * 4;
    bmp.pixels.resize(bmp.height() * rowsize);
    for (int r = bmp.height() - 1; r >= 0; r--) {
        size_t readed = 0;
        for (int x = 0; x < (int)bmp.width(); x++) {
            f >> bmp.at(x, r);
            readed += 4;
        }
        f.read(NULL, rowsize - readed);
    }

    return bmp;
}

void Bitmap32::write_file(string path) const
{
    ofstream f;
    f.open(path, ios::binary);
    
    size_t rowsize = this->width() * 4;
    struct bmp_file_header fh_c = this->fh;
    fh_c.offset = 14 + this->ih.size;
    fh_c.size = 14 + this->ih.size + (rowsize * this->height());

    f << fh_c;
    f << this->ih;

    for (int r = this->height() - 1; r >= 0; r--) {
        size_t wrote = 0;
        for (int x = 0; x < (int)this->width(); x++) {
            f << this->at_const(x, r);
            wrote += 4;
        }
        for (; wrote < rowsize; wrote++) {
            f << '\0';
        }
    }
    f.flush();
    f.close();
}

unsigned long Bitmap32::height() const
{
    return this->ih.height;
}

unsigned long Bitmap32::width() const 
{
    return this->ih.width;
}

struct pixel_32bit &Bitmap32::at(size_t x, size_t y)
{
    if (x < this->width() && y < this->height()) {
        return this->pixels[y * this->width() + x];
    }
    throw "Out of bound!";
}

const struct pixel_32bit &Bitmap32::at_const(size_t x, size_t y) const
{
    if (x < this->width() && y < this->height()) {
        return this->pixels[y * this->width() + x];
    }
    throw "Out of bound!";
}

Bitmap Bitmap::load_file(string path)
{
    Bitmap bmp;
    ifstream f;
    f.open(path, ios::binary);
    f >> bmp.fh >> bmp.ih;
    f.seekg(bmp.fh.offset);
    bmp.data.resize(bmp.fh.size - bmp.fh.offset);
    f.read((char *)&bmp.data[0], bmp.data.size());

    return bmp;
}

unsigned long Bitmap::height() const
{
    return this->ih.height;
}

unsigned long Bitmap::width() const 
{
    return this->ih.width;
}

Bitmap24 Bitmap::into_24bit() const
{
    Bitmap24 bmp24;
    bmp24.fh = this->fh;
    bmp24.ih = this->ih;
    size_t rowsize = (3 * this->width() + 3) / 4 * 4;
    bmp24.pixels.resize(this->height() * rowsize);

    for (int r = bmp24.height() - 1; r >= 0; r--) {
        size_t i = (bmp24.height() - r - 1) * rowsize;
        for (int x = 0; x < (int)bmp24.width(); x++) {
            bmp24.at(x, r).B = this->data[i++];
            bmp24.at(x, r).G = this->data[i++];
            bmp24.at(x, r).R = this->data[i++];
        }
    }

    return bmp24;
}

Bitmap32 Bitmap::into_32bit() const
{
    Bitmap32 bmp32;
    bmp32.fh = this->fh;
    bmp32.ih = this->ih;
    size_t rowsize = this->width() * 4;
    bmp32.pixels.resize(this->height() * rowsize);

    for (int r = bmp32.height() - 1; r >= 0; r--) {
        size_t i = (bmp32.height() - r - 1) * rowsize;
        for (int x = 0; x < (int)bmp32.width(); x++) {
            bmp32.at(x, r).B = this->data[i++];
            bmp32.at(x, r).G = this->data[i++];
            bmp32.at(x, r).R = this->data[i++];
            bmp32.at(x, r).A = this->data[i++];
        }
    }

    return bmp32;
}

int Bitmap::color_bits() const
{
    return this->ih.color_depth;
}
