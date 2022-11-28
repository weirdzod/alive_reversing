#pragma once

#include "BaseAliveGameObject.hpp"

namespace relive
{
    struct Path_Mine;
}

namespace AO {

class Mine final : public BaseAliveGameObject
{
public:
    Mine(relive::Path_Mine* pTlv, const Guid& tlvId);
    ~Mine();

    void LoadAnimations();

    virtual void VScreenChanged() override;
    virtual s16 VTakeDamage(BaseGameObject* pFrom) override;
    virtual void VOnThrowableHit(BaseGameObject* pFrom) override;
    virtual void VOnPickUpOrSlapped() override;
    virtual void VRender(PrimHeader** ppOt) override;
    virtual void VUpdate() override;

    s16 IsColliding();

    bool mDetonating = false;
    Guid mTlvId;
    s32 mExplosionTimer = 0;
    Animation mFlashAnim;
    s32 field_1B0_flags = 0;
};

} // namespace AO
