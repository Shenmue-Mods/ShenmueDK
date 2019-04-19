#include <stdint.h>
#include <string>

namespace shendk {

	class MurmurHash2 {

	public:
		/**
		* @brief Hashes the input data by the given length
		* @param data
		* @param length
		*/
		static uint32_t hashData(const uint8_t* data, uint32_t length);

		/**
		* @brief Hashes the input filename
		* @param filename Filename that will be hashed
		* @param toLower True for converting filename to lowercase
		*/
		static uint32_t hashFilenamePlain(std::string filename, bool toLower = true);

		/**
		* @brief Hashes the input filename
		* @param filename Filename that will be hashed
		* @param hasHash True if the filename has a hash in it
		*/
		//static uint32_t hashFilenameFull(const std::string& filename, bool hasHash = true);

		/**
		* @brief Creates a filename with the hash
		* @param filename Filename that will be used as a base
		* @param hash Hash of the filename
		* @param includeHash True for including hash in filename
		*/
		//static std::string createFilename(const std::string& filename, uint32_t hash, bool includeHash = true);

	private:
		static const uint32_t m_initializationSeed = 0x066EE5D0;
		static const uint32_t m_multiplier = 0x5BD1E995;
		static const int32_t m_rotationAmount = 0x18;
		static const std::string m_prefix;
	};

}