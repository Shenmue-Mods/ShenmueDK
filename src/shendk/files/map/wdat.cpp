#include "shendk/files/map/wdat.h"

#include <Windows.h>
//#define ORIG_READER

namespace shendk {

    WDAT::WDAT() = default;
    WDAT::WDAT(const std::string& filepath) { read(filepath); }
    WDAT::~WDAT() {}

    void WDAT::ReadWeatherInternal(shendk::WDAT::weather_header_t* InFileData)
    {
        shendk::WDAT::weather_header_t* WDAT, *CLUD, *RAIN, *SNOW, *FCAS;

        int tmpToken;
        __int64 tmpFileOffs, offs, offs_1, offs_2, offs_3;

        tmpToken = InFileData->token;
        WDAT = InFileData;
        WDATRoot = InFileData;
        if (tmpToken != 'TADW')
        {
            do
            {
                tmpFileOffs = WDAT->offs;
                if (!(int)tmpFileOffs)
                    break;
                WDAT = (weather_header_t*)((char*)WDAT + tmpFileOffs);
            } while (WDAT->token != 'TADW');
        }
        FCAS = 0i64;
        CLUD = InFileData;

        if (tmpToken != 'DULC')
        {
            while (1)
            {
                offs_1 = CLUD->offs;
                if (!(int)offs_1)
                    break;
                CLUD = (weather_header_t*)((char*)CLUD + offs_1);
                if (CLUD->token == 'DULC')
                    goto SET_CLUD;
            }
            CLUD = 0i64;
        }
    SET_CLUD:
        CLUDData = (signed int)CLUD->offs / 4 - 2;
        CLUDNode = CLUD + 1;
        RAIN = InFileData;

        if (tmpToken != 'NIAR')
        {
            while (1)
            {
                offs_2 = RAIN->offs;
                if (!(int)offs_2)
                    break;
                RAIN = (weather_header_t*)((char*)RAIN + offs_2);
                if (RAIN->token == 'NIAR')
                    goto SET_RAIN;
            }
            RAIN = 0i64;
        }
    SET_RAIN:
        RAINData = (signed int)RAIN->offs / 4 - 2;
        RAINNode = RAIN + 1;
        SNOW = InFileData;

        if (tmpToken != 'WONS')
        {
            while (1)
            {
                offs = SNOW->offs;
                if (!(int)offs)
                    break;
                SNOW = (weather_header_t*)((char*)SNOW + offs);
                if (SNOW->token == 'WONS')
                    goto SET_SNOW;
            }
            SNOW = 0i64;
        }
    SET_SNOW:
        SNOWData = (signed int)SNOW->offs / 4 - 2;
        SNOWNode = SNOW + 1;

        if (tmpToken == 'SACF')
        {
        SET_FCAS:
            FCAS = InFileData;
        }
        else
        {
            while (1)
            {
                offs_3 = InFileData->offs;
                if (!(int)offs_3)
                    break;
                InFileData = (weather_header_t*)((char*)InFileData + offs_3);
                if (InFileData->token == 'SACF')
                    goto SET_FCAS;
            }
        }
        FCASData = (signed int)FCAS->offs / 4 - 2;
        FCASNode = FCAS + 1;
    }
    void WDAT::ReadData(weather_runtime_data_t* weather_data, int InDateTime)
    {
        weather_data_t* curr, * next;
        unsigned int dateTime, it_dateTime;
        float date_time, curr_value, next_value;

        curr = weather_data->data_ptr;
        for (next = curr + 1; ; --next) {
            while (1) {
                dateTime = *(unsigned int*)curr & 0xFFFFFF;
                it_dateTime = *(unsigned int*)next & 0xFFFFFF;
                if (it_dateTime > InDateTime)
                    break;
                ++curr;
                ++next;
            }

            if (InDateTime >= dateTime)
                break;
            --curr;
        }
        weather_data->data_ptr = curr;

        date_time = (float)(int)(InDateTime - dateTime) / (float)(int)(it_dateTime - dateTime);
        weather_data->date_time_difference = date_time;

        curr_value = (float)curr->val4 / 255.0;
        next_value = (float)((float)next->val4 / 255.0) - curr_value;

        float abs_next_value = fabs(next_value);
        if (abs_next_value < 0.00000011920929 || (next_value = next_value * date_time, abs_next_value = fabs(next_value), abs_next_value < 0.00000011920929))
            next_value = 0.0;

        weather_data->value = next_value + curr_value;
    }
    void WDAT::ReadDataForDateTime(weather_global_data_t* in_data, int32_t inYear, uint32_t inMonth, uint32_t inDay, uint32_t inHour, uint32_t inMinute)
    {
        unsigned int packedDateTime = 0;
        signed int newYear = inYear - in_data->year_offset;
        uint32_t newHour = 0, newMinute = 0, newMonth = inMonth;
        
        // process 
        if (newYear >= 0)
        {
            if (newYear <= 15)
            {
                if ((int)inMonth < 1)
                {
                    newMonth = 1;
                    newHour = 0;
                    inDay = 1;
                    newMinute = 0;
                    goto PROCESS;
                }
                if ((int)inMonth <= 12)
                {
                    if ((int)inDay < 1)
                    {
                        newHour = 0;
                        inDay = 1;
                        newMinute = 0;
                        goto PROCESS;
                    }
                    if ((int)inDay <= 31)
                    {
                        newHour = inHour;
                        if ((inHour & 0x80000000) != 0)
                        {
                            newHour = 0;
                            newMinute = 0;
                            goto PROCESS;
                        }
                        if ((int)inHour <= 23)
                        {
                            newMinute = inMinute;
                            if ((inMinute & 0x80000000) != 0)
                            {
                                newMinute = 0;
                                goto PROCESS;
                            }
                            if ((int)inMinute <= 59)
                                goto PROCESS;
                        }
                        else
                        {
                            newHour = 23;
                        }
                    }
                    else
                    {
                        inDay = 31;
                        newHour = 23;
                    }
                }
                else
                {
                    newMonth = 12;
                    inDay = 31;
                    newHour = 23;
                }
            }
            else
            {
                newYear = 15;
                newMonth = 12;
                inDay = 31;
                newHour = 23;
            }
            newMinute = 59;
            goto PROCESS;
        }
        newYear = 0;
        newHour = 0;
        newMinute = 0;
        newMonth = 1;
        inDay = 1;
    PROCESS:
        in_data->year = newYear;
        in_data->month = newMonth;
        in_data->day = inDay;
        packedDateTime = (newMinute | ((newHour | ((unsigned __int8)newMonth << 10)) << 6)) & 0xFFFFFF | ((inDay & 0x1FFF | ((newYear & 0xF) << 9)) << 11);
        WDAT::ReadData(in_data->CLUD, packedDateTime);
        WDAT::ReadData(in_data->RAIN, packedDateTime);
        WDAT::ReadData(in_data->SNOW, packedDateTime);
        WDAT::ReadData(in_data->FCAS, packedDateTime);
    }

