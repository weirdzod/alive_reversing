#pragma once

#include "../relive_lib/BaseAnimatedWithPhysicsGameObject.hpp"
#include "../AliveLibCommon/Function.hpp"
#include "../relive_lib/Layer.hpp"

struct BloodParticle final
{
    FP x;
    FP y;
    FP mOffX;
    FP mOffY;
    Prim_Sprt field_10_prims[2];
};
ALIVE_ASSERT_SIZEOF(BloodParticle, 0x40);

class Blood final : public BaseAnimatedWithPhysicsGameObject
{
public:
    Blood(FP xpos, FP ypos, FP xOff, FP yOff, FP scale, s32 count);
    ~Blood();

    virtual void VUpdate() override;
    virtual void VRender(PrimHeader** ppOt) override;
    virtual void VScreenChanged() override;

private:
    BloodParticle* mBloodParticle = nullptr;
    s16 mBloodXPos = 0;
    s16 mBloodYPos = 0;
    s16 mCurrentBloodCount = 0;
    u8 mRandSeed = 0;
    s16 mTotalBloodCount = 0;
    s32 mUpdateCalls = 0;
    Layer mOtLayer = Layer::eLayer_0;
};
