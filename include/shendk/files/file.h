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
                _read(fstream);
                fstream.close();
            } else {
                throw new std::runtime_error("Couldn't open file: " + filepath + "\n");
            }
        }

        /**
        * @brief Writes a file.
        * @param filepath Path of the file.
        **/
        void write(const std::string& filepath) {
            std::ofstream fstream(filepath, std::ios::binary);
            if (fstream.is_open()) {
                _write(fstream);
                fstream.close();
            }
            else {
                throw new std::runtime_error("Couldn't open file: " + filepath + "\n");
            }
        }

	protected:
        virtual void _read(std::ifstream& stream) = 0;
        virtual void _write(std::ofstream& stream) = 0;
	};
}
