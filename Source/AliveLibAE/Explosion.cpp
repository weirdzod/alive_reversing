#include "stdafx.h"
#include "Explosion.hpp"
#include "Function.hpp"
#include "Path.hpp"
#include "Map.hpp"
#include "Particle.hpp"
#include "Flash.hpp"
#include "ScreenShake.hpp"
#include "Gibs.hpp"
#include "Events.hpp"
#include "ParticleBurst.hpp"
#include "Sound/Midi.hpp"
#include "stdlib.hpp"
#include "Slig.hpp"
#include "ExplosionSet.hpp"

Explosion::Explosion(FP xpos, FP ypos, FP scale, bool bSmall)
    : BaseAnimatedWithPhysicsGameObject(0)
{
    SetType(AETypes::eExplosion_109);

    field_F4_bSmall = bSmall;
    if (field_F4_bSmall)
    {
        const AnimRecord& rec = AnimRec(AnimId::Explosion_Small);
        u8** ppRes = Add_Resource(ResourceManager::Resource_Animation, rec.mResourceId);
        Animation_Init(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1, 1);
    }
    else
    {
        const AnimRecord& rec = AnimRec(AnimId::Explosion);
        u8** ppRes = Add_Resource(ResourceManager::Resource_Animation, rec.mResourceId);
        Animation_Init(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1, 1);
    }

    mAnim.mAnimFlags.Clear(AnimFlags::eBit18_IsLastFrame);
    mAnim.mRenderMode = TPageAbr::eBlend_1;
    field_F8_scale = scale;
    mScale = scale == FP_FromInteger(1);
    mSpriteScale = scale * FP_FromInteger(2);

    if (field_F4_bSmall)
    {
        field_FC_explosion_size = scale * FP_FromDouble(0.5);
    }
    else
    {
        field_FC_explosion_size = scale;
    }

    mApplyShadows &= ~1u;
    mXPos = xpos;
    mYPos = ypos;

    ae_new<ScreenShake>(bEnabled_5C1BB6 ? 0 : 1, field_F4_bSmall);

    PSX_RECT rect = {};
    rect.x = FP_GetExponent(FP_FromInteger(-10) * field_FC_explosion_size);
    rect.y = FP_GetExponent(FP_FromInteger(-10) * field_FC_explosion_size);
    rect.w = FP_GetExponent(FP_FromInteger(10) * field_FC_explosion_size);
    rect.h = FP_GetExponent(FP_FromInteger(10) * field_FC_explosion_size);

    DealBlastDamage(&rect);

    SND_SEQ_PlaySeq(SeqId::Explosion1_14, 1, 1);
}

