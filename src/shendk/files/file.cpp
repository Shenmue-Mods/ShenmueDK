#include "shendk/files/file.h"

namespace shendk {

/**
* @brief Reads a file.
* @param filepath Path of the file.
**/
void File::read(const std::string& filepath) {
    if (!fs::exists(filepath)) return;
    this->filepath = filepath;
    std::ifstream fstream(filepath, std::ios::binary);
    if (fstream.is_open()) {
        read(fstream);
        fstream.close();
    } else {
        throw new std::runtime_error("Couldn't open file: " + filepath + "\n");
    }
}

void File::read(std::istream& stream) {
    baseOffset = stream.tellg();
    _read(stream);
}

/**
* @brief Writes a file.
* @param filepath Path of the file.
**/
void File::write(const std::string& filepath) {
    this->filepath = filepath;
    std::ofstream fstream(filepath, std::ios::binary);
    if (fstream.is_open()) {
        write(fstream);
        fstream.close();
    }
    else {
        throw new std::runtime_error("Couldn't open file: " + filepath + "\n");
    }
}

void File::write(std::ostream& stream) {
    baseOffset = stream.tellp();
    _write(stream);
}

/**
* @brief Verifies a file's signature
* @param signature Signature to verify
**/
bool File::isValid(unsigned int signature) {
    return _isValid(signature);
}

}
