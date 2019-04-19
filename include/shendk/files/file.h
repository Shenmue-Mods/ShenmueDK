#include <string>
#include <fstream>

namespace shendk {

	/**
	* @brief Base class for all file classes
	**/
	class File {
	public:
		File() {}
		virtual ~File() {}

		/**
		* @brief Reads a file.
		* @param filepath Path of the file.
		**/
		void read(const std::string& filepath) {
			std::ifstream fstream;
			fstream.open(filepath);
			if (fstream.is_open()) {
				_read(fstream);
				fstream.close();
			}
			else {
				throw new std::runtime_error("Couldn't open file: " + filepath + "\n");
			}
		}

		/**
		* @brief Writes a file.
		* @param filepath Path of the file.
		**/
		void write(const std::string& filepath) {
			std::ofstream fstream;
			fstream.open(filepath);
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