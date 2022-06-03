#include "stdafx_ao.h"
#include "Function.hpp"
#include "Shrykull.hpp"
#include "Map.hpp"
#include "stdlib.hpp"
#include "ResourceManager.hpp"
#include "Abe.hpp"
#include "ZapLine.hpp"
#include "Shadow.hpp"
#include "Sfx.hpp"
#include "PossessionFlicker.hpp"
#include "ParticleBurst.hpp"
#include "Flash.hpp"
#include "Electrocute.hpp"
#include "AbilityRing.hpp"
#include "Math.hpp"
#include "Game.hpp"

namespace AO {

Shrykull::~Shrykull()
{
    if (field_118_zap_line)
    {
        field_118_zap_line->field_C_refCount--;
        field_118_zap_line->mGameObjectFlags.Set(Options::eDead);
    }

    if (field_11C_obj_being_zapped)
    {
        field_11C_obj_being_zapped->field_C_refCount--;
    }
}

void Shrykull::VScreenChanged()
{
    if (gMap.mCurrentLevel != gMap.mLevel || gMap.mOverlayId != gMap.GetOverlayId())
    {
        mGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

Shrykull::Shrykull()
    : BaseAliveGameObject()
{
    mGameObjectFlags.Set(Options::eCanExplode_Bit7);
    mTypeId = Types::eShrykull_85;
    
    const AnimRecord& rec = AO::AnimRec(AnimId::Mudokon_ToShrykull);
    u8** ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);
    Animation_Init_417FD0(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1);
    field_118_zap_line = nullptr;
    field_11C_obj_being_zapped = nullptr;

    mXPos = sActiveHero->mXPos;
    mYPos = sActiveHero->mYPos;
    mSpriteScale = sActiveHero->mSpriteScale;
    mScale = sActiveHero->mScale;
    field_10C_state = State::eTransform_0;

    mAnim.mAnimFlags.Set(AnimFlags::eBit5_FlipX, sActiveHero->mAnim.mAnimFlags.Get(AnimFlags::eBit5_FlipX));

    mShadow = ao_new<Shadow>();

    field_122_bResetRingTimer = 0;
}

void Shrykull::VOnThrowableHit(BaseGameObject*)
{
    // Empty
}

bool Shrykull::CanKill(BaseAnimatedWithPhysicsGameObject* pObj)
{
    return (pObj->mTypeId == Types::eTimedMine_8
            || pObj->mTypeId == Types::eMine_57
            || pObj->mTypeId == Types::eUXB_99
            || pObj->mTypeId == Types::eSlig_88
            || pObj->mTypeId == Types::eSlog_89
            || pObj->mTypeId == Types::eBackgroundGlukkon_42
            || pObj->mTypeId == Types::eSecurityClaw_31
            || pObj->mTypeId == Types::SecurityOrb_53)
        && pObj->mAnim.mAnimFlags.Get(AnimFlags::eBit3_Render)
        && !pObj->mGameObjectFlags.Get(BaseGameObject::eDead)
        && gMap.Is_Point_In_Current_Camera_4449C0(
            pObj->mLvlNumber,
            pObj->mPathNumber,
            pObj->mXPos,
            pObj->mYPos,
            0);
}

bool Shrykull::CanElectrocute(BaseGameObject* pObj) const
{
    switch (pObj->mTypeId)
    {
        case Types::eElum_26:
        case Types::eBackgroundGlukkon_42:
        case Types::eMudokon_52:
        case Types::eParamite_62:
        case Types::eMudokon_75:
        case Types::eScrab_77:
        case Types::eSlig_88:
        case Types::eSlog_89:
            return true;
        default:
            return false;
    }
}

void Shrykull::VUpdate()
{
    switch (field_10C_state)
    {
        case State::eTransform_0:
            if (mAnim.field_92_current_frame == 0)
            {
                SFX_Play_Pitch(SoundEffect::Shrykull1_108, 127, -2000);
                SFX_Play_Pitch(SoundEffect::Shrykull2_109, 127, 0);
                SFX_Play_Mono(SoundEffect::IngameTransition_107, 127);
            }

            if (mAnim.mAnimFlags.Get(AnimFlags::eBit12_ForwardLoopCompleted))
            {
                mAnim.Set_Animation_Data(92040, nullptr);
                field_10C_state = State::eZapTargets_1;
            }
            break;

        case State::eZapTargets_1:
            if (mAnim.field_92_current_frame == 0)
            {
                if (Math_NextRandom() >= 128u)
                {
                    SFX_Play_Pitch(SoundEffect::Shrykull2_109, 127, 0);
                }
                else
                {
                    SFX_Play_Pitch(SoundEffect::Shrykull1_108, 127, 0);
                }
            }

            for (s32 i = 0; i < gBaseAliveGameObjects_4FC8A0->Size(); i++)
            {
                BaseAliveGameObject* pObj = gBaseAliveGameObjects_4FC8A0->ItemAt(i);
                if (!pObj)
                {
                    break;
                }

                if (CanKill(pObj) && !pObj->mAliveGameObjectFlags.Get(Flags_10A::e10A_Bit3))
                {
                    pObj->field_C_refCount++;
                    field_11C_obj_being_zapped = pObj;

                    PSX_RECT objRect = {};
                    pObj->VGetBoundingRect(&objRect, 1);

                    PSX_RECT ourRect = {};
                    VGetBoundingRect(&ourRect, 1);

                    if (field_118_zap_line)
                    {
                        field_118_zap_line->CalculateSourceAndDestinationPositions(
                            FP_FromInteger((ourRect.x + ourRect.w) / 2),
                            FP_FromInteger((ourRect.y + ourRect.h) / 2),
                            FP_FromInteger((objRect.x + objRect.w) / 2),
                            FP_FromInteger((objRect.y + objRect.h) / 2));
                    }
                    else
                    {
                        auto pZapLine = ao_new<ZapLine>(
                            FP_FromInteger((ourRect.x + ourRect.w) / 2),
                            FP_FromInteger((ourRect.y + ourRect.h) / 2),
                            FP_FromInteger((objRect.x + objRect.w) / 2),
                            FP_FromInteger((objRect.y + objRect.h) / 2),
                            0, ZapLineType::eThin_1,
                            Layer::eLayer_ZapLinesElum_28);
                        if (pZapLine)
                        {
                            pZapLine->field_C_refCount++;
                            field_118_zap_line = pZapLine;
                        }
                    }

                    field_120_bElectrocute = CanElectrocute(pObj);
                    if (field_120_bElectrocute)
                    {
                        ao_new<Electrocute>(pObj, 0);
                        field_114_timer = gnFrameCount_507670 + 3;

                        if (pObj->mTypeId == Types::eBackgroundGlukkon_42)
                        {
                            pObj->VTakeDamage(this);
                        }
                    }

                    ao_new<PossessionFlicker>(field_11C_obj_being_zapped, 8, 255, 255, 255);

                    ao_new<AbilityRing>(
                        FP_FromInteger((objRect.x + objRect.w) / 2),
                        FP_FromInteger((objRect.y + objRect.h) / 2),
                        RingTypes::eShrykull_Pulse_Large_5);

                    ao_new<PossessionFlicker>(this, 8, 255, 255, 255);

                    ao_new<AbilityRing>(
                        FP_FromInteger((ourRect.x + ourRect.w) / 2),
                        FP_FromInteger((ourRect.y + ourRect.h) / 2),
                        RingTypes::eShrykull_Pulse_Large_5);

                    field_11C_obj_being_zapped->mAliveGameObjectFlags.Set(Flags_10A::e10A_Bit3);

                    SFX_Play_Pitch(SoundEffect::Respawn_22, 100, 2000);
                    SFX_Play_Mono(SoundEffect::Zap1_57, 0);

                    field_10C_state = State::eKillTargets_4;
                    field_110_timer = gnFrameCount_507670 + 12;
                    field_122_bResetRingTimer = 1;
                    return;
                }
            }

            if (field_118_zap_line)
            {
                field_118_zap_line->field_C_refCount--;
                field_118_zap_line->mGameObjectFlags.Set(Options::eDead);
                field_118_zap_line = nullptr;
            }
            field_10C_state = State::eDetransform_2;
            break;

        case State::eDetransform_2:
            if (mAnim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                mAnim.Set_Animation_Data(92152, nullptr);
                field_10C_state = State::eFinish_3;
            }
            break;

        case State::eFinish_3:
            if (mAnim.field_92_current_frame == 0)
            {
                SFX_Play_Pitch(SoundEffect::Shrykull1_108, 127, -2000);
                SFX_Play_Pitch(SoundEffect::Shrykull2_109, 127, 0);
            }

            if (mAnim.mAnimFlags.Get(AnimFlags::eBit12_ForwardLoopCompleted))
            {
                sActiveHero->ExitShrykull_42F440(field_122_bResetRingTimer);
                mGameObjectFlags.Set(BaseGameObject::eDead);
            }
            break;

        case State::eKillTargets_4:
            if (mAnim.field_92_current_frame == 0)
            {
                if (Math_NextRandom() >= 128u)
                {
                    SFX_Play_Pitch(SoundEffect::Shrykull2_109, 127, 0);
                }
                else
                {
                    SFX_Play_Pitch(SoundEffect::Shrykull1_108, 127, 0);
                }
            }

            if (field_11C_obj_being_zapped)
            {
                if (field_11C_obj_being_zapped->mGameObjectFlags.Get(BaseGameObject::eDead))
                {
                    field_11C_obj_being_zapped->field_C_refCount--;
                    field_11C_obj_being_zapped = nullptr;
                }
                else
                {
                    PSX_RECT zapRect = {};
                    field_11C_obj_being_zapped->VGetBoundingRect(&zapRect, 1);

                    PSX_RECT ourRect = {};
                    VGetBoundingRect(&ourRect, 1);
                    if (static_cast<s32>(gnFrameCount_507670) == field_114_timer)
                    {
                        ao_new<ParticleBurst>(
                            FP_FromInteger((zapRect.x + zapRect.w) / 2),
                            FP_FromInteger((zapRect.y + zapRect.h) / 2),
                            20,
                            mSpriteScale,
                            BurstType::eBigPurpleSparks_2);

                       ao_new<Flash>(Layer::eLayer_Above_FG1_39, 255u, 255u, 255u);
                    }
                    field_118_zap_line->CalculateSourceAndDestinationPositions(
                        FP_FromInteger((ourRect.x + ourRect.w) / 2),
                        FP_FromInteger((ourRect.y + ourRect.h) / 2),
                        FP_FromInteger((zapRect.x + zapRect.w) / 2),
                        FP_FromInteger((zapRect.y + zapRect.h) / 2));
                }
            }

            if (static_cast<s32>(gnFrameCount_507670) > field_110_timer)
            {
                field_10C_state = State::eZapTargets_1;

                if (field_11C_obj_being_zapped)
                {
                    if (!field_120_bElectrocute)
                    {
                        field_11C_obj_being_zapped->VTakeDamage(this);
                    }

                    field_11C_obj_being_zapped->field_C_refCount--;
                    field_11C_obj_being_zapped = nullptr;
                }
            }
            break;

        default:
            return;
    }
}

} // namespace AO
