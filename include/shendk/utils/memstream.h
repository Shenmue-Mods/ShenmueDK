#include <istream>
#include <ostream>
#include <streambuf>

struct imembuf : std::streambuf {
    imembuf(const char* base, size_t size) {
        char* p(const_cast<char*>(base));
        this->setg(p, p, p + size);
    }
};

struct imemstream : virtual imembuf, std::istream {
    imemstream(const char* mem, size_t size)
        : imembuf(mem, size)
        , std::istream(static_cast<std::streambuf*>(this))
    {}
};

struct omembuf : std::streambuf {
    omembuf(char* base, size_t size) {
        this->setp(base, base + size);
    }
};

struct omemstream : virtual omembuf, std::ostream {
    omemstream(char* mem, size_t size)
        : omembuf(mem, size)
        , std::ostream(static_cast<std::streambuf*>(this))
    {}
};
