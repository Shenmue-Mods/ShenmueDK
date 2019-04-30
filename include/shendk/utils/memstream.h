#pragma once

#include <istream>
#include <ostream>
#include <streambuf>

#include <iostream>

#include <cstring>


/**
 * @brief Memory stream buffer that is based on the standard library stream buffer.
 *        Allocates memory dynamically when needed.
 */
template<typename CharT = char, typename traits_type = std::char_traits<CharT>>
class basic_mstreambuf : public std::basic_streambuf<CharT>
{
public:
    using Base = std::basic_streambuf<CharT>;
    using char_type = typename Base::char_type;
    using int_type = typename Base::int_type;
    using pos_type = typename Base::pos_type;
    using off_type = typename Base::off_type;

    basic_mstreambuf() {
        m_bufferOwner = true;
        m_buffer = new char_type[4];
        m_bufferSize = 4;
        std::memset(m_buffer, 0, 4);
        setOut(m_buffer, m_buffer, m_buffer + m_bufferSize);
        setIn(m_buffer, m_buffer, m_buffer + m_bufferSize);
    }

    basic_mstreambuf(char_type* buffer, size_t size) {
        m_bufferOwner = false; // TODO: take ownership of memory?
        m_buffer = buffer;
        m_bufferSize = size;
        setOut(m_buffer, m_buffer + m_bufferSize, m_buffer + m_bufferSize);
        setIn(m_buffer, m_buffer, m_buffer + m_bufferSize);
    }

    ~basic_mstreambuf() {
        if (m_bufferOwner) {
            delete[] m_buffer;
        }
    }

    int overflow( int c = EOF )
    {
        if (out_cur() >= out_beg() + m_bufferSize) {
            size_t oldSize = m_bufferSize;
            m_bufferSize = m_bufferSize << 1;
            if (m_bufferSize >= SIZE_MAX) {
                return Base::overflow(c);
            } else {
                // allocate more memory
                m_buffer = reinterpret_cast<char*>(realloc(m_buffer, m_bufferSize));
                setOut(m_buffer, m_buffer + oldSize, m_buffer + m_bufferSize);
                int inCurOffset = static_cast<int>(in_cur() - in_beg());
                setIn(m_buffer, m_buffer + inCurOffset, m_buffer + m_bufferSize);
                std::memset(m_buffer + oldSize, 0, m_bufferSize - oldSize);
            }
        } else {
            setIn(in_beg(), in_cur(), in_end() + 1);
            setOut(out_beg(), out_cur(), out_end() + 1);
        }

        // write character
        *out_cur() = static_cast<char_type>(c);
        Base::pbump(1);
        return c;
    }

    int underflow() {
        return traits_type::eof();
    }

    pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                     std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
    {
        if (dir == std::ios_base::beg) {
            if (in_beg() + off > in_end() ||
                in_beg() + off < in_beg()) {
                return -1;
            }
            if (mode == std::ios_base::in) {
                setIn(in_beg(), in_beg() + off, in_end());
                return in_cur() - in_beg();
            }
            if (mode == std::ios_base::out) {
                setOut(out_beg(), out_beg() + off, out_end());
                return out_cur() - out_beg();
            }
        } else if (dir == std::ios_base::cur) {
            if (mode == std::ios_base::in) {
                if (in_cur() + off > in_end() ||
                    in_cur() + off < in_beg()) {
                    return -1;
                }
                setIn(in_beg(), in_cur() + off, in_end());
                return in_cur() - in_beg();
            }
            if (mode == std::ios_base::out) {
                if (out_cur() + off > out_end() ||
                    out_cur() + off < out_beg()) {
                    return -1;
                }
                setOut(out_beg(), out_cur() + off, out_end());
                return out_cur() - out_beg();
            }
        } else if (dir == std::ios_base::end) {
            if (in_end() - off > in_end() ||
                in_end() - off < in_beg()) {
				return -1;
			}
            if (mode == std::ios_base::in) {
                setIn(in_beg(), in_end() - off, in_end());
                return in_cur() - in_beg();
            }
            if (mode == std::ios_base::out) {
                setOut(out_beg(), out_end() - off, out_end());
                return out_cur() - out_beg();
            }
        }
        return 0;
    }

    pos_type seekpos(pos_type pos, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) {
        if (in_beg() + pos > in_end() ||
            in_beg() + pos < in_beg()) {
            return -1;
        }
        if (mode == std::ios_base::in) {
            setIn(in_beg(), in_beg() + pos, in_end());
            return in_cur() - in_beg();
        }
        if (mode == std::ios_base::out) {
            setOut(out_beg(), out_beg() + pos, out_end());
            return out_cur() - out_beg();
        }
        return 0;
    }

    int sync()
    {
        return 0;
    }

    std::streamsize showmanyc() {
        return in_end() - in_cur();
    }

    char_type* getBuffer(size_t& bufferSize) {
        bufferSize = m_bufferSize;
        return m_buffer;
    }

private:

    char_type* in_beg() { return Base::eback(); }
    char_type* in_cur() { return Base::gptr(); }
    char_type* in_end() { return Base::egptr(); }

    char_type* out_beg() { return Base::pbase(); }
    char_type* out_cur() { return Base::pptr(); }
    char_type* out_end() { return Base::epptr(); }

    void setIn(char_type* beg, char_type* cur, char_type* end) {
        Base::setg(beg, cur, end);
    }

    void setOut(char_type* beg, char_type* cur, char_type* end) {
        Base::setp(beg, end);
        Base::pbump(static_cast<int>(cur - beg));
    }

    char_type* m_buffer;
    size_t m_bufferSize;
    bool m_bufferOwner;

};

typedef basic_mstreambuf<char> mstreambuf;


/**
 * @brief Output stream using the dynamic memory stream buffer.
 */
struct omstream
    : virtual mstreambuf
    , std::ostream
{
    omstream()
        : mstreambuf()
        , std::ostream(static_cast<mstreambuf*>(this))
    {}

    omstream(char* mem, size_t size)
        : mstreambuf(mem, size)
        , std::ostream(static_cast<mstreambuf*>(this))
    {}

    omstream(basic_mstreambuf* stream)
        : std::ostream(stream) {
    }

    char* getBuffer(size_t& bufferSize) {
        mstreambuf* buf = dynamic_cast<mstreambuf*>(rdbuf());
        return buf->getBuffer(bufferSize);
    }
};

/**
 * @brief Input stream using the dynamic memory stream buffer.
 */
struct imstream
    : virtual mstreambuf
    , std::istream
{
    imstream()
        : mstreambuf()
        , std::istream(static_cast<mstreambuf*>(this))
    {}

    imstream(char* mem, size_t size)
        : mstreambuf(mem, size)
        , std::istream(static_cast<mstreambuf*>(this))
    {}

    imstream(basic_mstreambuf* stream)
        : std::istream(stream) {
    }

    char* getBuffer(size_t& bufferSize) {
        mstreambuf* buf = dynamic_cast<mstreambuf*>(rdbuf());
        return buf->getBuffer(bufferSize);
    }
};
