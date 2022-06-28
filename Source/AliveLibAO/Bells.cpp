#include "stdafx_ao.h"
#include "Function.hpp"
#include "Bells.hpp"
#include "ResourceManager.hpp"
#include "stdlib.hpp"
#include "ScreenWave.hpp"
#include "Sparks.hpp"
#include "Game.hpp"
#include "Math.hpp"
#include "Sfx.hpp"

namespace AO {

Bells::Bells(BellSize bellType, FP xpos, FP ypos, FP scale)
{
    mBaseGameObjectFlags.Clear(Options::eCanExplode_Bit7);

    mBaseGameObjectTypeId = ReliveTypes::eBells;

    const AnimRecord rec = AO::AnimRec(AnimId::BigChime);
    const AnimRecord rec2 = AO::AnimRec(AnimId::MediumChime);
    const AnimRecord rec3 = AO::AnimRec(AnimId::SmallChime);
    u8** ppRes = ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);

    switch (bellType)
    {
        case BellSize::eBig_0:
            field_EA_sound = Sounds::eLowPitch_0;
            Animation_Init_417FD0(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1);
            break;

        case BellSize::eMedium_1:
            field_EA_sound = Sounds::eMediumPitch_1;
            Animation_Init_417FD0(rec2.mFrameTableOffset, rec2.mMaxW, rec2.mMaxH, ppRes, 1);
            break;
        case BellSize::eSmall_2:
            field_EA_sound = Sounds::eHighPitch_2;
            Animation_Init_417FD0(rec3.mFrameTableOffset, rec3.mMaxW, rec3.mMaxH, ppRes, 1);
            break;
    }

    mVisualFlags.Clear(VisualFlags::eApplyShadowZoneColour);
    mBaseAnimatedWithPhysicsGameObject_SpriteScale = scale;

    mBaseAnimatedWithPhysicsGameObject_XPos = xpos;
    mBaseAnimatedWithPhysicsGameObject_YPos = ypos;

    mBaseAnimatedWithPhysicsGameObject_Anim.mRenderLayer = Layer::eLayer_Foreground_36;

    field_E8_bSmashing = 0;

    field_EC_bDoScreenWave = 0;
    field_F4_sound_cooldown_timer = 0;
    field_F0_sound_pitch_factor = 0;
}

