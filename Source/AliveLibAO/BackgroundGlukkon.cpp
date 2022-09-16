#include "stdafx_ao.h"
#include "Function.hpp"
#include "BackgroundGlukkon.hpp"
#include "ResourceManager.hpp"
#include "../AliveLibAE/stdlib.hpp"
#include "Explosion.hpp"
#include "Math.hpp"
#include "Sfx.hpp"
#include "Abe.hpp"
#include "../relive_lib/Events.hpp"
#include "Game.hpp"

namespace AO {

BackgroundGlukkon::~BackgroundGlukkon()
{
    if (mHealth <= FP_FromInteger(0))
    {
        Path::TLV_Reset(field_10C_tlvInfo, -1, 0, 1);
    }
    else
    {
        Path::TLV_Reset(field_10C_tlvInfo, -1, 0, 0);
    }
}

void BackgroundGlukkon::LoadAnimations()
{
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::Background_Glukkon_Dying));
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::Background_Glukkon_Idle));
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::Background_Glukkon_KillHim1));
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::Background_Glukkon_KillHim2));
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::Background_Glukkon_Laugh));
}

BackgroundGlukkon::BackgroundGlukkon(relive::Path_BackgroundGlukkon* pTlv, const Guid& tlvId)
    : BaseAliveGameObject()
{
    SetType(ReliveTypes::eBackgroundGlukkon);

    LoadAnimations();

    Animation_Init(GetAnimRes(AnimId::Background_Glukkon_Idle));

    field_10C_tlvInfo = tlvId;

    mXPos = FP_FromInteger(pTlv->mTopLeftX);
    mYPos = FP_FromInteger(pTlv->mTopLeftY);

    mSpriteScale = FP_FromInteger(pTlv->mScalePercent) / FP_FromInteger(100);

    PalId pal = PalId::Default;
    if (pTlv->mPalId == AOResourceID::kGlukredAOResID)
    {
        pal = PalId::RedBackgroundGlukkon;
    }
    else if (pTlv->mPalId == AOResourceID::kGlukgrenAOResID)
    {
        pal = PalId::GreenBackgroundGlukkon;
    }
    else if (pTlv->mPalId == AOResourceID::kGlukblueAOResID)
    {
        pal = PalId::BlueBackgroundGlukkon;
    }
    else if (pTlv->mPalId == AOResourceID::kGlukaquaAOResID)
    {
        pal = PalId::AquaBackgroundGlukkon;
    }
    else
    {
        ALIVE_FATAL("Invalid background glukkon pal");
    }

    mLoadedPals.push_back(ResourceManagerWrapper::LoadPal(pal));
    mAnim.LoadPal(GetPalRes(pal));

    field_110_state = BackgroundGlukkon::State::eToSetSpeakPauseTimer_0;
}

void BackgroundGlukkon::VScreenChanged()
{
    mBaseGameObjectFlags.Set(BaseGameObject::eDead);
}

