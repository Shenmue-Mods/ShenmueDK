#pragma once

#include "shendk/files/model_file.h"
#include "shendk/files/model/mt5/ptrl.h"
#include "shendk/files/model/mt5/texl.h"
#include "shendk/files/model/mt5/texd.h"
#include "shendk/files/model/mt5/name.h"
#include "shendk/node/texn.h"

namespace shendk {

/**
 * @brief SM1 model container file
 */
struct MT5 : public ModelFile {
    const static uint32_t signature = 1296257608;

    struct Header {
        uint32_t signature;
        uint32_t nodesSize;
		uint32_t firstNodeOffset;
    };

    MT5();
    MT5(const std::string& filepath);
    ~MT5();

	MT5::Header header;

    // texture stuff
    std::vector<TEXN> texnEntries;
    mt5::TEXD* texd = nullptr;
    mt5::TEXL* texl = nullptr;
    mt5::PTRL* ptrl = nullptr;
    mt5::NAME* name = nullptr;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);

};
}
