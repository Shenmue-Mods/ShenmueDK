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
        void read(const std::string& filepath) {
            if (!fs::exists(filepath)) return;
            std::ifstream fstream(filepath, std::ios::binary);
            if (fstream.is_open()) {
                read(fstream);
                fstream.close();
            } else {
                throw new std::runtime_error("Couldn't open file: " + filepath + "\n");
            }
        }

        void read(std::istream& stream) {
            baseOffset = stream.tellg();
            _read(stream);
        }

        /**
        * @brief Writes a file.
        * @param filepath Path of the file.
        **/
        void write(const std::string& filepath) {
            std::ofstream fstream(filepath, std::ios::binary);
            if (fstream.is_open()) {
                write(fstream);
                fstream.close();
            }
            else {
                throw new std::runtime_error("Couldn't open file: " + filepath + "\n");
            }
        }

        void write(std::ostream& stream) {
            baseOffset = stream.tellp();
            _write(stream);
        }

		/**
		* @brief Verifies a file's signature
		* @param signature Signature to verify
		**/
		bool isValid(unsigned int signature) {
			return _isValid(signature);
		}

	protected:
        virtual bool _isValid(unsigned int signature) = 0;
        virtual void _read(std::istream& stream) = 0;
        virtual void _write(std::ostream& stream) = 0;

        int64_t baseOffset;
	};
}
