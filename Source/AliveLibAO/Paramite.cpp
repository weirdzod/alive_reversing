#include "stdafx_ao.h"
#include "Function.hpp"
#include "Paramite.hpp"
#include "Math.hpp"
#include "Sfx.hpp"
#include "ResourceManager.hpp"
#include "Shadow.hpp"
#include "stdlib.hpp"
#include "Abe.hpp"
#include "Collisions.hpp"
#include "ParamiteWeb.hpp"
#include "MusicController.hpp"
#include "Midi.hpp"
#include "PlatformBase.hpp"
#include "Gibs.hpp"
#include "Blood.hpp"
#include "Game.hpp"
#include "Events.hpp"
#include "DDCheat.hpp"
#include "VRam.hpp"
#include "Meat.hpp"
#include "CameraSwapper.hpp"
#include "PlatformBase.hpp"
#include "LiftPoint.hpp"
#include "Particle.hpp"
#include "SwitchStates.hpp"
#include "Grid.hpp"

void Paramite_ForceLink()
{ }

namespace AO {

using TParamiteMotionFunction = decltype(&Paramite::Motion_0_Idle);

const TParamiteMotionFunction sParamiteMotionTable_4CDCB0[] = {
    &Paramite::Motion_0_Idle,
    &Paramite::Motion_1_WalkBegin,
    &Paramite::Motion_2_Walking,
    &Paramite::Motion_3_Running,
    &Paramite::Motion_4_Unknown,
    &Paramite::Motion_5_Turn,
    &Paramite::Motion_6_Hop,
    &Paramite::Motion_7_Unknown,
    &Paramite::Motion_8_WalkRunTransition,
    &Paramite::Motion_9_WalkEnd,
    &Paramite::Motion_10_RunBegin,
    &Paramite::Motion_11_RunEnd,
    &Paramite::Motion_12_Falling,
    &Paramite::Motion_13_GameSpeakBegin,
    &Paramite::Motion_14_PreHiss,
    &Paramite::Motion_15_Hiss,
    &Paramite::Motion_16_PostHiss,
    &Paramite::Motion_17_GameSpeakEnd,
    &Paramite::Motion_18_RunningAttack,
    &Paramite::Motion_19_Empty,
    &Paramite::Motion_20_SurpriseWeb,
    &Paramite::Motion_21_WebLeaveDown,
    &Paramite::Motion_22_Unknown,
    &Paramite::Motion_23_Eating,
    &Paramite::Motion_24_Struggle,
    &Paramite::Motion_25_Death,
};

const static AnimId sParamiteAnimIdTable_4CDD18[] = {
    AnimId::Paramite_Idle,
    AnimId::Paramite_WalkBegin,
    AnimId::Paramite_Walking,
    AnimId::Paramite_Running,
    AnimId::Paramite_Idle,
    AnimId::Paramite_Turn,
    AnimId::Paramite_Hop,
    AnimId::Paramite_AO_M_7_Unknown,
    AnimId::Paramite_WalkRunTransition,
    AnimId::Paramite_WalkEnd,
    AnimId::Paramite_RunBegin,
    AnimId::Paramite_RunEnd,
    AnimId::Paramite_Falling,
    AnimId::Paramite_GameSpeakBegin,
    AnimId::Paramite_PreHiss,
    AnimId::Paramite_Hiss,
    AnimId::Paramite_PostHiss,
    AnimId::Paramite_GameSpeakEnd,
    AnimId::Paramite_RunningAttack,
    AnimId::Paramite_SurpriseWeb,
    AnimId::Paramite_SurpriseWeb,
    AnimId::Paramite_WebLeaveDown,
    AnimId::Paramite_AO_M_22_Unknown,
    AnimId::Paramite_Eating,
    AnimId::Paramite_Struggle,
    AnimId::Paramite_Death};

static BrainFunctionData<Paramite::TParamiteBrain> sParamiteBrainTable[]{
    {&Paramite::Brain_0_Patrol, 0x447A10, "Brain_0_Patrol"},
    {&Paramite::Brain_1_SurpriseWeb, 0x448D00, "Brain_1_SurpriseWeb"},
    {&Paramite::Brain_2_Struggling, 0x44DD70, "Brain_2_Struggling"},
    {&Paramite::Brain_3_Death, 0x448BF0, "Brain_3_Death"},
    {&Paramite::Brain_4_ChasingAbe, 0x449170, "Brain_4_ChasingAbe"},
    {&Paramite::Brain_5_SpottedMeat, 0x449CD0, "Brain_5_SpottedMeat"},
};

Paramite::Paramite(Path_Paramite* pTlv, s32 tlvInfo)
    : BaseAliveGameObject()
{
    mBaseGameObjectTypeId = ReliveTypes::eParamite;

    for (s32 i = 0; i < ALIVE_COUNTOF(field_150_resources); i++)
    {
        field_150_resources[i] = nullptr;
    }

    field_150_resources[0] = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kArjbasicAOResID, 1, 0);
    field_150_resources[5] = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kArjpumpAOResID, 1, 0);
    field_150_resources[14] = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kArjponceAOResID, 1, 0);
    field_150_resources[4] = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kArjclimbAOResID, 1, 0);
    field_150_resources[9] = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kArjscrchAOResID, 1, 0);
    field_150_resources[1] = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kArjeatAOResID, 1, 0);
    field_150_resources[15] = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kArjfalrkAOResID, 1, 0);
    field_150_resources[10] = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kArjwaspAOResID, 1, 0);

    ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kWebAOResID, 1, 0);

    const AnimRecord& rec = AO::AnimRec(AnimId::Paramite_Idle);
    Animation_Init_417FD0(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, field_150_resources[0], 1);

    field_12A_res_idx = 0;


    mBaseAliveGameObjectFlags.Set(Flags_10A::e10A_Bit4_SetOffExplosives);
    field_114_timer = 0;
    field_110_brain_sub_state = 0;
    SetNextMotion(eParamiteMotions::Motion_0_Idle);
    field_EC = 3;
    mLiftPoint = nullptr;
    SetCurrentMotion(eParamiteMotions::Motion_0_Idle);
    field_140_use_prev_motion = 0;

    field_148_pMeat = nullptr;

    field_14C_pWeb = nullptr;

    field_A8_xpos = FP_FromInteger(pTlv->field_10_top_left.field_0_x);
    field_AC_ypos = FP_FromInteger(pTlv->field_10_top_left.field_2_y);

    if (pTlv->field_18_scale == Scale_short::eHalf_1)
    {
        field_BC_sprite_scale = FP_FromDouble(0.5);
        field_10_anim.mRenderLayer = Layer::eLayer_8;
        field_C6_scale = 0;
    }
    else
    {
        field_BC_sprite_scale = FP_FromInteger(1);
        field_10_anim.mRenderLayer = Layer::eLayer_27;
        field_C6_scale = 1;
    }

    if (pTlv->field_1A_bEnter_from_web == Choice_short::eYes_1)
    {
        SetBrain(&Paramite::Brain_1_SurpriseWeb);
    }
    else
    {
        SetBrain(&Paramite::Brain_0_Patrol);
    }

    field_11E_alone_chase_delay = pTlv->field_1C_alone_chase_delay;
    field_112_surprise_web_delay_timer = pTlv->field_1E_surprise_web_delay_timer;
    field_11C_meat_eating_time = pTlv->field_20_meat_eating_time;
    field_134_group_chase_delay = pTlv->field_22_group_chase_delay;
    field_13C_surprise_web_switch_id = pTlv->field_26_surprise_web_switch_id;
    field_13E_hiss_before_attack = pTlv->field_28_hiss_before_attack;
    field_144_delete_when_far_away = pTlv->field_2A_delete_when_far_away;

    FP hitX = {};
    FP hitY = {};
    if (sCollisions_DArray_504C6C->RayCast(
            field_A8_xpos,
            field_AC_ypos,
            field_A8_xpos,
            field_AC_ypos + FP_FromInteger(24),
            &BaseAliveGameObjectCollisionLine,
            &hitX,
            &hitY,
            field_BC_sprite_scale != FP_FromInteger(0) ? 0x01 : 0x10)
        == 1)
    {
        SetCurrentMotion(eParamiteMotions::Motion_0_Idle);
        field_AC_ypos = hitY;
    }

    field_C0_r = 105;
    field_C2_g = 105;
    field_C4_b = 105;

    field_128_never_read = 0;
    field_12C_tlvInfo = tlvInfo;

    if (!VIsFacingMe(sActiveHero_507678))
    {
        field_10_anim.mAnimFlags.Toggle(AnimFlags::eBit5_FlipX);
    }

    VStackOnObjectsOfType(ReliveTypes::eParamite);

    mBaseAliveGameObjectFlags.Set(Flags_10A::e10A_Bit6);

    field_D0_pShadow = ao_new<Shadow>();
}

Paramite::~Paramite()
{
    if (field_14C_pWeb)
    {
        field_14C_pWeb->mBaseGameObjectFlags.Set(Options::eDead);
        field_14C_pWeb->mBaseGameObjectRefCount--;
        field_14C_pWeb = nullptr;
    }

    if (field_148_pMeat)
    {
        field_148_pMeat->mBaseGameObjectRefCount--;
    }

    VOnTrapDoorOpen();

    for (s32 i = 0; i < ALIVE_COUNTOF(field_150_resources); i++)
    {
        if (field_10_anim.field_20_ppBlock != field_150_resources[i])
        {
            if (field_150_resources[i])
            {
                ResourceManager::FreeResource_455550(field_150_resources[i]);
            }
        }
    }

    ResourceManager::FreeResource_455550(ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, AOResourceID::kWebAOResID, 0, 0));
    if (mHealth <= FP_FromInteger(0))
    {
        gMap.TLV_Reset(field_12C_tlvInfo, -1, 0, 1);
    }
    else
    {
        gMap.TLV_Reset(field_12C_tlvInfo, -1, 0, 0);
    }

    SND_Seq_Stop_477A60(SeqId::eParamiteNearby_30);

    MusicController::static_PlayMusic(MusicController::MusicTypes::eType0, this, 0, 0);
    MusicController::ClearObject(this);
}

u8** Paramite::ResBlockForMotion(s16 motion)
{
    s16 idx = 0;
    eParamiteMotions eMotion = static_cast<eParamiteMotions>(motion);
    if (eMotion < eParamiteMotions::Motion_13_GameSpeakBegin)
    {
        idx = 0;
    }
    else if (eMotion < eParamiteMotions::Motion_18_RunningAttack)
    {
        idx = 5;
    }
    else if (eMotion < eParamiteMotions::Motion_19_Empty)
    {
        idx = 14;
    }
    else if (eMotion < eParamiteMotions::Motion_22_Unknown)
    {
        idx = 4;
    }
    else if (eMotion < eParamiteMotions::Motion_23_Eating)
    {
        idx = 9;
    }
    else if (eMotion < eParamiteMotions::Motion_24_Struggle)
    {
        idx = 1;
    }
    else if (eMotion >= eParamiteMotions::Motion_25_Death)
    {
        idx = motion >= 26 ? 0 : 10;
    }
    else
    {
        idx = 15;
    }
    field_12A_res_idx = idx;
    return field_150_resources[idx];
}

void Paramite::VRender(PrimHeader** ppOt)
{
    if (mBaseGameObjectUpdateDelay == 0)
    {
        BaseAnimatedWithPhysicsGameObject::VRender(ppOt);
    }
}

s16 Paramite::VTakeDamage(BaseGameObject* pFrom)
{
    if (mHealth <= FP_FromInteger(0))
    {
        return 1;
    }

    if (field_148_pMeat)
    {
        field_148_pMeat->mBaseGameObjectRefCount--;
        field_148_pMeat = nullptr;
    }

    switch (pFrom->mBaseGameObjectTypeId)
    {
        case AOTypes::eBaseBomb_30:
        case AOTypes::eExplosion_74:
        {
            ao_new<Gibs>(
                GibType::Slog_2,
                field_A8_xpos,
                field_AC_ypos,
                field_B4_velx,
                field_B8_vely,
                field_BC_sprite_scale);

            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
            field_10_anim.mAnimFlags.Clear(AnimFlags::eBit3_Render);
            mHealth = FP_FromInteger(0);
            return 1;
        }

        case AOTypes::eAbilityRing_69:
            return 0;

        case AOTypes::eBeeSwarm_95:
            mHealth -= FP_FromDouble(0.2);
            if (mHealth > FP_FromInteger(0))
            {
                if (!BrainIs(&Paramite::Brain_2_Struggling))
                {
                    SetNextMotion(eParamiteMotions::Motion_25_Death);
                    SetBrain(&Paramite::Brain_2_Struggling);
                    field_110_brain_sub_state = 0;
                }
            }
            else
            {
                mHealth = FP_FromInteger(0);
                SetBrain(&Paramite::Brain_3_Death);
                field_128_never_read = 2;
                field_114_timer = sGnFrame + 90;
                SetCurrentMotion(eParamiteMotions::Motion_24_Struggle);
                VUpdateAnimData();
            }
            break;

        default:
        {
            mHealth = FP_FromInteger(0);
            SetBrain(&Paramite::Brain_3_Death);
            field_128_never_read = 2;
            field_114_timer = sGnFrame + 90;
            SetCurrentMotion(eParamiteMotions::Motion_24_Struggle);
            VUpdateAnimData();

            ao_new<Blood>(
                field_A8_xpos,
                field_AC_ypos,
                FP_FromInteger(0),
                FP_FromInteger(5),
                field_BC_sprite_scale,
                50);
            break;
        }
    }
    return 1;
}

void Paramite::VOn_TLV_Collision(Path_TLV* pTlv)
{
    while (pTlv)
    {
        if (pTlv->field_4_type == TlvTypes::DeathDrop_5)
        {
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
            mHealth = FP_FromInteger(0);
            field_128_never_read = 2;
        }
        pTlv = gMap.TLV_Get_At_446060(pTlv, field_A8_xpos, field_AC_ypos, field_A8_xpos, field_AC_ypos);
    }
}

