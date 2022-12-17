#pragma once

#include "../relive_lib/GameObjects/BaseGameObject.hpp"
#include "../relive_lib/data_conversion/relive_tlvs.hpp"
#include "../relive_lib/FixedPoint.hpp"

namespace AO {

class SlogSpawner final : public ::BaseGameObject
{
public:
    SlogSpawner(relive::Path_SlogSpawner* pTlv, const Guid& tlvId);

    virtual void VScreenChanged() override;
    virtual void VUpdate() override;

    Guid mTlvInfo;
    s16 mSpawnedSlogsCount = 0;
    FP mXPos = {};
    FP mYPos = {};
    s32 mSpawnTimer = 0;
    relive::reliveScale mScale = relive::reliveScale::eFull;
    u16 mMaxSlogs = 0;
    u16 mMaxSlogsAtATime = 0;
    relive::Path_SlogSpawner::StartDirection mStartDirection = relive::Path_SlogSpawner::StartDirection::eRight;
    u16 mSlogSpawnInterval = 0;
    u16 mSpawnerSwitchId = 0;
};


} // namespace AO
