#pragma once

struct Color {
    unsigned char r, g, b;
    
    Color() : r(0), g(0), b(0) {}
    Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
};
