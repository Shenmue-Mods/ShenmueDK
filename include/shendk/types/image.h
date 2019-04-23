#pragma once

#include <stdint.h>

namespace shendk {

struct BGR {
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

struct BGRA {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};


template<typename PixType>
struct Image {

    Image(uint32_t width, uint32_t height)
        : m_width(width)
        , m_height(height)
        , m_rawData(nullptr)
    {
        m_rawData = new PixType[width * height];
    }

    ~Image() {
        delete[] m_rawData;
    }

    int width()  const { return m_width;  }
    int height() const { return m_height; }
    int size()   const { return m_width * m_height * sizeof(PixType); }

    PixType const* getDataPtr() const { return m_rawData; }
    PixType *      getDataPtr()       { return m_rawData; }

    PixType const* begin() const { return getDataPtr(); }
    PixType*       begin()       { return getDataPtr(); }
    PixType const* end() const { return getDataPtr() + m_width * m_height; }
    PixType*       end()       { return getDataPtr() + m_width * m_height; }

    PixType const& operator[](int index) const { return getDataPtr()[index]; }
    PixType&       operator[](int index)       { return getDataPtr()[index]; }

protected:
    uint32_t m_width;
    uint32_t m_height;
    PixType* m_rawData;

};

typedef Image<BGRA> ArgbImage;
typedef Image<BGR>  RgbImage;

}
