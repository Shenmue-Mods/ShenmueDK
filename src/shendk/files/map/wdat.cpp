#include "shendk/files/map/wdat.h"

#include <Windows.h>

namespace shendk {

    WDAT::WDAT() = default;
    WDAT::WDAT(const std::string& filepath) { read(filepath); }
    WDAT::~WDAT() {}

    void WDAT::ReadWeatherInternal(weather_header_t* InFileData)
    {
        int tmpToken;
        __int64 tmpFileOffs; 

        tmpToken = InFileData->token;
        WDATRoot = InFileData;
        if (tmpToken != 0x54414457)
        {
            do
            {
                tmpFileOffs = InFileData->offs;
                if (!(signed int)tmpFileOffs)
                    break;
                InFileData = (weather_header_t*)((char*)InFileData + tmpFileOffs);
            } while (InFileData->token != 0x54414457);
        }

        if (tmpToken != 0x44554C43)
        {
            while (1)
            {
                tmpFileOffs = InFileData->offs;
                if (!(signed int)tmpFileOffs)
                    break;
                InFileData = (weather_header_t*)((char*)InFileData + tmpFileOffs);
                if (InFileData->token == 0x44554C43)
                    goto LABEL_9;
            }
        }
    LABEL_9:
        CLUDData = (signed int)InFileData->offs / 4 - 2;
        CLUDNode = &InFileData[1];
        if (tmpToken != 0x4E494152)
        {
            while (1)
            {
                tmpFileOffs = InFileData->offs;
                if (!(signed int)tmpFileOffs)
                    break;
                InFileData = (weather_header_t*)((char*)InFileData + tmpFileOffs);
                if (InFileData->token == 0x4E494152)
                    goto LABEL_14;
            }
        }
    LABEL_14:
        RAINData = (signed int)InFileData->offs / 4 - 2;
        RAINNode = &InFileData[1];
        if (tmpToken != 0x574F4E53)
        {
            while (1)
            {
                tmpFileOffs = InFileData->offs;
                if (!(signed int)tmpFileOffs)
                    break;
                InFileData = (weather_header_t*)((char*)InFileData + tmpFileOffs);
                if (InFileData->token == 0x574F4E53)
                    goto LABEL_19;
            }
        }
    LABEL_19:
        SNOWData = (signed int)InFileData->offs / 4 - 2;
        SNOWNode = &InFileData[1];

        if (tmpToken != 0x53414346)
        {
            do
            {
                tmpFileOffs = InFileData->offs;
                if (!(signed int)tmpFileOffs)
                    break;
                InFileData = (weather_header_t*)((char*)InFileData + tmpFileOffs);
            } while (InFileData->token != 0x53414346);
        }
        FCASData = (signed int)InFileData->offs / 4 - 2;
        FCASNode = &InFileData[1];
    }
    void WDAT::ReadData(weather_data_t* inData, int InDateTime)
    {
        weather_node_t* tmpPtr; 
        int* i; 
        unsigned int v4, tmpDateTime; 
        float v6, v7, v8, v9, v10, v11; 

        tmpPtr = inData->data;
        for (i = &tmpPtr->dword4; ; --i)
        {
            while (1)
            {
                v4 = tmpPtr->dword0 & 0xFFFFFF;
                tmpDateTime = *i & 0xFFFFFF;
                if (tmpDateTime > InDateTime)
                    break;
                tmpPtr = (weather_node_t*)((char*)tmpPtr + 4);
                ++i;
            }
            if (InDateTime >= v4)
                break;
            tmpPtr = CONTAINING_RECORD(tmpPtr, weather_node_t, dword4);
        }
        inData->data = tmpPtr;
        v6 = (float)(int)(InDateTime - v4) / (float)(int)(tmpDateTime - v4);
        inData->float14 = v6;
        v7 = (float)(char)(tmpPtr->dword0) / 255.0;
        v8 = (float)((float)*((unsigned __int8*)i + 3) / 255.0) - v7;
        v9 = fabs(v8);
        if (v9 < 0.00000011920929 || (v10 = v8 * v6, v11 = fabs(v10), v11 < 0.00000011920929))
            v10 = 0.0;
        inData->float18 = v10 + v7;
        printf("%f\n", inData->float14);
        printf("%f\n", inData->float18);
    }
    void WDAT::ReadDataForDateTime(weather_data_t* inData, int32_t InYear, uint32_t InMonth, uint32_t newDay, uint32_t InHour, uint32_t InMinute)
    {
        uint32_t packedDateTime = 0, newMonth = 0, newHour = 0, newMinute = 0, newYear = 0;

        newYear = InYear - 0;
        newMonth = InMonth;
        if (newYear >= 0)
        {
            if (newYear <= 15)
            {
                if (InMonth < 1)
                {
                    newMonth = 1;
                    newHour = 0;
                    newDay = 1;
                    newMinute = 0;
                    goto LABEL_21;
                }
                if (InMonth <= 12)
                {
                    if (newDay < 1)
                    {
                        newHour = 0;
                        newDay = 1;
                        newMinute = 0;
                        goto LABEL_21;
                    }
                    if (newDay <= 31)
                    {
                        newHour = InHour;
                        if ((InHour & 0x80000000) != 0)
                        {
                            newHour = 0;
                            newMinute = 0;
                            goto LABEL_21;
                        }
                        if (InHour <= 23)
                        {
                            newMinute = InMinute;
                            if ((InMinute & 0x80000000) != 0)
                            {
                                newMinute = 0;
                                goto LABEL_21;
                            }
                            if (InMinute <= 59)
                                goto LABEL_21;
                        }
                        else
                        {
                            newHour = 23;
                        }
                    }
                    else
                    {
                        newDay = 31;
                        newHour = 23;
                    }
                }
                else
                {
                    newMonth = 12;
                    newDay = 31;
                    newHour = 23;
                }
            }
            else
            {
                newYear = 15;
                newMonth = 12;
                newDay = 31;
                newHour = 23;
            }
            newMinute = 59;
            goto LABEL_21;
        }
        newYear = 0;
        newHour = 0;
        newMinute = 0;
        newMonth = 1;
        newDay = 1;
    LABEL_21:
        packedDateTime = (unsigned int)(((unsigned char)newMinute | (((unsigned char)newHour & 0xF | ((unsigned char)newMonth << 10)) << 6)) & 0xFFFFFF | (((unsigned char)newDay & 0x1FFF | (((unsigned char)newYear & 0xF) << 9)) << 11));
        WDAT::ReadData(inData, packedDateTime);
    }

