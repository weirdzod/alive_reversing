#include "stdafx_ao.h"
#include "Function.hpp"
#include "BirdPortal.hpp"
#include "ResourceManager.hpp"
#include "stdlib.hpp"
#include "Collisions.hpp"
#include "Abe.hpp"
#include "ScreenClipper.hpp"
#include "ThrowableTotalIndicator.hpp"
#include "Flash.hpp"
#include "Midi.hpp"
#include "Dove.hpp"
#include "OrbWhirlWind.hpp"
#include "Particle.hpp"
#include "Events.hpp"
#include "Game.hpp"
#include "Math.hpp"
#include "ScreenManager.hpp"
#include "Sfx.hpp"
#include "PsxDisplay.hpp"
#include "../relive_lib/BaseGameObject.hpp"
#include "AbilityRing.hpp"
#include "Sys_common.hpp"

namespace AO {

void BirdPortalTerminator::VScreenChanged()
{
    // Staying alive
}

BirdPortalTerminator::BirdPortalTerminator(FP xpos, FP ypos, FP scale, PortalType /*portalType*/)
{
    mBaseGameObjectTypeId = ReliveTypes::eClawOrBirdPortalTerminator;

    const AnimRecord& rec = AO::AnimRec(AnimId::BirdPortal_TerminatorGrow);
    u8** ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);
    Animation_Init_417FD0(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1);
    
    field_10_anim.mRenderMode = TPageAbr::eBlend_1;
    field_BC_sprite_scale = scale;
    if (scale == FP_FromInteger(1))
    {
        field_10_anim.mRenderLayer = Layer::eLayer_Above_FG1_39;
    }
    else
    {
        field_10_anim.mRenderLayer = Layer::eLayer_Above_FG1_Half_20;
    }

    field_CC_bApplyShadows &= ~1u;

    mBaseAnimatedWithPhysicsGameObject_YPos = ypos;
    mBaseAnimatedWithPhysicsGameObject_XPos = xpos;

    field_C0_r = 255;
    field_C2_g = 128;
    field_C4_b = 64;
}

void BirdPortalTerminator::Fadeout()
{
    const s16 r = field_C0_r;
    field_C0_r = (r >> 1) + (r >> 2);

    const s16 g = field_C2_g;
    field_C2_g = (g >> 1) + (g >> 2);

    const s16 b = field_C4_b;
    field_C4_b = (b >> 1) + (b >> 2);
}

// ==========================================================================

BirdPortal::~BirdPortal()
{
    if (field_3C_pTerminator1)
    {
        field_3C_pTerminator1->mBaseGameObjectFlags.Set(Options::eDead);
    }
    if (field_40_pTerminator2)
    {
        field_40_pTerminator2->mBaseGameObjectFlags.Set(Options::eDead);
    }

    if (field_44_pScreenClipper1)
    {
        field_44_pScreenClipper1->mBaseGameObjectFlags.Set(Options::eDead);
    }

    if (field_48_pScreenClipper2)
    {
        field_48_pScreenClipper2->mBaseGameObjectFlags.Set(Options::eDead);
    }

    if (field_4C_pDovesArray)
    {
        for (s32 i = 0; i < field_4C_pDovesArray->Size(); i++)
        {
            Dove* pObj = field_4C_pDovesArray->ItemAt(i);
            if (!pObj)
            {
                break;
            }

            pObj->mBaseGameObjectRefCount--;
            pObj->mBaseGameObjectFlags.Set(Options::eDead);
        }

        field_4C_pDovesArray->field_4_used_size = 0;

        relive_delete field_4C_pDovesArray;
    }

    if (field_5C_pThrowableTotalIndicator)
    {
        field_5C_pThrowableTotalIndicator->mBaseGameObjectFlags.Set(Options::eDead);
        field_5C_pThrowableTotalIndicator->mBaseGameObjectRefCount--;
        field_5C_pThrowableTotalIndicator = nullptr;
    }

    u8** ppRes = nullptr;
    ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kPortalTerminatorAOResID, 0, 0);
    ResourceManager::FreeResource_455550(ppRes);
    ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kPortliteAOResID, 0, 0);
    ResourceManager::FreeResource_455550(ppRes);
    ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kPortlitAOResID, 0, 0);
    ResourceManager::FreeResource_455550(ppRes);
    ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kDovbasicAOResID, 0, 0);
    ResourceManager::FreeResource_455550(ppRes);

    if (field_68_sfx_ret)
    {
        SND_Stop_Channels_Mask_4774A0(field_68_sfx_ret);
        field_68_sfx_ret = 0;
    }

    gMap.TLV_Reset(field_2C_tlvInfo, -1, 0, 0);
    if (field_14_state >= PortalStates::PortalExit_SetPosition_17)
    {
        if (sActiveHero_507678)
        {
            if (field_34_scale == FP_FromInteger(1))
            {
                sActiveHero_507678->field_10_anim.mRenderLayer = Layer::eLayer_AbeMenu_32;
            }
            else
            {
                sActiveHero_507678->field_10_anim.mRenderLayer = Layer::eLayer_AbeMenu_Half_13;
            }
        }
    }
}

