#include "bitmap.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

void convert24(Bitmap24 &m, int y, int x, float mul)
{
    struct pixel_24bit &pix = m.at(x, y);
    bool over = false;
    if (pix.B * mul > 255) {
        over = true;
        pix.B = 255;
    } else {
        pix.B *= mul;
    }
    if (pix.G * mul > 255) {
        over = true;
        pix.G = 255;
    } else {
        pix.G *= mul;
    }
    if (pix.R * mul > 255) {
        over = true;
        pix.R = 255;
    } else {
        pix.R *= mul;
    }
    if (over) {
        throw "Pixel (" + to_string(x) + "," + to_string(y) + ") is overflow.";
    }
}

void convert32(Bitmap32 &m, int y, int x, float mul)
{
    struct pixel_32bit &pix = m.at(x, y);
    bool over = false;
    if (pix.B * mul > 255) {
        over = true;
        pix.B = 255;
    } else {
        pix.B *= mul;
    }
    if (pix.G * mul > 255) {
        over = true;
        pix.G = 255;
    } else {
        pix.G *= mul;
    }
    if (pix.R * mul > 255) {
        over = true;
        pix.R = 255;
    } else {
        pix.R *= mul;
    }
    if (over) {
        throw "Pixel (" + to_string(x) + "," + to_string(y) + ") is overflow.";
    }
}

int main(int argc, char **argv)
{
    string f1, f2;
    float multiply;
    for (int i = 0; i < argc; ++i) {
        if (i == 0) {
            continue;
        } else if (i == 1) {
            f1 = argv[i];
        } else if (i == 2) {
            f2 = argv[i];
        } else if (i == 3) {
            stringstream ss;
            ss << argv[i];
            ss >> multiply;
        }
    }
    
    Bitmap m = Bitmap::load_file(f1);
    if (m.color_bits() == 24) {
        cerr << "24bit BMP detected" << endl;
        Bitmap24 bmp24 = m.into_24bit();
        for (int y = 0; y < (int)bmp24.height(); y++) {
            for (int x = 0; x < (int)bmp24.width(); x++) {
                try {
                    convert24(bmp24, y, x, multiply);
                } catch (string s) {
                    cout << s << endl;
                }
            }
        }
        bmp24.write_file(f2);
    } else if (m.color_bits() == 32) {
        cerr << "32bit BMP detected" << endl;
        Bitmap32 bmp32 = m.into_32bit();
        for (int y = 0; y < (int)bmp32.height(); y++) {
            for (int x = 0; x < (int)bmp32.width(); x++) {
                try {
                    convert32(bmp32, y, x, multiply);
                } catch (string s) {
                    cout << s << endl;
                }
            }
        }
        bmp32.write_file(f2);
    }

    return 0;
}
