#pragma once

#include <stdint.h>

namespace shendk {
namespace sm2 {


struct vec3
{
    float x, y, z;
};

struct vec4
{
    float x, y, z, w;
};

struct cameraSwitch {
    uint32_t f0;
    int32_t f4;
};

struct taskToken {
    char one;
    char two;
    char three;
    char four;
};

struct Task {
    uint64_t* callbackFuncPtr;
    char taskName[4];
    uint8_t unk1;
    uint8_t unk2;
    uint16_t pad0;
    uint64_t* unk3;
    uint64_t* nextTask;
    uint64_t* unk5;
    uint64_t* unk6;
    uint64_t* unk7;
    uint64_t* unk8;
    uint64_t* unk9;
    uint64_t* unk10;
    uint64_t* unk11;
    uint64_t* unk12;
    uint64_t* unk13;
    uint64_t* callbackParamPtr;
};

struct TaskQueue {
    Task Tasks[300];
};

struct StorageEntry {
    char typeName[4];
    uint32_t pad0;
    uint64_t contentSize;
    uint64_t* previousEntry;
    uint64_t* nextEntry;
    char usageToken[4];
    uint32_t pad1;
    uint64_t pad2;
    uint64_t pad3;
    uint64_t pad4;
    void* contentData;
};

struct sm2_ctrl {
    uint64_t* playerCwep; // Player character handler
    uint64_t* unk1;
    uint64_t* meveTask; // Map event control (HLTaskFunc_MapEventTaskEntry)
    uint64_t* scenTask; // Scenario control (HLTaskFunc_ScenarioTaskEntry)
    uint64_t* camtTask; // Camera control (HLTaskFunc_CameraCtrl)
    uint64_t* objeTask; // Object render task (HLTaskFunc_OBJ)
    uint64_t* airdTask; // Skybox task (HLTaskFunc_CtrlAir)
    uint64_t* mdlmTask; // (HLTaskFunc_ModelManageTask)
    uint64_t* filmTask; // (HLTaskFunc_ScreenFilmTaskEntry)
    uint64_t* clipTask; // (HLTaskFunc_CharDispCheck)
    uint64_t* drwpTask; // (HLTaskFunc_CharDraw)
    uint64_t* windTask; // (HLTaskFunc_WindowTaskEntry)
    uint64_t* unk2;
    uint64_t* unk3;
};

struct sm2_scen {
    uint64_t* scnwPtr;
    uint64_t* unk1;
    uint64_t* scncPtr;
    uint64_t* chidPtr1;
    uint64_t* chidPtr2;
    uint64_t* chidPtr3;
    uint64_t* chidPtr4;
    uint64_t* chidPtr5;
    uint64_t* chidPtr6;
    uint64_t* chidPtr7;
    uint64_t* chidPtr8;
    uint64_t* chidPtr9;
    uint64_t* chidPtr10;
    uint64_t* chidPtr11;
    uint64_t* chidPtr12;
    uint64_t* chidPtr13;
    uint64_t* chidPtr14;
    uint64_t* wndt;
};

struct sm2_chid {
    uint64_t* unk1;
    uint64_t* unk2;
    char charID[4];
    char pad[4];
    uint64_t* unk3;
    uint64_t* unk4;
};

struct sm2_cwep {
    char token[4];
    int64_t f4;
    float posX;
    float posY;
    float posZ;
    int32_t f24;
    int64_t f28;
    int32_t f36;
    int64_t f40;
    int32_t f48;
    int64_t f52;
    float f60; //x
    float f64; //y
    float f68; //z
    int64_t f72;
    int64_t f80;
    float f88;
    int64_t f92;
    int64_t f100;
    uint32_t f108;
    signed char pad116[4];
    int64_t f116;
    float f124; //x
    float f128; //y
    float f132; //z
    int64_t f136;
    int64_t f144;
    unsigned char f152;
    signed char f153;
    int16_t f154;
    int64_t f156;
    int64_t f164;
    int64_t* f172;
    int64_t f180;
    int64_t f188;
    int16_t f196;
    int16_t f198;
    int64_t* functionPtr1;
    int64_t* functionPtr2;
    int64_t* functionPtr3;
    int64_t* functionPtr4;
    int32_t f232;
    int16_t f236;
    signed char pad272[34];
    int32_t f272;
    signed char pad296[20];
    int64_t f296;
    int32_t f304;
    int64_t f308;
    signed char pad316[4];
};

struct sm2_player {
    uint32_t f0;
    int32_t f4;
    int32_t f8;
    int16_t f12;
    int16_t f14;
    signed char f16;
    signed char f17;
    char pad18[138];
    int32_t f156;
    uint16_t f160;
    uint16_t f162;
    char pad164[156];
    int32_t f320; //Next X
    int32_t f324; //Next Y
    int32_t f328; //Next Z
    uint16_t f332;
    uint16_t f334;
    uint16_t f336;
    char pad338[2];
    int32_t f340;
    int32_t f344;
    int32_t f348;
    int32_t f352;
    int32_t f356;
    int32_t f360;
    int32_t f364;
    int32_t f368;
    int32_t f372;
    int32_t f376;
    int32_t f380; //Current X
    int32_t f384; //Current Y
    int32_t f388; //Current Z
    char pad392[12];
    int16_t f404;
    int16_t f406;
    int16_t f408;
    int16_t f410;
    char pad412[28];
    int32_t f440;
    int32_t f444;
    int32_t f448;
    int32_t f452;
    int32_t f456;
    int32_t f460;
    int16_t f464;
    int16_t f466;
    int16_t f468;
    int16_t f470;
    int32_t f472;
    int32_t f476;
    int32_t f480;
    int32_t f484;
    uint16_t f488;
    uint16_t f490;
    uint16_t f492;
    char pad494[10];
    int16_t f504;
    signed char f506;
    signed char f507;
    int32_t f508;
    int32_t f512;
    int32_t f516;
    int32_t f520;
    int32_t f524;
    int32_t f528;
    char pad532[28];
    int64_t f560;
    int64_t f568;
    int64_t f576;
    int64_t f584;
    int32_t f592;
    int32_t f596;
    int32_t f600;
    char pad604[4];
    int32_t f608;
    int32_t f612;
    int32_t f616;
    int32_t f620;
    int32_t f624;
    int32_t f628;
    int16_t f632;
    int16_t f634;
    int16_t f636;
    unsigned char f638;
    unsigned char f639;
    int16_t f640;
    char pad642[18];
    int16_t f660;
    int16_t f662;
    int32_t f664;
    int32_t f668;
    char pad672[8];
    int32_t f680;
    int32_t f684;
    int32_t f688;
    int32_t f692;
    int32_t f696;
    int32_t f700;
    int32_t f704;
    int32_t f708;
    int32_t f712;
    int16_t f716;
    int16_t f718;
    int16_t f720;
    char pad722[2];
    int32_t f724;
    int32_t f728;
    char pad732[4];
    int32_t f736;
    char pad740[8];
    int32_t f748;
    char pad752[8];
    int16_t f760;
    char pad762[4];
    int16_t f766;
    int32_t f768;
    char pad772[16];
    int32_t f788;
    int32_t f792;
    int16_t f796;
    signed char f798;
    signed char f799;
    signed char f800;
    char pad801[3];
    int32_t f804;
    char pad808[64];
    int32_t f872;
};

struct charID {
    signed char pad16[16];
    char ID[4];
};

}
}