BirdPortal::BirdPortal(Path_BirdPortal* pTlv, s32 tlvInfo)
    : BaseGameObject(TRUE, 0)
{
    mBaseGameObjectTypeId = ReliveTypes::eBirdPortal;

    ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kPortalTerminatorAOResID, 1, 0);
    ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kDovbasicAOResID, 1, 0);
    ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kPortliteAOResID, 1, 0);
    ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kPortlitAOResID, 1, 0);

    field_2C_tlvInfo = tlvInfo;


    field_50_dest_level = MapWrapper::FromAO(pTlv->field_1A_dest_level);
    field_12_side = pTlv->field_18_side;
    field_54_dest_camera = pTlv->field_1E_dest_camera;
    field_52_dest_path = pTlv->field_1C_dest_path;
    field_10_portal_type = pTlv->field_24_portal_type;
    field_38_movie_id = pTlv->field_22_movie_id;
    field_56_num_muds_for_shrykull = pTlv->field_26_num_muds_for_shrykul;
    if (pTlv->field_20_scale == Scale_short::eHalf_1)
    {
        field_34_scale = FP_FromDouble(0.5);
    }
    else
    {
        field_34_scale = FP_FromInteger(1);
    }

    field_66_path = gMap.mCurrentPath;
    field_64_level = gMap.mCurrentLevel;
    field_14_state = PortalStates::CreatePortal_0;
    field_30_timer = 0;

    field_3C_pTerminator1 = nullptr;
    field_40_pTerminator2 = nullptr;
    field_44_pScreenClipper1 = nullptr;
    field_48_pScreenClipper2 = nullptr;
    field_4C_pDovesArray = nullptr;
    field_60_pOrbWhirlWind = nullptr;
    field_5C_pThrowableTotalIndicator = nullptr;

    PathLine* pLine = nullptr;
    FP hitX = {};
    sCollisions_DArray_504C6C->RayCast(
        FP_FromInteger(pTlv->field_10_top_left.field_0_x),
        FP_FromInteger(pTlv->field_10_top_left.field_2_y),
        FP_FromInteger(pTlv->field_14_bottom_right.field_0_x),
        FP_FromInteger(pTlv->field_14_bottom_right.field_2_y),
        &pLine,
        &hitX,
        &field_28_ypos,
        0xFFFFFFFF); // -1 ??

    field_18_xpos = FP_FromInteger(pTlv->field_10_top_left.field_0_x);
    field_68_sfx_ret = 0;
    field_1C_ypos = field_28_ypos - (FP_FromInteger(55) * field_34_scale);
}

void BirdPortal::CreateDovesAndShrykullNumber()
{
    for (u8 i = 0; i < 6; i++)
    {
        const AnimRecord& doveRec = AO::AnimRec(AnimId::Dove_Flying);
        auto pDove = ao_new<Dove>(
            doveRec.mFrameTableOffset,
            doveRec.mMaxW,
            doveRec.mMaxH,
            doveRec.mResourceId,
            field_18_xpos,
            field_1C_ypos,
            field_34_scale);
        if (pDove)
        {
            pDove->mBaseGameObjectRefCount++;
            if (field_10_portal_type == PortalType::eAbe_0)
            {
                pDove->AsAlmostACircle(field_18_xpos, field_1C_ypos + (field_34_scale * FP_FromInteger(30)), 42 * i);
            }
            else
            {
                pDove->AsACircle(field_18_xpos, field_1C_ypos + (field_34_scale * FP_FromInteger(30)), 42 * i);
            }

            pDove->field_BC_sprite_scale = field_34_scale;
            field_4C_pDovesArray->Push_Back(pDove);
        }
    }

    if (field_10_portal_type == PortalType::eShrykull_2)
    {
        Layer indicatorLayer = Layer::eLayer_0;
        if (field_34_scale != FP_FromDouble(0.5))
        {
            indicatorLayer = Layer::eLayer_27;
        }
        else
        {
            indicatorLayer = Layer::eLayer_8;
        }
        field_5C_pThrowableTotalIndicator = ao_new<ThrowableTotalIndicator>(
            field_18_xpos,
            field_1C_ypos + FP_FromInteger(10),
            indicatorLayer,
            field_34_scale,
            field_56_num_muds_for_shrykull,
            0);
        if (field_5C_pThrowableTotalIndicator)
        {
            field_5C_pThrowableTotalIndicator->mBaseGameObjectRefCount++;
        }
    }
}

void BirdPortal::CreateTerminators()
{
    field_3C_pTerminator1 = ao_new<BirdPortalTerminator>(
        field_18_xpos,
        field_1C_ypos,
        field_34_scale,
        field_10_portal_type);

    field_40_pTerminator2 = ao_new<BirdPortalTerminator>(
        field_18_xpos,
        field_1C_ypos,
        field_34_scale,
        field_10_portal_type);
}