    void WDAT::_read(std::istream& stream) {
        int signature = -1;
        stream.read(reinterpret_cast<char*>(&signature), 4);
        if (!isValid(signature))
            return;

        stream.seekg(0, std::ios::end);
        int size = (int)stream.tellg();
        stream.seekg(0, std::ios::beg);
//#define ORIG_READER
#ifdef ORIG_READER
        std::vector<char> bytes = std::vector<char>(size);
        stream.read(&bytes[0], size);
        if (bytes.size()) {
            ReadWeatherInternal((weather_header_t*)bytes.data());
            printf("RAINData = %d\n", RAINData);
            printf("CLUDData = %d\n", CLUDData);
            printf("SNOWData = %d\n", SNOWData);
            printf("FCASData = %d\n", FCASData);

            WDAT::weatherInfo test;
            test.RAIN->data = (weather_node_t*)RAINNode;
            test.CLUD->data = (weather_node_t*)CLUDNode;
            test.SNOW->data = (weather_node_t*)SNOWNode;
            test.FCAS->data = (weather_node_t*)FCASNode;

            printf("RAIN\n");
            ReadDataForDateTime(test.RAIN, 1, 1, 7, 11, 00);

            printf("CLUD\n");
            ReadDataForDateTime(test.CLUD, 1, 1, 7, 11, 00);

            printf("SNOW\n");
            ReadDataForDateTime(test.SNOW, 1, 1, 7, 11, 00);

            printf("FCAS\n");
            ReadDataForDateTime(test.FCAS, 1, 1, 7, 11, 00);
        }
#else
        stream.seekg(8, std::ios::beg);
        while ((int)stream.tellg() < size) {
            unsigned int tmp = 0x00;
            stream.read(reinterpret_cast<char*>(&tmp), 4);

            signed int chunkSize = 0;
            if (tmp == RAIN_identifier || tmp == CLUD_identifier ||
                tmp == SNOW_identifier || tmp == FCAS_identifier) {
                unsigned int chunkStart = (int)stream.tellg();
                chunkSize = 0x00;
                stream.read(reinterpret_cast<char*>(&chunkSize), 4);
            }

            if (chunkSize) {
                int entrycalc = (chunkSize / 4) / 4;
                weather_info_t info;
                for (int i = 0; i < entrycalc; ++i) {
                    unsigned int value = 0x00;
                    stream.read(reinterpret_cast<char*>(&value), 4);
                    printf("val = %d\n", value);
                    info.value[0] = value;

                    value = 0x00;
                    stream.read(reinterpret_cast<char*>(&value), 4);
                    printf("val = %d\n", value);
                    info.value[1] = value;

                    value = 0x00;
                    stream.read(reinterpret_cast<char*>(&value), 4);
                    printf("val = %d\n", value);
                    info.value[2] = value;

                    value = 0x00;
                    stream.read(reinterpret_cast<char*>(&value), 4);
                    printf("val = %d\n", value);
                    info.value[3] = value;
                    
                    values.push_back(info);
                }
            }
        }
#endif
    }

    void WDAT::_write(std::ostream& stream) {

    }

    bool WDAT::_isValid(uint32_t signature) {
        if (signature != WDAT::signature)
            return false;
        return true;
    }

}