    void WDAT::_read(std::istream& stream) {
        int signature = -1;
        stream.read(reinterpret_cast<char*>(&signature), 4);
        if (!isValid(signature))
            return;

        stream.seekg(0, std::ios::end);
        int size = (int)stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::vector<char> bytes = std::vector<char>(size);
        stream.read(&bytes[0], size);
        if (bytes.size()) {
            weather_global_data_t * weather_data = new weather_global_data_t();
            ReadWeatherInternal((weather_header_t*)bytes.data());
            printf("RAINData = %d\n", RAINData);
            printf("CLUDData = %d\n", CLUDData);
            printf("SNOWData = %d\n", SNOWData);
            printf("FCASData = %d\n", FCASData);

            weather_data->RAIN->data_ptr = (weather_data_t*)RAINNode;
            weather_data->RAIN->num_entries = RAINData;
            weather_data->RAIN->ptr = (weather_data_t*)RAINNode;

            weather_data->CLUD->data_ptr = (weather_data_t*)CLUDNode;
            weather_data->CLUD->num_entries = CLUDData;
            weather_data->CLUD->ptr = (weather_data_t*)CLUDNode;

            weather_data->SNOW->data_ptr = (weather_data_t*)SNOWNode;
            weather_data->SNOW->num_entries = SNOWData;
            weather_data->SNOW->ptr = (weather_data_t*)SNOWNode;

            weather_data->FCAS->data_ptr = (weather_data_t*)FCASNode;
            weather_data->FCAS->num_entries = FCASData;
            weather_data->FCAS->ptr = (weather_data_t*)FCASNode;

#ifdef ORIG_READER
            // Jan 7th 1987, 11:00AM
            uint32_t year = 87, month = 1, day = 7, hour = 11, minute = 0;
            printf("%02d/%02d/%04d %02d:%02d\n", day, month, 1900 + year, hour, minute);
            ReadDataForDateTime(weather_data, year, month, day, hour, minute);

            printf("Data\n");
            printf("CLUDValue = %f\n", weather_data->CLUD->value);
            printf("RAINValue = %f\n", weather_data->RAIN->value);
            printf("SNOWValue = %f\n", weather_data->SNOW->value);
            printf("FCASValue = %f\n", weather_data->FCAS->value);
            return;
#else
            weather_data_t* curr = (weather_data_t*)CLUDNode;
            for (int entry = 0; entry < weather_data->CLUD->num_entries; ++entry) {
                unsigned int packedDateTime = *(unsigned int*)curr & 0xFFFFFF;
                uint32_t    minute = packedDateTime & MIN_MASK,
                    hour = (packedDateTime & HOUR_MASK) >> 6,
                    day = (packedDateTime & DAY_MASK) >> 11,
                    month = (packedDateTime & MONTH_MASK) >> 16,
                    year = 1986 + ((packedDateTime & YEAR_MASK) >> 20);

                weather_info_t info;
                info.minute = minute;
                info.day = day;
                info.hour = hour;
                info.month = month;
                info.year = year;
                info.value = (float)curr->val4 / 255.0;
                info.val_raw = curr->val4;

                printf("%02d/%02d/%04d %02d:%02d [raw: 0x%08X | 0x%08X]\n", day, month, year, hour, minute, *(unsigned int*)curr, packedDateTime);
                printf("value = %f (raw: %08X)\n", info.value, info.val_raw);
                weather_data_collection.CLUD.push_back(info);
                curr++;
            }

            curr = (weather_data_t*)RAINNode;
            for (int entry = 0; entry < weather_data->RAIN->num_entries; ++entry) {
                unsigned int packedDateTime = *(unsigned int*)curr & 0xFFFFFF;
                uint32_t    minute = packedDateTime & MIN_MASK,
                    hour = (packedDateTime & HOUR_MASK) >> 6,
                    day = (packedDateTime & DAY_MASK) >> 11,
                    month = (packedDateTime & MONTH_MASK) >> 16,
                    year = 1986 + ((packedDateTime & YEAR_MASK) >> 20);

                weather_info_t info;
                info.minute = minute;
                info.day = day;
                info.hour = hour;
                info.month = month;
                info.year = year;
                info.value = (float)curr->val4 / 255.0;
                info.val_raw = curr->val4;

                printf("%02d/%02d/%04d %02d:%02d [raw: 0x%08X | 0x%08X]\n", day, month, year, hour, minute, *(unsigned int*)curr, packedDateTime);
                printf("value = %f (raw: %08X)\n", info.value, info.val_raw);
                weather_data_collection.RAIN.push_back(info);
                curr++;
            }

            curr = (weather_data_t*)SNOWNode;
            for (int entry = 0; entry < weather_data->SNOW->num_entries; ++entry) {
                unsigned int packedDateTime = *(unsigned int*)curr & 0xFFFFFF;
                uint32_t    minute = packedDateTime & MIN_MASK,
                    hour = (packedDateTime & HOUR_MASK) >> 6,
                    day = (packedDateTime & DAY_MASK) >> 11,
                    month = (packedDateTime & MONTH_MASK) >> 16,
                    year = 1986 + ((packedDateTime & YEAR_MASK) >> 20);

                weather_info_t info;
                info.minute = minute;
                info.day = day;
                info.hour = hour;
                info.month = month;
                info.year = year;
                info.value = (float)curr->val4 / 255.0;
                info.val_raw = curr->val4;

                printf("%02d/%02d/%04d %02d:%02d [raw: 0x%08X | 0x%08X]\n", day, month, year, hour, minute, *(unsigned int*)curr, packedDateTime);
                printf("value = %f (raw: %08X)\n", info.value, info.val_raw);
                weather_data_collection.SNOW.push_back(info);
                curr++;
            }

            curr = (weather_data_t*)FCASNode;
            for (int entry = 0; entry < weather_data->FCAS->num_entries; ++entry) {
                unsigned int packedDateTime = *(unsigned int*)curr & 0xFFFFFF;
                uint32_t    minute = packedDateTime & MIN_MASK,
                    hour = (packedDateTime & HOUR_MASK) >> 6,
                    day = (packedDateTime & DAY_MASK) >> 11,
                    month = (packedDateTime & MONTH_MASK) >> 16,
                    year = 1986 + ((packedDateTime & YEAR_MASK) >> 20);

                weather_info_t info;
                info.minute = minute;
                info.day = day;
                info.hour = hour;
                info.month = month;
                info.year = year;
                info.value = (float)curr->val4 / 255.0;
                info.val_raw = curr->val4;

                printf("%02d/%02d/%04d %02d:%02d [raw: 0x%08X | 0x%08X]\n", day, month, year, hour, minute, *(unsigned int*)curr, packedDateTime);
                printf("value = %f (raw: %08X)\n", info.value, info.val_raw);
                weather_data_collection.FCAS.push_back(info);
                curr++;
            }
#endif
        }
    }

