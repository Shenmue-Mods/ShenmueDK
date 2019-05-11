#pragma once

#include <map>

#include "shendk/types/vector.h"
#include "shendk/node/node.h"
#include "shendk/node/chrd/strg.h"

namespace shendk {
namespace chrd {

struct CHRS : public Node {

    std::set<std::string> Tokens1 {
        "Flags",
        "Group",
        "CommonMotionWork"
    };

    std::set<std::string> Tokens2 {
        "Player",
        "Image",
        "Entry",
        "Flags",
        "Coli",
        "Human",
        "Object",
        "Party",
        "Face",
        "Hand",
        "Item",
        "Range",
        "ColiOff",
        "LineWalk",
        "ForkLift",
        "MotClip",
        "Shadow",
        "ShadowOff",
        "Book",
        "KeysRing",
        "Fight",
        "Group",
        "MotOnly",
        "CasetteDeck",
        "CasetteTape",
        "Handcuffs",
        "Cloth",
        "TinyMot",
        "Tel",
        "Step",
        "Walk3D",
        "CommonMotionWork",
        "FootMark",
        "RefBoard",
        "Osage",
        "Knife",
        "TParty",
        "Trans",
        "Vehicle",
        "Bone",
        "DummyChara",
        "Bicycle",
        "Patrol",
        "Vender",
        "LowQuality",
        "DesignOK",
        "Clock",
        "ProjectionTrans",
        "TailLamp",
        "TinyGame",
        "Calendar",
        "CutMvShadow",
        "ObjBlur",
        "ForceTrans",
        "ColiOn",
        };

    std::set<std::string> Tokens3 {
        "Character",
        "DefImage",
        "LocalPos",
        "PreLoad"
    };

    struct Entry {
        std::string func;
        std::string model;
        std::string image;

        Vector3f position;
        Vector3f eulerAngles;
    };

    virtual ~CHRS();

    STRG* strg = nullptr;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);

};

}
}