void Paramite::VScreenChanged()
{
    if (gMap.mOverlayId != gMap.GetOverlayId()
        || gMap.mCurrentPath != gMap.mPath
        || gMap.mCurrentLevel != gMap.mLevel)
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

s16 Paramite::VOnSameYLevel(BaseAnimatedWithPhysicsGameObject* pOther)
{
    return FP_Abs(pOther->field_AC_ypos - field_AC_ypos) < (field_BC_sprite_scale * FP_FromInteger(40));
}

void Paramite::VOnTrapDoorOpen()
{
    if (mLiftPoint)
    {
        mLiftPoint->VRemove(this);
        mLiftPoint->mBaseGameObjectRefCount--;
        mLiftPoint = nullptr;
        SetCurrentMotion(eParamiteMotions::Motion_12_Falling);
    }
}

void Paramite::VUpdate()
{
    if (mHealth > FP_FromInteger(0)
        && gMap.Is_Point_In_Current_Camera_4449C0(
            field_B2_lvl_number,
            field_B0_path_number,
            field_A8_xpos,
            field_AC_ypos,
            0))
    {
        SND_SEQ_PlaySeq_4775A0(SeqId::eParamiteNearby_30, 1, 0);
    }
    else
    {
        bool bFound = false;
        for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
        {
            BaseGameObject* pObjIter = gBaseGameObjects->ItemAt(i);
            if (!pObjIter)
            {
                break;
            }

            if (pObjIter->mBaseGameObjectTypeId == ReliveTypes::eParamite && pObjIter != this)
            {
                Paramite* pOther = static_cast<Paramite*>(pObjIter);
                if (gMap.Is_Point_In_Current_Camera_4449C0(
                        pOther->field_B2_lvl_number,
                        pOther->field_B0_path_number,
                        pOther->field_A8_xpos,
                        pOther->field_AC_ypos,
                        0))
                {
                    bFound = true;
                    break;
                }
            }
        }

        if (!bFound)
        {
            SND_Seq_Stop_477A60(SeqId::eParamiteNearby_30);
        }
    }

    if (Event_Get(kEventDeathReset_4) || Event_Get(kEvent_9))
    {
        mBaseGameObjectFlags.Set(Options::eDead);
    }

    if (FP_Abs(field_A8_xpos - sActiveHero_507678->field_A8_xpos) > FP_FromInteger(1536) || FP_Abs(field_AC_ypos - sActiveHero_507678->field_AC_ypos) > FP_FromInteger(480))
    {
        if (field_144_delete_when_far_away == Choice_short::eYes_1)
        {
            mBaseGameObjectFlags.Set(Options::eDead);
        }
        else
        {
            if (field_148_pMeat)
            {
                if (field_148_pMeat->VIsFalling() || field_148_pMeat->mBaseGameObjectFlags.Get(BaseGameObject::eDead))
                {
                    field_148_pMeat->mBaseGameObjectRefCount--;
                    field_148_pMeat = nullptr;
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    SetBrain(&Paramite::Brain_0_Patrol);
                    field_110_brain_sub_state = 0;
                }
            }
        }

        field_10_anim.mAnimFlags.Clear(AnimFlags::eBit2_Animate);
        field_10_anim.mAnimFlags.Clear(AnimFlags::eBit3_Render);

        if (field_10_anim.field_84_vram_rect.w)
        {
            Vram_free_450CE0(
                {field_10_anim.field_84_vram_rect.x, field_10_anim.field_84_vram_rect.y},
                {field_10_anim.field_84_vram_rect.w, field_10_anim.field_84_vram_rect.h});
            field_10_anim.field_84_vram_rect.w = 0;
        }
    }
    else
    {
        if (field_10_anim.field_84_vram_rect.w == 0)
        {
            vram_alloc_450B20(138u, 49, 8u, &field_10_anim.field_84_vram_rect);
        }

        field_10_anim.mAnimFlags.Set(AnimFlags::eBit2_Animate);
        field_10_anim.mAnimFlags.Set(AnimFlags::eBit3_Render);


        const auto oldMotion = mCurrentMotion;
        field_110_brain_sub_state = (this->*field_10C_fn)();

        static auto oldBrain = field_10C_fn;
        if (field_10C_fn != oldBrain)
        {
            //LOG_INFO("Brain changed from " << GetOriginalFn(oldBrain, sParamiteBrainTable).fnName << " to " << GetOriginalFn(field_10C_fn, sParamiteBrainTable).fnName);
        }
        oldBrain = field_10C_fn;

        if (showDebugCreatureInfo_5076E0)
        {
            DDCheat::DebugStr(
                "Paramite %d %d %d %d\n",
                field_110_brain_sub_state,
                field_114_timer,
                mCurrentMotion,
                mNextMotion);
        }

        const FP oldx = field_A8_xpos;
        const FP oldy = field_AC_ypos;

        (this->*sParamiteMotionTable_4CDCB0[mCurrentMotion])();

        if (oldx != field_A8_xpos || oldy != field_AC_ypos)
        {
            BaseAliveGameObjectPathTLV = gMap.TLV_Get_At_446060(
                nullptr,
                field_A8_xpos,
                field_AC_ypos,
                field_A8_xpos,
                field_AC_ypos);

            VOn_TLV_Collision(BaseAliveGameObjectPathTLV);
        }

        if (field_106_shot)
        {
            ToKnockBack();
            SetNextMotion(-1);
            SetCurrentMotion(mNextMotion);
            field_106_shot = 0;
        }

        if (oldMotion == mCurrentMotion)
        {
            if (field_140_use_prev_motion)
            {
                SetCurrentMotion(mPreviousMotion);
                VUpdateAnimData();
                field_10_anim.SetFrame(mBaseAliveGameObjectLastAnimFrame);
                field_140_use_prev_motion = 0;
            }
        }
        else
        {
            VUpdateAnimData();
            if (static_cast<eParamiteMotions>(oldMotion) == eParamiteMotions::Motion_4_Unknown)
            {
                field_10_anim.SetFrame(mBaseAliveGameObjectLastAnimFrame);
            }
        }
    }
}


void Paramite::ToIdle()
{
    field_10_anim.mAnimFlags.Clear(AnimFlags::eBit6_FlipY);
    field_10_anim.mAnimFlags.Clear(AnimFlags::eBit7_SwapXY);

    field_124_XSpeed = FP_FromInteger(0);
    field_B4_velx = FP_FromInteger(0);
    field_B8_vely = FP_FromInteger(0);

    SetCurrentMotion(eParamiteMotions::Motion_0_Idle);

    MapFollowMe_401D30(TRUE);
}

s16 Paramite::ToNextMotion()
{
    switch (GetNextMotion())
    {
        case eParamiteMotions::Motion_5_Turn:
        case eParamiteMotions::Motion_23_Eating:
        case eParamiteMotions::Motion_25_Death:
        case eParamiteMotions::Motion_13_GameSpeakBegin:
            SetCurrentMotion(mNextMotion);
            SetNextMotion(-1);
            return 1;

        case eParamiteMotions::Motion_3_Running:
            if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                field_B4_velx = -(ScaleToGridSize(field_BC_sprite_scale) / FP_FromDouble(3.5));
                SetCurrentMotion(eParamiteMotions::Motion_10_RunBegin);
            }
            else
            {
                field_B4_velx = (ScaleToGridSize(field_BC_sprite_scale) / FP_FromDouble(3.5));
                SetCurrentMotion(eParamiteMotions::Motion_10_RunBegin);
            }
            SetNextMotion(-1);
            return 1;

        case eParamiteMotions::Motion_2_Walking:
            if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                field_B4_velx = -ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(7);
                SetCurrentMotion(eParamiteMotions::Motion_1_WalkBegin);
            }
            else
            {
                field_B4_velx = (ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(7));
                SetCurrentMotion(eParamiteMotions::Motion_1_WalkBegin);
            }
            SetNextMotion(-1);
            return 1;

        case eParamiteMotions::Motion_15_Hiss:
            SetCurrentMotion(eParamiteMotions::Motion_13_GameSpeakBegin);
            return 1;

        case eParamiteMotions::Motion_18_RunningAttack:
            SetCurrentMotion(eParamiteMotions::Motion_18_RunningAttack);
            SetNextMotion(-1);
            if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                field_B4_velx = -(ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4));
            }
            else
            {
                field_B4_velx = (ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4));
            }
            return 1;

        case eParamiteMotions::Motion_6_Hop:
            VOnTrapDoorOpen();
            SetCurrentMotion(eParamiteMotions::Motion_6_Hop);
            SetNextMotion(-1);
            BaseAliveGameObjectLastLineYPos = field_AC_ypos;
            field_B8_vely = FP_FromDouble(-7.2) * field_BC_sprite_scale;
            field_AC_ypos += field_B8_vely;
            BaseAliveGameObjectCollisionLine = nullptr;
            return 1;

        default:
            return 0;
    }
}

void Paramite::VUpdateAnimData()
{
    const AnimRecord& rec = AO::AnimRec(sParamiteAnimIdTable_4CDD18[mCurrentMotion]);
    field_10_anim.Set_Animation_Data(
        rec.mFrameTableOffset,
        ResBlockForMotion(mCurrentMotion));
}

