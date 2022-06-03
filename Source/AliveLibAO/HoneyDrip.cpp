#include "stdafx_ao.h"
#include "Function.hpp"
#include "HoneyDrip.hpp"
#include "ResourceManager.hpp"
#include "Sfx.hpp"
#include "stdlib.hpp"

namespace AO {

HoneyDrip::HoneyDrip(FP xpos, FP ypos)
{
    mBlue = 128;
    mGreen = 128;
    mRed = 128;

    const AnimRecord& rec = AO::AnimRec(AnimId::Honey_Drip);
    u8** ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);
    Animation_Init_417FD0(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1);
    mYPos = ypos;
    mXPos = xpos;

    mAnim.mRenderLayer = Layer::eLayer_BeforeWell_22;

    field_E4_bSoundPlaying = FALSE;
}

void HoneyDrip::VUpdate()
{
    mXPos += mVelX;
    mYPos += mVelY;

    if (mAnim.field_92_current_frame == 7 && !field_E4_bSoundPlaying)
    {
        SFX_Play_Mono(SoundEffect::HoneyDrip_26, 0, 0);
        field_E4_bSoundPlaying = TRUE;
    }

    if (mAnim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

} // namespace AO
