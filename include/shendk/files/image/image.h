#pragma once

#include <stdint.h>

template<typename PixType>
struct Image {

    Image(uint32_t width, uint32_t height)
        : m_width(width)
        , m_height(height)
        , m_rawData(nullptr)
    {
        m_rawData = new PixType[width * height];
    }

    uint32_t width() { return m_width; }
    uint32_t height() { return m_height; }
    const PixType* getPtr() { return m_rawData; }

protected:
    uint32_t m_width;
    uint32_t m_height;
    PixType* m_rawData;

};


struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct RGBA {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct BGRA {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

struct ARGB {
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
