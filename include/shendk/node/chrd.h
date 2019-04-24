#pragma once

#include <map>
#include <set>

#include "shendk/node/node.h"
#include "shendk/node/chrd/chrs.h"
#include "shendk/node/chrd/strg.h"

namespace shendk {

struct CHRD : Node {

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


    virtual ~CHRD() {}

    chrd::STRG strg;
    chrd::CHRS chrs;

protected:

    virtual void _read(std::istream& stream) {
        // skip chrs
        int64_t chrsOffset = stream.tellg();
        Node::Header chrsHeader;
        stream.read(reinterpret_cast<char*>(&chrsHeader), sizeof(Node::Header));
        stream.seekg(chrsOffset + chrsHeader.size, std::ios::beg);

        // read strg
        strg.read(stream);

        // read chrs
        chrs.strg = &strg;
        stream.seekg(chrsOffset, std::ios::beg);
        chrs.read(stream);
    }

    virtual void _write(std::ostream& stream) {
    }

};

}
