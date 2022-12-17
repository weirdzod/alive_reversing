#pragma once

#include "../relive_lib/GameObjects/BaseGameObject.hpp"
#include "../relive_lib/data_conversion/relive_tlvs.hpp"

namespace AO {

class BellSong final : public ::BaseGameObject
{
public:
    BellSong(relive::Path_BellsongStone::BellsongTypes type, u32 code);

    virtual void VUpdate() override;

    s32 mCodeIdx = 0;
    bool mDone = false;
    relive::Path_BellsongStone::BellsongTypes mType = relive::Path_BellsongStone::BellsongTypes::eWhistle;
    s32 mCode = 0;
    s16 mCodeLength = 0;
};

} // namespace AO