s16 Paramite::AnotherParamiteNear()
{
    for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
    {
        BaseGameObject* pObjIter = gBaseGameObjects->ItemAt(i);
        if (!pObjIter)
        {
            break;
        }

        if (pObjIter->mBaseGameObjectTypeId == ReliveTypes::eParamite && pObjIter != this)
        {
            Paramite* pOther = static_cast<Paramite*>(pObjIter);
            if (gMap.Is_Point_In_Current_Camera_4449C0(
                    pOther->field_B2_lvl_number,
                    pOther->field_B0_path_number,
                    pOther->field_A8_xpos,
                    pOther->field_AC_ypos,
                    0)
                && gMap.Is_Point_In_Current_Camera_4449C0(
                    field_B2_lvl_number,
                    field_B0_path_number,
                    field_A8_xpos,
                    field_AC_ypos,
                    0))
            {
                if (FP_Abs(pOther->field_AC_ypos - field_AC_ypos) < (field_BC_sprite_scale * FP_FromInteger(100)))
                {
                    if (pOther->BrainIs(&Paramite::Brain_0_Patrol) || pOther->BrainIs(&Paramite::Brain_4_ChasingAbe))
                    {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}


void Paramite::ToKnockBack()
{
    const FP nextX = field_A8_xpos - field_B4_velx;
    field_A8_xpos = FP_FromInteger((FP_GetExponent(nextX) & 0xFC00) + SnapToXGrid(field_BC_sprite_scale, FP_GetExponent(nextX) & 0x3FF));

    MapFollowMe_401D30(1);

    if (field_B8_vely < FP_FromInteger(0))
    {
        field_B8_vely = FP_FromInteger(0);
    }

    if (BaseAliveGameObjectCollisionLine)
    {
        ToIdle();
    }
    else
    {
        SetCurrentMotion(eParamiteMotions::Motion_12_Falling);
    }
}

Meat* Paramite::FindMeat()
{
    for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
    {
        BaseGameObject* pObjIter = gBaseGameObjects->ItemAt(i);
        if (!pObjIter)
        {
            break;
        }

        if (pObjIter->mBaseGameObjectTypeId == ReliveTypes::eMeat)
        {
            auto pMeat = static_cast<Meat*>(pObjIter);
            if (pMeat->VCanEatMe())
            {
                if (gMap.Is_Point_In_Current_Camera_4449C0(
                        pMeat->field_B2_lvl_number,
                        pMeat->field_B0_path_number,
                        pMeat->field_A8_xpos,
                        pMeat->field_AC_ypos,
                        0)
                    && !WallHit_401930(field_AC_ypos, pMeat->field_A8_xpos - field_A8_xpos))
                {
                    if (!pMeat->field_124_pLine)
                    {
                        return pMeat;
                    }

                    if (FP_Abs(pMeat->field_AC_ypos - field_AC_ypos) < (field_BC_sprite_scale * FP_FromInteger(100)))
                    {
                        return pMeat;
                    }
                }
            }
        }
    }
    return nullptr;
}

void Paramite::MoveOnLine()
{
    const FP old_x = field_A8_xpos;
    if (BaseAliveGameObjectCollisionLine)
    {
        BaseAliveGameObjectCollisionLine = BaseAliveGameObjectCollisionLine->MoveOnLine(
            &field_A8_xpos,
            &field_AC_ypos,
            field_B4_velx);
        if (BaseAliveGameObjectCollisionLine)
        {
            if (mLiftPoint)
            {
                if (BaseAliveGameObjectCollisionLine->field_8_type != eLineTypes::eUnknown_32 && BaseAliveGameObjectCollisionLine->field_8_type != eLineTypes::eUnknown_36)
                {
                    const auto oldMotion = mCurrentMotion;
                    VOnTrapDoorOpen();
                    SetCurrentMotion(oldMotion);
                }
            }
            else
            {
                if (BaseAliveGameObjectCollisionLine->field_8_type == eLineTypes::eUnknown_32 || BaseAliveGameObjectCollisionLine->field_8_type == eLineTypes::eUnknown_36)
                {
                    PSX_RECT bRect = {};
                    VGetBoundingRect(&bRect, 1);

                    VOnCollisionWith(
                        {bRect.x, static_cast<s16>(bRect.y + 5)},
                        {bRect.w, static_cast<s16>(bRect.h + 5)},
                        ObjListPlatforms_50766C,
                        1,
                        (TCollisionCallBack) &BaseAliveGameObject::OnTrapDoorIntersection_401C10);
                }
            }
        }
        else
        {
            VOnTrapDoorOpen();
            BaseAliveGameObjectLastLineYPos = field_AC_ypos;
            SetCurrentMotion(eParamiteMotions::Motion_12_Falling);
            field_A8_xpos = field_B4_velx + old_x;
        }
    }
    else
    {
        SetCurrentMotion(eParamiteMotions::Motion_12_Falling);
        BaseAliveGameObjectLastLineYPos = field_AC_ypos;
    }
}

const SfxDefinition stru_4CDD98[9] = {
    {0, 37, 60, 90, -520, -520, 0},
    {0, 37, 61, 75, -520, -520, 0},
    {0, 37, 62, 50, -520, -520, 0},
    {0, 37, 64, 40, -520, -520, 0},
    {0, 37, 65, 40, -520, -520, 0},
    {0, 37, 66, 30, -520, -520, 0},
    {0, 37, 66, 50, 0, 0, 0},
    {0, 37, 67, 40, -520, -520, 0},
    {0, 37, 68, 30, -520, -520, 0}};



void Paramite::Sound(ParamiteSpeak idx)
{
    const CameraPos direction = gMap.GetDirection(
        field_B2_lvl_number,
        field_B0_path_number,
        field_A8_xpos,
        field_AC_ypos);

    s16 volRight = stru_4CDD98[static_cast<s16>(idx)].field_C_default_volume;
    s16 volLeft = 0;

    PSX_RECT rect = {};
    gMap.Get_Camera_World_Rect(direction, &rect);

    switch (direction)
    {
        case CameraPos::eCamCurrent_0:
            volLeft = volRight;
            break;

        case CameraPos::eCamTop_1:
        case CameraPos::eCamBottom_2:
        {
            const FP tempVol = (FP_FromRaw(stru_4CDD98[static_cast<s16>(idx)].field_C_default_volume) / FP_FromInteger(3));
            volLeft = FP_GetExponent(tempVol);
            volRight = FP_GetExponent(tempVol);
            break;
        }

        case CameraPos::eCamLeft_3:
        {
            const FP numScreensAway = ((FP_FromInteger(rect.w) - field_A8_xpos) / FP_FromInteger(640));
            volLeft = volRight - FP_GetExponent(numScreensAway * FP_FromInteger(volRight - (volRight / 3)));
            volRight -= FP_GetExponent((numScreensAway * FP_FromInteger(volRight)));
            break;
        }

        case CameraPos::eCamRight_4:
        {
            const FP numScreensAway = ((field_A8_xpos - FP_FromInteger(rect.x)) / FP_FromInteger(640));
            volLeft = volRight - FP_GetExponent(numScreensAway * FP_FromInteger(volRight));
            volRight -= FP_GetExponent((numScreensAway * FP_FromInteger(volRight - (volRight / 3))));
            break;
        }

        default:
            return;
    }

    SFX_SfxDefinition_Play_477330(&stru_4CDD98[static_cast<s16>(idx)], volLeft, volRight, -520, -520);
}

void Paramite::SetMusic()
{
    if (gMap.GetDirection(
            field_B2_lvl_number,
            field_B0_path_number,
            field_A8_xpos,
            field_AC_ypos)
        >= CameraPos::eCamCurrent_0)
    {
        if (BrainIs(&Paramite::Brain_4_ChasingAbe))
        {
            MusicController::static_PlayMusic(MusicController::MusicTypes::eType12, this, 0, 0);
        }
        else
        {
            MusicController::static_PlayMusic(MusicController::MusicTypes::eType11, this, 0, 0);
        }
    }
}

void Paramite::ToHop()
{
    VOnTrapDoorOpen();

    SetCurrentMotion(eParamiteMotions::Motion_6_Hop);

    SetNextMotion(-1);

    BaseAliveGameObjectLastLineYPos = field_AC_ypos;
    field_B8_vely = -FP_FromDouble(7.2) * field_BC_sprite_scale;
    field_AC_ypos += field_B8_vely;
    BaseAliveGameObjectCollisionLine = nullptr;
}
enum Brain_0_Patrol
{
    eBrain0_Inactive_0 = 0,
    eBrain0_IdleForAbe_1 = 1,
    eBrain0_FearingAbe_2 = 2,
    eBrain0_RunningFromAbe_3 = 3,
    eBrain0_ApproachingAbe_4 = 4,
    eBrain0_StopApproachingAbe_5 = 5,
    eBrain0_TurningForAbe_6 = 6,
    eBrain0_HittingAbe_7 = 7,
    eBrain0_StuckToWall_8 = 8,
    eBrain0_Attacking_9 = 9,

    eBrain0_Idle_12 = 12,
    eBrain0_IdleAnimation_13 = 13,
    eBrain0_Turning_14 = 14,
    eBrain0_Panic_15 = 15
};

s16 Paramite::Brain_0_Patrol()
{
    if (Event_Get(kEventDeathReset_4) || Event_Get(kEvent_9))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    const FP kGridSize = ScaleToGridSize(field_BC_sprite_scale);

    switch (field_110_brain_sub_state)
    {
        case Brain_0_Patrol::eBrain0_Inactive_0:
            if (sNumCamSwappers_507668 > 0)
            {
                return field_110_brain_sub_state;
            }
            SetNextMotion(eParamiteMotions::Motion_0_Idle);
            field_128_never_read = 1;
            field_130_not_used = 0;
            return Brain_0_Patrol::eBrain0_IdleForAbe_1;

        case Brain_0_Patrol::eBrain0_IdleForAbe_1:
        {
            field_148_pMeat = FindMeat();
            if (field_148_pMeat)
            {
                SetBrain(&Paramite::Brain_5_SpottedMeat);
                field_148_pMeat->mBaseGameObjectRefCount++;
                return Brain_0_Patrol::eBrain0_Inactive_0;
            }

            if (!VOnSameYLevel(sActiveHero_507678)
                || field_BC_sprite_scale != sActiveHero_507678->field_BC_sprite_scale
                || WallHit_401930(field_BC_sprite_scale * FP_FromInteger(5), sActiveHero_507678->field_A8_xpos - field_A8_xpos))
            {
                field_120_wait_timer = sGnFrame + Math_RandomRange_450F20(45, 135);
                return Brain_0_Patrol::eBrain0_Idle_12;
            }

            if (Event_Get(kEventAbeOhm_8) && VIsFacingMe(sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_15_Hiss);
                return Brain_0_Patrol::eBrain0_Panic_15;
            }

            // TODO: Inlined HandleEnemyStopper but the directional logic isn't quite the same
            // so can't use it.
            BaseAliveGameObjectPathTLV = gMap.TLV_Get_At_446260(
                FP_GetExponent(field_A8_xpos),
                FP_GetExponent(field_AC_ypos),
                FP_GetExponent(field_A8_xpos),
                FP_GetExponent(field_AC_ypos),
                TlvTypes::EnemyStopper_79);
            if (BaseAliveGameObjectPathTLV)
            {
                auto pStopper = static_cast<Path_EnemyStopper*>(BaseAliveGameObjectPathTLV);
                if ((pStopper->field_18_direction == Path_EnemyStopper::StopDirection::Left_0 && sActiveHero_507678->field_A8_xpos < field_A8_xpos) || (pStopper->field_18_direction == Path_EnemyStopper::StopDirection::Right_1 && sActiveHero_507678->field_A8_xpos > field_A8_xpos))
                {
                    if (!SwitchStates_Get(pStopper->field_1A_switch_id))
                    {
                        return Brain_0_Patrol::eBrain0_IdleForAbe_1;
                    }
                }
            }

            if ((AnotherParamiteNear() || sActiveHero_507678->mHealth <= FP_FromInteger(0)) && field_BC_sprite_scale == sActiveHero_507678->field_BC_sprite_scale)
            {
                Sound(ParamiteSpeak::Howdy_5);
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                SetBrain(&Paramite::Brain_4_ChasingAbe);
                field_138_attack_timer = sGnFrame + field_134_group_chase_delay;
                return Brain_0_Patrol::eBrain0_Inactive_0;
            }

            if (!VIsObjNearby(kGridSize * FP_FromInteger(6), sActiveHero_507678))
            {
                if (!VIsObjNearby(kGridSize * FP_FromInteger(8), sActiveHero_507678))
                {
                    if (!VIsFacingMe(sActiveHero_507678))
                    {
                        if ((field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX) && !Check_IsOnEndOfLine_4021A0(0, 2)) || (!field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX) && !Check_IsOnEndOfLine_4021A0(1, 2)))
                        {
                            SetNextMotion(eParamiteMotions::Motion_5_Turn);
                            return Brain_0_Patrol::eBrain0_TurningForAbe_6;
                        }
                    }
                    else
                    {
                        if ((field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX) && !Check_IsOnEndOfLine_4021A0(1, 2)) || (!field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX) && !Check_IsOnEndOfLine_4021A0(0, 2)))
                        {
                            SetNextMotion(eParamiteMotions::Motion_2_Walking);
                            return Brain_0_Patrol::eBrain0_ApproachingAbe_4;
                        }
                    }
                }

                if (!VIsFacingMe(sActiveHero_507678))
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_0_Patrol::eBrain0_TurningForAbe_6;
                }

                return field_110_brain_sub_state;
            }
            else if (VIsFacingMe(sActiveHero_507678))
            {
                if ((field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX) && Check_IsOnEndOfLine_4021A0(0, 2)) || (!field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX) && Check_IsOnEndOfLine_4021A0(1, 2)))
                {
                    SetNextMotion(eParamiteMotions::Motion_13_GameSpeakBegin);
                    return Brain_0_Patrol::eBrain0_StuckToWall_8;
                }

                Sound(ParamiteSpeak::Howdy_5);
                SetNextMotion(eParamiteMotions::Motion_5_Turn);
                return Brain_0_Patrol::eBrain0_TurningForAbe_6;
            }
            else if (!VIsObjNearby(kGridSize * FP_FromInteger(4), sActiveHero_507678))
            {
                if ((field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX) && Check_IsOnEndOfLine_4021A0(1, 2)) || (!field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX) && Check_IsOnEndOfLine_4021A0(0, 2)))
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_0_Patrol::eBrain0_HittingAbe_7;
                }

                SetNextMotion(eParamiteMotions::Motion_2_Walking);
                field_128_never_read = 0;
                return Brain_0_Patrol::eBrain0_FearingAbe_2;
            }
            else
            {
                if ((field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX) && Check_IsOnEndOfLine_4021A0(1, 2)) || (!field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX) && Check_IsOnEndOfLine_4021A0(0, 2)))
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_0_Patrol::eBrain0_HittingAbe_7;
                }
                SetNextMotion(eParamiteMotions::Motion_3_Running);
                return Brain_0_Patrol::eBrain0_RunningFromAbe_3;
            }
            break;
        }

        case Brain_0_Patrol::eBrain0_FearingAbe_2:
            if (!VOnSameYLevel(sActiveHero_507678) || field_BC_sprite_scale != sActiveHero_507678->field_BC_sprite_scale)
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_0_Patrol::eBrain0_IdleForAbe_1;
            }

            if (field_B4_velx < FP_FromInteger(0))
            {
                if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(5), -kGridSize) || Check_IsOnEndOfLine_4021A0(1, 2))
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_0_Patrol::eBrain0_HittingAbe_7;
                }
            }

            if (field_B4_velx > FP_FromInteger(0))
            {
                if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(5), kGridSize) || Check_IsOnEndOfLine_4021A0(0, 2))
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_0_Patrol::eBrain0_HittingAbe_7;
                }
            }

            if (AnotherParamiteNear())
            {
                Sound(ParamiteSpeak::Howdy_5);
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                SetBrain(&Paramite::Brain_4_ChasingAbe);
                field_138_attack_timer = sGnFrame + field_134_group_chase_delay;
                return 0;
            }

            if (VIsObj_GettingNear_On_X(sActiveHero_507678) && VIsObjNearby(kGridSize * FP_FromInteger(4), sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_3_Running);
                return Brain_0_Patrol::eBrain0_RunningFromAbe_3;
            }

            if (VIsObj_GettingNear_On_X(sActiveHero_507678))
            {
                return field_110_brain_sub_state;
            }

            if (VIsObjNearby(kGridSize * FP_FromInteger(7), sActiveHero_507678))
            {
                return field_110_brain_sub_state;
            }

            SetNextMotion(eParamiteMotions::Motion_0_Idle);
            field_128_never_read = 1;
            return Brain_0_Patrol::eBrain0_IdleForAbe_1;

        case Brain_0_Patrol::eBrain0_RunningFromAbe_3:
            if (!VOnSameYLevel(sActiveHero_507678) || field_BC_sprite_scale != sActiveHero_507678->field_BC_sprite_scale)
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_0_Patrol::eBrain0_IdleForAbe_1;
            }

            if (field_B4_velx < FP_FromInteger(0))
            {
                if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(5), -kGridSize) || Check_IsOnEndOfLine_4021A0(1, 3))
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_0_Patrol::eBrain0_HittingAbe_7;
                }
            }

            if (field_B4_velx > FP_FromInteger(0))
            {
                if ((WallHit_401930(field_BC_sprite_scale * FP_FromInteger(5), kGridSize) || Check_IsOnEndOfLine_4021A0(0, 3)))
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_0_Patrol::eBrain0_HittingAbe_7;
                }
            }

            if (AnotherParamiteNear())
            {
                Sound(ParamiteSpeak::Howdy_5);
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                SetBrain(&Paramite::Brain_4_ChasingAbe);
                field_138_attack_timer = sGnFrame + field_134_group_chase_delay;
                return 0;
            }

            if (VIsObj_GettingNear_On_X(sActiveHero_507678))
            {
                return field_110_brain_sub_state;
            }

            if (VIsObjNearby(kGridSize * FP_FromInteger(7), sActiveHero_507678))
            {
                return field_110_brain_sub_state;
            }

            SetNextMotion(eParamiteMotions::Motion_2_Walking);
            return Brain_0_Patrol::eBrain0_FearingAbe_2;

        case Brain_0_Patrol::eBrain0_ApproachingAbe_4:
            if (!VOnSameYLevel(sActiveHero_507678) || field_BC_sprite_scale != sActiveHero_507678->field_BC_sprite_scale)
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_0_Patrol::eBrain0_IdleForAbe_1;
            }

            if (AnotherParamiteNear())
            {
                Sound(ParamiteSpeak::Howdy_5);
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                field_138_attack_timer = sGnFrame + field_134_group_chase_delay;
                SetBrain(&Paramite::Brain_4_ChasingAbe);
                return Brain_0_Patrol::eBrain0_Inactive_0;
            }

            if (field_B4_velx < FP_FromInteger(0))
            {
                if (HandleEnemyStopper(-2, Path_EnemyStopper::StopDirection::Left_0))
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_0_Patrol::eBrain0_IdleForAbe_1;
                }

                if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(5), -kGridSize) || Check_IsOnEndOfLine_4021A0(1, 2))
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_0_Patrol::eBrain0_IdleForAbe_1;
                }
            }

            if (field_B4_velx > FP_FromInteger(0))
            {
                if (HandleEnemyStopper(2, Path_EnemyStopper::StopDirection::Right_1))
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_0_Patrol::eBrain0_IdleForAbe_1;
                }

                if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(5), kGridSize) || Check_IsOnEndOfLine_4021A0(0, 2))
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_0_Patrol::eBrain0_IdleForAbe_1;
                }
            }

            if (VIsObj_GettingNear_On_X(sActiveHero_507678) || VIsObjNearby(kGridSize * FP_FromInteger(10), sActiveHero_507678))
            {
                if (!VIsObj_GettingNear_On_X(sActiveHero_507678))
                {
                    return field_110_brain_sub_state;
                }

                if (!VIsObjNearby(kGridSize * FP_FromInteger(8), sActiveHero_507678))
                {
                    return field_110_brain_sub_state;
                }
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_0_Patrol::eBrain0_IdleForAbe_1;
            }
            SetNextMotion(eParamiteMotions::Motion_3_Running);
            return Brain_0_Patrol::eBrain0_StopApproachingAbe_5;

        case Brain_0_Patrol::eBrain0_StopApproachingAbe_5:
            if (!VOnSameYLevel(sActiveHero_507678) || field_BC_sprite_scale != sActiveHero_507678->field_BC_sprite_scale)
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_0_Patrol::eBrain0_IdleForAbe_1;
            }

            if (AnotherParamiteNear())
            {
                Sound(ParamiteSpeak::Howdy_5);
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                field_138_attack_timer = sGnFrame + field_134_group_chase_delay;
                SetBrain(&Paramite::Brain_4_ChasingAbe);
                return 0;
            }

            if (field_B4_velx < FP_FromInteger(0))
            {
                if (HandleEnemyStopper(-2, Path_EnemyStopper::StopDirection::Left_0))
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_0_Patrol::eBrain0_IdleForAbe_1;
                }

                if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(5), -kGridSize) || Check_IsOnEndOfLine_4021A0(1, 3))
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_0_Patrol::eBrain0_IdleForAbe_1;
                }
            }

            if (field_B4_velx > FP_FromInteger(0))
            {
                if (HandleEnemyStopper(2, Path_EnemyStopper::StopDirection::Right_1))
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_0_Patrol::eBrain0_IdleForAbe_1;
                }

                if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(5), kGridSize) || Check_IsOnEndOfLine_4021A0(0, 3))
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_0_Patrol::eBrain0_IdleForAbe_1;
                }
            }

            if (!VIsObj_GettingNear_On_X(sActiveHero_507678))
            {
                return field_110_brain_sub_state;
            }

            if (VIsObjNearby(kGridSize * FP_FromInteger(8), sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_0_Patrol::eBrain0_IdleForAbe_1;
            }

            if (!VIsObjNearby(kGridSize * FP_FromInteger(10), sActiveHero_507678))
            {
                return field_110_brain_sub_state;
            }

            SetNextMotion(eParamiteMotions::Motion_2_Walking);
            return Brain_0_Patrol::eBrain0_ApproachingAbe_4;

        case Brain_0_Patrol::eBrain0_TurningForAbe_6:
            if (GetCurrentMotion() != eParamiteMotions::Motion_5_Turn || !field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_110_brain_sub_state;
            }
            SetNextMotion(eParamiteMotions::Motion_0_Idle);
            return Brain_0_Patrol::eBrain0_IdleForAbe_1;

        case Brain_0_Patrol::eBrain0_HittingAbe_7:
            if (GetCurrentMotion() != eParamiteMotions::Motion_5_Turn || !(field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame)))
            {
                return field_110_brain_sub_state;
            }
            SetNextMotion(eParamiteMotions::Motion_13_GameSpeakBegin);
            return Brain_0_Patrol::eBrain0_StuckToWall_8;

        case Brain_0_Patrol::eBrain0_StuckToWall_8:
            field_148_pMeat = FindMeat();
            if (field_148_pMeat)
            {
                SetBrain(&Paramite::Brain_5_SpottedMeat);
                field_148_pMeat->mBaseGameObjectRefCount++;
                return 0;
            }

            if (!VOnSameYLevel(sActiveHero_507678) || field_BC_sprite_scale != sActiveHero_507678->field_BC_sprite_scale)
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_0_Patrol::eBrain0_IdleForAbe_1;
            }

            if (VIsObjNearby(kGridSize * FP_FromInteger(3), sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_18_RunningAttack);
                return Brain_0_Patrol::eBrain0_Attacking_9;
            }

            if (VIsObj_GettingNear_On_X(sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_15_Hiss);
            }
            else
            {
                if (!VIsObjNearby(kGridSize * FP_FromInteger(8), sActiveHero_507678))
                {
                    SetNextMotion(eParamiteMotions::Motion_2_Walking);
                    return Brain_0_Patrol::eBrain0_ApproachingAbe_4;
                }
            }

            if (GetCurrentMotion() != eParamiteMotions::Motion_14_PreHiss)
            {
                return field_110_brain_sub_state;
            }

            if (Math_NextRandom() >= 6u)
            {
                return field_110_brain_sub_state;
            }

            SetNextMotion(eParamiteMotions::Motion_15_Hiss);
            return field_110_brain_sub_state;

        case Brain_0_Patrol::eBrain0_Attacking_9:
            if (GetCurrentMotion() != eParamiteMotions::Motion_18_RunningAttack
                || !(field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame)))
            {
                return field_110_brain_sub_state;
            }
            SetNextMotion(eParamiteMotions::Motion_0_Idle);
            if (sActiveHero_507678->mHealth > FP_FromInteger(0))
            {
                return Brain_0_Patrol::eBrain0_IdleForAbe_1;
            }
            SetBrain(&Paramite::Brain_4_ChasingAbe);
            return 0;

        case Brain_0_Patrol::eBrain0_Idle_12:
            field_148_pMeat = FindMeat();
            if (field_148_pMeat)
            {
                SetBrain(&Paramite::Brain_5_SpottedMeat);
                field_148_pMeat->mBaseGameObjectRefCount++;
                return 0;
            }

            if (VOnSameYLevel(sActiveHero_507678)
                && field_BC_sprite_scale == sActiveHero_507678->field_BC_sprite_scale
                && !WallHit_401930(field_BC_sprite_scale * FP_FromInteger(5), sActiveHero_507678->field_A8_xpos - field_A8_xpos))
            {
                return Brain_0_Patrol::eBrain0_IdleForAbe_1;
            }

            if (Event_Get(kEventAbeOhm_8))
            {
                SetNextMotion(eParamiteMotions::Motion_15_Hiss);
                return Brain_0_Patrol::eBrain0_Panic_15;
            }

            if (field_120_wait_timer <= static_cast<s32>(sGnFrame))
            {
                if (Math_NextRandom() >= 6u)
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_0_Patrol::eBrain0_Turning_14;
                }
                else
                {
                    SetNextMotion(eParamiteMotions::Motion_13_GameSpeakBegin);
                    return Brain_0_Patrol::eBrain0_IdleAnimation_13;
                }
            }
            return field_110_brain_sub_state;

        case Brain_0_Patrol::eBrain0_IdleAnimation_13:
            if (GetCurrentMotion() != eParamiteMotions::Motion_14_PreHiss
                || !(field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame)))
            {
                return field_110_brain_sub_state;
            }
            SetNextMotion(eParamiteMotions::Motion_0_Idle);
            field_120_wait_timer = sGnFrame + Math_RandomRange_450F20(45, 135);
            return Brain_0_Patrol::eBrain0_Idle_12;

        case Brain_0_Patrol::eBrain0_Turning_14:
            if (GetCurrentMotion() != eParamiteMotions::Motion_5_Turn
                || !(field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame)))
            {
                return field_110_brain_sub_state;
            }
            field_120_wait_timer = sGnFrame + Math_RandomRange_450F20(45, 135);
            return Brain_0_Patrol::eBrain0_Idle_12;

        case Brain_0_Patrol::eBrain0_Panic_15:
            if (!Event_Get(kEventAbeOhm_8))
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_0_Patrol::eBrain0_Idle_12;
            }
            SetNextMotion(eParamiteMotions::Motion_15_Hiss);
            return field_110_brain_sub_state;

        default:
            return field_110_brain_sub_state;
    }
}

