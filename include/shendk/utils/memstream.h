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
        this->setp(m_buffer, m_buffer + m_bufferSize);
        this->setg(m_buffer, m_buffer, m_buffer + m_bufferSize);
    }

    basic_mstreambuf(char_type* buffer, size_t size) {
        m_bufferOwner = false; // TODO: take ownership of memory?
        m_buffer = buffer;
        m_bufferSize = size;
        this->setp(m_buffer, m_buffer + m_bufferSize);
        this->pbump(m_bufferSize);
        this->setg(m_buffer, m_buffer, m_buffer + m_bufferSize);
    }

    ~basic_mstreambuf() {
        if (m_bufferOwner) {
            delete[] m_buffer;
        }
    }

    int overflow( int c = EOF )
    {
        if (Base::_M_out_cur >= Base::_M_out_beg + m_bufferSize) {
            size_t oldSize = m_bufferSize;
            m_bufferSize = m_bufferSize << 1;
            if (m_bufferSize >= SIZE_MAX) {
                return Base::overflow(c);
            } else {
                // allocate more memory
                m_buffer = reinterpret_cast<char*>(realloc(m_buffer, m_bufferSize));
                this->setp(m_buffer, m_buffer + m_bufferSize);
                this->pbump(oldSize);
                this->setg(m_buffer, m_buffer, m_buffer + m_bufferSize);
                std::memset(m_buffer + oldSize, 0, m_bufferSize - oldSize);
            }
        } else {
            Base::_M_out_end += 1;
            Base::_M_in_end += 1;
        }

        // write character
        *this->pptr() = static_cast<char_type>(c);
        this->pbump(1);
        return c;
    }

    int underflow() {
        return traits_type::eof();
    }

    pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                     std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
    {
        if (dir == std::ios_base::beg) {
            if (Base::_M_in_beg + off > Base::_M_in_end ||
                Base::_M_in_beg + off < Base::_M_in_beg) {
                return -1;
            }
            if (mode == std::ios_base::in) {
                Base::_M_in_cur = Base::_M_in_beg + off;
            }
            if (mode == std::ios_base::out) {
                Base::_M_out_cur = Base::_M_in_beg + off;
            }
        } else if (dir == std::ios_base::cur) {
            if (mode == std::ios_base::in) {
                if (Base::_M_in_cur + off > Base::_M_in_end ||
                    Base::_M_in_cur + off < Base::_M_in_beg) {
                    return -1;
                }
                Base::_M_in_cur = Base::_M_in_cur + off;
            }
            if (mode == std::ios_base::out) {
                if (Base::_M_out_cur + off > Base::_M_in_end ||
                    Base::_M_out_cur + off < Base::_M_in_beg) {
                    return -1;
                }
                Base::_M_out_cur = Base::_M_out_cur + off;
            }
        } else if (dir == std::ios_base::end) {
            if (mode == std::ios_base::in) {
                Base::_M_in_cur = Base::_M_in_end - off;
            }
            if (mode == std::ios_base::out) {
                Base::_M_out_cur = Base::_M_in_end - off;
            }
        }
        return 0;
    }

    pos_type seekpos(pos_type pos, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) {
        if (Base::_M_in_beg + pos > Base::_M_in_end ||
            Base::_M_in_beg + pos < Base::_M_in_beg) {
            return -1;
        }
        if (mode == std::ios_base::in) {
            Base::_M_in_cur = Base::_M_in_beg + pos;
        }
        if (mode == std::ios_base::out) {
            Base::_M_out_cur = Base::_M_in_beg + pos;
        }
        return 0;
    }

    int sync()
    {
        return 0;
    }

    ptrdiff_t showmanyc() {
        return this->egptr() - this->gptr();
    }

    char_type* getBuffer(size_t& bufferSize) {
        bufferSize = m_bufferSize;
        return m_buffer;
    }

private:
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

    char* getBuffer(uint64_t& bufferSize) {
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

    char* getBuffer(uint64_t& bufferSize) {
        mstreambuf* buf = dynamic_cast<mstreambuf*>(rdbuf());
        return buf->getBuffer(bufferSize);
    }
};
