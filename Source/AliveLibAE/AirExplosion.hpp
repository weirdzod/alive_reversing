#pragma once

#include "../relive_lib/BaseAnimatedWithPhysicsGameObject.hpp"

class AirExplosion final : public BaseAnimatedWithPhysicsGameObject
{
public:
    AirExplosion(FP xpos, FP ypos, FP scale, bool bSmall);

    virtual void VUpdate() override;
    virtual void VScreenChanged() override;

private:
    void DealBlastDamage(PSX_RECT* pRect);

private:
    s16 mSmallExplosion = 0;
    FP field_F8_scale = {};
    FP field_FC_explosion_size = {};
};