    void WDAT::_write(std::ostream& stream) {
        if (weather_data_collection.CLUD.size()) {
            stream << "CLUD_Day, CLUD_Month, CLUD_Year, CLUD_Hour, CLUD_Minute, CLUD_Value, CLUD_ValueRaw" << std::endl;
            for (auto& entry : weather_data_collection.CLUD) {
                stream << entry.day << "," << entry.month << "," << entry.year << "," << entry.hour << "," << entry.minute << "," << std::to_string(entry.value) << "," << std::to_string(entry.val_raw) << std::endl;
            }
        }
        if (weather_data_collection.RAIN.size()) {
            stream << "RAIN_Day, RAIN_Month, RAIN_Year, RAIN_Hour, RAIN_Minute, RAIN_Value, RAIN_ValueRaw" << std::endl;
            for (auto& entry : weather_data_collection.RAIN) {
                stream << entry.day << "," << entry.month << "," << entry.year << "," << entry.hour << "," << entry.minute << "," << std::to_string(entry.value) << "," << std::to_string(entry.val_raw) << std::endl;
            }
        }
        if (weather_data_collection.SNOW.size()) {
            stream << "SNOW_Day, SNOW_Month, SNOW_Year, SNOW_Hour, SNOW_Minute, SNOW_Value, SNOW_ValueRaw" << std::endl;
            for (auto& entry : weather_data_collection.SNOW) {
                stream << entry.day << "," << entry.month << "," << entry.year << "," << entry.hour << "," << entry.minute << "," << std::to_string(entry.value) << "," << std::to_string(entry.val_raw) << std::endl;
            }
        }
        if (weather_data_collection.FCAS.size()) {
            stream << "FCAS_Day, FCAS_Month, FCAS_Year, FCAS_Hour, FCAS_Minute, FCAS_Value, FCAS_ValueRaw" << std::endl;
            for (auto& entry : weather_data_collection.FCAS) {
                stream << entry.day << "," << entry.month << "," << entry.year << "," << entry.hour << "," << entry.minute << "," << std::to_string(entry.value) << "," << std::to_string(entry.val_raw) << std::endl;
            }
        }
    }

    bool WDAT::_isValid(uint32_t signature) {
        if (signature != WDAT::signature)
            return false;
        return true;
    }

}
