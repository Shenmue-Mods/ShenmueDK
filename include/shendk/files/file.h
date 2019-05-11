#pragma once

#include <string>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace shendk {

/**
* @brief Base class for all file classes
**/
struct File {

    virtual ~File() {}

    /**
    * @brief Reads a file.
    * @param filepath Path of the file.
    **/
    void read(const std::string& filepath);

    void read(std::istream& stream);

    /**
    * @brief Writes a file.
    * @param filepath Path of the file.
    **/
    void write(const std::string& filepath);

    void write(std::ostream& stream);

    /**
    * @brief Verifies a file's signature
    * @param signature Signature to verify
    **/
    bool isValid(unsigned int signature);

    std::string filepath;

protected:
    virtual void _read(std::istream& stream) = 0;
    virtual void _write(std::ostream& stream) = 0;
    virtual bool _isValid(uint32_t signature) = 0;

    int64_t baseOffset;
};

}
