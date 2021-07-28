#pragma once

#include "shendk/files/file.h"

namespace shendk {

/**
 * @brief Shenmue weather description file.
 */
struct WDAT : public File {

    WDAT();
    WDAT(const std::string& filepath);
    ~WDAT();

    struct weather_header_t {
        int token;
        unsigned int offs;
    };
    struct weather_node_t
    {
        int dword0;
        int dword4;
    };
    struct weather_data_t
    {
        unsigned int* ptr;
        weather_node_t* data;
        char gap10[4];
        float float14 = 0.f;
        float float18 = 0.f;
    };
    struct weatherInfo {
    public:
        weather_data_t* RAIN;
        weather_data_t* CLUD;
        weather_data_t* SNOW;
        weather_data_t* FCAS;

        weatherInfo() {
            RAIN = new weather_data_t();
            CLUD = new weather_data_t();
            SNOW = new weather_data_t();
            FCAS = new weather_data_t();
        }
    };

    void ReadWeatherInternal(weather_header_t* InFileData);
    void ReadData(weather_data_t* inData, int InDateTime);
    void ReadDataForDateTime(weather_data_t* inData, int32_t InYear, uint32_t InMonth, uint32_t newDay, uint32_t InHour, uint32_t InMinute);

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);

    static const uint32_t signature = 0x54414457;  // "WDAT"
    static const uint32_t RAIN_identifier = 0x44554C43;
    static const uint32_t CLUD_identifier = 0x4E494152;
    static const uint32_t SNOW_identifier = 0x574F4E53;
    static const uint32_t FCAS_identifier = 0x53414346;

    weather_header_t* WDATRoot = nullptr;
    weather_header_t* CLUDNode = nullptr;
    weather_header_t* RAINNode = nullptr;
    weather_header_t* SNOWNode = nullptr;
    weather_header_t* FCASNode = nullptr;

    int CLUDData = 0x00000000;
    int RAINData = 0x00000000;
    int SNOWData = 0x00000000;
    int FCASData = 0x00000000;
    
    struct weather_info_t {
        unsigned int value[4];
    };
    std::vector<weather_info_t> values;
};

}