void BirdPortal::VUpdate()
{
    const CameraPos direction = gMap.GetDirection(
        field_64_level,
        field_66_path,
        field_18_xpos,
        field_1C_ypos);

    if (!field_68_sfx_ret)
    {
        if (field_14_state > PortalStates::CreateTerminators_4 && field_14_state < PortalStates::StopSound_11)
        {
            field_68_sfx_ret = SFX_Play_Camera(SoundEffect::PortalOpening_67, 45, direction);
        }
    }

    switch (field_14_state)
    {
        case PortalStates::CreatePortal_0:
            field_4C_pDovesArray = ao_new<DynamicArrayT<Dove>>(6);

            CreateDovesAndShrykullNumber();
            field_14_state = PortalStates::IdlePortal_1;
            break;

        case PortalStates::IdlePortal_1:
        {
            if (static_cast<s32>(sGnFrame) >= field_30_timer)
            {
                SFX_Play_Mono(SoundEffect::Dove_16, 35, 0);
                field_30_timer = sGnFrame + Math_RandomRange_450F20(24, 40);
            }

            auto pTarget = Abe::FindObjectToPossess_421410();
            if (!Event_Get(kEventAbeOhm) || pTarget)
            {
                if (IsScaredAway() || Event_Get(kEventShooting) || (Event_Get(kEventAbeOhm) && pTarget))
                {
                    for (s32 i = 0; i < field_4C_pDovesArray->Size(); i++)
                    {
                        Dove* pDove = field_4C_pDovesArray->ItemAt(i);
                        if (!pDove)
                        {
                            break;
                        }
                        pDove->mBaseGameObjectRefCount--;
                        pDove->FlyAway(1);
                    }

                    field_4C_pDovesArray->field_4_used_size = 0;
                    relive_delete field_4C_pDovesArray;
                    field_4C_pDovesArray = nullptr;

                    if (field_5C_pThrowableTotalIndicator)
                    {
                        field_5C_pThrowableTotalIndicator->mBaseGameObjectFlags.Set(Options::eDead);
                        field_5C_pThrowableTotalIndicator->mBaseGameObjectRefCount--;
                        field_5C_pThrowableTotalIndicator = nullptr;
                    }

                    SFX_Play_Mono(SoundEffect::Dove_16, 70, 0);
                    mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                }
            }
            else
            {
                if (field_5C_pThrowableTotalIndicator)
                {
                    field_5C_pThrowableTotalIndicator->mBaseGameObjectFlags.Set(Options::eDead);
                    field_5C_pThrowableTotalIndicator->mBaseGameObjectRefCount--;
                    field_5C_pThrowableTotalIndicator = nullptr;
                }

                for (s32 i = 0; i < field_4C_pDovesArray->Size(); i++)
                {
                    Dove* pDove = field_4C_pDovesArray->ItemAt(i);
                    if (!pDove)
                    {
                        break;
                    }
                    pDove->AsJoin(field_18_xpos, field_1C_ypos + (field_34_scale * FP_FromInteger(20)));
                }

                field_30_timer = sGnFrame + 15;
                field_14_state = PortalStates::JoinDovesInCenter_2;
                Event_Broadcast(kEventPortalOpen, this);
                SFX_Play_Mono(SoundEffect::Dove_16, 70, 0);
            }
        }
        break;

        case PortalStates::JoinDovesInCenter_2:
            Event_Broadcast(kEventPortalOpen, this);
            if (static_cast<s32>(sGnFrame) > field_30_timer)
            {
                CreateTerminators();
                field_30_timer = sGnFrame + 5;
                field_14_state = PortalStates::KillDoves_3;
            }
            break;

        case PortalStates::KillDoves_3:
            Event_Broadcast(kEventPortalOpen, this);
            if (static_cast<s32>(sGnFrame) > field_30_timer)
            {
                for (s32 i = 0; i < field_4C_pDovesArray->Size(); i++)
                {
                    Dove* pDove = field_4C_pDovesArray->ItemAt(i);
                    if (!pDove)
                    {
                        break;
                    }

                    pDove->mBaseGameObjectRefCount--;
                    pDove->mBaseGameObjectFlags.Set(Options::eDead);
                }

                field_4C_pDovesArray->field_4_used_size = 0;
                relive_delete field_4C_pDovesArray;
                field_4C_pDovesArray = nullptr;
                field_14_state = PortalStates::CreateTerminators_4;
            }
            break;

        case PortalStates::CreateTerminators_4:
            Event_Broadcast(kEventPortalOpen, this);
            if (field_3C_pTerminator1->field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                const AnimRecord& rec = AO::AnimRec(AnimId::BirdPortal_TerminatorIdle);
                field_3C_pTerminator1->field_10_anim.Set_Animation_Data(rec.mFrameTableOffset, 0);
                field_40_pTerminator2->field_10_anim.Set_Animation_Data(rec.mFrameTableOffset, 0);
                field_30_timer = sGnFrame + 12;
                field_14_state = PortalStates::ExpandTerminators_5;
                field_68_sfx_ret = SFX_Play_Mono(SoundEffect::PortalOpening_67, 0, 0);
                SFX_Play_Pitch(SoundEffect::IngameTransition_107, 115, 300, 0);
            }
            break;

        case PortalStates::ExpandTerminators_5:
            Event_Broadcast(kEventPortalOpen, this);
            field_3C_pTerminator1->mBaseAnimatedWithPhysicsGameObject_YPos -= (FP_FromDouble(3.5) * field_34_scale);
            field_40_pTerminator2->mBaseAnimatedWithPhysicsGameObject_YPos += (FP_FromDouble(3.5) * field_34_scale);
            if (static_cast<s32>(sGnFrame) > field_30_timer)
            {
                field_14_state = PortalStates::ActivePortal_6;
            }
            break;

        case PortalStates::ActivePortal_6:
            Event_Broadcast(kEventPortalOpen, this);
            if ((field_10_portal_type != PortalType::eWorker_1 && field_10_portal_type != PortalType::eShrykull_2) || Event_Get(kEventAbeOhm))
            {
                if ((Math_NextRandom() % 8) == 0)
                {
                    const AnimRecord& rec = AO::AnimRec(AnimId::BirdPortal_Sparks);
                    u8** ppLightRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);
                    if (ppLightRes)
                    {
                        auto pParticle = ao_new<Particle>(
                            field_40_pTerminator2->mBaseAnimatedWithPhysicsGameObject_XPos,
                            (FP_FromInteger(10) * field_34_scale) + field_40_pTerminator2->mBaseAnimatedWithPhysicsGameObject_YPos,
                            rec.mFrameTableOffset,
                            rec.mMaxW,
                            rec.mMaxH,
                            ppLightRes);
                        if (pParticle)
                        {
                            pParticle->field_CC_bApplyShadows &= ~1u;
                            pParticle->field_10_anim.mRenderMode = TPageAbr::eBlend_1;
                            pParticle->mBaseGameObjectTypeId = ReliveTypes::eBirdPortalTerminator;
                            pParticle->field_BC_sprite_scale = field_34_scale;

                            if (sGnFrame % 2)
                            {
                                pParticle->field_10_anim.mAnimFlags.Set(AnimFlags::eBit19_LoopBackwards);
                                pParticle->field_10_anim.SetFrame(pParticle->field_10_anim.Get_Frame_Count());
                            }
                        }

                        if (direction == CameraPos::eCamCurrent_0)
                        {
                            SFX_Play_Pitch(SoundEffect::BirdPortalSpark_48, 50, 2400, 0);
                        }
                    }
                }
                if (!(sGnFrame % 8))
                {
                    AbilityRing::Factory(
                        field_3C_pTerminator1->mBaseAnimatedWithPhysicsGameObject_XPos,
                        field_3C_pTerminator1->mBaseAnimatedWithPhysicsGameObject_YPos + FP_FromInteger(7),
                        RingTypes::eShrykull_Pulse_Orange_6);
                    AbilityRing::Factory(
                        field_40_pTerminator2->mBaseAnimatedWithPhysicsGameObject_XPos,
                        field_40_pTerminator2->mBaseAnimatedWithPhysicsGameObject_YPos + FP_FromInteger(7),
                        RingTypes::eShrykull_Pulse_Orange_6);
                }
            }
            else
            {
                VGiveShrykull(0);
                if (field_14_state != PortalStates::ShrykullGetDoves_7)
                {
                    field_14_state = PortalStates::FadeoutTerminators_22;
                    field_30_timer = sGnFrame + 30;
                }
            }
            break;

        case PortalStates::ShrykullGetDoves_7:
            if (field_58_received_doves >= 7)
            {
                field_14_state = PortalStates::GetShrykull_9;
                field_30_timer = sGnFrame + 20;
            }
            else
            {
                FP xOff = {};
                if (field_12_side == PortalSide::eLeft_1)
                {
                    xOff = (field_34_scale * FP_FromInteger(16));
                }
                else
                {
                    xOff = (field_34_scale * FP_FromInteger(-16));
                }

                const s16 scale32 = FP_GetExponent(field_34_scale * FP_FromInteger(32));
                if (static_cast<s32>(sGnFrame) >= field_30_timer)
                {
                    field_30_timer = sGnFrame + Math_RandomRange_450F20(4, 12);
                    const AnimRecord& doveRec = AO::AnimRec(AnimId::Dove_Flying);

                    auto pDoveMem = ao_new<Dove>(
                        doveRec.mFrameTableOffset,
                        doveRec.mMaxW,
                        doveRec.mMaxH,
                        doveRec.mResourceId,
                        field_18_xpos + FP_FromInteger(FP_GetExponent(xOff)),
                        field_1C_ypos + FP_FromInteger(Math_RandomRange_450F20(-scale32, scale32)),
                        field_34_scale);
                    if (pDoveMem)
                    {
                        SFX_Play_Mono(SoundEffect::Dove_16, 70, 0);
                        pDoveMem->field_BC_sprite_scale = field_34_scale;
                        pDoveMem->AsJoin(sActiveHero_507678->mBaseAnimatedWithPhysicsGameObject_XPos,
                            FP_FromInteger(Math_RandomRange_450F20(-36, 4)) + sActiveHero_507678->mBaseAnimatedWithPhysicsGameObject_YPos);
                    }

                    field_58_received_doves++;
                    if (field_58_received_doves == 6)
                    {
                        field_60_pOrbWhirlWind->ToSpin(
                            sActiveHero_507678->mBaseAnimatedWithPhysicsGameObject_XPos,
                            sActiveHero_507678->mBaseAnimatedWithPhysicsGameObject_YPos - (sActiveHero_507678->field_BC_sprite_scale * FP_FromInteger(38)),
                            sActiveHero_507678);
                        field_60_pOrbWhirlWind = nullptr;
                    }
                }
            }
            break;

        case PortalStates::GetShrykull_9:
            if (static_cast<s32>(sGnFrame) >= field_30_timer)
            {
                Abe::Get_Shrykull_Resources_42F480();
                field_14_state = PortalStates::CollapseTerminators_10;
                sActiveHero_507678->field_168_ring_pulse_timer = sGnFrame + 32000;
                sActiveHero_507678->field_16C_bHaveShrykull = TRUE;
                if (sActiveHero_507678->mCurrentMotion == eAbeMotions::Motion_150_Chant_42FD50)
                {
                    sActiveHero_507678->ChangeChantState_430510(0);
                }
            }
            break;

        case PortalStates::CollapseTerminators_10:
            field_3C_pTerminator1->mBaseAnimatedWithPhysicsGameObject_YPos += FP_FromDouble(3.5) * field_34_scale;
            field_40_pTerminator2->mBaseAnimatedWithPhysicsGameObject_YPos -= FP_FromDouble(3.5) * field_34_scale;

            if (FP_GetExponent(field_3C_pTerminator1->mBaseAnimatedWithPhysicsGameObject_YPos) >= FP_GetExponent(field_40_pTerminator2->mBaseAnimatedWithPhysicsGameObject_YPos))
            {
                const AnimRecord& rec = AO::AnimRec(AnimId::BirdPortal_Flash);
                u8** ppLightRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);
                if (ppLightRes)
                {
                    auto pParticle_1 = ao_new<Particle>(
                        field_40_pTerminator2->mBaseAnimatedWithPhysicsGameObject_XPos,
                        field_40_pTerminator2->mBaseAnimatedWithPhysicsGameObject_YPos,
                        rec.mFrameTableOffset,
                        rec.mMaxW,
                        rec.mMaxH,
                        ppLightRes);
                    if (pParticle_1)
                    {
                        pParticle_1->field_CC_bApplyShadows &= ~1u;
                        pParticle_1->field_10_anim.mRenderMode = TPageAbr::eBlend_1;
                        pParticle_1->field_BC_sprite_scale = field_34_scale;
                    }

                }

                field_14_state = PortalStates::StopSound_11;
                field_30_timer = sGnFrame + 5;
                field_3C_pTerminator1->mBaseGameObjectFlags.Set(Options::eDead);
                field_40_pTerminator2->mBaseGameObjectFlags.Set(Options::eDead);
                field_3C_pTerminator1 = nullptr;
                field_40_pTerminator2 = nullptr;
                SFX_Play_Pitch(SoundEffect::MenuNavigation_61, 100, -1800, 0);
            }
            break;

        case PortalStates::StopSound_11:
            if (static_cast<s32>(sGnFrame) > field_30_timer)
            {
                if (field_68_sfx_ret)
                {
                    SND_Stop_Channels_Mask_4774A0(field_68_sfx_ret);
                    field_68_sfx_ret = 0;
                }
                field_14_state = PortalStates::CreateFlash1_12;
            }
            break;

        case PortalStates::CreateFlash1_12:
        {
            ao_new<Flash>(Layer::eLayer_FadeFlash_40, 255u, 255u, 255u);
            field_14_state = PortalStates::CreateFlash2_13;
        }
        break;

        case PortalStates::CreateFlash2_13:
        {
            ao_new<Flash>(Layer::eLayer_FadeFlash_40, 255u, 255u, 255u, 1, TPageAbr::eBlend_0, 1);
            field_14_state = PortalStates::CreateFlash3_14;
        }
        break;

        case PortalStates::CreateFlash3_14:
        {
            ao_new<Flash>(Layer::eLayer_FadeFlash_40, 255u, 255u, 255u, 0, TPageAbr::eBlend_0, 1);
            field_14_state = PortalStates::KillPortal_15;
            field_30_timer = sGnFrame + 5;
        }
        break;

        case PortalStates::KillPortal_15:
            if (static_cast<s32>(sGnFrame) > field_30_timer)
            {
                if (field_10_portal_type == PortalType::eWorker_1 || field_10_portal_type == PortalType::eShrykull_2)
                {
                    mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                }
                else
                {
                    field_14_state = PortalStates::State_16;
                }
            }
            break;

        case PortalStates::PortalExit_SetPosition_17:
        {
            pScreenManager_4FF7C8->field_36_flags = (pScreenManager_4FF7C8->field_36_flags & ~1) ^ 1;
            pScreenManager_4FF7C8->InvalidateRect(
                0,
                0,
                gPsxDisplay_504C78.field_0_width,
                gPsxDisplay_504C78.field_2_height,
                pScreenManager_4FF7C8->field_2E_idx);

            CreateTerminators();

            field_14_state = PortalStates::PortalExit_CreateTerminators_18;
            field_68_sfx_ret = SFX_Play_Mono(SoundEffect::PortalOpening_67, 0, 0);
        }
        break;

        case PortalStates::PortalExit_CreateTerminators_18:
            if (field_3C_pTerminator1->field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                const AnimRecord& rec = AO::AnimRec(AnimId::BirdPortal_TerminatorIdle);
                field_3C_pTerminator1->field_10_anim.Set_Animation_Data(rec.mFrameTableOffset, nullptr);
                field_40_pTerminator2->field_10_anim.Set_Animation_Data(rec.mFrameTableOffset, nullptr);
                field_14_state = PortalStates::PortalExit_ExpandTerminators_19;
                field_30_timer = sGnFrame + 12;
            }
            break;

        case PortalStates::PortalExit_ExpandTerminators_19:
            field_3C_pTerminator1->mBaseAnimatedWithPhysicsGameObject_YPos -= (FP_FromDouble(3.5) * field_34_scale);
            field_40_pTerminator2->mBaseAnimatedWithPhysicsGameObject_YPos += (FP_FromDouble(3.5) * field_34_scale);
            if (static_cast<s32>(sGnFrame) > field_30_timer)
            {
                field_14_state = PortalStates::State_20;
            }
            break;

        case PortalStates::KillPortalClipper_21:
            if (static_cast<s32>(sGnFrame) > field_30_timer)
            {
                const AnimRecord& rec = AO::AnimRec(AnimId::BirdPortal_TerminatorShrink);
                field_3C_pTerminator1->field_10_anim.Set_Animation_Data(rec.mFrameTableOffset, nullptr);
                field_40_pTerminator2->field_10_anim.Set_Animation_Data(rec.mFrameTableOffset, nullptr);
                field_14_state = PortalStates::FadeoutTerminators_22;
                field_30_timer = sGnFrame + 30;

                if (field_44_pScreenClipper1)
                {
                    field_44_pScreenClipper1->mBaseGameObjectFlags.Set(Options::eDead);
                    field_44_pScreenClipper1 = nullptr;
                }

                if (field_68_sfx_ret)
                {
                    SND_Stop_Channels_Mask_4774A0(field_68_sfx_ret);
                    field_68_sfx_ret = 0;
                }
            }
            break;

        case PortalStates::FadeoutTerminators_22:
            if (static_cast<s32>(sGnFrame) <= field_30_timer)
            {
                field_3C_pTerminator1->Fadeout();
                field_40_pTerminator2->Fadeout();
            }
            else
            {
                mBaseGameObjectFlags.Set(BaseGameObject::eDead);
            }
            break;

        default:
            break;
    }

    if (Event_Get(kEventDeathReset))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

