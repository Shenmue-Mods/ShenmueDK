#pragma once

#include <map>

#include "eigen3/Eigen/Dense"

#include "shendk/node/node.h"
#include "shendk/node/chrd/strg.h"

namespace shendk {
namespace chrd {

struct CHRS : Node {

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

        Eigen::Vector3f position;
        Eigen::Vector3f eulerAngles;
    };

    virtual ~CHRS() {}

    STRG* strg = nullptr;

protected:

    virtual void _read(std::istream& stream) {
        if (!strg) {
            throw new std::runtime_error("CHRD can not be read without an assigned STRG node!");
        }
    }

    virtual void _write(std::ostream& stream) {
        if (!strg) {
            throw new std::runtime_error("CHRD can not be written without an assigned STRG node!");
        }
    }

};

}
}
