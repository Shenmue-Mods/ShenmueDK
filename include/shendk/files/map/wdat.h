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

    #define MIN_MASK    0x3F
    #define HOUR_MASK   0x7C0
    #define DAY_MASK    0xF800
    #define MONTH_MASK  0xF0000
    #define YEAR_MASK   0x7FF00000

    struct weather_header_t {
        int token;
        unsigned int offs;
    };
    struct weather_data_t
    {
        unsigned __int8 val;
        unsigned __int8 val2;
        unsigned __int8 val3;
        unsigned __int8 val4;
    };
    struct weather_runtime_data_t
    {
        shendk::WDAT::weather_data_t* ptr;
        shendk::WDAT::weather_data_t* data_ptr;
        unsigned int num_entries;
        float date_time_difference;
        float value;
    };
    struct weather_global_data_t
    {
    public:
        signed int year_offset = 86;
        signed int year = 0;
        signed int month = 1;
        signed int day = 1;
        weather_runtime_data_t* CLUD;
        weather_runtime_data_t* RAIN;
        weather_runtime_data_t* SNOW;
        weather_runtime_data_t* FCAS;
        
        weather_global_data_t() {
            CLUD = new weather_runtime_data_t();
            RAIN = new weather_runtime_data_t();
            SNOW = new weather_runtime_data_t();
            FCAS = new weather_runtime_data_t();
        }
    };

    void ReadWeatherInternal(weather_header_t* InFileData);
    void ReadData(weather_runtime_data_t* weather_data, int InDateTime);
    void ReadDataForDateTime(weather_global_data_t* inData, int32_t InYear, uint32_t InMonth, uint32_t newDay, uint32_t InHour, uint32_t InMinute);

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
        uint32_t minute;
        uint32_t hour;
        uint32_t day;
        uint32_t month;
        uint32_t year;

        float value;
        unsigned char val_raw;
    };

    struct weather_infos_t {
        std::vector < weather_info_t > CLUD;
        std::vector < weather_info_t > RAIN;
        std::vector < weather_info_t > SNOW;
        std::vector < weather_info_t > FCAS;
    };


    weather_infos_t weather_data_collection;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);

    static const uint32_t signature = 0x54414457;  // "WDAT"
    static const uint32_t RAIN_identifier = 0x44554C43;
    static const uint32_t CLUD_identifier = 0x4E494152;
    static const uint32_t SNOW_identifier = 0x574F4E53;
    static const uint32_t FCAS_identifier = 0x53414346;

};

}