s16 BirdPortal::IsScaredAway()
{
    for (s32 i = 0; i < gBaseAliveGameObjects_4FC8A0->Size(); i++)
    {
        BaseAliveGameObject* pObj = gBaseAliveGameObjects_4FC8A0->ItemAt(i);
        if (!pObj)
        {
            return FALSE;
        }

        switch (pObj->mBaseGameObjectTypeId)
        {
            case ReliveTypes::eElum:
            case ReliveTypes::eAbe:
            case ReliveTypes::eRingOrLiftMud:
            case ReliveTypes::eParamite:
            case ReliveTypes::eScrab:
            case ReliveTypes::eSlig:
                if (pObj->field_B0_path_number != field_66_path)
                {
                    continue;
                }

                if (FP_Abs(pObj->mBaseAnimatedWithPhysicsGameObject_XPos - field_18_xpos) >= FP_FromInteger(75) || FP_Abs(pObj->mBaseAnimatedWithPhysicsGameObject_YPos - field_28_ypos) >= FP_FromInteger(30) || pObj->field_BC_sprite_scale != field_34_scale)
                {
                    continue;
                }
                return TRUE;

            default:
                continue;
        }
    }
    return FALSE;
}

void BirdPortal::VGiveShrykull(s16 bPlaySound)
{
    if (field_10_portal_type == PortalType::eShrykull_2 && field_56_num_muds_for_shrykull <= 0)
    {
        field_14_state = PortalStates::ShrykullGetDoves_7;
        field_30_timer = sGnFrame + 12;
        field_58_received_doves = 0;

        field_60_pOrbWhirlWind = ao_new<OrbWhirlWind>(
            sActiveHero_507678->mBaseAnimatedWithPhysicsGameObject_XPos,
            sActiveHero_507678->mBaseAnimatedWithPhysicsGameObject_YPos - (sActiveHero_507678->field_BC_sprite_scale * FP_FromInteger(38)),
            sActiveHero_507678->field_BC_sprite_scale);

        if (sActiveHero_507678->mCurrentMotion == eAbeMotions::Motion_150_Chant_42FD50)
        {
            sActiveHero_507678->ChangeChantState_430510(1);
        }
    }
    else
    {
        field_14_state = PortalStates::CollapseTerminators_10;
        if ((field_10_portal_type == PortalType::eWorker_1 || field_10_portal_type == PortalType::eShrykull_2) && sActiveHero_507678->mCurrentMotion == eAbeMotions::Motion_150_Chant_42FD50)
        {
            sActiveHero_507678->ChangeChantState_430510(0);
        }
    }

    for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
    {
        BaseGameObject* pObj = gBaseGameObjects->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj->mBaseGameObjectTypeId == ReliveTypes::eBirdPortalTerminator)
        {
            pObj->mBaseGameObjectFlags.Set(Options::eDead);
        }
    }

    if (bPlaySound)
    {
        SFX_Play_Pitch(SoundEffect::MenuNavigation_61, 70, -1600, 0);
    }
}