enum Brain_1_SurpriseWeb
{
    eBrain1_Loading_0 = 0,
    eBrain1_AppearingRight_1 = 1,
    eBrain1_AppearingLeft_2 = 2,
    eBrain1_StartAnimation_3 = 3,
    eBrain1_StateLoop1_4 = 4,
    eBrain1_StateLoop2_5 = 5
};

s16 Paramite::Brain_1_SurpriseWeb()
{
    if (Event_Get(kEventDeathReset_4) || Event_Get(kEvent_9))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    switch (field_110_brain_sub_state)
    {
        case Brain_1_SurpriseWeb::eBrain1_Loading_0:
        {
            if (sNumCamSwappers_507668 > 0)
            {
                return field_110_brain_sub_state;
            }

            SetCurrentMotion(eParamiteMotions::Motion_0_Idle);

            MapFollowMe_401D30(TRUE);
            auto pWeb = ao_new<ParamiteWeb>(
                field_A8_xpos,
                FP_GetExponent(field_AC_ypos) - 20,
                FP_GetExponent(field_AC_ypos) - 10,
                field_BC_sprite_scale);
            if (pWeb)
            {
                pWeb->mBaseGameObjectRefCount++;
                field_14C_pWeb = pWeb;
            }

            if (sActiveHero_507678->field_A8_xpos >= field_A8_xpos)
            {
                return Brain_1_SurpriseWeb::eBrain1_AppearingLeft_2;
            }
            return Brain_1_SurpriseWeb::eBrain1_AppearingRight_1;
        }

        case Brain_1_SurpriseWeb::eBrain1_AppearingRight_1:
            if (sActiveHero_507678->field_A8_xpos <= field_A8_xpos
                || field_BC_sprite_scale != sActiveHero_507678->field_BC_sprite_scale
                || !gMap.Is_Point_In_Current_Camera_4449C0(
                    field_B2_lvl_number,
                    field_B0_path_number,
                    field_A8_xpos,
                    field_AC_ypos,
                    1))
            {
                if (!SwitchStates_Get(field_13C_surprise_web_switch_id))
                {
                    return field_110_brain_sub_state;
                }
            }
            field_10_anim.mAnimFlags.Clear(AnimFlags::eBit5_FlipX);
            field_114_timer = sGnFrame + field_112_surprise_web_delay_timer;
            return Brain_1_SurpriseWeb::eBrain1_StartAnimation_3;

        case Brain_1_SurpriseWeb::eBrain1_AppearingLeft_2:
            if (sActiveHero_507678->field_A8_xpos >= field_A8_xpos
                || field_BC_sprite_scale != sActiveHero_507678->field_BC_sprite_scale
                || !gMap.Is_Point_In_Current_Camera_4449C0(
                    field_B2_lvl_number,
                    field_B0_path_number,
                    field_A8_xpos,
                    field_AC_ypos,
                    1))
            {
                if (!SwitchStates_Get(field_13C_surprise_web_switch_id))
                {
                    return field_110_brain_sub_state;
                }
            }
            field_10_anim.mAnimFlags.Set(AnimFlags::eBit5_FlipX);
            field_114_timer = sGnFrame + field_112_surprise_web_delay_timer;
            return Brain_1_SurpriseWeb::eBrain1_StartAnimation_3;

        case Brain_1_SurpriseWeb::eBrain1_StartAnimation_3:
            if (field_114_timer > static_cast<s32>(sGnFrame))
            {
                return field_110_brain_sub_state;
            }
            field_B8_vely = FP_FromInteger(0);
            SetCurrentMotion(eParamiteMotions::Motion_20_SurpriseWeb);
            return Brain_1_SurpriseWeb::eBrain1_StateLoop1_4;

        case Brain_1_SurpriseWeb::eBrain1_StateLoop1_4:
            field_14C_pWeb->field_EA_ttl_remainder = FP_GetExponent(FP_Abs(field_AC_ypos)) - 10;
            field_14C_pWeb->field_AC_ypos = FP_FromInteger(field_14C_pWeb->field_EA_ttl_remainder);
            if (GetCurrentMotion() == eParamiteMotions::Motion_0_Idle)
            {
                field_14C_pWeb->field_F0_bEnabled = TRUE;
                field_14C_pWeb->mBaseGameObjectRefCount--;
                field_14C_pWeb = nullptr;
                SetBrain(&Paramite::Brain_0_Patrol);
                return Brain_0_Patrol::eBrain0_Inactive_0;
            }

            if (field_B8_vely < (field_BC_sprite_scale * FP_FromInteger(8)))
            {
                field_B8_vely += (field_BC_sprite_scale * FP_FromDouble(0.5));
                return field_110_brain_sub_state;
            }
            return Brain_1_SurpriseWeb::eBrain1_StateLoop2_5;

        case Brain_1_SurpriseWeb::eBrain1_StateLoop2_5:
            field_14C_pWeb->field_EA_ttl_remainder = FP_GetExponent(FP_Abs(field_AC_ypos)) - 10;
            field_14C_pWeb->field_AC_ypos = FP_FromInteger(field_14C_pWeb->field_EA_ttl_remainder);
            if (GetCurrentMotion() != eParamiteMotions::Motion_0_Idle)
            {
                if (field_B8_vely <= (field_BC_sprite_scale * FP_FromInteger(-1)))
                {
                    return Brain_1_SurpriseWeb::eBrain1_StateLoop1_4;
                }
                else
                {
                    field_B8_vely -= (field_BC_sprite_scale * FP_FromInteger(1));
                    return field_110_brain_sub_state;
                }
            }
            else
            {
                field_14C_pWeb->field_F0_bEnabled = TRUE;
                field_14C_pWeb->mBaseGameObjectRefCount--;
                field_14C_pWeb = nullptr;
                SetBrain(&Paramite::Brain_0_Patrol);
                return Brain_0_Patrol::eBrain0_Inactive_0;
            }
            break;

        default:
            break;
    }

    return field_110_brain_sub_state;
}

enum Brain_2_Struggling
{
    eBrain2_SetTimer_0 = 0,
    eBrain2_Hiss_1 = 1,
    eBrain2_Turn_2 = 2,
    eBrain2_Death_3 = 3
};

s16 Paramite::Brain_2_Struggling()
{
    if (Event_Get(kEventDeathReset_4) || Event_Get(kEvent_9))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    if (IsBeeSwarmChasingMe_4022B0())
    {
        switch (field_110_brain_sub_state)
        {
            case Brain_2_Struggling::eBrain2_SetTimer_0:
                field_114_timer = sGnFrame + 30;
                return 1;

            case Brain_2_Struggling::eBrain2_Hiss_1:
                if (field_114_timer <= static_cast<s32>(sGnFrame))
                {
                    SetCurrentMotion(eParamiteMotions::Motion_13_GameSpeakBegin);
                    SetNextMotion(eParamiteMotions::Motion_15_Hiss);
                    return Brain_2_Struggling::eBrain2_Turn_2;
                }
                break;

            case Brain_2_Struggling::eBrain2_Turn_2:
                if (GetCurrentMotion() == eParamiteMotions::Motion_15_Hiss)
                {
                    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
                    {
                        SetNextMotion(eParamiteMotions::Motion_5_Turn);
                        return Brain_2_Struggling::eBrain2_Death_3;
                    }
                }
                break;

            case Brain_2_Struggling::eBrain2_Death_3:
                if (GetCurrentMotion() == eParamiteMotions::Motion_5_Turn)
                {
                    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
                    {
                        SetNextMotion(eParamiteMotions::Motion_25_Death);
                        field_114_timer = sGnFrame + 30;
                        return 1;
                    }
                }
                break;
            default:
                break;
        }
        return field_110_brain_sub_state;
    }
    else if (GetCurrentMotion() == eParamiteMotions::Motion_5_Turn)
    {
        SetNextMotion(eParamiteMotions::Motion_0_Idle);
        SetBrain(&Paramite::Brain_0_Patrol);
        return Brain_0_Patrol::eBrain0_Inactive_0;
    }
    else
    {
        SetCurrentMotion(eParamiteMotions::Motion_0_Idle);
        SetNextMotion(-1);
        SetBrain(&Paramite::Brain_0_Patrol);
        return field_110_brain_sub_state;
    }
}

s16 Paramite::Brain_3_Death()
{
    if (static_cast<s32>(sGnFrame) < field_114_timer && (field_114_timer < static_cast<s32>(sGnFrame) + 80))
    {
        field_BC_sprite_scale -= FP_FromDouble(0.008);
        field_C0_r -= 2;
        field_C2_g -= 2;
        field_C4_b -= 2;
        if (static_cast<s32>(sGnFrame) < field_114_timer - 24)
        {
            DeathSmokeEffect(true);
        }
    }

    if (field_114_timer < (s32) sGnFrame)
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    return 100;
}

enum Brain_4_ChasingAbe
{
    eBrain4_Inactive_0 = 0,
    eBrain4_Attacking_1 = 1,
    eBrain4_ToWarning_2 = 2,
    eBrain4_Warning_3 = 3,
    eBrain4_CloseAttack_4 = 4,
    eBrain4_ToChasing_5 = 5,
    eBrain4_QuickAttack_6 = 6,
    eBrain4_Chasing_7 = 7,
    eBrain4_Jumping_8 = 8,
    eBrain4_TurningWhileChasing_9 = 9,
    eBrain4_Turning_10 = 10,
    eBrain4_Walking_11 = 11,
    eBrain4_WalkingToHop_12 = 12,
    eBrain4_Eating_13 = 13,
    eBrain4_Idle_14 = 14,
    eBrain4_Panic_15 = 15
};

