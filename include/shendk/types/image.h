#pragma once

#include <stdint.h>
#include <vector>

namespace shendk {

/**
 * @brief Shenmue internal pixel format
 */
struct BGRA {
    uint8_t b = 0;
    uint8_t g = 0;
    uint8_t r = 0;
    uint8_t a = 0;

    BGRA& operator+=(const BGRA& rhs);
    BGRA& operator-=(const BGRA& rhs);
    BGRA& operator*=(const BGRA& rhs);
    BGRA& operator/=(const BGRA& rhs);
};

struct RGBA {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;

    RGBA& operator+=(const RGBA& rhs);
    RGBA& operator-=(const RGBA& rhs);
    RGBA& operator*=(const RGBA& rhs);
    RGBA& operator/=(const RGBA& rhs);
};

struct Image {

    Image(Image& image);
    Image(uint32_t width, uint32_t height);
    ~Image();

    int width()  const;
    int height() const;
    int size()   const;

    RGBA const* getDataPtr() const;
    RGBA *      getDataPtr();

    RGBA const* begin() const;
    RGBA*       begin();
    RGBA const* end() const;
    RGBA*       end();

    RGBA const& operator[](int index) const;
    RGBA&       operator[](int index);

    void flipVertical();
    void flipHorizontal();
    Image mirrorRepeat();
    void writeImage(Image& src, int srcX, int srcY, int dstX, int dstY, int width, int height);
    Image* resize(uint32_t width, uint32_t height);
    std::vector<BGRA> createBGRA8();

protected:
    uint32_t m_width;
    uint32_t m_height;
    RGBA* m_rawData;
};


}