void BirdPortal::VScreenChanged()
{
    if (field_14_state <= PortalStates::IdlePortal_1 || field_14_state >= PortalStates::KillPortalClipper_21 || ((gMap.mCurrentLevel != gMap.mLevel || gMap.mCurrentPath != gMap.mPath) && (field_14_state != PortalStates::State_16 || field_10_portal_type != PortalType::eAbe_0 || gMap.mLevel != field_50_dest_level || gMap.mPath != field_52_dest_path)))
    {
        mBaseGameObjectFlags.Set(Options::eDead);
    }

    if (mBaseGameObjectFlags.Get(BaseGameObject::eDead))
    {
        if (field_3C_pTerminator1)
        {
            field_3C_pTerminator1->mBaseGameObjectFlags.Set(Options::eDead);
            field_3C_pTerminator1 = nullptr;
        }

        if (field_40_pTerminator2)
        {
            field_40_pTerminator2->mBaseGameObjectFlags.Set(Options::eDead);
            field_40_pTerminator2 = nullptr;
        }

        if (field_44_pScreenClipper1)
        {
            field_44_pScreenClipper1->mBaseGameObjectFlags.Set(Options::eDead);
            field_44_pScreenClipper1 = nullptr;
        }

        if (field_48_pScreenClipper2)
        {
            field_48_pScreenClipper2->mBaseGameObjectFlags.Set(Options::eDead);
            field_48_pScreenClipper2 = nullptr;
        }
    }
    else
    {
        if (field_68_sfx_ret)
        {
            SND_Stop_Channels_Mask_4774A0(field_68_sfx_ret);
            field_68_sfx_ret = 0;
        }
    }
}