s16 Paramite::Brain_4_ChasingAbe()
{
    if (Event_Get(kEventDeathReset_4) || Event_Get(kEvent_9))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    if (field_138_attack_timer <= static_cast<s32>(sGnFrame)
        && (!VOnSameYLevel(sActiveHero_507678)
            || field_BC_sprite_scale != sActiveHero_507678->field_BC_sprite_scale)
        && sActiveHero_507678->mHealth > FP_FromInteger(0))
    {
        SetNextMotion(eParamiteMotions::Motion_0_Idle);
        SetBrain(&Paramite::Brain_0_Patrol);
        return 0;
    }

    if (Event_Get(kEventAbeOhm_8))
    {
        SetNextMotion(eParamiteMotions::Motion_15_Hiss);
        return Brain_4_ChasingAbe::eBrain4_Panic_15;
    }

    const FP kGridSize = ScaleToGridSize(field_BC_sprite_scale);

    switch (field_110_brain_sub_state)
    {
        case Brain_4_ChasingAbe::eBrain4_Inactive_0:
            if (!VOnSameYLevel(sActiveHero_507678))
            {
                return Brain_4_ChasingAbe::eBrain4_Inactive_0;
            }

            if (sActiveHero_507678->mHealth > FP_FromInteger(0))
            {
                if (VIsFacingMe(sActiveHero_507678))
                {
                    if (field_13E_hiss_before_attack == Choice_short::eYes_1)
                    {
                        field_114_timer = sGnFrame + Math_RandomRange_450F20(0, 6);
                        return Brain_4_ChasingAbe::eBrain4_ToWarning_2;
                    }
                    else
                    {
                        field_114_timer = sGnFrame + field_11E_alone_chase_delay;
                        return Brain_4_ChasingAbe::eBrain4_ToChasing_5;
                    }
                }
                else
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_4_ChasingAbe::eBrain4_CloseAttack_4;
                }
            }
            else
            {
                if (VIsFacingMe(sActiveHero_507678))
                {
                    if (VIsObjNearby(kGridSize, sActiveHero_507678))
                    {
                        SetNextMotion(eParamiteMotions::Motion_23_Eating);
                        field_114_timer = sGnFrame + 28;
                        return Brain_4_ChasingAbe::eBrain4_Eating_13;
                    }

                    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
                    {
                        if (Check_IsOnEndOfLine_4021A0(1, 1))
                        {
                            SetNextMotion(eParamiteMotions::Motion_6_Hop);
                            return Brain_4_ChasingAbe::eBrain4_Jumping_8;
                        }
                    }
                    else
                    {
                        if (Check_IsOnEndOfLine_4021A0(0, 1))
                        {
                            SetNextMotion(eParamiteMotions::Motion_6_Hop);
                            return Brain_4_ChasingAbe::eBrain4_Jumping_8;
                        }
                    }
                    SetNextMotion(eParamiteMotions::Motion_2_Walking);
                    return Brain_4_ChasingAbe::eBrain4_Walking_11;
                }
                else
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_4_ChasingAbe::eBrain4_Turning_10;
                }
            }
            break;

        case Brain_4_ChasingAbe::eBrain4_Attacking_1:
            if (GetCurrentMotion() != eParamiteMotions::Motion_18_RunningAttack
                || !field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_110_brain_sub_state;
            }

            if (sActiveHero_507678->mHealth > FP_FromInteger(0))
            {
                if (VIsFacingMe(sActiveHero_507678))
                {
                    if (VIsObjNearby(kGridSize * FP_FromInteger(4), sActiveHero_507678))
                    {
                        if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), sActiveHero_507678->field_A8_xpos - field_A8_xpos))
                        {
                            SetNextMotion(eParamiteMotions::Motion_0_Idle);
                            return Brain_4_ChasingAbe::eBrain4_ToChasing_5;
                        }
                        else
                        {
                            SetNextMotion(eParamiteMotions::Motion_18_RunningAttack);
                            return Brain_4_ChasingAbe::eBrain4_Attacking_1;
                        }
                    }

                    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
                    {
                        if (Check_IsOnEndOfLine_4021A0(1, 1))
                        {
                            SetNextMotion(eParamiteMotions::Motion_6_Hop);
                            return Brain_4_ChasingAbe::eBrain4_Jumping_8;
                        }
                    }
                    else
                    {
                        if (Check_IsOnEndOfLine_4021A0(0, 1))
                        {
                            SetNextMotion(eParamiteMotions::Motion_6_Hop);
                            return Brain_4_ChasingAbe::eBrain4_Jumping_8;
                        }
                    }
                    SetNextMotion(eParamiteMotions::Motion_3_Running);
                    return Brain_4_ChasingAbe::eBrain4_Chasing_7;
                }
                else
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_4_ChasingAbe::eBrain4_QuickAttack_6;
                }
            }
            else
            {
                if (VIsFacingMe(sActiveHero_507678))
                {
                    if (VIsObjNearby(kGridSize, sActiveHero_507678))
                    {
                        SetNextMotion(eParamiteMotions::Motion_23_Eating);
                        field_114_timer = sGnFrame + 28;
                        return Brain_4_ChasingAbe::eBrain4_Eating_13;
                    }

                    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
                    {
                        if (Check_IsOnEndOfLine_4021A0(1, 1))
                        {
                            SetNextMotion(eParamiteMotions::Motion_6_Hop);
                            return Brain_4_ChasingAbe::eBrain4_Jumping_8;
                        }
                    }
                    else
                    {
                        if (Check_IsOnEndOfLine_4021A0(0, 1))
                        {
                            SetNextMotion(eParamiteMotions::Motion_6_Hop);
                            return Brain_4_ChasingAbe::eBrain4_Jumping_8;
                        }
                    }
                    SetNextMotion(eParamiteMotions::Motion_2_Walking);
                    return Brain_4_ChasingAbe::eBrain4_Walking_11;
                }
                else
                {
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_4_ChasingAbe::eBrain4_Turning_10;
                }
            }
            break;

        case Brain_4_ChasingAbe::eBrain4_ToWarning_2:
            if (field_114_timer > static_cast<s32>(sGnFrame))
            {
                return field_110_brain_sub_state;
            }
            SetNextMotion(eParamiteMotions::Motion_15_Hiss);
            field_114_timer = sGnFrame + field_11E_alone_chase_delay;
            return Brain_4_ChasingAbe::eBrain4_Warning_3;

        case Brain_4_ChasingAbe::eBrain4_Warning_3:
            if (GetCurrentMotion() != eParamiteMotions::Motion_15_Hiss)
            {
                return field_110_brain_sub_state;
            }
            return Brain_4_ChasingAbe::eBrain4_ToChasing_5;

        case Brain_4_ChasingAbe::eBrain4_CloseAttack_4:
            if (GetCurrentMotion() != eParamiteMotions::Motion_5_Turn
                || !field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_110_brain_sub_state;
            }

            if (field_13E_hiss_before_attack == Choice_short::eYes_1)
            {
                field_114_timer = sGnFrame + Math_RandomRange_450F20(0, 6);
                return Brain_4_ChasingAbe::eBrain4_ToWarning_2;
            }
            else
            {
                field_114_timer = sGnFrame + field_11E_alone_chase_delay;
                return Brain_4_ChasingAbe::eBrain4_ToChasing_5;
            }
            break;

        case Brain_4_ChasingAbe::eBrain4_ToChasing_5:
            if (!VOnSameYLevel(sActiveHero_507678))
            {
                return Brain_4_ChasingAbe::eBrain4_Inactive_0;
            }

            if (!VIsFacingMe(sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_5_Turn);
                return 6;
            }

            // TODO: Inlined HandleEnemyStopper but the directional logic isn't quite the same
            // so can't use it.
            BaseAliveGameObjectPathTLV = gMap.TLV_Get_At_446260(
                FP_GetExponent(field_A8_xpos),
                FP_GetExponent(field_AC_ypos),
                FP_GetExponent(field_A8_xpos),
                FP_GetExponent(field_AC_ypos),
                TlvTypes::EnemyStopper_79);
            if (BaseAliveGameObjectPathTLV)
            {
                auto pStopper = static_cast<Path_EnemyStopper*>(BaseAliveGameObjectPathTLV);
                if ((pStopper->field_18_direction == Path_EnemyStopper::StopDirection::Left_0 && sActiveHero_507678->field_A8_xpos < field_A8_xpos) || (pStopper->field_18_direction == Path_EnemyStopper::StopDirection::Right_1 && sActiveHero_507678->field_A8_xpos > field_A8_xpos))
                {
                    if (!SwitchStates_Get(pStopper->field_1A_switch_id))
                    {
                        return Brain_4_ChasingAbe::eBrain4_ToChasing_5;
                    }
                }
            }

            if (VIsObjNearby(kGridSize * FP_FromInteger(4), sActiveHero_507678))
            {
                if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), sActiveHero_507678->field_A8_xpos - field_A8_xpos))
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_4_ChasingAbe::eBrain4_ToChasing_5;
                }
                else
                {
                    SetNextMotion(eParamiteMotions::Motion_18_RunningAttack);
                    return Brain_4_ChasingAbe::eBrain4_Attacking_1;
                }
            }

            if (field_114_timer > static_cast<s32>(sGnFrame) && field_13E_hiss_before_attack == Choice_short::eYes_1)
            {
                return field_110_brain_sub_state;
            }

            if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                if (Check_IsOnEndOfLine_4021A0(1, 1))
                {
                    SetNextMotion(eParamiteMotions::Motion_6_Hop);
                    return Brain_4_ChasingAbe::eBrain4_Jumping_8;
                }
            }
            else if (!field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                if (Check_IsOnEndOfLine_4021A0(0, 1))
                {
                    SetNextMotion(eParamiteMotions::Motion_6_Hop);
                    return Brain_4_ChasingAbe::eBrain4_Jumping_8;
                }
            }

            SetNextMotion(eParamiteMotions::Motion_3_Running);
            return Brain_4_ChasingAbe::eBrain4_Chasing_7;

        case Brain_4_ChasingAbe::eBrain4_QuickAttack_6:
            if (GetCurrentMotion() != eParamiteMotions::Motion_5_Turn
                || !(field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame)))
            {
                return field_110_brain_sub_state;
            }

            if (field_13E_hiss_before_attack == Choice_short::eYes_1)
            {
                SetNextMotion(eParamiteMotions::Motion_15_Hiss);
            }
            return Brain_4_ChasingAbe::eBrain4_ToChasing_5;

        case Brain_4_ChasingAbe::eBrain4_Chasing_7:
        {
            if (!VOnSameYLevel(sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_4_ChasingAbe::eBrain4_Inactive_0;
            }

            if (!VIsFacingMe(sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_5_Turn);
                return Brain_4_ChasingAbe::eBrain4_TurningWhileChasing_9;
            }

            const s16 x_exp = FP_GetExponent(field_A8_xpos);
            const s32 xSnapped = (x_exp & 0xFC00) + SnapToXGrid(field_BC_sprite_scale, x_exp & 0x3FF);
            if (field_B4_velx < FP_FromInteger(0))
            {
                if (HandleEnemyStopper(-2, Path_EnemyStopper::StopDirection::Left_0))
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_4_ChasingAbe::eBrain4_ToChasing_5;
                }

                if (abs(xSnapped - x_exp) < 6 && Check_IsOnEndOfLine_4021A0(1, 1))
                {
                    ToHop();
                    return Brain_4_ChasingAbe::eBrain4_Jumping_8;
                }
                else
                {
                    // Drop through to idle/attack case
                }
            }
            else if (field_B4_velx > FP_FromInteger(0))
            {
                if (HandleEnemyStopper(2, Path_EnemyStopper::StopDirection::Right_1))
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_4_ChasingAbe::eBrain4_ToChasing_5;
                }

                if (abs(xSnapped - x_exp) < 6 && Check_IsOnEndOfLine_4021A0(0, 1))
                {
                    ToHop();
                    return Brain_4_ChasingAbe::eBrain4_Jumping_8;
                }
                else
                {
                    // Drop through to idle/attack case
                }
            }

            if (!VIsObjNearby(kGridSize * FP_FromInteger(4), sActiveHero_507678))
            {
                return field_110_brain_sub_state;
            }

            if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), sActiveHero_507678->field_A8_xpos - field_A8_xpos))
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_4_ChasingAbe::eBrain4_ToChasing_5;
            }
            else
            {
                SetNextMotion(eParamiteMotions::Motion_18_RunningAttack);
                return Brain_4_ChasingAbe::eBrain4_Attacking_1;
            }
        }
        break;

        case Brain_4_ChasingAbe::eBrain4_Jumping_8:
            if (GetCurrentMotion() != eParamiteMotions::Motion_0_Idle)
            {
                return field_110_brain_sub_state;
            }
            return Brain_4_ChasingAbe::eBrain4_ToChasing_5;

        case Brain_4_ChasingAbe::eBrain4_TurningWhileChasing_9:
            if (GetCurrentMotion() != eParamiteMotions::Motion_5_Turn
                || !(field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame)))
            {
                return field_110_brain_sub_state;
            }
            SetNextMotion(eParamiteMotions::Motion_0_Idle);
            return Brain_4_ChasingAbe::eBrain4_ToChasing_5;

        case Brain_4_ChasingAbe::eBrain4_Turning_10:
            if (GetCurrentMotion() != eParamiteMotions::Motion_5_Turn
                || !field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_110_brain_sub_state;
            }

            if (VIsObjNearby(kGridSize, sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_23_Eating);
                field_114_timer = sGnFrame + 28;
                return Brain_4_ChasingAbe::eBrain4_Eating_13;
            }

            if (!field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                if (!Check_IsOnEndOfLine_4021A0(0, 1))
                {
                    SetNextMotion(eParamiteMotions::Motion_2_Walking);
                    return Brain_4_ChasingAbe::eBrain4_Walking_11;
                }
            }
            else if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                if (!Check_IsOnEndOfLine_4021A0(1, 1))
                {
                    SetNextMotion(eParamiteMotions::Motion_2_Walking);
                    return Brain_4_ChasingAbe::eBrain4_Walking_11;
                }
            }
            SetNextMotion(eParamiteMotions::Motion_6_Hop);
            return Brain_4_ChasingAbe::eBrain4_Jumping_8;

        case Brain_4_ChasingAbe::eBrain4_Walking_11:
        {
            if (!VIsFacingMe(sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_5_Turn);
                return Brain_4_ChasingAbe::eBrain4_Turning_10;
            }

            if (!VOnSameYLevel(sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_4_ChasingAbe::eBrain4_Walking_11;
            }

            if (VIsObjNearby(kGridSize, sActiveHero_507678))
            {
                SetNextMotion(eParamiteMotions::Motion_23_Eating);
                field_114_timer = sGnFrame + 28;
                return Brain_4_ChasingAbe::eBrain4_Eating_13;
            }

            const s16 x_exp = FP_GetExponent(field_A8_xpos);
            const s32 xSnapped = (x_exp & 0xFC00) + SnapToXGrid(field_BC_sprite_scale, x_exp & 0x3FF);
            if (field_B4_velx > FP_FromInteger(0))
            {
                if (abs(xSnapped - x_exp) < 6 && Check_IsOnEndOfLine_4021A0(0, 1))
                {
                    ToHop();
                    return Brain_4_ChasingAbe::eBrain4_WalkingToHop_12;
                }
            }
            else if (field_B4_velx < FP_FromInteger(0))
            {
                if (abs(xSnapped - x_exp) < 6 && Check_IsOnEndOfLine_4021A0(1, 1))
                {
                    ToHop();
                    return Brain_4_ChasingAbe::eBrain4_WalkingToHop_12;
                }
            }
        }
            return field_110_brain_sub_state;

        case Brain_4_ChasingAbe::eBrain4_WalkingToHop_12:
            if (GetCurrentMotion() != eParamiteMotions::Motion_0_Idle)
            {
                return field_110_brain_sub_state;
            }

            if (VIsFacingMe(sActiveHero_507678))
            {
                if (VIsObjNearby(kGridSize, sActiveHero_507678))
                {
                    SetNextMotion(eParamiteMotions::Motion_23_Eating);
                    field_114_timer = sGnFrame + 28;
                    return Brain_4_ChasingAbe::eBrain4_Eating_13;
                }
                else
                {
                    SetNextMotion(eParamiteMotions::Motion_2_Walking);
                    return Brain_4_ChasingAbe::eBrain4_Walking_11;
                }
            }
            else
            {
                SetNextMotion(eParamiteMotions::Motion_5_Turn);
                return Brain_4_ChasingAbe::eBrain4_Turning_10;
            }
            break;

        case Brain_4_ChasingAbe::eBrain4_Eating_13:
            if (field_114_timer > static_cast<s32>(sGnFrame))
            {
                return field_110_brain_sub_state;
            }
            SetNextMotion(eParamiteMotions::Motion_0_Idle);
            field_114_timer = sGnFrame + 15;
            return Brain_4_ChasingAbe::eBrain4_Idle_14;

        case Brain_4_ChasingAbe::eBrain4_Idle_14:
            if (field_114_timer > static_cast<s32>(sGnFrame))
            {
                return field_110_brain_sub_state;
            }
            SetNextMotion(eParamiteMotions::Motion_23_Eating);
            field_114_timer = sGnFrame + 28;
            return Brain_4_ChasingAbe::eBrain4_Eating_13;

        case Brain_4_ChasingAbe::eBrain4_Panic_15:
            if (Event_Get(kEventAbeOhm_8))
            {
                return field_110_brain_sub_state;
            }
            SetNextMotion(eParamiteMotions::Motion_0_Idle);
            return Brain_4_ChasingAbe::eBrain4_Inactive_0;

        default:
            return field_110_brain_sub_state;
    }
}

