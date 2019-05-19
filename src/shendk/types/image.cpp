#include "shendk/types/image.h"

#include <stdint.h>

namespace shendk {

BGRA& BGRA::operator+=(const BGRA& rhs) {
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    a += rhs.a;
    return *this;
}

BGRA& BGRA::operator-=(const BGRA& rhs) {
    r -= rhs.r;
    g -= rhs.g;
    b -= rhs.b;
    a -= rhs.a;
    return *this;
}

BGRA& BGRA::operator*=(const BGRA& rhs) {
    r *= rhs.r;
    g *= rhs.g;
    b *= rhs.b;
    a *= rhs.a;
    return *this;
}

BGRA& BGRA::operator/=(const BGRA& rhs) {
    r /= rhs.r;
    g /= rhs.g;
    b /= rhs.b;
    a /= rhs.a;
    return *this;
}

RGBA& RGBA::operator+=(const RGBA& rhs) {
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    a += rhs.a;
    return *this;
}

RGBA& RGBA::operator-=(const RGBA& rhs) {
    r -= rhs.r;
    g -= rhs.g;
    b -= rhs.b;
    a -= rhs.a;
    return *this;
}

RGBA& RGBA::operator*=(const RGBA& rhs) {
    r *= rhs.r;
    g *= rhs.g;
    b *= rhs.b;
    a *= rhs.a;
    return *this;
}

RGBA& RGBA::operator/=(const RGBA& rhs) {
    r /= rhs.r;
    g /= rhs.g;
    b /= rhs.b;
    a /= rhs.a;
    return *this;
}

Image::Image(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
    , m_rawData(nullptr)
{
    m_rawData = new RGBA[width * height];
}

Image::~Image() {
    delete[] m_rawData;
}

int Image::width()  const { return m_width;  }
int Image::height() const { return m_height; }
int Image::size()   const { return m_width * m_height * sizeof(RGBA); }

RGBA const* Image::getDataPtr() const { return m_rawData; }
RGBA *      Image::getDataPtr()       { return m_rawData; }

RGBA const* Image::begin() const { return getDataPtr(); }
RGBA*       Image::begin()       { return getDataPtr(); }
RGBA const* Image::end() const { return getDataPtr() + m_width * m_height; }
RGBA*       Image::end()       { return getDataPtr() + m_width * m_height; }

RGBA const& Image::operator[](int index) const { return getDataPtr()[index]; }
RGBA&       Image::operator[](int index)       { return getDataPtr()[index]; }

void Image::flipVertical() {
    for (uint32_t y = 0; y < m_height / 2; y++) {
        for (uint32_t x = 0; x < m_width; x++) {
            uint32_t dstY = m_height - 1 - y;
            uint32_t srcIndex = y * m_width + x;
            uint32_t dstIndex = dstY * m_width + x;
            RGBA src = m_rawData[srcIndex];
            m_rawData[srcIndex] = m_rawData[dstIndex];
            m_rawData[dstIndex] = src;
        }
    }
}

Image* Image::resize(uint32_t width, uint32_t height) {
    Image* resizedImage = new Image(width, height);

    // TODO: implement
    return resizedImage;
}

}
