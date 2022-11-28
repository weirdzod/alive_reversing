#pragma once


#include "../relive_lib/BaseGameObject.hpp"
#include "../AliveLibCommon/FixedPoint_common.hpp"

namespace relive
{
    struct Path_LiftMover;
}

namespace AO {

class LiftPoint;

class LiftMover final : public ::BaseGameObject
{
public:
    LiftMover(relive::Path_LiftMover* pTlv, const Guid& tlvId);
    ~LiftMover();
    
    virtual void VUpdate() override;

    LiftPoint* FindLiftPointWithId(s16 id);

    u16 field_10_lift_mover_switch_id = 0;
    s16 mTargetLiftPointId = 0;
    Guid field_14_tlvInfo;
    BaseGameObject* mTargetLift = nullptr; // TODO: LiftPoint* or actually no because it can be a platform as well?
    FP mLiftSpeed = {};
    s16 mState = 0;
};

} // namespace AO