void Bells::VUpdate()
{
    if (field_F0_sound_pitch_factor > 0 && static_cast<s32>(sGnFrame) >= field_F4_sound_cooldown_timer)
    {
        field_F4_sound_cooldown_timer = sGnFrame + 4;
        field_F0_sound_pitch_factor--;

        if (field_EA_sound == Sounds::eLowPitch_0)
        {
            SFX_Play_Mono(SoundEffect::BellChime_LowPitch_52, 0, 0);
        }
        else if (field_EA_sound == Sounds::eMediumPitch_1)
        {
            SFX_Play_Pitch(SoundEffect::BellChime_MediumPitch_51, 45 * (field_F0_sound_pitch_factor + 1), 128 - (field_F0_sound_pitch_factor << 7), 0);
        }
        else if (field_EA_sound == Sounds::eHighPitch_2)
        {
            SFX_Play_Pitch(SoundEffect::BellChime_HighPitch_50, 30 * (field_F0_sound_pitch_factor + 1), (2 - field_F0_sound_pitch_factor) << 7, 0);
        }
    }

    if (field_E8_bSmashing == 1)
    {
        if (field_EC_bDoScreenWave > 0)
        {
            field_EC_bDoScreenWave--;

            FP xOff = {};
            FP yOff = {};
            if (field_EA_sound == Sounds::eLowPitch_0)
            {
                xOff = FP_FromInteger(-35);
                yOff = FP_FromInteger(36);
                const FP wave_ypos = mBaseAnimatedWithPhysicsGameObject_YPos + FP_FromInteger(36);
                const FP wave_xpos = mBaseAnimatedWithPhysicsGameObject_XPos - FP_FromInteger(35);
                relive_new ScreenWave(wave_xpos, wave_ypos, Layer::eLayer_FG1_37, FP_FromInteger(18), FP_FromInteger(12), 0);
            }
            else if (field_EA_sound == Sounds::eHighPitch_2)
            {
                xOff = FP_FromInteger(37);
                yOff = FP_FromInteger(32);
                const FP wave_ypos = mBaseAnimatedWithPhysicsGameObject_YPos + FP_FromInteger(32);
                const FP wave_xpos = mBaseAnimatedWithPhysicsGameObject_XPos + FP_FromInteger(37);
                relive_new ScreenWave(wave_xpos, wave_ypos, Layer::eLayer_FG1_37, FP_FromInteger(12), FP_FromInteger(12), 0);
            }
            else if (field_EA_sound == Sounds::eMediumPitch_1)
            {
                xOff = FP_FromInteger(-4);
                yOff = FP_FromInteger(24);
                const FP wave_ypos = mBaseAnimatedWithPhysicsGameObject_YPos + FP_FromInteger(24);
                const FP wave_xpos = mBaseAnimatedWithPhysicsGameObject_XPos - FP_FromInteger(4);
                relive_new ScreenWave(wave_xpos, wave_ypos, Layer::eLayer_FG1_37, FP_FromInteger(14), FP_FromInteger(12), 0);
            }

            for (s32 i = 0; i < 4; i++)
            {
                const FP sparkx = mBaseAnimatedWithPhysicsGameObject_XPos + FP_FromInteger(Math_RandomRange(-2, 2)) + xOff;
                const FP sparky = mBaseAnimatedWithPhysicsGameObject_YPos + FP_FromInteger(Math_RandomRange(-2, 2)) + yOff;
                relive_new Sparks(sparkx, sparky, mBaseAnimatedWithPhysicsGameObject_SpriteScale);
            }
        }

        PlaySounds();
    }
}

void Bells::PlaySounds()
{
    if (mBaseAnimatedWithPhysicsGameObject_Anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        if (field_EA_sound == Sounds::eLowPitch_0)
        {
            mBaseAnimatedWithPhysicsGameObject_Anim.Set_Animation_Data(19240, 0);
        }
        else if (field_EA_sound == Sounds::eMediumPitch_1)
        {
            mBaseAnimatedWithPhysicsGameObject_Anim.Set_Animation_Data(19368, 0);
        }
        else if (field_EA_sound == Sounds::eHighPitch_2)
        {
            mBaseAnimatedWithPhysicsGameObject_Anim.Set_Animation_Data(19252, 0);
        }

        field_E8_bSmashing = 0;
    }
}

Bool32 Bells::CanSmash()
{
    return field_E8_bSmashing == 0;
}

void Bells::Ring()
{
    if (field_E8_bSmashing == 0)
    {
        field_E8_bSmashing = 1;
        field_EC_bDoScreenWave = 1;
        field_F4_sound_cooldown_timer = 0;

        if (field_EA_sound == Sounds::eLowPitch_0)
        {
            field_F0_sound_pitch_factor = 1;
            const AnimRecord& rec = AO::AnimRec(AnimId::BigChime_Moving);
            mBaseAnimatedWithPhysicsGameObject_Anim.Set_Animation_Data(rec.mFrameTableOffset, 0);
        }
        else if (field_EA_sound == Sounds::eMediumPitch_1)
        {
            field_F0_sound_pitch_factor = 2;
            const AnimRecord& rec = AO::AnimRec(AnimId::MediumChime_Moving);
            mBaseAnimatedWithPhysicsGameObject_Anim.Set_Animation_Data(rec.mFrameTableOffset, 0);
        }
        else if (field_EA_sound == Sounds::eHighPitch_2)
        {
            field_F0_sound_pitch_factor = 3;
            const AnimRecord& rec = AO::AnimRec(AnimId::SmallChime_Moving);
            mBaseAnimatedWithPhysicsGameObject_Anim.Set_Animation_Data(rec.mFrameTableOffset, 0);
        }
    }
}

} // namespace AO
