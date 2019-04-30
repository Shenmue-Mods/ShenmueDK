#pragma once

#include <stdint.h>

namespace shendk {

struct BGRA {
    uint8_t b = 0;
    uint8_t g = 0;
    uint8_t r = 0;
    uint8_t a = 0;

    BGRA& operator+=(const BGRA& rhs) {
        r += rhs.r;
        g += rhs.g;
        b += rhs.b;
        a += rhs.a;
        return *this;
    }

    BGRA& operator-=(const BGRA& rhs) {
        r -= rhs.r;
        g -= rhs.g;
        b -= rhs.b;
        a -= rhs.a;
        return *this;
    }

    BGRA& operator*=(const BGRA& rhs) {
        r *= rhs.r;
        g *= rhs.g;
        b *= rhs.b;
        a *= rhs.a;
        return *this;
    }

    BGRA& operator/=(const BGRA& rhs) {
        r /= rhs.r;
        g /= rhs.g;
        b /= rhs.b;
        a /= rhs.a;
        return *this;
    }

};

struct Image {

    Image(uint32_t width, uint32_t height)
        : m_width(width)
        , m_height(height)
        , m_rawData(nullptr)
    {
        m_rawData = new BGRA[width * height];
    }

    ~Image() {
        delete[] m_rawData;
    }

    int width()  const { return m_width;  }
    int height() const { return m_height; }
    int size()   const { return m_width * m_height * sizeof(BGRA); }

    BGRA const* getDataPtr() const { return m_rawData; }
    BGRA *      getDataPtr()       { return m_rawData; }

    BGRA const* begin() const { return getDataPtr(); }
    BGRA*       begin()       { return getDataPtr(); }
    BGRA const* end() const { return getDataPtr() + m_width * m_height; }
    BGRA*       end()       { return getDataPtr() + m_width * m_height; }

    BGRA const& operator[](int index) const { return getDataPtr()[index]; }
    BGRA&       operator[](int index)       { return getDataPtr()[index]; }

    Image* resize(uint32_t width, uint32_t height) {
        Image* resizedImage = new Image(width, height);

        // TODO: implement
		return resizedImage;
    }

protected:
    uint32_t m_width;
    uint32_t m_height;
    BGRA* m_rawData;
};


}