enum Brain_5_SpottedMeat
{
    eBrain5_Idle_0 = 0,
    eBrain5_Running_1 = 1,
    eBrain5_Walking_2 = 2,
    eBrain5_Jumping_3 = 3,
    eBrain5_Turning_4 = 4,
    eBrain5_AttentiveToMeat_5 = 5,
    eBrain5_Eating_6 = 6
};

s16 Paramite::Brain_5_SpottedMeat()
{
    if (Event_Get(kEventDeathReset_4) || Event_Get(kEvent_9))
    {
        mBaseGameObjectFlags.Set(Options::eDead);
    }

    if (field_148_pMeat->VIsFalling() || field_148_pMeat->mBaseGameObjectFlags.Get(BaseGameObject::eDead))
    {
        Sound(ParamiteSpeak::DetectedMeat_7);
        SetBrain(&Paramite::Brain_0_Patrol);
        field_148_pMeat->mBaseGameObjectRefCount--;
        field_148_pMeat = nullptr;
        SetNextMotion(eParamiteMotions::Motion_0_Idle);
        return Brain_0_Patrol::eBrain0_Inactive_0;
    }

    if (field_148_pMeat->field_124_pLine)
    {
        if (FP_Abs(field_148_pMeat->field_AC_ypos - field_AC_ypos) > FP_FromInteger(20))
        {
            field_148_pMeat->mBaseGameObjectRefCount--;
            field_148_pMeat = nullptr;
            SetNextMotion(eParamiteMotions::Motion_0_Idle);
            SetBrain(&Paramite::Brain_0_Patrol);
            return Brain_0_Patrol::eBrain0_Inactive_0;
        }
    }

    const FP kGridSize = ScaleToGridSize(field_BC_sprite_scale);

    switch (field_110_brain_sub_state)
    {
        case Brain_5_SpottedMeat::eBrain5_Idle_0:
            if (!VIsFacingMe(field_148_pMeat))
            {
                if (FP_Abs(field_148_pMeat->field_A8_xpos - field_A8_xpos) > FP_FromInteger(5))
                {
                    Sound(ParamiteSpeak::DetectedMeat_7);
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_5_SpottedMeat::eBrain5_Turning_4;
                }
            }

            if (VIsObj_GettingNear_On_X(field_148_pMeat))
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_5_SpottedMeat::eBrain5_AttentiveToMeat_5;
            }

            if (!field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                if (Check_IsOnEndOfLine_4021A0(0, 1))
                {
                    ToHop();
                    return Brain_5_SpottedMeat::eBrain5_Jumping_3;
                }
            }
            else if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                if (Check_IsOnEndOfLine_4021A0(1, 1))
                {
                    ToHop();
                    return Brain_5_SpottedMeat::eBrain5_Jumping_3;
                }
            }

            if (!VIsObjNearby(kGridSize * FP_FromInteger(3), field_148_pMeat))
            {
                SetNextMotion(eParamiteMotions::Motion_3_Running);
                return Brain_5_SpottedMeat::eBrain5_Running_1;
            }

            if (VIsObjNearby(field_BC_sprite_scale * FP_FromInteger(40), field_148_pMeat))
            {
                if (field_A8_xpos == field_148_pMeat->field_A8_xpos)
                {
                    SetNextMotion(eParamiteMotions::Motion_2_Walking);
                    return Brain_5_SpottedMeat::eBrain5_Walking_2;
                }

                if (!field_148_pMeat->field_124_pLine || !BaseAliveGameObjectCollisionLine)
                {
                    SetNextMotion(eParamiteMotions::Motion_0_Idle);
                    return Brain_5_SpottedMeat::eBrain5_AttentiveToMeat_5;
                }

                SetNextMotion(eParamiteMotions::Motion_23_Eating);
                field_114_timer = sGnFrame + field_11C_meat_eating_time;
                return Brain_5_SpottedMeat::eBrain5_Eating_6;
            }

            SetNextMotion(eParamiteMotions::Motion_2_Walking);
            return Brain_5_SpottedMeat::eBrain5_Walking_2;

        case Brain_5_SpottedMeat::eBrain5_Running_1:
        {
            const s16 x_exp = FP_GetExponent(field_A8_xpos);
            const s32 xSnapped = (x_exp & 0xFC00) + SnapToXGrid(field_BC_sprite_scale, x_exp & 0x3FF);
            if (field_B4_velx < FP_FromInteger(0))
            {
                if (abs(xSnapped - x_exp) < 6 && Check_IsOnEndOfLine_4021A0(1, 1))
                {
                    ToHop();
                    return Brain_5_SpottedMeat::eBrain5_Jumping_3;
                }
            }

            if (field_B4_velx > FP_FromInteger(0))
            {
                if (abs(xSnapped - x_exp) < 6 && Check_IsOnEndOfLine_4021A0(0, 1))
                {
                    ToHop();
                    return Brain_5_SpottedMeat::eBrain5_Jumping_3;
                }
            }

            if (!VIsFacingMe(field_148_pMeat))
            {
                if (FP_Abs(field_148_pMeat->field_A8_xpos - field_A8_xpos) > FP_FromInteger(5))
                {
                    Sound(ParamiteSpeak::DetectedMeat_7);
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_5_SpottedMeat::eBrain5_Turning_4;
                }
            }

            if (VIsObjNearby(field_BC_sprite_scale * FP_FromInteger(40), field_148_pMeat))
            {
                if (field_148_pMeat->field_124_pLine)
                {
                    SetNextMotion(eParamiteMotions::Motion_23_Eating);
                    field_114_timer = sGnFrame + field_11C_meat_eating_time;
                    return Brain_5_SpottedMeat::eBrain5_Eating_6;
                }
                return Brain_5_SpottedMeat::eBrain5_Running_1;
            }

            if (!VIsObjNearby(kGridSize * FP_FromInteger(3), field_148_pMeat))
            {
                return field_110_brain_sub_state;
            }
            SetNextMotion(eParamiteMotions::Motion_2_Walking);
            return Brain_5_SpottedMeat::eBrain5_Walking_2;
        }

        case Brain_5_SpottedMeat::eBrain5_Walking_2:
        {
            const s16 x_exp = FP_GetExponent(field_A8_xpos);
            const s32 xSnapped = (x_exp & 0xFC00) + SnapToXGrid(field_BC_sprite_scale, x_exp & 0x3FF);
            if (field_B4_velx < FP_FromInteger(0))
            {
                if (abs(xSnapped - x_exp) < 6 && Check_IsOnEndOfLine_4021A0(1, 1))
                {
                    ToHop();
                    return Brain_5_SpottedMeat::eBrain5_Jumping_3;
                }
            }

            if (field_B4_velx > FP_FromInteger(0))
            {
                if (abs(xSnapped - x_exp) >= 6 && Check_IsOnEndOfLine_4021A0(0, 1))
                {
                    ToHop();
                    return Brain_5_SpottedMeat::eBrain5_Jumping_3;
                }
            }

            if (!VIsFacingMe(field_148_pMeat))
            {
                if (FP_Abs(field_148_pMeat->field_A8_xpos - field_A8_xpos) > FP_FromInteger(5))
                {
                    Sound(ParamiteSpeak::DetectedMeat_7);
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_5_SpottedMeat::eBrain5_Turning_4;
                }
            }

            if (!VIsObjNearby(field_BC_sprite_scale * FP_FromInteger(40), field_148_pMeat))
            {
                return field_110_brain_sub_state;
            }

            if (field_148_pMeat->field_124_pLine)
            {
                SetNextMotion(eParamiteMotions::Motion_23_Eating);
                field_114_timer = sGnFrame + field_11C_meat_eating_time;
                return Brain_5_SpottedMeat::eBrain5_Eating_6;
            }
            return Brain_5_SpottedMeat::eBrain5_Walking_2;
        }

        case Brain_5_SpottedMeat::eBrain5_Jumping_3:
            if (GetCurrentMotion() != eParamiteMotions::Motion_0_Idle)
            {
                return field_110_brain_sub_state;
            }
            return Brain_5_SpottedMeat::eBrain5_Idle_0;

        case Brain_5_SpottedMeat::eBrain5_Turning_4:
            if (GetCurrentMotion() != eParamiteMotions::Motion_5_Turn
                || !field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_110_brain_sub_state;
            }

            if (!VIsFacingMe(field_148_pMeat))
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_5_SpottedMeat::eBrain5_Idle_0;
            }

            if (FP_Abs(field_148_pMeat->field_A8_xpos - field_A8_xpos) > FP_FromInteger(5))
            {
                Sound(ParamiteSpeak::DetectedMeat_7);
                SetNextMotion(eParamiteMotions::Motion_5_Turn);
                return 4;
            }
            else
            {
                SetNextMotion(eParamiteMotions::Motion_0_Idle);
                return Brain_5_SpottedMeat::eBrain5_Idle_0;
            }
            break;

        case Brain_5_SpottedMeat::eBrain5_AttentiveToMeat_5:
            if (!VIsFacingMe(field_148_pMeat))
            {
                Sound(ParamiteSpeak::DetectedMeat_7);
                SetNextMotion(eParamiteMotions::Motion_5_Turn);
                return Brain_5_SpottedMeat::eBrain5_Turning_4;
            }

            if (VIsObj_GettingNear_On_X(field_148_pMeat))
            {
                return field_110_brain_sub_state;
            }

            if (!field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                if (Check_IsOnEndOfLine_4021A0(0, 1))
                {
                    ToHop();
                    return Brain_5_SpottedMeat::eBrain5_Jumping_3;
                }
            }
            else if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                if (Check_IsOnEndOfLine_4021A0(1, 1))
                {
                    ToHop();
                    return Brain_5_SpottedMeat::eBrain5_Jumping_3;
                }
            }

            if (!VIsObjNearby(kGridSize * FP_FromInteger(3), field_148_pMeat))
            {
                SetNextMotion(eParamiteMotions::Motion_3_Running);
                return Brain_5_SpottedMeat::eBrain5_Running_1;
            }

            if (!VIsObjNearby(field_BC_sprite_scale * FP_FromInteger(40), field_148_pMeat))
            {
                SetNextMotion(eParamiteMotions::Motion_2_Walking);
                return Brain_5_SpottedMeat::eBrain5_Walking_2;
            }

            if (!field_148_pMeat->field_124_pLine || !BaseAliveGameObjectCollisionLine)
            {
                return Brain_5_SpottedMeat::eBrain5_AttentiveToMeat_5;
            }

            SetNextMotion(eParamiteMotions::Motion_23_Eating);
            field_114_timer = sGnFrame + field_11C_meat_eating_time;
            return Brain_5_SpottedMeat::eBrain5_Eating_6;

        case Brain_5_SpottedMeat::eBrain5_Eating_6:
            if (!VIsObjNearby(field_BC_sprite_scale * FP_FromInteger(40), field_148_pMeat))
            {
                SetNextMotion(eParamiteMotions::Motion_2_Walking);
                return Brain_5_SpottedMeat::eBrain5_Walking_2;
            }

            if (!VIsFacingMe(field_148_pMeat))
            {
                if (FP_Abs(field_148_pMeat->field_A8_xpos - field_A8_xpos) > FP_FromInteger(5))
                {
                    Sound(ParamiteSpeak::DetectedMeat_7);
                    SetNextMotion(eParamiteMotions::Motion_5_Turn);
                    return Brain_5_SpottedMeat::eBrain5_Turning_4;
                }
            }

            if (field_114_timer > static_cast<s32>(sGnFrame))
            {
                return field_110_brain_sub_state;
            }

            field_148_pMeat->mBaseGameObjectRefCount--;
            field_148_pMeat->mBaseGameObjectFlags.Set(Options::eDead);
            field_148_pMeat = nullptr;
            SetNextMotion(eParamiteMotions::Motion_0_Idle);
            SetBrain(&Paramite::Brain_0_Patrol);
            return Brain_0_Patrol::eBrain0_Inactive_0;

        default:
            return field_110_brain_sub_state;
    }
}

void Paramite::SetBrain(Paramite::TParamiteBrain fn)
{
    ::SetBrain(fn, field_10C_fn, sParamiteBrainTable);
}

bool Paramite::BrainIs(Paramite::TParamiteBrain fn)
{
    return ::BrainIs(fn, field_10C_fn, sParamiteBrainTable);
}

s16 Paramite::HandleEnemyStopper(s16 numGridBlocks, Path_EnemyStopper::StopDirection dir)
{
    const FP kGridSize = ScaleToGridSize(field_BC_sprite_scale);
    const FP numGridBlocksScaled = (kGridSize * FP_FromInteger(numGridBlocks));
    BaseAliveGameObjectPathTLV = gMap.TLV_Get_At_446260(
        FP_GetExponent(field_A8_xpos + numGridBlocksScaled),
        FP_GetExponent(field_AC_ypos),
        FP_GetExponent(field_A8_xpos + numGridBlocksScaled),
        FP_GetExponent(field_AC_ypos),
        TlvTypes::EnemyStopper_79);

    if (BaseAliveGameObjectPathTLV)
    {
        // No stopper or its disabled
        auto pEnemyStopper = static_cast<Path_EnemyStopper*>(BaseAliveGameObjectPathTLV);
        if (!pEnemyStopper || !SwitchStates_Get(pEnemyStopper->field_1A_switch_id))
        {
            return 0;
        }

        // Matches the direction we want?
        if (pEnemyStopper->field_18_direction == dir)
        {
            return 1;
        }
    }
    return 0;
}

void Paramite::Motion_0_Idle()
{
    if (!ToNextMotion()
        && gMap.GetDirection(
               field_B2_lvl_number,
               field_B0_path_number,
               field_A8_xpos,
               field_AC_ypos)
               >= CameraPos::eCamCurrent_0)
    {
        if (BrainIs(&Paramite::Brain_4_ChasingAbe))
        {
            MusicController::static_PlayMusic(MusicController::MusicTypes::eType12, this, 0, 0);
        }
        else
        {
            MusicController::static_PlayMusic(MusicController::MusicTypes::eType11, this, 0, 0);
        }
    }
}

const FP sWalkBeginVelTable_4BBC88[3] = {
    FP_FromInteger(0),
    FP_FromDouble(1.402099609375),
    FP_FromDouble(9.025436401367188)};

void Paramite::Motion_1_WalkBegin()
{
    Event_Broadcast(kEventNoise_0, this);

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
    {
        field_B4_velx = field_BC_sprite_scale * -sWalkBeginVelTable_4BBC88[field_10_anim.field_92_current_frame];
    }
    else
    {
        field_B4_velx = field_BC_sprite_scale * sWalkBeginVelTable_4BBC88[field_10_anim.field_92_current_frame];
    }

    if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), field_B4_velx))
    {
        ToKnockBack();
    }
    else
    {
        MoveOnLine();

        if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            SetCurrentMotion(eParamiteMotions::Motion_2_Walking);
        }
    }
}

const FP sWalkVelTable_4BBC50[14] = {
    FP_FromDouble(2.03741455078125),
    FP_FromDouble(4.02911376953125),
    FP_FromDouble(4.46575927734375),
    FP_FromDouble(3.3473663330078125),
    FP_FromDouble(3.1887054443359375),
    FP_FromDouble(4.937652587890625),
    FP_FromDouble(3.126708984375),
    FP_FromDouble(2.50823974609375),
    FP_FromDouble(4.440032958984375),
    FP_FromDouble(3.9687652587890625),
    FP_FromDouble(2.9050140380859375),
    FP_FromDouble(3.6402130126953125),
    FP_FromDouble(4.701416015625),
    FP_FromDouble(2.72283935546875)};