void BirdPortal::VStopAudio()
{
    if (field_68_sfx_ret)
    {
        SND_Stop_Channels_Mask_4774A0(field_68_sfx_ret);
        field_68_sfx_ret = 0;
    }
}

void BirdPortal::VKillPortalClipper()
{
    if (field_44_pScreenClipper1)
    {
        field_44_pScreenClipper1->mBaseGameObjectFlags.Set(Options::eDead);
        field_44_pScreenClipper1 = nullptr;

        field_48_pScreenClipper2->mBaseGameObjectFlags.Set(Options::eDead);
        field_48_pScreenClipper2 = nullptr;
    }
}

Bool32 BirdPortal::VStateIs6()
{
    return field_14_state == PortalStates::ActivePortal_6;
}

Bool32 BirdPortal::VStateIs16()
{
    return field_14_state == PortalStates::State_16;
}

void BirdPortal::VExitPortal()
{
    field_66_path = gMap.mCurrentPath;
    field_64_level = gMap.mCurrentLevel;

    auto pPortalExitTlv = static_cast<Path_BirdPortalExit*>(gMap.TLV_First_Of_Type_In_Camera(TlvTypes::BirdPortalExit_53, 0));

    if (pPortalExitTlv)
    {
        PathLine* pLine = nullptr;
        sCollisions_DArray_504C6C->RayCast(
            FP_FromInteger(pPortalExitTlv->field_10_top_left.field_0_x),
            FP_FromInteger(pPortalExitTlv->field_10_top_left.field_2_y),
            FP_FromInteger(pPortalExitTlv->field_14_bottom_right.field_0_x),
            FP_FromInteger(pPortalExitTlv->field_14_bottom_right.field_2_y),
            &pLine,
            &field_20_exit_x,
            &field_24_exit_y,
            0xFFFFFFFF); // -1 ??

        field_1C_ypos = field_24_exit_y - FP_FromInteger(55);

        field_20_exit_x = FP_FromInteger(pPortalExitTlv->field_10_top_left.field_0_x);
        field_18_xpos = field_20_exit_x;
        field_12_side = pPortalExitTlv->field_18_side;

        if (pPortalExitTlv->field_1A_scale == Scale_short::eHalf_1)
        {
            field_34_scale = FP_FromDouble(0.5);
            sActiveHero_507678->field_10_anim.mRenderLayer = Layer::eLayer_InBirdPortal_Half_11;
        }
        else
        {
            field_34_scale = FP_FromInteger(1);
            sActiveHero_507678->field_10_anim.mRenderLayer = Layer::eLayer_InBirdPortal_30;
        }

        sActiveHero_507678->field_BC_sprite_scale = field_34_scale;
        field_14_state = PortalStates::PortalExit_SetPosition_17;
        sActiveHero_507678->field_B2_lvl_number = gMap.mCurrentLevel;
        sActiveHero_507678->field_B0_path_number = gMap.mCurrentPath;
    }
    else
    {
        LOG_WARNING("Bird portal exit object not found!");
    }
}