s16 BackgroundGlukkon::VTakeDamage(BaseGameObject* pFrom)
{
    if (mBaseGameObjectFlags.Get(BaseGameObject::eDead))
    {
        return 0;
    }

    if (pFrom->Type() == ReliveTypes::eShrykull)
    {
        mAnim.Set_Animation_Data(GetAnimRes(AnimId::Background_Glukkon_Dying));
        const auto rndVol = Math_RandomRange(110, 127);
        const auto rndPitch = (75 * (Math_NextRandom() % 4)) + 200;

        if (Math_NextRandom() >= 128u)
        {
            SFX_Play_Pitch(relive::SoundEffects::Empty, rndVol, rndPitch);
        }
        else
        {
            SFX_Play_Pitch(relive::SoundEffects::Empty, rndVol, rndPitch);
        }

        mAnim.Set_Animation_Data(GetAnimRes(AnimId::Background_Glukkon_Dying));
        field_110_state = BackgroundGlukkon::State::eKilledByShrykull_7;
    }
    else if (pFrom->Type() == ReliveTypes::eElectrocute && mHealth > FP_FromInteger(0))
    {
        mHealth = FP_FromInteger(0);

        relive_new Explosion(
            mXPos,
            mYPos - (mSpriteScale * FP_FromInteger(40)),
            mSpriteScale);

        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
    return 1;
}

void BackgroundGlukkon::VUpdate()
{
    if (EventGet(kEventDeathReset))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    switch (field_110_state)
    {
        case BackgroundGlukkon::State::eToSetSpeakPauseTimer_0:
            field_110_state = BackgroundGlukkon::State::eSetSpeakPauseTimer_1;
            field_118_never_read = sGnFrame + Math_RandomRange(20, 40);
            break;

        case BackgroundGlukkon::State::eSetSpeakPauseTimer_1:
            field_110_state = BackgroundGlukkon::State::eRandomizedLaugh_2;
            field_114_speak_pause_timer = sGnFrame + Math_RandomRange(12, 20);
            break;

        case BackgroundGlukkon::State::eRandomizedLaugh_2:
            if (static_cast<s32>(sGnFrame) > field_114_speak_pause_timer)
            {
                const auto rndVol = Math_RandomRange(110, 127);
                const auto rndPitch = ((Math_NextRandom() % 4) * 128) + 200;

                switch (Math_NextRandom() % 5)
                {
                    case 0:
                        if (sActiveHero->mHealth <= FP_FromInteger(0))
                        {
                            mAnim.Set_Animation_Data(GetAnimRes(AnimId::Background_Glukkon_Laugh));
                            SFX_Play_Pitch(relive::SoundEffects::GlukkonLaugh1, rndVol, rndPitch);
                        }
                        else
                        {
                            mAnim.Set_Animation_Data(GetAnimRes(AnimId::Background_Glukkon_KillHim1));
                            SFX_Play_Pitch(relive::SoundEffects::GlukkonKillHim1, rndVol, rndPitch);
                        }
                        field_110_state = BackgroundGlukkon::State::eAfterLaugh_SetSpeakPauseTimer_3;
                        break;

                    case 1:
                        if (sActiveHero->mHealth <= FP_FromInteger(0))
                        {
                            mAnim.Set_Animation_Data(GetAnimRes(AnimId::Background_Glukkon_Laugh));
                            SFX_Play_Pitch(relive::SoundEffects::GlukkonLaugh1, rndVol, rndPitch);
                        }
                        else
                        {
                            mAnim.Set_Animation_Data(GetAnimRes(AnimId::Background_Glukkon_KillHim2));
                            SFX_Play_Pitch(relive::SoundEffects::GlukkonKillHim2, rndVol, rndPitch);
                        }
                        field_110_state = BackgroundGlukkon::State::eAfterLaugh_SetSpeakPauseTimer_3;
                        break;

                    case 2:
                        if (sActiveHero->mHealth <= FP_FromInteger(0))
                        {
                            mAnim.Set_Animation_Data(GetAnimRes(AnimId::Background_Glukkon_Laugh));
                            SFX_Play_Pitch(relive::SoundEffects::GlukkonLaugh1, rndVol, rndPitch);
                        }
                        else
                        {
                            mAnim.Set_Animation_Data(GetAnimRes(AnimId::Background_Glukkon_KillHim1));
                            SFX_Play_Pitch(relive::SoundEffects::Empty, rndVol, rndPitch);
                        }
                        field_110_state = BackgroundGlukkon::State::eAfterLaugh_SetSpeakPauseTimer_3;
                        break;

                    case 3:
                        if (sActiveHero->mHealth > FP_FromInteger(0))
                        {
                            mAnim.Set_Animation_Data(GetAnimRes(AnimId::Background_Glukkon_KillHim2));
                            SFX_Play_Pitch(relive::SoundEffects::Empty, rndVol, rndPitch);
                        }
                        field_110_state = BackgroundGlukkon::State::eAfterLaugh_SetSpeakPauseTimer_3;
                        break;

                    case 4:
                        return;

                    default:
                        field_110_state = BackgroundGlukkon::State::eAfterLaugh_SetSpeakPauseTimer_3;
                        break;
                }
            }
            break;

        case BackgroundGlukkon::State::eAfterLaugh_SetSpeakPauseTimer_3:
            if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                mAnim.Set_Animation_Data(GetAnimRes(AnimId::Background_Glukkon_Idle));
                field_110_state = BackgroundGlukkon::State::eSetSpeakPauseTimer_1;
            }
            break;

        default:
            return;
    }
}

} // namespace AO