void Paramite::Motion_2_Walking()
{
    Event_Broadcast(kEventNoise_0, this);

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
    {
        field_B4_velx = field_BC_sprite_scale * -sWalkVelTable_4BBC50[field_10_anim.field_92_current_frame];
    }
    else
    {
        field_B4_velx = field_BC_sprite_scale * sWalkVelTable_4BBC50[field_10_anim.field_92_current_frame];
    }

    if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), field_B4_velx))
    {
        ToKnockBack();
        return;
    }

    MoveOnLine();

    if (GetCurrentMotion() != eParamiteMotions::Motion_2_Walking)
    {
        return;
    }

    switch (field_10_anim.field_92_current_frame)
    {
        case 0:
            [[fallthrough]];
        case 7:
            Sound(field_10_anim.field_92_current_frame == 7 ? ParamiteSpeak::LoudStep_3 : ParamiteSpeak::SlightStep_4);

            if (GetNextMotion() == eParamiteMotions::Motion_0_Idle
                || GetNextMotion() == eParamiteMotions::Motion_13_GameSpeakBegin
                || GetNextMotion() == eParamiteMotions::Motion_5_Turn
                || GetNextMotion() == eParamiteMotions::Motion_25_Death
                || GetNextMotion() == eParamiteMotions::Motion_15_Hiss
                || GetNextMotion() == eParamiteMotions::Motion_23_Eating)
            {
                SetCurrentMotion(eParamiteMotions::Motion_9_WalkEnd);
            }
            break;

        case 3:
            [[fallthrough]];
        case 10:
            if (GetNextMotion() == eParamiteMotions::Motion_3_Running)
            {
                SetPreviousMotion(eParamiteMotions::Motion_3_Running);
                mBaseAliveGameObjectLastAnimFrame = 11;
                field_140_use_prev_motion = 1;
                SetNextMotion(-1);
            }

            if (GetNextMotion() == eParamiteMotions::Motion_18_RunningAttack)
            {
                SetCurrentMotion(eParamiteMotions::Motion_18_RunningAttack);
                SetNextMotion(-1);
                if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
                {
                    field_B4_velx = -(ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4));
                }
                else
                {
                    field_B4_velx = ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4);
                }
            }

            if (!field_142_bSnapped)
            {
                MapFollowMe_401D30(1);
                field_142_bSnapped = 1;
            }

            break;

        default:
            field_142_bSnapped = 0;
            break;
    }

    SetMusic();
}

const FP sRunningTable_4BBCC0[14] = {
    FP_FromDouble(5.1657867431640625),
    FP_FromDouble(5.624664306640625),
    FP_FromDouble(3.328460693359375),
    FP_FromDouble(3.532440185546875),
    FP_FromDouble(5.26165771484375),
    FP_FromDouble(10.463729858398438),
    FP_FromDouble(9.87750244140625),
    FP_FromDouble(8.148956298828125),
    FP_FromDouble(8.294830322265625),
    FP_FromDouble(7.1818389892578125),
    FP_FromDouble(6.7950592041015625),
    FP_FromDouble(8.932144165039062),
    FP_FromDouble(11.460769653320312),
    FP_FromDouble(5.9268646240234375)};

void Paramite::Motion_3_Running()
{
    Event_Broadcast(kEventNoise_0, this);

    FP frameVelx = {};
    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
    {
        frameVelx = field_BC_sprite_scale * -sRunningTable_4BBCC0[field_10_anim.field_92_current_frame];
    }
    else
    {
        frameVelx = field_BC_sprite_scale * sRunningTable_4BBCC0[field_10_anim.field_92_current_frame];
    }

    field_B4_velx = frameVelx;

    if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), field_B4_velx))
    {
        ToKnockBack();
        return;
    }

    MoveOnLine();

    if (GetCurrentMotion() != eParamiteMotions::Motion_3_Running)
    {
        return;
    }

    if (field_10_anim.field_92_current_frame == 3)
    {
        SFX_Play_Pitch(SoundEffect::PickupItem_33, 45, -600, 0);
        if (GetNextMotion() == eParamiteMotions::Motion_2_Walking)
        {
            SetCurrentMotion(eParamiteMotions::Motion_8_WalkRunTransition);
            SetNextMotion(-1);
        }

        if (!mNextMotion
            || GetNextMotion() == eParamiteMotions::Motion_5_Turn
            || GetNextMotion() == eParamiteMotions::Motion_23_Eating
            || GetNextMotion() == eParamiteMotions::Motion_25_Death
            || GetNextMotion() == eParamiteMotions::Motion_15_Hiss)
        {
            SetCurrentMotion(eParamiteMotions::Motion_11_RunEnd);
        }

        if (GetNextMotion() == eParamiteMotions::Motion_18_RunningAttack)
        {
            SetCurrentMotion(eParamiteMotions::Motion_18_RunningAttack);
            SetNextMotion(-1);

            if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                field_B4_velx = -(ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4));
            }
            else
            {
                field_B4_velx = (ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4));
            }
        }

        if (GetNextMotion() == eParamiteMotions::Motion_6_Hop)
        {
            VOnTrapDoorOpen();

            BaseAliveGameObjectLastLineYPos = field_AC_ypos;
            SetCurrentMotion(eParamiteMotions::Motion_6_Hop);
            SetNextMotion(-1);
            field_B8_vely = -FP_FromDouble(7.2) * field_BC_sprite_scale;
            field_AC_ypos += field_B8_vely;
            BaseAliveGameObjectCollisionLine = nullptr;

            if (field_142_bSnapped == 0)
            {
                MapFollowMe_401D30(1);
                field_142_bSnapped = 1;
            }

            SetMusic();
            return;
        }

        if (!field_142_bSnapped)
        {
            MapFollowMe_401D30(1);
            field_142_bSnapped = 1;
        }

        SetMusic();
        return;
    }

    if (field_10_anim.field_92_current_frame == 10)
    {
        Sound(ParamiteSpeak::LoudStep_3);
        Environment_SFX_42A220(EnvironmentSfx::eHitGroundSoft_6, 50, 600, 0);

        if (GetNextMotion() == eParamiteMotions::Motion_2_Walking)
        {
            SetCurrentMotion(eParamiteMotions::Motion_8_WalkRunTransition);
            SetNextMotion(-1);
        }

        if (GetNextMotion() == eParamiteMotions::Motion_0_Idle
            || GetNextMotion() == eParamiteMotions::Motion_5_Turn
            || GetNextMotion() == eParamiteMotions::Motion_23_Eating
            || GetNextMotion() == eParamiteMotions::Motion_25_Death
            || GetNextMotion() == eParamiteMotions::Motion_15_Hiss)
        {
            SetCurrentMotion(eParamiteMotions::Motion_11_RunEnd);
        }

        if (GetNextMotion() == eParamiteMotions::Motion_18_RunningAttack)
        {
            SetCurrentMotion(eParamiteMotions::Motion_18_RunningAttack);
            SetNextMotion(-1);

            if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
            {
                field_B4_velx = -(ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4));
            }
            else
            {
                field_B4_velx = (ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4));
            }
        }

        if (GetNextMotion() == eParamiteMotions::Motion_6_Hop)
        {
            ToHop();
        }

        if (!field_142_bSnapped)
        {
            MapFollowMe_401D30(1);
            field_142_bSnapped = 1;
        }

        SetMusic();
        return;
    }

    field_142_bSnapped = 0;
    SetMusic();
}

void Paramite::Motion_4_Unknown()
{
    if (sNumCamSwappers_507668 <= 0)
    {
        if (sControlledCharacter_50767C == this)
        {
            SetCurrentMotion(mPreviousMotion);
            if (mLiftPoint)
            {
                // TODO: Correct type ??
                static_cast<LiftPoint*>(mLiftPoint)->field_12C_bMoving |= 1u;
            }
        }
        else
        {
            SetCurrentMotion(mPreviousMotion);
            if (mLiftPoint)
            {
                field_A8_xpos = FP_FromInteger((BaseAliveGameObjectCollisionLine->field_0_rect.x + BaseAliveGameObjectCollisionLine->field_0_rect.w) / 2);
                field_AC_ypos = FP_FromInteger(BaseAliveGameObjectCollisionLine->field_0_rect.y);
            }
        }
    }
}

void Paramite::Motion_5_Turn()
{
    if (field_10_anim.field_92_current_frame == 0)
    {
        Sound(ParamiteSpeak::SlightStep_4);
    }

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        field_10_anim.mAnimFlags.Toggle(AnimFlags::eBit5_FlipX);

        if (ToNextMotion())
        {
            field_10_anim.Set_Animation_Data(field_10_anim.field_18_frame_table_offset, nullptr);
        }
        else
        {
            ToIdle();
        }
    }
}

const FP sHopVelTable_4BBD28[16] = {
    FP_FromDouble(5.33526611328125),
    FP_FromDouble(5.8092041015625),
    FP_FromDouble(3.43768310546875),
    FP_FromDouble(3.6483001708984375),
    FP_FromDouble(5.434295654296875),
    FP_FromDouble(10.9881591796875),
    FP_FromDouble(10.020431518554688),
    FP_FromDouble(5.2939453125),
    FP_FromDouble(5.3142852783203125),
    FP_FromDouble(4.234375),
    FP_FromDouble(4.7738189697265625),
    FP_FromDouble(6.2389373779296875),
    FP_FromDouble(5.5633392333984375),
    FP_FromDouble(7.9202728271484375),
    FP_FromDouble(10.229019165039062),
    FP_FromDouble(5.77606201171875)};


void Paramite::Motion_6_Hop()
{
    Event_Broadcast(kEventNoise_0, this);

    FP frameVelX = {};
    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
    {
        frameVelX = (field_BC_sprite_scale * -sHopVelTable_4BBD28[field_10_anim.field_92_current_frame]);
    }
    else
    {
        frameVelX = (field_BC_sprite_scale * sHopVelTable_4BBD28[field_10_anim.field_92_current_frame]);
    }

    field_B4_velx = frameVelX;

    if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), field_B4_velx))
    {
        ToKnockBack();
    }
    else
    {
        PathLine* pLine = nullptr;
        FP hitX = {};
        FP hitY = {};

        if (InAirCollision_4019C0(&pLine, &hitX, &hitY, FP_FromDouble(0.9)))
        {
            switch (pLine->field_8_type)
            {
                case eLineTypes::eFloor_0:
                case eLineTypes::eBackgroundFloor_4:
                case eLineTypes::eUnknown_32:
                case eLineTypes::eUnknown_36:
                {
                    ToIdle();

                    BaseAliveGameObjectCollisionLine = pLine;

                    PSX_RECT bRect = {};
                    VGetBoundingRect(&bRect, 1);
                    bRect.y += 5;
                    bRect.h += 5;
                    VOnCollisionWith(
                        {bRect.x, bRect.y},
                        {bRect.y, bRect.h},
                        ObjListPlatforms_50766C,
                        1,
                        (TCollisionCallBack) &BaseAliveGameObject::OnTrapDoorIntersection_401C10);
                    field_A8_xpos = hitX;
                    field_AC_ypos = hitY;
                    return;
                }

                case eLineTypes::eWallLeft_1:
                case eLineTypes::eWallRight_2:
                    field_B4_velx = (-field_B4_velx / FP_FromInteger(2));
                    return;

                default:
                    break;
            }
        }

        if (field_AC_ypos - BaseAliveGameObjectLastLineYPos > FP_FromInteger(5))
        {
            field_124_XSpeed = FP_FromDouble(0.75);
            SetCurrentMotion(eParamiteMotions::Motion_12_Falling);
        }

        if (gMap.GetDirection(
                field_B2_lvl_number,
                field_B0_path_number,
                field_A8_xpos,
                field_AC_ypos)
            >= CameraPos::eCamCurrent_0)
        {
            MusicController::static_PlayMusic(MusicController::MusicTypes::eType12, this, 0, 0);
        }
    }
}

const FP State_7_Unknown_VelTable_4BBCA8[2] = {
    FP_FromDouble(2.2522430419921875),
    FP_FromDouble(5.6306304931640625)};

void Paramite::Motion_7_Unknown()
{
    Event_Broadcast(kEventNoise_0, this);

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
    {
        field_B4_velx = field_BC_sprite_scale * -State_7_Unknown_VelTable_4BBCA8[field_10_anim.field_92_current_frame];
    }
    else
    {
        field_B4_velx = field_BC_sprite_scale * State_7_Unknown_VelTable_4BBCA8[field_10_anim.field_92_current_frame];
    }

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mPreviousMotion = 3;
        mBaseAliveGameObjectLastAnimFrame = 2;
        field_140_use_prev_motion = 1;
    }

    if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), field_B4_velx))
    {
        ToKnockBack();
    }
    else
    {
        MoveOnLine();
    }
}

const FP sWalkRunTransVelTable_4BBD18[3] = {
    FP_FromDouble(5.3964691162109375),
    FP_FromDouble(5.396453857421875),
    FP_FromDouble(5.396484375)};

void Paramite::Motion_8_WalkRunTransition()
{
    Event_Broadcast(kEventNoise_0, this);

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
    {
        field_B4_velx = field_BC_sprite_scale * -sWalkRunTransVelTable_4BBD18[field_10_anim.field_92_current_frame];
    }
    else
    {
        field_B4_velx = field_BC_sprite_scale * sWalkRunTransVelTable_4BBD18[field_10_anim.field_92_current_frame];
    }

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        SetCurrentMotion(eParamiteMotions::Motion_2_Walking);
    }

    if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), field_B4_velx))
    {
        ToKnockBack();
    }
    else
    {
        MoveOnLine();
    }
}

const FP sWalkEndVelTable_4BBC98[3] = {
    FP_FromDouble(2.3350982666015625),
    FP_FromDouble(5.0355224609375),
    FP_FromDouble(6.70159912109375)};

void Paramite::Motion_9_WalkEnd()
{
    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
    {
        field_B4_velx = field_BC_sprite_scale * -sWalkEndVelTable_4BBC98[field_10_anim.field_92_current_frame];
    }
    else
    {
        field_B4_velx = field_BC_sprite_scale * sWalkEndVelTable_4BBC98[field_10_anim.field_92_current_frame];
    }

    if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), field_B4_velx))
    {
        ToKnockBack();
    }
    else
    {
        MoveOnLine();
        Event_Broadcast(kEventNoise_0, this);

        if (!field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            return;
        }
    }

    ToIdle();
}

const FP sRunBeginVelTable_4BBCF8[3] = {
    FP_FromDouble(1.8789520263671875),
    FP_FromDouble(2.15924072265625),
    FP_FromDouble(3.330413818359375)};

void Paramite::Motion_10_RunBegin()
{
    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
    {
        field_B4_velx = (field_BC_sprite_scale * -sRunBeginVelTable_4BBCF8[field_10_anim.field_92_current_frame]);
    }
    else
    {
        field_B4_velx = field_BC_sprite_scale * sRunBeginVelTable_4BBCF8[field_10_anim.field_92_current_frame];
    }

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mPreviousMotion = 3;
        mBaseAliveGameObjectLastAnimFrame = 2;
        field_140_use_prev_motion = 1;
    }

    Event_Broadcast(kEventNoise_0, this);

    if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), field_B4_velx))
    {
        ToKnockBack();
    }
    else
    {
        MoveOnLine();
    }
}

const FP sRunEndVelTable_4BBD08[3] = {
    FP_FromDouble(3.11126708984375),
    FP_FromDouble(3.063446044921875),
    FP_FromDouble(3.43255615234375)};