void Explosion::VUpdate()
{
    Event_Broadcast(kEventShooting, this);
    Event_Broadcast(kEventLoudNoise, this);
    Event_Broadcast(kEventSuspiciousNoise, this);

    PSX_RECT rect = {};

    switch (mAnim.field_92_current_frame)
    {
        case 2:
            rect.x = FP_GetExponent(FP_FromInteger(-20) * field_FC_explosion_size);
            rect.w = FP_GetExponent(FP_FromInteger(20) * field_FC_explosion_size);
            rect.y = FP_GetExponent(FP_FromInteger(-20) * field_FC_explosion_size);
            rect.h = FP_GetExponent(FP_FromInteger(10) * field_FC_explosion_size);
            DealBlastDamage(&rect);
            break;

        case 4:
        {
            ae_new<Flash>(Layer::eLayer_Above_FG1_39, static_cast<u8>(255), static_cast<u8>(255), static_cast<u8>(255), 1, TPageAbr::eBlend_1, 1);
            rect.x = FP_GetExponent(FP_FromInteger(-38) * field_FC_explosion_size);
            rect.w = FP_GetExponent(FP_FromInteger(38) * field_FC_explosion_size);
            rect.y = FP_GetExponent(FP_FromInteger(-38) * field_FC_explosion_size);
            rect.h = FP_GetExponent(FP_FromInteger(19) * field_FC_explosion_size);
            DealBlastDamage(&rect);
        }
        break;

        case 3:
        case 6:
            rect.x = FP_GetExponent(FP_FromInteger(-60) * field_FC_explosion_size);
            rect.w = FP_GetExponent(FP_FromInteger(60) * field_FC_explosion_size);
            rect.y = FP_GetExponent(FP_FromInteger(-60) * field_FC_explosion_size);
            rect.h = FP_GetExponent(FP_FromInteger(30) * field_FC_explosion_size);
            DealBlastDamage(&rect);
            break;

        case 8:
        {
            ae_new<ParticleBurst>(mXPos, mYPos, field_F4_bSmall ? 6 : 20, field_F8_scale, BurstType::eBigRedSparks_3, field_F4_bSmall ? 11 : 13);
            ae_new<Flash>(Layer::eLayer_Above_FG1_39, static_cast<u8>(255), static_cast<u8>(255), static_cast<u8>(255), 1, TPageAbr::eBlend_3, 1);
            break;
        }

        default:
            break;
    }

    if (mAnim.field_92_current_frame > 9)
    {
        if (field_F4_bSmall)
        {
            mSpriteScale -= FP_FromDouble(0.066);
        }
        else
        {
            mSpriteScale -= FP_FromDouble(0.2);
        }
    }

    if (mAnim.field_92_current_frame == 1)
    {
        u8** ppRes = field_F4_bSmall ? Add_Resource(ResourceManager::Resource_Animation, AEResourceID::kSmallExplo2ResID) : Add_Resource(ResourceManager::Resource_Animation, AEResourceID::kExplo2ResID);
        if (ppRes)
        {
            const AnimRecord& rec = field_F4_bSmall ? AnimRec(AnimId::Explosion_Small) : AnimRec(AnimId::Explosion);
            auto pParticle = ae_new<Particle>(
                mXPos, mYPos, rec.mFrameTableOffset,
                202, // Same size for both explosions for some reason
                91,  // ^^^
                ppRes);

            if (pParticle)
            {
                if (pParticle->mGameObjectFlags.Get(BaseGameObject::eListAddFailed_Bit1))
                {
                    pParticle->mGameObjectFlags.Set(BaseGameObject::eDead);
                }

                pParticle->mApplyShadows &= ~1u;
                pParticle->mAnim.mRenderMode = TPageAbr::eBlend_1;

                if (mAnim.field_92_current_frame == 3)
                {
                    pParticle->mAnim.mAnimFlags.Set(AnimFlags::eBit5_FlipX);
                    pParticle->mSpriteScale = mSpriteScale * FP_FromDouble(0.5);
                }
                else
                {
                    pParticle->mAnim.mAnimFlags.Clear(AnimFlags::eBit5_FlipX);
                    pParticle->mSpriteScale = mSpriteScale * FP_FromDouble(0.25);
                }
            }
        }
    }

    if (mAnim.mAnimFlags.Get(AnimFlags::eBit12_ForwardLoopCompleted))
    {
        mGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

void Explosion::VScreenChanged()
{
    if (gMap.mOverlayId != gMap.GetOverlayId())
    {
        mGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

void Explosion::DealBlastDamage(PSX_RECT* pRect)
{
    if (!gBaseAliveGameObjects_5C1B7C)
    {
        return;
    }

    PSX_RECT expandedRect = {};
    expandedRect.x = std::min(pRect->x, pRect->w);
    expandedRect.w = std::max(pRect->x, pRect->w);

    expandedRect.y = std::min(pRect->y, pRect->h);
    expandedRect.h = std::max(pRect->y, pRect->h);

    expandedRect.x += FP_GetExponent(mXPos);
    expandedRect.w += FP_GetExponent(mXPos);

    expandedRect.y += FP_GetExponent(mYPos);
    expandedRect.h += FP_GetExponent(mYPos);

    for (s32 idx = 0; idx < gBaseAliveGameObjects_5C1B7C->Size(); idx++)
    {
        BaseAliveGameObject* pObj = gBaseAliveGameObjects_5C1B7C->ItemAt(idx);
        if (!pObj)
        {
            break;
        }

        if (pObj->mGameObjectFlags.Get(BaseGameObject::eIsBaseAliveGameObject_Bit6))
        {
            PSX_RECT boundRect = {};
            pObj->VGetBoundingRect(&boundRect, 1);

            if (PSX_Rects_overlap_no_adjustment(&boundRect, &expandedRect) && mScale == pObj->mScale)
            {
                pObj->VTakeDamage(this);
            }
        }
    }

    auto pTlv = static_cast<Path_Slig*>(sPath_dword_BB47C0->TLV_Get_At_4DB4B0(
        expandedRect.x,
        expandedRect.y,
        expandedRect.w,
        expandedRect.h,
        TlvTypes::Slig_15));

    if (pTlv)
    {
        if (!pTlv->field_0_flags.Get(TLV_Flags::eBit2_Destroyed) && pTlv->field_12_start_state == Path_Slig::StartState::Sleeping_2)
        {
            pTlv->field_0_flags.Set(TLV_Flags::eBit2_Destroyed);

            const CameraPos dir = gMap.GetDirection_4811A0(
                gMap.mCurrentLevel,
                gMap.mCurrentPath,
                FP_FromInteger(pTlv->field_8_top_left.field_0_x),
                FP_FromInteger(pTlv->field_8_top_left.field_2_y));

            if (dir == CameraPos::eCamLeft_3)
            {
                ae_new<Gibs>(GibType::Slig_1, mXPos + FP_FromInteger(656), mYPos, FP_FromInteger(0), FP_FromInteger(0), FP_FromInteger(1), 0);
            }
            else if (dir == CameraPos::eCamRight_4)
            {
                ae_new<Gibs>(GibType::Slig_1, mXPos - FP_FromInteger(656), mYPos, FP_FromInteger(0), FP_FromInteger(0), FP_FromInteger(1), 0);
            }

            Stop_slig_sounds(dir, 0);
        }
    }
}