Bool32 BirdPortal::VStateIs20()
{
    return field_14_state == PortalStates::State_20;
}

void BirdPortal::VIncreaseTimerAndKillPortalClipper()
{
    field_14_state = PortalStates::KillPortalClipper_21;
    field_30_timer = sGnFrame + 30;
}

void BirdPortal::VMudSaved()
{
    field_56_num_muds_for_shrykull--;
}

void BirdPortal::VGetMapChange(EReliveLevelIds* level, u16* path, u16* camera, CameraSwapEffects* screenChangeEffect, u16* movieId)
{
    *level = field_50_dest_level;
    *path = field_52_dest_path;
    *camera = field_54_dest_camera;

    // Positive
    if (field_38_movie_id > 0)
    {
        *movieId = field_38_movie_id;
        *screenChangeEffect = CameraSwapEffects::ePlay1FMV_5;
        return;
    }

    // Zero
    if (field_38_movie_id == 0)
    {
        *screenChangeEffect = CameraSwapEffects::eInstantChange_0;
        return;
    }

    // Negative cases - dead code ??
    if (sActiveHero_507678->field_2A8_flags.Get(Flags_2A8::e2A8_Bit12_bParamoniaDone) && sActiveHero_507678->field_2A8_flags.Get(Flags_2A8::e2A8_eBit13_bScrabaniaDone))
    {
        *movieId = 1617 - (10000 * field_38_movie_id);
        *screenChangeEffect = CameraSwapEffects::ePlay1FMV_5;
        return;
    }

    *movieId = 17 - (100 * field_38_movie_id);
    *screenChangeEffect = CameraSwapEffects::ePlay1FMV_5;
}

