#include "shendk/types/image.h"

#include <stdint.h>
#include <memory>

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

Image::Image(Image& image)
    : m_width(image.width())
    , m_height(image.height())
{
    m_rawData = new RGBA[m_width * m_height];
    memcpy(m_rawData, image.getDataPtr(), image.size());
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

void Image::flipHorizontal() {
    for (uint32_t y = 0; y < m_height; y++) {
        for (uint32_t x = 0; x < m_width / 2; x++) {
            uint32_t dstX = m_width - 1 - x;
            uint32_t srcIndex = y * m_width + x;
            uint32_t dstIndex = y * m_width + dstX;
            RGBA src = m_rawData[srcIndex];
            m_rawData[srcIndex] = m_rawData[dstIndex];
            m_rawData[dstIndex] = src;
        }
    }
}

Image Image::mirrorRepeat() {
    Image result(m_width * 2, m_height * 2);
    Image copy(*this);

    result.writeImage(copy, 0, 0, 0, 0, m_width, m_height);
    copy.flipVertical();
    result.writeImage(copy, 0, 0, 0, m_height, m_width, m_height);
    copy.flipHorizontal();
    result.writeImage(copy, 0, 0, m_width, m_height, m_width, m_height);
    copy.flipVertical();
    result.writeImage(copy, 0, 0, m_width, 0, m_width, m_height);

    return result;
}

void Image::writeImage(Image& src, int srcX, int srcY, int dstX, int dstY, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int srcIndex = (y + srcY) * src.width() + (x + srcX);
            int dstIndex = (y + dstY) * m_width + (x + dstX);
            m_rawData[dstIndex] = src[srcIndex];
        }
    }
}

Image* Image::resize(uint32_t width, uint32_t height) {
    Image* resizedImage = new Image(width, height);

    // TODO: implement
    return resizedImage;
}

std::vector<BGRA> Image::createBGRA8() {
    std::vector<BGRA> result(m_width * m_height);
    for (size_t i = 0; i < result.size(); i++) {
        result[i].r = m_rawData[i].r;
        result[i].g = m_rawData[i].g;
        result[i].b = m_rawData[i].b;
        result[i].a = m_rawData[i].a;
    }
    return result;
}

}
