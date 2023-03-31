#pragma once

#include <stdint.h>
#include <vector>
#include <map>

#include "shendk/files/file.h"
#include "shendk/types/model.h"

namespace shendk {
    constexpr uint16_t Pos_mask = 0x1C0;
    constexpr uint16_t PosX_msk = 0x100;
    constexpr uint16_t PosY_msk = 0x80;
    constexpr uint16_t PosZ_msk = 0x40;

    constexpr uint16_t Rot_mask = 0x38;
    constexpr uint16_t RotX_msk = 0x20;
    constexpr uint16_t RotY_msk = 0x10;
    constexpr uint16_t RotZ_msk = 0x08;



/**
 * @brief Shenmue animation file.
 */
struct MOTN : public File {
    struct Header {
        signed int seq_table_ofs = 0;
        signed int seq_name_ofs = 0;
        signed int seq_data_ofs = 0;
        signed int attributes = 0;
        signed int filelength = 0;
    } header;

    uint16_t sequenceCount() {
        return (header.attributes & 0x0FFF) - 1;
    }
    std::vector<Sequence> sequences;

    MOTN();
    MOTN(const std::string& filepath);
    MOTN(std::istream& stream);

    virtual ~MOTN();
    
protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);

private:
    const uint8_t countLookupTable[256] = {0,  1,  2,  3,  1,  2,  3,  4,  2,  3,  4,  5,  3,  4,  5,  6,
                                           1,  2,  3,  4,  2,  3,  4,  5,  3,  4,  5,  6,  4,  5,  6,  7,
                                           2,  3,  4,  5,  3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,
                                           3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,
                                           1,  2,  3,  4,  2,  3,  4,  5,  3,  4,  5,  6,  4,  5,  6,  7,
                                           2,  3,  4,  5,  3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,
                                           3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,
                                           4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10,
                                           2,  3,  4,  5,  3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,
                                           3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,
                                           4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10,
                                           5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10, 8,  9,  10, 11,
                                           3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,
                                           4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10,
                                           5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10, 8,  9,  10, 11,
                                           6,  7,  8,  9,  7,  8,  9,  10, 8,  9,  10, 11, 9,  10, 11, 12};

    uint8_t calcCountLookupTable(uint8_t index);

};

}