void Paramite::Motion_11_RunEnd()
{
    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
    {
        field_B4_velx = field_BC_sprite_scale * -sRunEndVelTable_4BBD08[field_10_anim.field_92_current_frame];
    }
    else
    {
        field_B4_velx = field_BC_sprite_scale * sRunEndVelTable_4BBD08[field_10_anim.field_92_current_frame];
    }

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToIdle();
    }

    if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), field_B4_velx))
    {
        ToKnockBack();
    }
    else
    {
        MoveOnLine();
    }
}

void Paramite::Motion_12_Falling()
{
    if (field_B4_velx > FP_FromInteger(0))
    {
        field_B4_velx -= (field_BC_sprite_scale * field_124_XSpeed);
        if (field_B4_velx < FP_FromInteger(0))
        {
            field_B4_velx = FP_FromInteger(0);
        }
    }
    else if (field_B4_velx < FP_FromInteger(0))
    {
        field_B4_velx += (field_BC_sprite_scale * field_124_XSpeed);
        if (field_B4_velx > FP_FromInteger(0))
        {
            field_B4_velx = FP_FromInteger(0);
        }
    }

    PathLine* pLine = nullptr;
    FP hitX = {};
    FP hitY = {};
    if (InAirCollision_4019C0(&pLine, &hitX, &hitY, FP_FromDouble(1.8)))
    {
        switch (pLine->field_8_type)
        {
            case 0:
            case 4:
            case 32:
            case 36:
            {
                ToIdle();

                BaseAliveGameObjectCollisionLine = pLine;

                PSX_RECT bRect = {};
                VGetBoundingRect(&bRect, 1);
                bRect.y += 5;
                bRect.h += 5;

                VOnCollisionWith(
                    {bRect.x, bRect.y},
                    {bRect.w, bRect.h},
                    ObjListPlatforms_50766C,
                    1,
                    (TCollisionCallBack) &BaseAliveGameObject::OnTrapDoorIntersection_401C10);

                field_A8_xpos = hitX;
                field_AC_ypos = hitY;
                MapFollowMe_401D30(TRUE);
                break;
            }

            case 1:
            case 2:
                field_B4_velx = (-field_B4_velx / FP_FromInteger(2));
                break;

            default:
                return;
        }
    }
}

void Paramite::Motion_13_GameSpeakBegin()
{
    PSX_RECT abeRect = {};
    sActiveHero_507678->VGetBoundingRect(&abeRect, 1);

    PSX_RECT ourRect = {};
    VGetBoundingRect(&ourRect, 1);

    if (abeRect.x <= ourRect.w
        && abeRect.w >= ourRect.x
        && abeRect.h >= ourRect.y
        && abeRect.y <= ourRect.h)
    {
        if (VIsObj_GettingNear_On_X(sActiveHero_507678))
        {
            if (field_BC_sprite_scale == sActiveHero_507678->field_BC_sprite_scale)
            {
                if (sActiveHero_507678->mHealth > FP_FromInteger(0))
                {
                    SFX_Play_Mono(SoundEffect::KillEffect_78, 0, 0);
                    Mudokon_SFX_42A4D0(MudSounds::eKnockbackOuch_10, 0, 0, sActiveHero_507678);
                }
                sActiveHero_507678->VTakeDamage(this);
            }
        }
    }

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        SetCurrentMotion(eParamiteMotions::Motion_14_PreHiss);
    }

    if (gMap.GetDirection(
            field_B2_lvl_number,
            field_B0_path_number,
            field_A8_xpos,
            field_AC_ypos)
        == CameraPos::eCamCurrent_0)
    {
        MusicController::static_PlayMusic(MusicController::MusicTypes::eType12, this, 0, 0);
    }
}

void Paramite::Motion_14_PreHiss()
{
    PSX_RECT abeRect = {};
    sActiveHero_507678->VGetBoundingRect(&abeRect, 1);

    PSX_RECT rect = {};
    VGetBoundingRect(&rect, 1);

    if (abeRect.x <= rect.w
        && abeRect.w >= rect.x
        && abeRect.h >= rect.y
        && abeRect.y <= rect.h)
    {
        if (VIsObj_GettingNear_On_X(sActiveHero_507678))
        {
            if (field_BC_sprite_scale == sActiveHero_507678->field_BC_sprite_scale)
            {
                sActiveHero_507678->VTakeDamage(this);
            }
        }
    }

    if (GetNextMotion() == eParamiteMotions::Motion_18_RunningAttack)
    {
        SetCurrentMotion(eParamiteMotions::Motion_18_RunningAttack);
        SetNextMotion(-1);
        if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
        {
            field_B4_velx = -(ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4));
        }
        else
        {
            field_B4_velx = (ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4));
        }
    }

    if (GetNextMotion() == eParamiteMotions::Motion_22_Unknown)
    {
        SetCurrentMotion(eParamiteMotions::Motion_22_Unknown);
        SetNextMotion(-1);
    }

    if (GetNextMotion() == eParamiteMotions::Motion_15_Hiss)
    {
        SetCurrentMotion(eParamiteMotions::Motion_15_Hiss);
        SetNextMotion(-1);
    }

    if (mNextMotion != -1)
    {
        SetCurrentMotion(eParamiteMotions::Motion_17_GameSpeakEnd);
    }

    if (gMap.GetDirection(
            field_B2_lvl_number,
            field_B0_path_number,
            field_A8_xpos,
            field_AC_ypos)
        == CameraPos::eCamCurrent_0)
    {
        MusicController::static_PlayMusic(MusicController::MusicTypes::eType12, this, 0, 0);
    }
}

void Paramite::Motion_15_Hiss()
{
    PSX_RECT abeRect = {};
    sActiveHero_507678->VGetBoundingRect(&abeRect, 1);

    PSX_RECT rect = {};
    VGetBoundingRect(&rect, 1);

    if (field_10_anim.field_92_current_frame == 2)
    {
        Sound(ParamiteSpeak::Stay_1);
    }

    if (abeRect.x <= rect.w
        && abeRect.w >= rect.x
        && abeRect.h >= rect.y
        && abeRect.y <= rect.h)
    {
        if (VIsObj_GettingNear_On_X(sActiveHero_507678))
        {
            if (field_BC_sprite_scale == sActiveHero_507678->field_BC_sprite_scale)
            {
                sActiveHero_507678->VTakeDamage(this);
            }
        }
    }

    if (GetNextMotion() == eParamiteMotions::Motion_18_RunningAttack)
    {
        SetCurrentMotion(eParamiteMotions::Motion_18_RunningAttack);
        SetNextMotion(-1);
        if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit5_FlipX))
        {
            field_B4_velx = -(ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4));
        }
        else
        {
            field_B4_velx = (ScaleToGridSize(field_BC_sprite_scale) / FP_FromInteger(4));
        }
    }
    else if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        SetCurrentMotion(eParamiteMotions::Motion_16_PostHiss);
    }
}

void Paramite::Motion_16_PostHiss()
{
    PSX_RECT abeRect = {};
    sActiveHero_507678->VGetBoundingRect(&abeRect, 1);

    PSX_RECT rect = {};
    VGetBoundingRect(&rect, 1);

    if (abeRect.x <= rect.w
        && abeRect.w >= rect.x
        && abeRect.h >= rect.y
        && abeRect.y <= rect.h)
    {
        if (VIsObj_GettingNear_On_X(sActiveHero_507678))
        {
            if (field_BC_sprite_scale == sActiveHero_507678->field_BC_sprite_scale)
            {
                sActiveHero_507678->VTakeDamage(this);
            }
        }
    }

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        SetCurrentMotion(eParamiteMotions::Motion_14_PreHiss);
    }
}

void Paramite::Motion_17_GameSpeakEnd()
{
    PSX_RECT abeRect = {};
    sActiveHero_507678->VGetBoundingRect(&abeRect, 1);

    PSX_RECT rect = {};
    VGetBoundingRect(&rect, 1);

    if (abeRect.x <= rect.w
        && abeRect.w >= rect.x
        && abeRect.h >= rect.y
        && abeRect.y <= rect.h)
    {
        if (VIsObj_GettingNear_On_X(sActiveHero_507678))
        {
            if (field_BC_sprite_scale == sActiveHero_507678->field_BC_sprite_scale)
            {
                sActiveHero_507678->VTakeDamage(this);
            }
        }
    }
    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        if (!ToNextMotion())
        {
            field_10_anim.mAnimFlags.Clear(AnimFlags::eBit6_FlipY);
            field_10_anim.mAnimFlags.Clear(AnimFlags::eBit7_SwapXY);
            field_124_XSpeed = FP_FromInteger(0);
            field_B4_velx = FP_FromInteger(0);
            field_B8_vely = FP_FromInteger(0);
            SetCurrentMotion(eParamiteMotions::Motion_0_Idle);
            MapFollowMe_401D30(TRUE);
        }
    }
}

void Paramite::Motion_18_RunningAttack()
{
    if (WallHit_401930(field_BC_sprite_scale * FP_FromInteger(10), field_B4_velx))
    {
        BaseAliveGameObjectCollisionLine = nullptr;

        ToKnockBack();
        return;
    }

    field_A8_xpos += field_B4_velx;
    MapFollowMe_401D30(FALSE);

    PSX_RECT abeRect = {};
    sActiveHero_507678->VGetBoundingRect(&abeRect, 1);

    PSX_RECT rect = {};
    VGetBoundingRect(&rect, 1);

    if (abeRect.x <= rect.w
        && abeRect.w >= rect.x
        && abeRect.h >= rect.y
        && abeRect.y <= rect.h
        && field_BC_sprite_scale == sActiveHero_507678->field_BC_sprite_scale)
    {
        sActiveHero_507678->VTakeDamage(this);
    }

    if (field_10_anim.field_92_current_frame == 3)
    {
        Sound(ParamiteSpeak::CMon_or_Attack_0);
    }

    if (field_10_anim.field_92_current_frame == 11)
    {
        FP gridSizeDirected = {};
        if (field_B4_velx >= FP_FromInteger(0))
        {
            gridSizeDirected = ScaleToGridSize(field_BC_sprite_scale);
        }
        else
        {
            gridSizeDirected = -ScaleToGridSize(field_BC_sprite_scale);
        }

        FP hitX = {};
        FP hitY = {};
        PathLine* pLine = nullptr;
        if (sCollisions_DArray_504C6C->RayCast(
                field_A8_xpos + gridSizeDirected,
                field_AC_ypos - FP_FromInteger(10),
                field_A8_xpos + gridSizeDirected,
                field_AC_ypos + FP_FromInteger(10),
                &pLine,
                &hitX,
                &hitY,
                field_BC_sprite_scale != FP_FromDouble(0.5) ? 7 : 0x70))
        {
            if (pLine->field_8_type == eLineTypes ::eUnknown_32 ||
                pLine->field_8_type == eLineTypes::eUnknown_36)
            {
                PSX_RECT r = {};
                VGetBoundingRect(&r, 1);
                r.y += 5;
                r.h += 5;

                VOnCollisionWith(
                    {r.x, r.y},
                    {r.w, r.h},
                    ObjListPlatforms_50766C,
                    1,
                    (TCollisionCallBack) &BaseAliveGameObject::OnTrapDoorIntersection_401C10);
            }
            field_AC_ypos = hitY;
            BaseAliveGameObjectCollisionLine = pLine;
        }
        else
        {
            BaseAliveGameObjectCollisionLine = nullptr;
            SetCurrentMotion(eParamiteMotions::Motion_12_Falling);
            field_124_XSpeed = FP_FromDouble(0.75);
            field_AC_ypos -= (field_BC_sprite_scale * FP_FromInteger(10));
        }
    }

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToIdle();
    }

    if (gMap.GetDirection(
            field_B2_lvl_number,
            field_B0_path_number,
            field_A8_xpos,
            field_AC_ypos)
        >= CameraPos::eCamCurrent_0)
    {
        MusicController::static_PlayMusic(MusicController::MusicTypes::eType12, this, 0, 0);
    }
}

void Paramite::Motion_19_Empty()
{
    // Empty
}

void Paramite::Motion_20_SurpriseWeb()
{
    FP hitX = {};
    FP hitY = {};

    if (sCollisions_DArray_504C6C->RayCast(
            field_A8_xpos,
            field_AC_ypos,
            field_A8_xpos,
            field_B8_vely + field_AC_ypos,
            &BaseAliveGameObjectCollisionLine,
            &hitX,
            &hitY,
            field_BC_sprite_scale != FP_FromDouble(0.5) ? 1 : 0x10)
        == 1)
    {
        field_AC_ypos = hitY;
        SetCurrentMotion(eParamiteMotions::Motion_21_WebLeaveDown);

        if (BaseAliveGameObjectCollisionLine->field_8_type == eLineTypes::eUnknown_32 || BaseAliveGameObjectCollisionLine->field_8_type == eLineTypes::eUnknown_36)
        {
            PSX_RECT bRect = {};
            VGetBoundingRect(&bRect, 1);
            bRect.y += 5;
            bRect.h += 5;

            VOnCollisionWith(
                {bRect.x, bRect.y},
                {bRect.w, bRect.h},
                ObjListPlatforms_50766C,
                1,
                (TCollisionCallBack) &BaseAliveGameObject::OnTrapDoorIntersection_401C10);
        }
    }
    else
    {
        field_AC_ypos += field_B8_vely;
    }

    if (field_10_anim.field_92_current_frame == 0 || field_10_anim.field_92_current_frame == 3)
    {
        Sound(ParamiteSpeak::ClimbingWeb_6);
    }

    if (gMap.GetDirection(
            field_B2_lvl_number,
            field_B0_path_number,
            field_A8_xpos,
            field_AC_ypos)
        >= CameraPos::eCamCurrent_0)
    {
        MusicController::static_PlayMusic(MusicController::MusicTypes::eType11, this, 0, 0);
    }
}

void Paramite::Motion_21_WebLeaveDown()
{
    if (field_10_anim.field_92_current_frame == 2)
    {
        Environment_SFX_42A220(EnvironmentSfx::eHitGroundSoft_6, 50, 600, 0);
    }

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToIdle();
    }
}

void Paramite::Motion_22_Unknown()
{
    PSX_RECT abeRect = {};
    sActiveHero_507678->VGetBoundingRect(&abeRect, 1);

    PSX_RECT rect = {};
    VGetBoundingRect(&rect, 1);

    if (abeRect.x <= rect.w
        && abeRect.w >= rect.x
        && abeRect.h >= rect.y
        && abeRect.y <= rect.h
        && field_BC_sprite_scale == sActiveHero_507678->field_BC_sprite_scale)
    {
        sActiveHero_507678->VTakeDamage(this);
    }

    if (field_10_anim.mAnimFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        SetCurrentMotion(eParamiteMotions::Motion_14_PreHiss);
    }
}

void Paramite::Motion_23_Eating()
{
    if (field_10_anim.field_92_current_frame == 5)
    {
        SFX_Play_Mono(static_cast<SoundEffect>(Math_RandomRange_450F20(SoundEffect::Eating1_79, SoundEffect::Eating2_80)), 0, 0);
    }

    if (mNextMotion != -1 && !ToNextMotion())
    {
        ToIdle();
    }
}

void Paramite::Motion_24_Struggle()
{
    if (field_10_anim.field_92_current_frame == 0)
    {
        SFX_Play_Mono(SoundEffect::KillEffect_78, 0, 0);
    }
}

void Paramite::Motion_25_Death()
{
    if (field_10_anim.field_92_current_frame == 0)
    {
        Sound(ParamiteSpeak::DoIt_2);
    }
}

} // namespace AO
