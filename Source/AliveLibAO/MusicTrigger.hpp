#pragma once

#include "../relive_lib/BaseGameObject.hpp"
#include "MusicController.hpp"
#include "../relive_lib/data_conversion/relive_tlvs.hpp"

namespace AO {

class MusicTrigger final : public ::BaseGameObject
{
public:
    MusicTrigger(relive::Path_MusicTrigger::MusicTriggerMusicType type, relive::Path_MusicTrigger::TriggeredBy triggeredBy, s32 switchId, s32 delay);
    MusicTrigger(relive::Path_MusicTrigger* pTlv, const Guid& tlvId);

    void Init(relive::Path_MusicTrigger::MusicTriggerMusicType type, relive::Path_MusicTrigger::TriggeredBy triggeredBy, u16 switchId, s16 delay);

    ~MusicTrigger();

    virtual void VScreenChanged() override;
    virtual void VUpdate() override;

    Guid mTlvId;
    /* enum Flags_14
    {
        eBit1_Unknown = 0x1,
        eBit2_Unknown = 0x2,
        eBit3_Unknown = 0x4,
    };*/
    s16 field_14_flags = 0;
    s16 field_16 = 0;
    s32 field_18_counter = 0;
    MusicController::MusicTypes field_1C_music_type = MusicController::MusicTypes::eType0;
    u16 field_1E_switch_id = 0;
};

} // namespace AO