s16 BirdPortal::VPortalClipper(s16 bUnknown)
{
    if (bUnknown && field_14_state != PortalStates::ActivePortal_6)
    {
        return 0;
    }

    if (field_44_pScreenClipper1)
    {
        return 1;
    }

    const s16 portalX = static_cast<s16>(PsxToPCX(pScreenManager_4FF7C8->field_14_xpos + FP_GetExponent(field_18_xpos) - FP_GetExponent(pScreenManager_4FF7C8->field_10_pCamPos->field_0_x), 11));

    PSX_Point xy = {};
    PSX_Point wh = {};
    if (field_12_side != PortalSide::eRight_0)
    {
        xy.field_0_x = 0;
        wh.field_0_x = portalX;
    }
    else
    {
        xy.field_0_x = portalX;
        wh.field_0_x = 640;
    }

    xy.field_2_y = 0;
    wh.field_2_y = 240;

    field_44_pScreenClipper1 = ao_new<ScreenClipper>(xy, wh, Layer::eLayer_0);
    if (field_44_pScreenClipper1)
    {
        if (field_34_scale == FP_FromInteger(1))
        {
            field_44_pScreenClipper1->field_38_ot_layer = Layer::eLayer_BirdPortal_29;
        }
        else
        {
            field_44_pScreenClipper1->field_38_ot_layer = Layer::eLayer_BirdPortal_Half_10;
        }
    }

    field_48_pScreenClipper2 = ao_new<ScreenClipper>(PSX_Point{ 0, 0 }, PSX_Point{ 640, 240 }, Layer::eLayer_0);
    if (field_48_pScreenClipper2)
    {
        if (field_34_scale == FP_FromInteger(1))
        {
            field_48_pScreenClipper2->field_38_ot_layer = Layer::eLayer_DoorFlameRollingBallPortalClip_Half_31;
        }
        else
        {
            field_48_pScreenClipper2->field_38_ot_layer = Layer::eLayer_DoorFlameRollingBallPortalClip_12;
        }
    }

    return 1;
}

} // namespace AO
