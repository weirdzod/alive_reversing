#include "stdafx.h"
#include "Scrab.hpp"
#include "Function.hpp"
#include "Game.hpp"
#include "Map.hpp"
#include "stdlib.hpp"
#include "../relive_lib/Shadow.hpp"
#include "MusicController.hpp"
#include "Abe.hpp"
#include "../relive_lib/ObjectIds.hpp"
#include "PlatformBase.hpp"
#include "../relive_lib/Events.hpp"
#include "DDCheat.hpp"
#include "Particle.hpp"
#include "Sfx.hpp"
#include "Sound/Midi.hpp"
#include "GameSpeak.hpp"
#include "SwitchStates.hpp"
#include "SlamDoor.hpp"
#include "LiftPoint.hpp"
#include "Slurg.hpp"
#include "Grid.hpp"

static const TintEntry sScrabTints_560260[15] = {
    {EReliveLevelIds::eMines, 127u, 127u, 127u},
    {EReliveLevelIds::eNecrum, 137u, 137u, 137u},
    {EReliveLevelIds::eMudomoVault, 127u, 127u, 127u},
    {EReliveLevelIds::eMudancheeVault, 127u, 127u, 127u},
    {EReliveLevelIds::eFeeCoDepot, 127u, 127u, 127u},
    {EReliveLevelIds::eBarracks, 127u, 127u, 127u},
    {EReliveLevelIds::eMudancheeVault_Ender, 127u, 127u, 127u},
    {EReliveLevelIds::eBonewerkz, 127u, 127u, 127u},
    {EReliveLevelIds::eBrewery, 127u, 127u, 127u},
    {EReliveLevelIds::eBrewery_Ender, 127u, 127u, 127u},
    {EReliveLevelIds::eMudomoVault_Ender, 127u, 127u, 127u},
    {EReliveLevelIds::eFeeCoDepot_Ender, 127u, 127u, 127u},
    {EReliveLevelIds::eBarracks_Ender, 127u, 127u, 127u},
    {EReliveLevelIds::eBonewerkz_Ender, 127u, 127u, 127u},
    {EReliveLevelIds::eNone, 127u, 127u, 127u}};

// TODO: repetition with `MainMenu.cpp`
static const SfxDefinition scrab_sScrabSfx_560330[9] = {
    {0u, 4u, 60u, 55u, 0, 0},
    {0u, 4u, 61u, 70u, 0, 0},
    {0u, 4u, 62u, 80u, 0, 0},
    {0u, 4u, 63u, 80u, 0, 0},
    {0u, 4u, 64u, 60u, -127, 127},
    {0u, 4u, 66u, 90u, 0, 0},
    {0u, 4u, 67u, 50u, -511, 0},
    {0u, 4u, 67u, 50u, 0, 511},
    {0u, 4u, 68u, 110u, -1791, -1791},
};


static const TScrabMotionFn sScrab_motion_table_560120[40] = {
#define MAKE_FN(VAR) &Scrab::VAR,
    SCRAB_MOTIONS_ENUM_AE(MAKE_FN)
#undef MAKE_FN
};

static const TScrabBrainFn sScrabBrainTable[6] = {
    &Scrab::Brain_0_Patrol_4AA630,
    &Scrab::Brain_1_ChasingEnemy_4A6470,
    &Scrab::Brain_2_Fighting_4A5840,
    &Scrab::Brain_3_Death_4A62B0,
    &Scrab::Brain_4_ShrinkDeath_4A6420,
    &Scrab::Brain_5_Possessed_4A6180};

void Scrab::SetBrain(TScrabBrainFn fn)
{
    field_118_brain_state = fn;
}

bool Scrab::BrainIs(TScrabBrainFn fn)
{
    return field_118_brain_state == fn;
}

Scrab::Scrab(Path_Scrab* pTlv, s32 tlvInfo, ScrabSpawnDirection spawnDirection)
    : BaseAliveGameObject(14)
{
    field_190_unused = 0;
    field_17C_last_event = -1;
    field_192_unused = -1;

    SetType(ReliveTypes::eScrab);

    if (tlvInfo != 0xFFFF)
    {
        mBaseGameObjectTlvInfo = tlvInfo;
    }

    field_124_fight_target_obj_id = -1;
    field_120_obj_id = -1;

    field_10_resources_array.SetAt(0, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArsbasicResID, 1, 0));
    field_10_resources_array.SetAt(11, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArschewResID, 1, 0));
    field_10_resources_array.SetAt(6, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArsdanceResID, 1, 0));
    field_10_resources_array.SetAt(8, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArsdeadResID, 1, 0));
    field_10_resources_array.SetAt(1, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArseatResID, 1, 0));
    field_10_resources_array.SetAt(10, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArsgrwlResID, 1, 0));
    field_10_resources_array.SetAt(5, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArshowlResID, 1, 0));
    field_10_resources_array.SetAt(2, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArsprceResID, 1, 0));
    field_10_resources_array.SetAt(9, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArsroarResID, 1, 0));
    field_10_resources_array.SetAt(3, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArsskwrResID, 1, 0));
    field_10_resources_array.SetAt(4, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArswhirlResID, 1, 0));
    field_10_resources_array.SetAt(13, ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArscrshResID, 1, 0));

    const AnimRecord& rec = AnimRec(AnimId::Scrab_Idle);
    u8** ppRes = Add_Resource(ResourceManager::Resource_Animation, rec.mResourceId);
    Animation_Init(AnimId::Scrab_Idle, ppRes);

    field_140_motion_resource_block_index = 0;
    field_12C_timer = 0;
    mNextMotion = eScrabMotions::M_Stand_0_4A8220;
    BaseAliveGameObject_PlatformId = -1;
    mCurrentMotion = eScrabMotions::M_Stand_0_4A8220;
    field_11E_return_to_previous_motion = 0;

    field_1AA_flags.Clear(Flags_1AA::eBit1_attacking);
    field_1AA_flags.Clear(Flags_1AA::eBit4_force_update_animation);

    mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit3_Can_Be_Possessed);
    mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit6_SetOffExplosives);
    mBaseAliveGameObjectFlags.Clear(Flags_114::e114_Bit9_RestoredFromQuickSave);

    field_164_prevent_depossession = 0;
    field_16C_input = 0;
    field_170_unused = sGnFrame;
    field_1A2_speak_counter = 0;
    field_1A4_unused = 0;
    field_1A6_unused = 25;
    field_178_shred_power_active = 0;
    field_154_movement_timer = 0;

    mXPos = FP_FromInteger(pTlv->mTopLeft.x + 12);
    mYPos = FP_FromInteger(pTlv->mTopLeft.y);

    if (pTlv->field_10_scale == Scale_short::eHalf_1)
    {
        mSpriteScale = FP_FromDouble(0.5);
        mAnim.mRenderLayer = Layer::eLayer_8;
        mScale = Scale::Bg;
    }
    else if (pTlv->field_10_scale == Scale_short::eFull_0)
    {
        mSpriteScale = FP_FromInteger(1);
        mAnim.mRenderLayer = Layer::eLayer_27;
        mScale = Scale::Fg;
    }

    if (spawnDirection == ScrabSpawnDirection::eLeft_1)
    {
        mXPos -= ScaleToGridSize(mSpriteScale);
    }
    else if (spawnDirection == ScrabSpawnDirection::eRight_2)
    {
        mXPos += ScaleToGridSize(mSpriteScale);
    }

    field_128_attack_delay = pTlv->field_12_attack_delay;
    field_12A_patrol_type_run_or_walk_chance = pTlv->field_14_patrol_type_run_or_walk_chance;
    field_158_left_min_delay = pTlv->field_16_left_min_delay;
    field_15A_left_max_delay = pTlv->field_18_left_max_delay;
    field_15C_right_min_delay = pTlv->field_1A_right_min_delay;
    field_15E_right_max_delay = pTlv->field_1C_right_max_delay;
    field_148_pause_after_chase_delay = pTlv->field_1E_pause_after_chase_delay;
    field_174_possessed_max_whirl_attack_duration = pTlv->field_26_possessed_max_whirl_attack_duration;
    field_176_unused = pTlv->field_28_unused;
    field_1A8_bKill_enemy = pTlv->field_2A_bKill_enemy;

    field_1A0_speak_max = 3;

    field_1AA_flags.Set(Flags_1AA::eBit5_roar_randomly, pTlv->field_22_roar_randomly == Choice_short::eYes_1);
    field_1AA_flags.Set(Flags_1AA::eBit6_persistant, pTlv->field_24_persistant == Choice_short::eYes_1);
    field_1AA_flags.Clear(Flags_1AA::eBit3_unused);

    if (!OnFloor())
    {
        mCurrentMotion = eScrabMotions::M_JumpToFall_8_4A9220;
        vUpdateAnim();
        SetUpdateDelay(30);
    }

    SetTint(&sScrabTints_560260[0], gMap.mCurrentLevel);

    mVisualFlags.Set(VisualFlags::eDoPurpleLightEffect);

    field_14C_pause_after_chase_timer = 0;
    field_150_attack_delay_timer = 0;
    field_144_tlvInfo = tlvInfo;
    field_11C_brain_sub_state = 0;

    ToPatrol();

    mShadow = relive_new Shadow();
}

void Scrab::VOnTlvCollision(Path_TLV* pTlv)
{
    while (pTlv != nullptr)
    {
        if (pTlv->mTlvType32 == TlvTypes::DeathDrop_4)
        {
            Scrab_SFX(ScrabSounds::eYell_8, 127, -1000, 0);
            mBaseGameObjectFlags.Set(Options::eDead);
            mHealth = FP_FromInteger(0);
        }
        else if (pTlv->mTlvType32 == TlvTypes::EnemyStopper_47)
        {
            const auto enemyStopperPath = static_cast<Path_EnemyStopper*>(BaseAliveGameObjectPathTLV); //TODO it should probably be pTlv, instead - OG bug?
            const Path_EnemyStopper::StopDirection stopDirection = enemyStopperPath->mStopDirection;
            if ((stopDirection == Path_EnemyStopper::StopDirection::Left_0 && mXPos < field_198_max_xpos) || (stopDirection == Path_EnemyStopper::StopDirection::Right_1 && mXPos > field_198_max_xpos) || stopDirection == Path_EnemyStopper::StopDirection::Both_2)
            {
                if (SwitchStates_Get(enemyStopperPath->mSwitchId))
                {
                    if (sControlledCharacter != this)
                    {
                        mXPos = field_198_max_xpos;
                    }
                }
            }
        }
        pTlv = sPathInfo->TlvGetAt(
            pTlv,
            mXPos,
            mYPos,
            mXPos,
            mYPos);
    }
}

const AnimId sScrabMotionAnimIds[40] = {
    AnimId::Scrab_Idle,
    AnimId::Scrab_Walk,
    AnimId::Scrab_Run,
    AnimId::Scrab_Turn,
    AnimId::Scrab_RunToStand,
    AnimId::Scrab_HopBegin,
    AnimId::Scrab_Jump,
    AnimId::Scrab_Landing,
    AnimId::Scrab_JumpAndRunToFall,
    AnimId::Scrab_StandToWalk,
    AnimId::Scrab_StandToRun,
    AnimId::Scrab_WalkToStand,
    AnimId::Scrab_Jump,
    AnimId::Scrab_Landing,
    AnimId::Scrab_WalkToFall,
    AnimId::Scrab_JumpAndRunToFall,
    AnimId::Scrab_WalkToRun,
    AnimId::Scrab_RunToWalk,
    AnimId::Scrab_Knockback,
    AnimId::Scrab_GetEaten,
    AnimId::Scrab_Landing,
    AnimId::Scrab_Stamp,
    AnimId::Scrab_Stamp,
    AnimId::Scrab_DeathEnd,
    AnimId::Scrab_DeathEnd,
    AnimId::Scrab_Empty,
    AnimId::Scrab_HowlBegin,
    AnimId::Scrab_HowlEnd,
    AnimId::Scrab_HowlBegin,
    AnimId::Scrab_HowlEnd,
    AnimId::Scrab_Shriek,
    AnimId::Scrab_ScrabBattleAnim,
    AnimId::Scrab_AttackSpin,
    AnimId::Scrab_FeedToGulp,
    AnimId::Scrab_GulpToStand,
    AnimId::Scrab_StandToFeed,
    AnimId::Scrab_Feed,
    AnimId::Scrab_AttackLunge,
    AnimId::Scrab_LegKick,
    AnimId::Scrab_DeathBegin};

s32 Scrab::CreateFromSaveState(const u8* pBuffer)
{
    auto pState = reinterpret_cast<const Scrab_State*>(pBuffer);

    auto pTlv = static_cast<Path_Scrab*>(sPathInfo->TLV_From_Offset_Lvl_Cam(pState->field_44_tlvInfo));

    if (!ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AEResourceID::kArsbasicResID, FALSE, FALSE))
    {
        ResourceManager::LoadResourceFile_49C170("SCRAB.BND", nullptr);
    }

    auto pScrab = relive_new Scrab(pTlv, pState->field_44_tlvInfo, ScrabSpawnDirection::eNone_0);
    if (pScrab)
    {
        pScrab->mBaseGameObjectTlvInfo = pState->field_4_obj_id;

        if (pState->field_40_bIsControlled)
        {
            sControlledCharacter = pScrab;
        }

        pScrab->BaseAliveGameObjectPathTLV = nullptr;
        pScrab->BaseAliveGameObjectCollisionLine = nullptr;

        pScrab->mXPos = pState->field_8_xpos;
        pScrab->mYPos = pState->field_C_ypos;
        pScrab->mVelX = pState->field_10_velx;
        pScrab->mVelY = pState->field_14_vely;

        pScrab->field_134_falling_velx_scale_factor = pState->field_64_falling_velx_scale_factor;
        pScrab->mCurrentPath = pState->field_18_path_number;
        pScrab->mCurrentLevel = MapWrapper::FromAESaveData(pState->field_1A_lvl_number);
        pScrab->mSpriteScale = pState->field_1C_sprite_scale;
        pScrab->mRGB.SetRGB(pState->mRingRed, pState->mRingGreen, pState->mRingBlue);
        pScrab->mCurrentMotion = pState->field_28_current_motion;

        u8** ppRes = pScrab->ResBlockForMotion(pState->field_28_current_motion);
        pScrab->mAnim.Set_Animation_Data(sScrabMotionAnimIds[pState->field_28_current_motion], ppRes);

        pScrab->mAnim.mCurrentFrame = pState->field_2A_current_frame;
        pScrab->mAnim.mFrameChangeCounter = pState->field_2C_frame_change_counter;

        pScrab->mBaseGameObjectFlags.Set(BaseGameObject::eDrawable_Bit4, pState->field_2F_bDrawable & 1);

        pScrab->mAnim.mFlags.Set(AnimFlags::eBit5_FlipX, pState->field_26_bAnimFlipX & 1);
        pScrab->mAnim.mFlags.Set(AnimFlags::eBit3_Render, pState->field_2E_bAnimRender & 1);

        if (IsLastFrame(&pScrab->mAnim))
        {
            pScrab->mAnim.mFlags.Set(AnimFlags::eBit18_IsLastFrame);
        }

        pScrab->mHealth = pState->field_30_health;
        pScrab->mCurrentMotion = pState->field_34_current_motion;
        pScrab->mNextMotion = pState->field_36_next_motion;
        pScrab->BaseAliveGameObjectLastLineYPos = FP_FromInteger(pState->field_38_last_line_ypos);
        pScrab->field_130_depossession_timer = pState->field_60_depossession_timer;
        pScrab->mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit9_RestoredFromQuickSave);
        pScrab->field_12C_timer = pState->field_5C_timer;
        pScrab->BaseAliveGameObjectCollisionLineType = pState->field_3A_line_type;
        pScrab->field_144_tlvInfo = pState->field_44_tlvInfo;

        pScrab->SetBrain(sScrabBrainTable[pState->field_48_brain_idx]);

        pScrab->field_11C_brain_sub_state = pState->field_50_sub_state;
        pScrab->field_120_obj_id = pState->field_54_obj_id;
        pScrab->field_124_fight_target_obj_id = pState->field_58_target_obj_id;

        pScrab->field_140_motion_resource_block_index = pState->field_68_motion_resource_block_index;
        pScrab->field_14C_pause_after_chase_timer = pState->field_6C_spotting_abe_timer;
        pScrab->field_150_attack_delay_timer = pState->field_70_attack_delay_timer;
        pScrab->field_154_movement_timer = pState->field_74_movement_timer;
        pScrab->field_160_sfx_bitmask = pState->field_78_sfx_bitmask;
        pScrab->field_164_prevent_depossession = pState->field_7C_prevent_depossession;

        pScrab->field_166_return_level = MapWrapper::FromAESaveData(pState->field_7E_return_level);
        pScrab->field_168_return_path = pState->field_80_return_path;
        pScrab->field_16A_return_camera = pState->field_82_return_camera;
        pScrab->field_16C_input = InputObject::PsxButtonsToKeyboardInput_45EE40(pState->field_84_input);
        pScrab->field_170_unused = pState->field_88_unused;
        pScrab->field_178_shred_power_active = pState->field_8C_shred_power_active;
        pScrab->field_194_speak = pState->field_8E_speak;
        pScrab->field_198_max_xpos = pState->field_90_max_xpos;
        pScrab->field_19C_max_ypos = pState->field_94_max_ypos;
        pScrab->field_1A2_speak_counter = pState->field_98_speak_counter;
        pScrab->field_1A4_unused = pState->field_9A_unused;
        pScrab->field_1A6_unused = pState->field_9C_unused;

        pScrab->field_1AA_flags.Set(Flags_1AA::eBit1_attacking, pState->field_9E_flags.Get(Scrab_State::eBit1_attacking));
        pScrab->field_1AA_flags.Set(Flags_1AA::eBit2_unused, pState->field_9E_flags.Get(Scrab_State::eBit2_unused));
        pScrab->field_1AA_flags.Set(Flags_1AA::eBit3_unused, pState->field_9E_flags.Get(Scrab_State::eBit3_unused));
        pScrab->field_1AA_flags.Set(Flags_1AA::eBit4_force_update_animation, pState->field_9E_flags.Get(Scrab_State::eBit4_force_update_animation));
        pScrab->field_1AA_flags.Set(Flags_1AA::eBit5_roar_randomly, pState->field_9E_flags.Get(Scrab_State::eBit5_roar_randomly));
        pScrab->field_1AA_flags.Set(Flags_1AA::eBit6_persistant, pState->field_9E_flags.Get(Scrab_State::eBit6_persistant));
    }

    return sizeof(Scrab_State);
}

s32 Scrab::VGetSaveState(u8* pSaveBuffer)
{
    if (mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit7_Electrocuted))
    {
        return 0;
    }

    auto pState = reinterpret_cast<Scrab_State*>(pSaveBuffer);

    pState->field_0_type = AETypes::eScrab_112;
    pState->field_4_obj_id = mBaseGameObjectTlvInfo;

    pState->field_8_xpos = mXPos;
    pState->field_C_ypos = mYPos;
    pState->field_10_velx = mVelX;
    pState->field_14_vely = mVelY;

    pState->field_64_falling_velx_scale_factor = field_134_falling_velx_scale_factor;

    pState->field_18_path_number = mCurrentPath;
    pState->field_1A_lvl_number = MapWrapper::ToAE(mCurrentLevel);
    pState->field_1C_sprite_scale = mSpriteScale;

    pState->mRingRed = mRGB.r;
    pState->mRingGreen = mRGB.g;
    pState->mRingBlue = mRGB.b;

    pState->field_26_bAnimFlipX = mAnim.mFlags.Get(AnimFlags::eBit5_FlipX);
    pState->field_28_current_motion = mCurrentMotion;
    pState->field_2A_current_frame = mAnim.mCurrentFrame;
    pState->field_2C_frame_change_counter = mAnim.mFrameChangeCounter;
    pState->field_2F_bDrawable = mBaseGameObjectFlags.Get(BaseGameObject::eDrawable_Bit4);
    pState->field_2E_bAnimRender = mAnim.mFlags.Get(AnimFlags::eBit3_Render);
    pState->field_30_health = mHealth;
    pState->field_34_current_motion = mCurrentMotion;
    pState->field_36_next_motion = mNextMotion;
    pState->field_3A_line_type = -1;

    // TODO: Check correctness
    pState->field_38_last_line_ypos = FP_GetExponent(BaseAliveGameObjectLastLineYPos);
    if (BaseAliveGameObjectCollisionLine)
    {
        pState->field_3A_line_type = BaseAliveGameObjectCollisionLine->mLineType;
    }

    pState->field_40_bIsControlled = (this == sControlledCharacter);
    pState->field_60_depossession_timer = field_130_depossession_timer;
    pState->field_5C_timer = field_12C_timer;
    pState->field_44_tlvInfo = field_144_tlvInfo;
    pState->field_48_brain_idx = 0;

    s32 idx = 0;
    for (const auto& fn : sScrabBrainTable)
    {
        if (BrainIs(fn))
        {
            pState->field_48_brain_idx = idx;
        }
        idx++;
    }

    pState->field_54_obj_id = -1;
    pState->field_50_sub_state = field_11C_brain_sub_state;

    if (field_120_obj_id != -1)
    {
        BaseGameObject* pObj = sObjectIds.Find_Impl(field_120_obj_id);
        if (pObj)
        {
            pState->field_54_obj_id = pObj->mBaseGameObjectTlvInfo;
        }
    }

    pState->field_58_target_obj_id = -1;
    if (field_124_fight_target_obj_id != -1)
    {
        BaseGameObject* pObj = sObjectIds.Find_Impl(field_124_fight_target_obj_id);
        if (pObj)
        {
            pState->field_58_target_obj_id = pObj->mBaseGameObjectTlvInfo;
        }
    }

    pState->field_68_motion_resource_block_index = field_140_motion_resource_block_index;
    pState->field_6C_spotting_abe_timer = field_14C_pause_after_chase_timer;
    pState->field_70_attack_delay_timer = field_150_attack_delay_timer;
    pState->field_74_movement_timer = field_154_movement_timer;
    pState->field_78_sfx_bitmask = field_160_sfx_bitmask;
    pState->field_7C_prevent_depossession = field_164_prevent_depossession;
    pState->field_7E_return_level = MapWrapper::ToAE(field_166_return_level);
    pState->field_80_return_path = field_168_return_path;
    pState->field_82_return_camera = field_16A_return_camera;
    pState->field_84_input = InputObject::KeyboardInputToPsxButtons_45EF70(field_16C_input);
    pState->field_88_unused = field_170_unused;
    pState->field_8C_shred_power_active = field_178_shred_power_active;
    pState->field_8E_speak = field_194_speak;
    pState->field_90_max_xpos = field_198_max_xpos;
    pState->field_94_max_ypos = field_19C_max_ypos;
    pState->field_98_speak_counter = field_1A2_speak_counter;
    pState->field_9A_unused = field_1A4_unused;
    pState->field_9C_unused = field_1A6_unused;

    pState->field_9E_flags.Set(Scrab_State::eBit1_attacking, field_1AA_flags.Get(Flags_1AA::eBit1_attacking));
    pState->field_9E_flags.Set(Scrab_State::eBit2_unused, field_1AA_flags.Get(Flags_1AA::eBit2_unused));
    pState->field_9E_flags.Set(Scrab_State::eBit3_unused, field_1AA_flags.Get(Flags_1AA::eBit3_unused));
    pState->field_9E_flags.Set(Scrab_State::eBit4_force_update_animation, field_1AA_flags.Get(Flags_1AA::eBit4_force_update_animation));
    pState->field_9E_flags.Set(Scrab_State::eBit5_roar_randomly, field_1AA_flags.Get(Flags_1AA::eBit5_roar_randomly));
    pState->field_9E_flags.Set(Scrab_State::eBit6_persistant, field_1AA_flags.Get(Flags_1AA::eBit6_persistant));

    return sizeof(Scrab_State);
}

Scrab::~Scrab()
{
    field_120_obj_id = -1;
    field_124_fight_target_obj_id = -1;

    VOnTrapDoorOpen();

    if (mHealth <= FP_FromInteger(0))
    {
        Path::TLV_Reset(field_144_tlvInfo, -1, 0, 1);
    }
    else
    {
        Path::TLV_Reset(field_144_tlvInfo, -1, 0, 0);
    }

    MusicController::static_PlayMusic(MusicController::MusicTypes::eNone_0, this, 0, 0);

    if (sControlledCharacter == this)
    {
        sControlledCharacter = sActiveHero;
        if (gMap.mNextLevel != EReliveLevelIds::eMenu)
        {
            gMap.SetActiveCam(
                field_166_return_level,
                field_168_return_path,
                field_16A_return_camera,
                CameraSwapEffects::eInstantChange_0,
                0,
                0);
        }
    }
}

void Scrab::VOnTrapDoorOpen()
{
    auto pPlatform = static_cast<PlatformBase*>(sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId));
    if (pPlatform)
    {
        pPlatform->VRemove(this);
        field_1AA_flags.Set(Flags_1AA::eBit4_force_update_animation);
        BaseAliveGameObject_PlatformId = -1;
        mCurrentMotion = eScrabMotions::M_RunToFall_15_4A9430;
    }
}

void Scrab::vUpdateAnim()
{
    mAnim.Set_Animation_Data(sScrabMotionAnimIds[mCurrentMotion], ResBlockForMotion(mCurrentMotion));
}

s16 Scrab::OnFloor()
{
    FP hitX = {};
    FP hitY = {};
    if (sCollisions->Raycast(
            mXPos,
            mYPos,
            mXPos,
            mYPos + FP_FromInteger(30),
            &BaseAliveGameObjectCollisionLine,
            &hitX,
            &hitY,
            mScale == Scale::Fg ? kFgFloor : kBgFloor))
    {
        mYPos = hitY;
        ToStand();
        if (BaseAliveGameObjectCollisionLine->mLineType == eLineTypes::eDynamicCollision_32 ||
            BaseAliveGameObjectCollisionLine->mLineType == eLineTypes::eBackgroundDynamicCollision_36)
        {
            PlatformCollide();
        }
        return TRUE;
    }
    return FALSE;
}

const FP velx_input_entries_546D84[8] = {
    FP_FromInteger(4),
    FP_FromInteger(4),
    FP_FromInteger(0),
    FP_FromInteger(-4),
    FP_FromInteger(-4),
    FP_FromInteger(-4),
    FP_FromInteger(0),
    FP_FromInteger(4)};

const FP vely_input_entries_546DA4[11] = {
    FP_FromInteger(0),
    FP_FromInteger(-4),
    FP_FromInteger(-4),
    FP_FromInteger(-4),
    FP_FromInteger(0),
    FP_FromInteger(4),
    FP_FromInteger(4),
    FP_FromInteger(4),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0)};


void Scrab::VUpdate()
{
    if (mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit9_RestoredFromQuickSave))
    {
        mBaseAliveGameObjectFlags.Clear(Flags_114::e114_Bit9_RestoredFromQuickSave);
        if (BaseAliveGameObjectCollisionLineType == -1)
        {
            BaseAliveGameObjectCollisionLine = nullptr;
        }
        else
        {
            sCollisions->Raycast(
                mXPos,
                mYPos - FP_FromInteger(20),
                mXPos,
                mYPos + FP_FromInteger(20),
                &BaseAliveGameObjectCollisionLine,
                &mXPos,
                &mYPos,
                CollisionMask(static_cast<eLineTypes>(BaseAliveGameObjectCollisionLineType)));
        }
        BaseAliveGameObjectCollisionLineType = 0;
        field_120_obj_id = BaseGameObject::RefreshId(field_120_obj_id);
        field_124_fight_target_obj_id = BaseGameObject::RefreshId(field_124_fight_target_obj_id);
    }

    if (EventGet(kEventDeathReset))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        return;
    }

    const FP xDelta = FP_Abs(mXPos - sControlledCharacter->mXPos);
    if (xDelta <= FP_FromInteger(750))
    {
        const FP yDelta = FP_Abs(mYPos - sControlledCharacter->mYPos);
        if (yDelta <= FP_FromInteger(520))
        {
            if (mHealth > FP_FromInteger(0))
            {
                mAnim.mFlags.Set(AnimFlags::eBit2_Animate);
                mAnim.mFlags.Set(AnimFlags::eBit3_Render);
            }

            if (!Input_IsChanting_45F260())
            {
                field_164_prevent_depossession = 0;
            }

            if (sDDCheat_FlyingEnabled_5C2C08 && sControlledCharacter == this)
            {
                // Handle DDCheat mode
                BaseAliveGameObjectCollisionLine = nullptr;

                if (Input().isPressed(InputCommands::Enum::eUp | InputCommands::Enum::eDown | InputCommands::Enum::eLeft | InputCommands::Enum::eRight))
                {
                    // TODO: InputCommand constants
                    mVelX = velx_input_entries_546D84[Input().mPads[sCurrentControllerIndex].mDir >> 5];
                    mVelY = vely_input_entries_546DA4[Input().mPads[sCurrentControllerIndex].mDir >> 5];

                    if (Input().isPressed(InputCommands::Enum::eRun))
                    {
                        mVelX += velx_input_entries_546D84[Input().mPads[sCurrentControllerIndex].mDir >> 5];
                        mVelX += velx_input_entries_546D84[Input().mPads[sCurrentControllerIndex].mDir >> 5];
                        mVelY += vely_input_entries_546DA4[Input().mPads[sCurrentControllerIndex].mDir >> 5];
                    }

                    mXPos += mVelX;
                    mYPos += mVelY;

                    // Keep in map bounds
                    PSX_Point point = {};
                    gMap.Get_map_size(&point);
                    if (mXPos < FP_FromInteger(0))
                    {
                        mXPos = FP_FromInteger(0);
                    }

                    if (mXPos >= FP_FromInteger(point.x))
                    {
                        mXPos = FP_FromInteger(point.x) - FP_FromInteger(1);
                    }

                    if (mYPos < FP_FromInteger(0))
                    {
                        mYPos = FP_FromInteger(0);
                    }

                    if (mYPos >= FP_FromInteger(point.y))
                    {
                        mYPos = FP_FromInteger(point.y) - FP_FromInteger(1);
                    }
                }
                else
                {
                    mVelX = FP_FromInteger(0);
                    mVelY = FP_FromInteger(0);
                }

                SetActiveCameraDelayedFromDir();
                BaseAliveGameObjectLastLineYPos = mYPos;
                return;
            }

            if (!gMap.Is_Point_In_Current_Camera(
                    mCurrentLevel,
                    mCurrentPath,
                    mXPos,
                    mYPos,
                    1))
            {
                field_1A2_speak_counter = 0;
            }

            const auto oldMotion = mCurrentMotion;

            field_11C_brain_sub_state = (this->*field_118_brain_state)();

            if (sDDCheat_ShowAI_Info_5C1BD8)
            {
                DDCheat::DebugStr(
                    "Scrab %d %d %d %d\n",
                    field_11C_brain_sub_state,
                    field_12C_timer,
                    mCurrentMotion,
                    mNextMotion);
            }

            field_19C_max_ypos = mYPos;
            field_198_max_xpos = mXPos;

            (this->*sScrab_motion_table_560120[mCurrentMotion])();

            if (field_198_max_xpos != mXPos || field_19C_max_ypos != mYPos)
            {
                BaseAliveGameObjectPathTLV = sPathInfo->TlvGetAt(
                    nullptr,
                    mXPos,
                    mYPos,
                    mXPos,
                    mYPos);
                VOnTlvCollision(BaseAliveGameObjectPathTLV);
            }

            if (oldMotion != mCurrentMotion || field_1AA_flags.Get(Flags_1AA::eBit4_force_update_animation))
            {
                field_1AA_flags.Clear(Flags_1AA::eBit4_force_update_animation);
                vUpdateAnim();
            }
            else if (field_11E_return_to_previous_motion)
            {
                // NOTE: This never gets set to true, so this branch will never be executed.
                mCurrentMotion = mPreviousMotion;
                vUpdateAnim();
                mAnim.SetFrame(mBaseAliveGameObjectLastAnimFrame);
                field_11E_return_to_previous_motion = 0;
            }

            Update_Slurg_Step_Watch_Points();

            if (field_178_shred_power_active)
            {
                if (sControlledCharacter != this)
                {
                    field_13C_last_ypos = mYPos;
                    return;
                }

                if (mHealth > FP_FromInteger(0) && !(static_cast<s32>(sGnFrame) % 4))
                {
                    // Draw green particle
                    s16 particleXRand = Math_RandomRange(40, 50);
                    const s16 particleYRand = Math_RandomRange(45, 55);
                    FP particleVelX = (FP_FromInteger(-7) * mSpriteScale);
                    FP particleVelY = (FP_FromDouble(0.3) * mSpriteScale);

                    if (Math_NextRandom() & 1)
                    {
                        particleVelX = -particleVelX;
                        particleXRand = -particleXRand;
                    }

                    if (Math_NextRandom() & 1)
                    {
                        particleVelY = -particleVelY;
                    }

                    const FP particleYPos = mYPos - (mSpriteScale * FP_FromInteger(particleYRand));
                    const FP particleXPos = mXPos + (mSpriteScale * FP_FromInteger(particleXRand));

                    New_Orb_Particle(
                        particleXPos,
                        particleYPos,
                        particleVelX,
                        particleVelY,
                        mSpriteScale,
                        Layer::eLayer_0,
                        255,
                        0,
                        0);
                }
            }

            if (sControlledCharacter == this && BaseAliveGameObject_PlatformId != -1)
            {
                mVelY = mYPos - field_13C_last_ypos;
                SetActiveCameraDelayedFromDir();
            }
            field_13C_last_ypos = mYPos;
            return;
        }
    }

    if (field_1AA_flags.Get(Flags_1AA::eBit6_persistant))
    {
        mAnim.mFlags.Clear(AnimFlags::eBit2_Animate);
        mAnim.mFlags.Clear(AnimFlags::eBit3_Render);
    }
    else
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

void Scrab::Update_Slurg_Step_Watch_Points()
{
    if (mCurrentMotion == eScrabMotions::M_Walk_1_4A84D0 || mCurrentMotion == eScrabMotions::M_Run_2_4A89C0 || mCurrentMotion == eScrabMotions::M_Stamp_21_4A9CC0 || mCurrentMotion == eScrabMotions::M_AttackSpin_32_4A8DC0)
    {
        if (sGnFrame & 1)
        {
            const s8 count = sSlurg_Step_Watch_Points_Count_5BD4DC[sSlurg_Step_Watch_Points_Idx_5C1C08];
            if (count < 5)
            {
                Slurg_Step_Watch_Points* pPoints = &sSlurg_Step_Watch_Points_5C1B28[sSlurg_Step_Watch_Points_Idx_5C1C08];
                pPoints->field_0_points[count].field_0_xPos = FP_GetExponent(mXPos);
                pPoints->field_0_points[count].field_2_yPos = BaseAliveGameObjectCollisionLine->mRect.y - 5;
                sSlurg_Step_Watch_Points_Count_5BD4DC[sSlurg_Step_Watch_Points_Idx_5C1C08] = count + 1;
            }
        }
    }
}

enum Brain_0_Patrol
{
    eBrain0_ToMoving_0 = 0,
    eBrain0_Moving_1 = 1,
    eBrain0_Turning_2 = 2,
    eBrain0_Idle_3 = 3,
    eBrain0_Howling_4 = 4,
    eBrain0_Shriek_5 = 5,
    eBrain0_OnLift_6 = 6,
    eBrain0_KickFleech_7 = 7,
    eBrain0_ToSpeak_8 = 8,
    eBrain0_UsingInvisibility_9 = 9,
    eBrain0_PostInvisibilitySmash_10 = 10,
    eBrain0_BeingSpawned_11 = 11,
};

enum Brain_1_ChasingEnemy
{
    eBrain1_Inactive_0 = 0,
    eBrain1_Idle_1 = 1,
    eBrain1_Running_2 = 2,
    eBrain1_Turning_3 = 3,
    eBrain1_Panic_4 = 4,
    eBrain1_Falling_5 = 5,
    eBrain1_ToIdle_6 = 6,
    eBrain1_Jumping_7 = 7,
    eBrain1_Attacking_8 = 8,
    eBrain1_Walking_9 = 9,
    eBrain1_EnemyDead_10 = 10,
    eBrain1_SmashingEnemy_11 = 11,
    eBrain1_Eating_12 = 12,
    eBrain1_Howl_13 = 13,
    eBrain1_Shriek_14 = 14,
    eBrain1_PreparingToHowlOrShriek_15 = 15,
    eBrain1_KilledPossessedScrab_16 = 16
};

enum Brain_2_Fighting
{
    eBrain2_LookingForOpponent_0 = 0,
    eBrain2_SpottedOpponent_1 = 1,
    eBrain2_Turning_2 = 2,
    eBrain2_Walking_3 = 3,
    eBrain2_SetInPosition_4 = 4,
    eBrain2_Idle_5 = 5,
    eBrain2_Yelling_6 = 6,
    eBrain2_Shriek_7 = 7,
    eBrain2_ToRun_8 = 8,
    eBrain2_Running_9 = 9,
    eBrain2_Battling_10 = 10,
    eBrain2_Victorious_11 = 11,
    eBrain2_SmashingOpponent_12 = 12,
    eBrain2_VictoryYell_13 = 13,
    eBrain2_InterruptVictoryStates_14 = 14,
    eBrain2_WaitingForBattle_15 = 15
};

static const s32 kPatrolTypeRunOrWalkChance_546FD8[6] = {256, 192, 128, 64, 0, 0};

s16 Scrab::Brain_0_Patrol_4AA630()
{
    Scrab* pFighter = FindScrabToFight();
    if (pFighter)
    {
        SetBrain(&Scrab::Brain_2_Fighting_4A5840);
        field_124_fight_target_obj_id = pFighter->mBaseGameObjectId;
        mNextMotion = eScrabMotions::M_Stand_0_4A8220;
        return Brain_2_Fighting::eBrain2_LookingForOpponent_0;
    }

    if (FindAbeOrMud())
    {
        auto pOtherScrab = static_cast<BaseAliveGameObject*>(sObjectIds.Find_Impl(field_120_obj_id));
        SetBrain(&Scrab::Brain_1_ChasingEnemy_4A6470);
        if (pOtherScrab->Type() == ReliveTypes::eScrab && pOtherScrab->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit4_bPossesed))
        {
            mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
            field_150_attack_delay_timer = sGnFrame + 90;
            return Brain_1_ChasingEnemy::eBrain1_Idle_1;
        }

        mNextMotion = eScrabMotions::M_Stand_0_4A8220;
        return Brain_1_ChasingEnemy::eBrain1_Inactive_0;
    }

    if (IsEventInRange(kEventAbeOhm, mXPos, mYPos, EventScale::Both) && !sActiveHero->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible))
    {
        mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
        return Brain_0_Patrol::eBrain0_Howling_4;
    }

    auto pSwitch = static_cast<PlatformBase*>(sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId));
    if (pSwitch && pSwitch->Type() == ReliveTypes::eLiftPoint && !(static_cast<LiftPoint*>(pSwitch)->vOnAnyFloor() || field_11C_brain_sub_state != Brain_0_Patrol::eBrain0_OnLift_6))
    {
        mNextMotion = eScrabMotions::M_Stand_0_4A8220;
        return Brain_0_Patrol::eBrain0_OnLift_6;
    }

    if (gMap.GetDirection(mCurrentLevel, mCurrentPath, mXPos, mYPos) >= CameraPos::eCamCurrent_0)
    {
        MusicController::static_PlayMusic(MusicController::MusicTypes::eTension_4, this, 0, 0);
    }

    if (mCurrentMotion == eScrabMotions::M_JumpToFall_8_4A9220 && field_11C_brain_sub_state != Brain_0_Patrol::eBrain0_BeingSpawned_11)
    {
        field_11C_brain_sub_state = Brain_0_Patrol::eBrain0_BeingSpawned_11;
    }

    switch (field_11C_brain_sub_state)
    {
        case Brain_0_Patrol::eBrain0_ToMoving_0:
            if (mCurrentMotion == eScrabMotions::M_Stand_0_4A8220)
            {
                field_194_speak = LastSpeak();
                if (field_1A2_speak_counter < field_1A0_speak_max && (field_194_speak == GameSpeakEvents::Scrab_Howl_53 || field_194_speak == GameSpeakEvents::Scrab_Shriek_54))
                {
                    return Brain_0_Patrol::eBrain0_ToSpeak_8;
                }
            }

            if (Check_IsOnEndOfLine(
                    mAnim.mFlags.Get(AnimFlags::eBit5_FlipX),
                    mCurrentMotion != eScrabMotions::M_Run_2_4A89C0 ? 1 : 3)
                || Handle_SlamDoor_or_EnemyStopper(mVelX, 1))
            {
                mNextMotion = eScrabMotions::M_Turn_3_4A91A0;
                return Brain_0_Patrol::eBrain0_Turning_2;
            }

            if (Math_NextRandom() >= kPatrolTypeRunOrWalkChance_546FD8[field_12A_patrol_type_run_or_walk_chance])
            {
                mNextMotion = eScrabMotions::M_Run_2_4A89C0;
            }
            else
            {
                mNextMotion = eScrabMotions::M_Walk_1_4A84D0;
            }
            return Brain_0_Patrol::eBrain0_Moving_1;

        case Brain_0_Patrol::eBrain0_Moving_1:
            field_194_speak = Scrab::LastSpeak();
            if (field_1A2_speak_counter >= field_1A0_speak_max || (field_194_speak != GameSpeakEvents::Scrab_Howl_53 && field_194_speak != GameSpeakEvents::Scrab_Shriek_54))
            {
                if (Find_Fleech())
                {
                    mNextMotion = eScrabMotions::M_LegKick_38_4AA120;
                    return Brain_0_Patrol::eBrain0_KickFleech_7;
                }

                if (Check_IsOnEndOfLine(
                        mAnim.mFlags.Get(AnimFlags::eBit5_FlipX),
                        mCurrentMotion != eScrabMotions::M_Run_2_4A89C0 ? 1 : 3)
                    || // TODO: check order is correct
                    Handle_SlamDoor_or_EnemyStopper(mVelX, 1))
                {
                    mNextMotion = eScrabMotions::M_Turn_3_4A91A0;
                    return Brain_0_Patrol::eBrain0_Turning_2;
                }

                if (Math_NextRandom() >= 3u || static_cast<s32>(sGnFrame - field_154_movement_timer) <= 150)
                {
                    return field_11C_brain_sub_state;
                }

                mNextMotion = eScrabMotions::M_Shriek_30_4A9EA0;
                field_154_movement_timer = sGnFrame;
                return Brain_0_Patrol::eBrain0_Shriek_5;
            }
            else
            {
                mNextMotion = eScrabMotions::M_Stand_0_4A8220;
                return Brain_0_Patrol::eBrain0_ToSpeak_8;
            }
            break;

        case Brain_0_Patrol::eBrain0_Turning_2:
        {
            if (mCurrentMotion != eScrabMotions::M_Turn_3_4A91A0 || !mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }

            mNextMotion = eScrabMotions::M_Stand_0_4A8220;

            s16 maxTimer = 0;
            s16 minTimer = 0;
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                maxTimer = field_15A_left_max_delay;
                minTimer = field_158_left_min_delay;
            }
            else
            {
                maxTimer = field_15E_right_max_delay;
                minTimer = field_15C_right_min_delay;
            }
            field_12C_timer = sGnFrame + Math_RandomRange(minTimer, maxTimer);
            return Brain_0_Patrol::eBrain0_Idle_3;
        }

        case Brain_0_Patrol::eBrain0_Idle_3:
            field_194_speak = LastSpeak();
            if (field_1A2_speak_counter >= field_1A0_speak_max || (field_194_speak != GameSpeakEvents::Scrab_Howl_53 && field_194_speak != GameSpeakEvents::Scrab_Shriek_54))
            {
                if (Find_Fleech())
                {
                    mNextMotion = eScrabMotions::M_LegKick_38_4AA120;
                    return Brain_0_Patrol::eBrain0_KickFleech_7;
                }
                else
                {
                    if (static_cast<s32>(sGnFrame) <= field_12C_timer)
                    {
                        return field_11C_brain_sub_state;
                    }

                    if (Math_NextRandom() >= 30u || static_cast<s32>(sGnFrame - field_154_movement_timer) <= 150)
                    {
                        return Brain_0_Patrol::eBrain0_ToMoving_0;
                    }
                    mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
                    field_154_movement_timer = sGnFrame;
                    return Brain_0_Patrol::eBrain0_Howling_4;
                }
            }
            else
            {
                mNextMotion = eScrabMotions::M_Stand_0_4A8220;
                return Brain_0_Patrol::eBrain0_ToSpeak_8;
            }
            break;

        case Brain_0_Patrol::eBrain0_Howling_4:
            if (IsEventInRange(kEventAbeOhm, mXPos, mYPos, EventScale::Both))
            {
                return field_11C_brain_sub_state;
            }
            if (mCurrentMotion == eScrabMotions::M_HowlBegin_26_4A9DA0 && mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                mNextMotion = eScrabMotions::M_Stand_0_4A8220;
                return Brain_0_Patrol::eBrain0_ToMoving_0;
            }
            return field_11C_brain_sub_state;

        case Brain_0_Patrol::eBrain0_Shriek_5:
            if (mCurrentMotion == eScrabMotions::M_Shriek_30_4A9EA0 && mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                mNextMotion = eScrabMotions::M_Stand_0_4A8220;
                return Brain_0_Patrol::eBrain0_ToMoving_0;
            }
            return field_11C_brain_sub_state;

        case Brain_0_Patrol::eBrain0_OnLift_6:
            if (pSwitch)
            {
                if (!static_cast<LiftPoint*>(pSwitch)->vOnAnyFloor())
                {
                    return field_11C_brain_sub_state;
                }
                return Brain_0_Patrol::eBrain0_ToMoving_0;
            }
            else
            {
                BaseAliveGameObject_PlatformId = -1;
                return Brain_0_Patrol::eBrain0_ToMoving_0;
            }
            break;

        case Brain_0_Patrol::eBrain0_KickFleech_7:
            if (mCurrentMotion == eScrabMotions::M_LegKick_38_4AA120 && mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                mNextMotion = eScrabMotions::M_Stand_0_4A8220;
                return Brain_0_Patrol::eBrain0_ToMoving_0;
            }
            return field_11C_brain_sub_state;


        case Brain_0_Patrol::eBrain0_ToSpeak_8:
            if (mCurrentMotion != eScrabMotions::M_Stand_0_4A8220)
            {
                return field_11C_brain_sub_state;
            }

            field_1A2_speak_counter++;

            if (field_194_speak == GameSpeakEvents::Scrab_Shriek_54)
            {
                mNextMotion = eScrabMotions::M_Shriek_30_4A9EA0;
                field_154_movement_timer = sGnFrame;
                return Brain_0_Patrol::eBrain0_Shriek_5;
            }
            else if (field_194_speak == GameSpeakEvents::Scrab_Howl_53)
            {
                mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
                return Brain_0_Patrol::eBrain0_Howling_4;
            }
            else
            {
                return field_11C_brain_sub_state;
            }
            break;

        case Brain_0_Patrol::eBrain0_UsingInvisibility_9:
            if (mCurrentMotion != eScrabMotions::M_HowlBegin_26_4A9DA0 || !(mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame)))
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_Stamp_21_4A9CC0;
            field_12C_timer = sGnFrame + 30;
            return Brain_0_Patrol::eBrain0_PostInvisibilitySmash_10;

        case Brain_0_Patrol::eBrain0_PostInvisibilitySmash_10:
            if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame) && static_cast<s32>(sGnFrame) > field_12C_timer)
            {
                mNextMotion = eScrabMotions::M_Stand_0_4A8220;
                return Brain_0_Patrol::eBrain0_ToMoving_0;
            }
            return field_11C_brain_sub_state;

        case Brain_0_Patrol::eBrain0_BeingSpawned_11:
            if (mCurrentMotion != eScrabMotions::M_Stand_0_4A8220)
            {
                return field_11C_brain_sub_state;
            }
            return Brain_0_Patrol::eBrain0_ToMoving_0;

        default:
            return field_11C_brain_sub_state;
    }
}

s16 Scrab::Brain_1_ChasingEnemy_4A6470()
{
    Scrab* pScrabToFight = FindScrabToFight();
    if (pScrabToFight)
    {
        SetBrain(&Scrab::Brain_2_Fighting_4A5840);
        field_124_fight_target_obj_id = pScrabToFight->mBaseGameObjectId;
        mNextMotion = eScrabMotions::M_Stand_0_4A8220;
        return Brain_2_Fighting::eBrain2_LookingForOpponent_0;
    }

    auto pObj = static_cast<BaseAliveGameObject*>(sObjectIds.Find_Impl(field_120_obj_id));
    if (!pObj || mBaseGameObjectFlags.Get(BaseGameObject::eDead) || (static_cast<s32>(sGnFrame) > field_14C_pause_after_chase_timer && !CanSeeAbe(pObj)))
    {
        field_120_obj_id = -1;
        mNextMotion = eScrabMotions::M_Stand_0_4A8220;
        ToPatrol();
        return Brain_0_Patrol::eBrain0_ToMoving_0;
    }

    if (pObj->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible))
    {
        field_120_obj_id = -1;
        mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
        ToPatrol();
        return Brain_0_Patrol::eBrain0_UsingInvisibility_9;
    }

    if (CanSeeAbe(pObj))
    {
        field_14C_pause_after_chase_timer = sGnFrame + field_148_pause_after_chase_delay;
    }

    if (IsEventInRange(kEventAbeOhm, mXPos, mYPos, EventScale::Both) && field_11C_brain_sub_state != 26) //TODO OG bug? mNextMotion instead of field_11C_sub_state
    {
        mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
        return Brain_1_ChasingEnemy::eBrain1_Panic_4;
    }

    LiftPoint* pLiftPoint = static_cast<LiftPoint*>(sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId));
    if (pLiftPoint && pLiftPoint->Type() != ReliveTypes::eLiftPoint)
    {
        pLiftPoint = nullptr; //OG bug fix: Before it could use the pointer as a LiftPoint even if it, in fact, wasn't one
    }
    if (pLiftPoint && !pLiftPoint->vOnAnyFloor() && field_11C_brain_sub_state != Brain_1_ChasingEnemy::eBrain1_Panic_4)
    {
        mNextMotion = eScrabMotions::M_Stand_0_4A8220;
        return Brain_1_ChasingEnemy::eBrain1_Panic_4;
    }

    if (gMap.GetDirection(
            mCurrentLevel,
            mCurrentPath,
            mXPos,
            mYPos)
        >= CameraPos::eCamCurrent_0)
    {
        MusicController::static_PlayMusic(MusicController::MusicTypes::eSoftChase_8, this, 0, 0);
    }

    switch (field_11C_brain_sub_state)
    {
        case Brain_1_ChasingEnemy::eBrain1_Inactive_0:
        {
            field_150_attack_delay_timer = sGnFrame + field_128_attack_delay;
            return Brain_1_ChasingEnemy::eBrain1_Idle_1;
        }
        case Brain_1_ChasingEnemy::eBrain1_Idle_1:
        {
            if (mCurrentMotion == eScrabMotions::M_Stand_0_4A8220)
            {
                field_194_speak = LastSpeak();
                if (field_1A2_speak_counter < field_1A0_speak_max
                    && (LastSpeak() == GameSpeakEvents::Scrab_Howl_53
                        || LastSpeak() == GameSpeakEvents::Scrab_Shriek_54))
                {
                    return Brain_1_ChasingEnemy::eBrain1_PreparingToHowlOrShriek_15;
                }
            }

            if (!VIsFacingMe(pObj))
            {
                if (gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0))
                {
                    mNextMotion = eScrabMotions::M_Turn_3_4A91A0;
                }
                else
                {
                    mCurrentMotion = eScrabMotions::M_Turn_3_4A91A0;
                    mNextMotion = -1;
                    MapFollowMe(TRUE);
                }
                return Brain_1_ChasingEnemy::eBrain1_Turning_3;
            }

            if (VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromDouble(1.5), pObj)
                && VOnSameYLevel(pObj))
            {
                if (!VIsObjNearby(ScaleToGridSize(mSpriteScale), pObj))
                {
                    mNextMotion = eScrabMotions::M_AttackLunge_37_4AA0B0;
                    return Brain_1_ChasingEnemy::eBrain1_Attacking_8;
                }
                mNextMotion = eScrabMotions::M_LegKick_38_4AA120;
                return Brain_1_ChasingEnemy::eBrain1_Attacking_8;
            }
            else if (CanSeeAbe(pObj) && static_cast<s32>(sGnFrame) >= field_150_attack_delay_timer)
            {
                if (Handle_SlamDoor_or_EnemyStopper(mVelX, 0))
                {
                    return field_11C_brain_sub_state;
                }
                mNextMotion = eScrabMotions::M_Run_2_4A89C0;
                return Brain_1_ChasingEnemy::eBrain1_Running_2;
            }
            else
            {
                if (Math_NextRandom() >= 16u || static_cast<s32>(sGnFrame - field_154_movement_timer) <= 60)
                {
                    return Brain_1_ChasingEnemy::eBrain1_Idle_1;
                }
                mNextMotion = eScrabMotions::M_Shriek_30_4A9EA0;
                field_154_movement_timer = sGnFrame;
                return Brain_1_ChasingEnemy::eBrain1_Shriek_14;
            }
        }
        case Brain_1_ChasingEnemy::eBrain1_Running_2:
            return Brain_ChasingEnemy_State_2_Running(pObj);

        case Brain_1_ChasingEnemy::eBrain1_Turning_3:
            if (mCurrentMotion == eScrabMotions::M_Turn_3_4A91A0)
            {
                if (!mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
                {
                    return field_11C_brain_sub_state;
                }
                mNextMotion = eScrabMotions::M_Stand_0_4A8220;
                return Brain_1_ChasingEnemy::eBrain1_Idle_1;
            }

            if (mNextMotion != eScrabMotions::M_Turn_3_4A91A0)
            {
                mNextMotion = eScrabMotions::M_Turn_3_4A91A0;
            }

            if (mCurrentMotion != eScrabMotions::M_Shriek_30_4A9EA0)
            {
                return field_11C_brain_sub_state;
            }

            if (!mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_Stand_0_4A8220;
            return Brain_1_ChasingEnemy::eBrain1_Idle_1;

        case Brain_1_ChasingEnemy::eBrain1_Panic_4:
            if (VIsObjNearby(ScaleToGridSize(mSpriteScale), pObj)
                && VOnSameYLevel(pObj))
            {
                mNextMotion = eScrabMotions::M_LegKick_38_4AA120;
                return Brain_1_ChasingEnemy::eBrain1_Attacking_8;
            }

            if (pLiftPoint)
            {
                if (!pLiftPoint->vOnAnyFloor())
                {
                    return field_11C_brain_sub_state;
                }
            }
            else
            {
                BaseAliveGameObject_PlatformId = -1;
            }
            return Brain_1_ChasingEnemy::eBrain1_Idle_1;

        case Brain_1_ChasingEnemy::eBrain1_Falling_5:
        case Brain_1_ChasingEnemy::eBrain1_ToIdle_6:
            if (mCurrentMotion)
            {
                return field_11C_brain_sub_state;
            }
            return Brain_1_ChasingEnemy::eBrain1_Idle_1;

        case Brain_1_ChasingEnemy::eBrain1_Jumping_7:
            if (mCurrentMotion != eScrabMotions::M_RunJumpEnd_13_4A9BE0)
            {
                return field_11C_brain_sub_state;
            }
            return Brain_1_ChasingEnemy::eBrain1_Idle_1;

        case Brain_1_ChasingEnemy::eBrain1_Attacking_8:
            if ((mCurrentMotion != eScrabMotions::M_AttackLunge_37_4AA0B0
                 && mCurrentMotion != eScrabMotions::M_LegKick_38_4AA120)
                || !mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }

            if (pObj->mHealth > FP_FromInteger(0))
            {
                return Brain_1_ChasingEnemy::eBrain1_Idle_1;
                break;
            }

            if (VIsFacingMe(pObj))
            {
                if (!VIsObjNearby(ScaleToGridSize(mSpriteScale), pObj))
                {
                    if (!VIsObjNearby(ScaleToGridSize(mSpriteScale), pObj))
                    {
                        mNextMotion = eScrabMotions::M_Walk_1_4A84D0;
                    }
                }
                return Brain_1_ChasingEnemy::eBrain1_Walking_9;
            }
            else
            {
                mNextMotion = eScrabMotions::M_Turn_3_4A91A0;
                return Brain_1_ChasingEnemy::eBrain1_EnemyDead_10;
            }

        case Brain_1_ChasingEnemy::eBrain1_Walking_9:
            if (!VIsObjNearby(ScaleToGridSize(mSpriteScale), pObj))
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_Stamp_21_4A9CC0;
            field_12C_timer = sGnFrame + 30;
            return Brain_1_ChasingEnemy::eBrain1_SmashingEnemy_11;

        case Brain_1_ChasingEnemy::eBrain1_EnemyDead_10:
            if (mCurrentMotion != eScrabMotions::M_Turn_3_4A91A0 || !mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }

            if (!VIsObjNearby(ScaleToGridSize(mSpriteScale), pObj))
            {
                mNextMotion = eScrabMotions::M_Walk_1_4A84D0;
            }
            return Brain_1_ChasingEnemy::eBrain1_Walking_9;

        case Brain_1_ChasingEnemy::eBrain1_SmashingEnemy_11:
            if (static_cast<s32>(sGnFrame) <= field_12C_timer)
            {
                return field_11C_brain_sub_state;
            }

            if (pObj->mYPos - mYPos < FP_FromInteger(0))
            {
                pObj->mYPos - mYPos = -(pObj->mYPos - mYPos);
            }

            if (pObj->mYPos - mYPos >= FP_FromInteger(5))
            {
                mNextMotion = eScrabMotions::M_Shriek_30_4A9EA0;
                return Brain_1_ChasingEnemy::eBrain1_Shriek_14;
            }
            else
            {
                mNextMotion = eScrabMotions::M_StandToFeed_35_4AA010;
                return Brain_1_ChasingEnemy::eBrain1_Eating_12;
            }

        case Brain_1_ChasingEnemy::eBrain1_Eating_12:
            if (mCurrentMotion == eScrabMotions::M_FeedToGulp_33_4A9FA0)
            {
                if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
                {
                    mNextMotion = eScrabMotions::M_StandToFeed_35_4AA010;
                }
            }
            return field_11C_brain_sub_state;

        case Brain_1_ChasingEnemy::eBrain1_Howl_13:
            if (IsEventInRange(kEventAbeOhm, mXPos, mYPos, EventScale::Both))
            {
                return field_11C_brain_sub_state;
            }

            mCurrentMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
            if (mCurrentMotion != eScrabMotions::M_Shriek_30_4A9EA0)
            {
                return field_11C_brain_sub_state;
            }
            if (!mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_Stand_0_4A8220;
            return Brain_1_ChasingEnemy::eBrain1_Idle_1;

        case Brain_1_ChasingEnemy::eBrain1_Shriek_14:
            if (mCurrentMotion != eScrabMotions::M_Shriek_30_4A9EA0)
            {
                return field_11C_brain_sub_state;
            }

            if (!mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_Stand_0_4A8220;
            return Brain_1_ChasingEnemy::eBrain1_Idle_1;

        case Brain_1_ChasingEnemy::eBrain1_PreparingToHowlOrShriek_15:
            if (mCurrentMotion)
            {
                return field_11C_brain_sub_state;
            }

            field_1A2_speak_counter++;
            if (field_194_speak == GameSpeakEvents::Scrab_Shriek_54)
            {
                mNextMotion = eScrabMotions::M_Shriek_30_4A9EA0;
                field_154_movement_timer = sGnFrame;
                return Brain_1_ChasingEnemy::eBrain1_Shriek_14;
            }

            if (field_194_speak != GameSpeakEvents::Scrab_Howl_53)
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
            return Brain_1_ChasingEnemy::eBrain1_Howl_13;

        case Brain_1_ChasingEnemy::eBrain1_KilledPossessedScrab_16:
            if (mCurrentMotion == eScrabMotions::M_JumpToFall_8_4A9220)
            {
                return Brain_1_ChasingEnemy::eBrain1_Falling_5;
            }
            else if (mCurrentMotion == eScrabMotions::M_AttackSpin_32_4A8DC0)
            {
                return field_11C_brain_sub_state;
            }
            else
            {
                return Brain_1_ChasingEnemy::eBrain1_Idle_1;
            }
        default:
            return field_11C_brain_sub_state;
    }
}

s16 Scrab::Brain_ChasingEnemy_State_2_Running(BaseAliveGameObject* pObj)
{
    field_194_speak = LastSpeak();
    if (field_1A2_speak_counter < field_1A0_speak_max
        && (LastSpeak() == GameSpeakEvents::Scrab_Howl_53
            || LastSpeak() == GameSpeakEvents::Scrab_Shriek_54))
    {
        mNextMotion = eScrabMotions::M_Stand_0_4A8220;
        return Brain_1_ChasingEnemy::eBrain1_PreparingToHowlOrShriek_15;
    }

    if (Handle_SlamDoor_or_EnemyStopper(mVelX, 0))
    {
        mNextMotion = eScrabMotions::M_Stand_0_4A8220;
        return Brain_1_ChasingEnemy::eBrain1_Idle_1;
    }

    s32 xPosition = abs(FP_GetExponent(mXPos) - SnapToXGrid(mSpriteScale, FP_GetExponent(mXPos)));

    //Jump at the end of the ledge
    FP xOffset = (mVelX >= FP_FromInteger(0)) ? ScaleToGridSize(mSpriteScale) : -ScaleToGridSize(mSpriteScale);
    if (xPosition < 6
        && Check_IsOnEndOfLine(mVelX < FP_FromInteger(0), 1)
        && ((pObj->mYPos - mYPos < FP_FromInteger(5))
            || sPathInfo->TLV_Get_At_4DB4B0(
                FP_GetExponent(mXPos + xOffset),
                FP_GetExponent(mYPos + FP_FromInteger(10)),
                FP_GetExponent(mXPos + xOffset),
                FP_GetExponent(mYPos + FP_FromInteger(10)),
                TlvTypes::ElectricWall_38))
        && !Check_IsOnEndOfLine(mVelX < FP_FromInteger(0), 3))
    {
        ToJump();
        mNextMotion = -1;
        return Brain_1_ChasingEnemy::eBrain1_Jumping_7;
    }

    if (VIsFacingMe(pObj))
    {
        if (VIsObjNearby(ScaleToGridSize(mSpriteScale) / FP_FromInteger(7), pObj)
            && pObj->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit4_bPossesed)
            && pObj->Type() == ReliveTypes::eScrab)
        {
            if (!field_178_shred_power_active)
            {
                mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
                return Brain_1_ChasingEnemy::eBrain1_Howl_13;
            }
            if (!Math_NextRandom())
            {
                mNextMotion = eScrabMotions::M_AttackSpin_32_4A8DC0;
                return Brain_1_ChasingEnemy::eBrain1_KilledPossessedScrab_16;
            }
        }

        if (VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromInteger(3), pObj)
            && mCurrentMotion == eScrabMotions::M_Run_2_4A89C0
            && VOnSameYLevel(pObj))
        {
            if (WallHit(mSpriteScale * FP_FromInteger(45), pObj->mXPos - mXPos))
            {
                mNextMotion = eScrabMotions::M_Stand_0_4A8220;
                return Brain_1_ChasingEnemy::eBrain1_Idle_1;
            }
            else
            {
                mNextMotion = eScrabMotions::M_AttackLunge_37_4AA0B0;
                return Brain_1_ChasingEnemy::eBrain1_Attacking_8;
            }
        }
        else
        {
            if (mCurrentMotion != eScrabMotions::M_JumpToFall_8_4A9220)
            {
                //A patch workaround: This situation should probably never happen in the first place, but since it does...
                if (mCurrentMotion == eScrabMotions::M_Stand_0_4A8220 && mNextMotion == -1)
                {
                    mNextMotion = eScrabMotions::M_Run_2_4A89C0;
                }
                return field_11C_brain_sub_state;
            }
            return Brain_1_ChasingEnemy::eBrain1_Falling_5;
        }
    }
    else
    {
        if (gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0))
        {
            mNextMotion = eScrabMotions::M_Turn_3_4A91A0;
        }
        else
        {
            mCurrentMotion = eScrabMotions::M_Turn_3_4A91A0;
            mNextMotion = -1;
            MapFollowMe(TRUE);
        }
        return Brain_1_ChasingEnemy::eBrain1_Turning_3;
    }
}

s16 Scrab::Brain_2_Fighting_4A5840()
{
    auto pTarget = static_cast<Scrab*>(sObjectIds.Find_Impl(field_124_fight_target_obj_id));
    if (field_11C_brain_sub_state != Brain_2_Fighting::eBrain2_Battling_10 && field_11C_brain_sub_state != Brain_2_Fighting::eBrain2_Victorious_11 && field_11C_brain_sub_state != Brain_2_Fighting::eBrain2_SmashingOpponent_12 && field_11C_brain_sub_state != Brain_2_Fighting::eBrain2_VictoryYell_13 && (!pTarget || pTarget->mBaseGameObjectFlags.Get(BaseGameObject::eDead) || (WallHit(mSpriteScale * FP_FromInteger(45), pTarget->mXPos - mXPos)) || !VOnSameYLevel(pTarget)))
    {
        mNextMotion = eScrabMotions::M_Stand_0_4A8220;
        ToPatrol();
        return Brain_0_Patrol::eBrain0_ToMoving_0;
    }

    if (pTarget && pTarget->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit4_bPossesed) && mHealth > FP_FromInteger(0) && pTarget->mHealth > FP_FromInteger(0))
    {
        field_120_obj_id = field_124_fight_target_obj_id;
        field_124_fight_target_obj_id = -1;
        SetBrain(&Scrab::Brain_1_ChasingEnemy_4A6470);
        field_150_attack_delay_timer = sGnFrame + 90;
        mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
        return Brain_2_Fighting::eBrain2_SpottedOpponent_1;
    }

    if (EventGet(kEventAbeOhm))
    {
        if (field_11C_brain_sub_state != Brain_2_Fighting::eBrain2_Running_9 && field_11C_brain_sub_state != Brain_2_Fighting::eBrain2_Battling_10)
        {
            if (field_11C_brain_sub_state != Brain_2_Fighting::eBrain2_Victorious_11 && field_11C_brain_sub_state != Brain_2_Fighting::eBrain2_SmashingOpponent_12 && field_11C_brain_sub_state != Brain_2_Fighting::eBrain2_VictoryYell_13)
            {
                mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
                ToPatrol();
                return Brain_0_Patrol::eBrain0_Howling_4;
            }

            if (pTarget->field_11C_brain_sub_state != Brain_2_Fighting::eBrain2_Running_9 && mNextMotion == -1)
            {
                field_11C_brain_sub_state = Brain_2_Fighting::eBrain2_InterruptVictoryStates_14;
                mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
            }
        }
    }

    if (gMap.GetDirection(mCurrentLevel, mCurrentPath, mXPos, mYPos) >= CameraPos::eCamCurrent_0)
    {
        MusicController::static_PlayMusic(MusicController::MusicTypes::eSoftChase_8, this, 0, 0);
    }

    switch (field_11C_brain_sub_state)
    {
        case Brain_2_Fighting::eBrain2_LookingForOpponent_0:
            if (mCurrentMotion)
            {
                if (mNextMotion != eScrabMotions::M_Stand_0_4A8220)
                {
                    mNextMotion = eScrabMotions::M_Stand_0_4A8220;
                }
                return Brain_2_Fighting::eBrain2_LookingForOpponent_0;
            }

            field_1AA_flags.Clear(Flags_1AA::eBit1_attacking);
            mNextMotion = -1;
            if (pTarget->field_124_fight_target_obj_id == -1 || pTarget->field_124_fight_target_obj_id == mBaseGameObjectId)
            {
                if (VIsFacingMe(pTarget))
                {
                    if (!VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromInteger(8), pTarget))
                    {
                        if (pTarget->mCurrentMotion == eScrabMotions::M_HowlBegin_26_4A9DA0)
                        {
                            return field_11C_brain_sub_state;
                        }
                        mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
                        return Brain_2_Fighting::eBrain2_Yelling_6;
                    }
                    mNextMotion = eScrabMotions::M_Turn_3_4A91A0;
                    return Brain_2_Fighting::eBrain2_Turning_2;
                }
                else
                {
                    mNextMotion = eScrabMotions::M_Turn_3_4A91A0;
                    return Brain_2_Fighting::eBrain2_SpottedOpponent_1;
                }
            }
            else
            {
                field_124_fight_target_obj_id = -1;
                mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
                return Brain_2_Fighting::eBrain2_WaitingForBattle_15;
            }
            break;

        case Brain_2_Fighting::eBrain2_SpottedOpponent_1:
            if (mCurrentMotion != eScrabMotions::M_Turn_3_4A91A0 || !mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }

            if (VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromInteger(8), pTarget))
            {
                mNextMotion = eScrabMotions::M_Turn_3_4A91A0;
                return Brain_2_Fighting::eBrain2_Turning_2;
            }
            else
            {
                mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
                return Brain_2_Fighting::eBrain2_Yelling_6;
            }
            break;

        case Brain_2_Fighting::eBrain2_Turning_2:
            if (mCurrentMotion != eScrabMotions::M_Turn_3_4A91A0 || !mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_Walk_1_4A84D0;
            return Brain_2_Fighting::eBrain2_Walking_3;

        case Brain_2_Fighting::eBrain2_Walking_3:
        {
            auto xPos = mXPos;
            s16 sIsFlipped = 0;
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                sIsFlipped = 1;
                xPos -= FP_FromInteger(50);
            }
            else
            {
                xPos += FP_FromInteger(50);
            }
            if (!Handle_SlamDoor_or_EnemyStopper(mVelX, 0) && !Check_IsOnEndOfLine(sIsFlipped, 2) && VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromInteger(10), pTarget))
            {
                if (gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, xPos, mYPos, 0))
                {
                    return field_11C_brain_sub_state;
                }
            }
            mNextMotion = eScrabMotions::M_Turn_3_4A91A0;
            return Brain_2_Fighting::eBrain2_SetInPosition_4;
        }
        case Brain_2_Fighting::eBrain2_SetInPosition_4:
            if (mCurrentMotion != eScrabMotions::M_Turn_3_4A91A0)
            {
                return field_11C_brain_sub_state;
            }
            if (!mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
            return Brain_2_Fighting::eBrain2_Yelling_6;

        case Brain_2_Fighting::eBrain2_Idle_5:
            if (static_cast<s32>(sGnFrame) <= field_12C_timer)
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_Shriek_30_4A9EA0;
            return Brain_2_Fighting::eBrain2_Shriek_7;

        case Brain_2_Fighting::eBrain2_Yelling_6:
            if (mCurrentMotion != eScrabMotions::M_HowlBegin_26_4A9DA0 || !mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_Stand_0_4A8220;
            field_12C_timer = sGnFrame + 20;
            return Brain_2_Fighting::eBrain2_Idle_5;

        case Brain_2_Fighting::eBrain2_Shriek_7:
            if (mCurrentMotion != eScrabMotions::M_Shriek_30_4A9EA0 || !mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }
            field_1AA_flags.Set(Flags_1AA::eBit1_attacking);
            mNextMotion = eScrabMotions::M_Stand_0_4A8220;
            return Brain_2_Fighting::eBrain2_ToRun_8;

        case Brain_2_Fighting::eBrain2_ToRun_8:
            if (!pTarget->field_1AA_flags.Get(Flags_1AA::eBit1_attacking))
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_Run_2_4A89C0;
            return Brain_2_Fighting::eBrain2_Running_9;

        case Brain_2_Fighting::eBrain2_Running_9:
            if (!VIsObjNearby(ScaleToGridSize(mSpriteScale), pTarget) || !BaseAliveGameObjectCollisionLine || !pTarget->BaseAliveGameObjectCollisionLine)
            {
                return field_11C_brain_sub_state;
            }

            if (pTarget->mAnim.mFlags.Get(AnimFlags::eBit3_Render))
            {
                mAnim.mFlags.Clear(AnimFlags::eBit3_Render);
                mHealth = FP_FromInteger(0);
            }

            MapFollowMe(TRUE);
            mCurrentMotion = eScrabMotions::M_ScrabBattleAnim_31_4A9F30;
            field_12C_timer = sGnFrame + 40;
            return Brain_2_Fighting::eBrain2_Battling_10;

        case Brain_2_Fighting::eBrain2_Battling_10:
            if (static_cast<s32>(sGnFrame) <= field_12C_timer)
            {
                return field_11C_brain_sub_state;
            }

            SND_Stop_Channels_Mask(field_160_sfx_bitmask);
            field_160_sfx_bitmask = 0;
            Scrab_SFX(ScrabSounds::eDeathHowl_1, 0, -1571, 1);
            Scrab_SFX(ScrabSounds::eYell_8, 0, -1571, 1);
            Environment_SFX_457A40(EnvironmentSfx::eHitGroundSoft_6, 0, -383, 0);

            if (!mAnim.mFlags.Get(AnimFlags::eBit3_Render))
            {
                SetBrain(&Scrab::Brain_3_Death_4A62B0);
                mXPos = pTarget->mXPos;
                mCurrentMotion = eScrabMotions::M_DeathBegin_39_4AA190;
                mAnim.mFlags.Set(AnimFlags::eBit3_Render);
                field_12C_timer = sGnFrame + 90;
                return field_11C_brain_sub_state;
            }
            mCurrentMotion = eScrabMotions::M_Stand_0_4A8220;
            field_12C_timer = sGnFrame + 5;
            return Brain_2_Fighting::eBrain2_Victorious_11;

        case Brain_2_Fighting::eBrain2_Victorious_11:
            if (static_cast<s32>(sGnFrame) <= field_12C_timer)
            {
                return field_11C_brain_sub_state;
            }
            field_12C_timer = 5;
            mNextMotion = eScrabMotions::M_Stamp_21_4A9CC0;
            return Brain_2_Fighting::eBrain2_SmashingOpponent_12;

        case Brain_2_Fighting::eBrain2_SmashingOpponent_12:
            if (mCurrentMotion != eScrabMotions::M_Stamp_21_4A9CC0)
            {
                return field_11C_brain_sub_state;
            }

            if (!mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }

            field_12C_timer--;
            if (field_12C_timer > 0)
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
            return Brain_2_Fighting::eBrain2_VictoryYell_13;

        case Brain_2_Fighting::eBrain2_VictoryYell_13:
            if (mCurrentMotion != eScrabMotions::M_HowlBegin_26_4A9DA0 || !mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }
            field_124_fight_target_obj_id = -1;
            mNextMotion = eScrabMotions::M_Stand_0_4A8220;
            ToPatrol();
            return Brain_0_Patrol::eBrain0_ToMoving_0;

        case Brain_2_Fighting::eBrain2_InterruptVictoryStates_14:
            if (EventGet(kEventAbeOhm))
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_Stand_0_4A8220;
            return Brain_2_Fighting::eBrain2_LookingForOpponent_0;

        case Brain_2_Fighting::eBrain2_WaitingForBattle_15:
        {
            if (mCurrentMotion != eScrabMotions::M_HowlBegin_26_4A9DA0)
            {
                return field_11C_brain_sub_state;
            }
            if (!mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                return field_11C_brain_sub_state;
            }

            auto pFoundTarget = FindScrabToFight();
            if (!pFoundTarget)
            {
                return field_11C_brain_sub_state;
            }
            mNextMotion = eScrabMotions::M_Stand_0_4A8220;
            field_124_fight_target_obj_id = pFoundTarget->mBaseGameObjectId;
            return Brain_2_Fighting::eBrain2_LookingForOpponent_0;
        }

        default:
            return field_11C_brain_sub_state;
    }
}

s16 Scrab::Brain_3_Death_4A62B0()
{
    if (field_12C_timer < static_cast<s32>((sGnFrame + 80)))
    {
        mSpriteScale -= FP_FromDouble(0.008);
        mRGB.r -= 2;
        mRGB.g -= 2;
        mRGB.b -= 2;
    }

    if (static_cast<s32>(sGnFrame) < field_12C_timer - 24)
    {
        DeathSmokeEffect(true);
    }

    if (field_12C_timer < static_cast<s32>(sGnFrame))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    return 0;
}

s16 Scrab::Brain_4_ShrinkDeath_4A6420()
{
    if (mScale == Scale::Fg)
    {
        mSpriteScale -= FP_FromDouble(0.06);
    }
    else
    {
        mSpriteScale -= FP_FromDouble(0.03);
    }

    if (field_12C_timer < static_cast<s32>(sGnFrame))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    return 0;
}

s16 Scrab::Brain_5_Possessed_4A6180()
{
    // Abe is dead, go back to patrolling
    if (sActiveHero->mHealth <= FP_FromInteger(0))
    {
        sControlledCharacter = sActiveHero;
        mBaseAliveGameObjectFlags.Clear(Flags_114::e114_Bit4_bPossesed);
        field_1A2_speak_counter = 0;
        MusicController::static_PlayMusic(MusicController::MusicTypes::eNone_0, this, 0, 0);
        ToPatrol();
        field_11C_brain_sub_state = Brain_0_Patrol::eBrain0_ToMoving_0;
        gMap.SetActiveCam(field_166_return_level, field_168_return_path, field_16A_return_camera, CameraSwapEffects::eInstantChange_0, 0, 0);
        return field_11C_brain_sub_state;
    }

    if (field_11C_brain_sub_state == 0)
    {
        if (static_cast<s32>(sGnFrame) <= field_12C_timer)
        {
            return field_11C_brain_sub_state;
        }
        mNextMotion = eScrabMotions::M_Stand_0_4A8220;
        return 1;
    }
    else
    {
        if (field_11C_brain_sub_state != 1 || gMap.GetDirection(mCurrentLevel, mCurrentPath, mXPos, mYPos) == CameraPos::eCamInvalid_m1)
        {
            return field_11C_brain_sub_state;
        }
        MusicController::static_PlayMusic(MusicController::MusicTypes::ePossessed_9, this, 0, 0);
        return field_11C_brain_sub_state;
    }
}

void Scrab::M_Stand_0_4A8220()
{
    if (BrainIs(&Scrab::Brain_5_Possessed_4A6180))
    {
        if (field_11C_brain_sub_state != 1)
        {
            return;
        }

        if (Input_IsChanting_45F260() && !field_164_prevent_depossession)
        {
            field_130_depossession_timer = sGnFrame + 30;
            SfxPlayMono(SoundEffect::PossessEffect_17, 0);
            mCurrentMotion = eScrabMotions::M_GetDepossessedBegin_28_4AA200;
            return;
        }

        if (Input().isHeld(sInputKey_GameSpeak2))
        {
            mCurrentMotion = eScrabMotions::M_Shriek_30_4A9EA0;
            mNextMotion = eScrabMotions::M_Stand_0_4A8220;
            return;
        }

        if (Input().isHeld(sInputKey_GameSpeak1))
        {
            mCurrentMotion = eScrabMotions::M_HowlBegin_26_4A9DA0;
            mNextMotion = eScrabMotions::M_Stand_0_4A8220;
            return;
        }

        if (Input().isPressed(InputCommands::Enum::eThrowItem | InputCommands::Enum::eDoAction) && field_178_shred_power_active)
        {
            mCurrentMotion = eScrabMotions::M_AttackSpin_32_4A8DC0;
            mNextMotion = -1;
            field_12C_timer = sGnFrame + field_174_possessed_max_whirl_attack_duration;
            return;
        }

        if (Input().isPressed(InputCommands::Enum::eThrowItem | InputCommands::Enum::eDoAction))
        {
            mCurrentMotion = eScrabMotions::M_Stamp_21_4A9CC0;
            mNextMotion = -1;
            return;
        }

        if (Input().isPressed(sInputKey_Hop))
        {
            const FP k45Scaled = (mSpriteScale * FP_FromInteger(45));
            const FP kGridSize = ScaleToGridSize(mSpriteScale);

            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                if (!WallHit(k45Scaled, -kGridSize))
                {
                    mCurrentMotion = eScrabMotions::M_HopBegin_5_4A96C0;
                    mNextMotion = -1;
                    return;
                }
            }
            else
            {
                if (!WallHit(k45Scaled, kGridSize))
                {
                    mCurrentMotion = eScrabMotions::M_HopBegin_5_4A96C0;
                    mNextMotion = -1;
                    return;
                }
            }
        }
    }
    ToNextMotion();
}

const FP sScrabWalkXVels_546E54[22] = {
    FP_FromDouble(1.570587158203125),
    FP_FromDouble(1.5780487060546875),
    FP_FromDouble(1.5256805419921875),
    FP_FromDouble(1.5651397705078125),
    FP_FromDouble(1.5966339111328125),
    FP_FromDouble(3.694122314453125),
    FP_FromDouble(3.9574737548828125),
    FP_FromDouble(3.9715423583984375),
    FP_FromDouble(3.3073272705078125),
    FP_FromDouble(2.629669189453125),
    FP_FromDouble(2.1679229736328125),
    FP_FromDouble(2.0220794677734375),
    FP_FromDouble(1.804046630859375),
    FP_FromDouble(1.6065216064453125),
    FP_FromDouble(2.6418609619140625),
    FP_FromDouble(2.685821533203125),
    FP_FromDouble(2.673095703125),
    FP_FromDouble(2.691070556640625),
    FP_FromDouble(1.3966827392578125),
    FP_FromDouble(1.376983642578125),
    FP_FromDouble(1.3767242431640625),
    FP_FromDouble(2.1646728515625)};

void Scrab::M_Walk_1_4A84D0()
{
    mVelX = mSpriteScale * sScrabWalkXVels_546E54[mAnim.mCurrentFrame];

    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mVelX = -mVelX;
    }

    MoveOnLine();

    if (mCurrentMotion != eScrabMotions::M_Walk_1_4A84D0)
    {
        return;
    }

    FP kGridSize = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        kGridSize = -ScaleToGridSize(mSpriteScale);
    }
    else
    {
        kGridSize = ScaleToGridSize(mSpriteScale);
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), kGridSize * FP_FromDouble(0.5)))
    {
        KnockBack();
        return;
    }

    switch (mAnim.mCurrentFrame)
    {
        case 3:
        case 13:
            Scrab_SFX(ScrabSounds::eWalk1_6, Math_RandomRange(40, 50), 0x7FFF, 1);
            return;

        case 5:
        case 15:
            if (WallHit(mSpriteScale * FP_FromInteger(45), kGridSize * FP_FromInteger(1)))
            {
                mCurrentMotion = eScrabMotions::M_WalkToStand_11_4A8880;
            }
            else if (sControlledCharacter != this || mHealth <= FP_FromInteger(0))
            {
                if (mNextMotion == eScrabMotions::M_Stand_0_4A8220 || mNextMotion == eScrabMotions::M_Turn_3_4A91A0 || mNextMotion == eScrabMotions::M_Stamp_21_4A9CC0 || mNextMotion == eScrabMotions::M_Empty_25_4A34D0 || mNextMotion == eScrabMotions::M_Shriek_30_4A9EA0 || mNextMotion == eScrabMotions::M_HowlBegin_26_4A9DA0 || mNextMotion == eScrabMotions::M_HopMidair_6_4A9490 || mNextMotion == eScrabMotions::M_AttackLunge_37_4AA0B0 || mNextMotion == eScrabMotions::M_LegKick_38_4AA120)
                {
                    mCurrentMotion = eScrabMotions::M_WalkToStand_11_4A8880;
                }
            }
            else
            {
                if ((mVelX > FP_FromInteger(0) && Input().isPressed(InputCommands::Enum::eLeft)) || (mVelX < FP_FromInteger(0) && Input().isPressed(InputCommands::Enum::eRight)) || !Input().isPressed(InputCommands::Enum::eRight | InputCommands::Enum::eLeft))
                {
                    mCurrentMotion = eScrabMotions::M_WalkToStand_11_4A8880;
                }
            }
            return;

        case 7:
        case 18:
            Scrab_SFX(ScrabSounds::eWalk1_6, Math_RandomRange(40, 50), 0x7FFF, 1);
            if (sControlledCharacter != this || mHealth <= FP_FromInteger(0))
            {
                if (mNextMotion != eScrabMotions::M_Run_2_4A89C0)
                {
                    MapFollowMe(TRUE);
                    return;
                }
                mCurrentMotion = eScrabMotions::M_WalkToRun_16_4A8D60;
                mNextMotion = -1;
                MapFollowMe(TRUE);
                return;
            }

            if (Input().isPressed(InputCommands::Enum::eThrowItem | InputCommands::Enum::eDoAction) && field_178_shred_power_active)
            {
                mCurrentMotion = eScrabMotions::M_AttackSpin_32_4A8DC0;
                field_12C_timer = sGnFrame + field_174_possessed_max_whirl_attack_duration;
                mNextMotion = -1;
                MapFollowMe(TRUE);
                return;
            }

            if (Input().isPressed(InputCommands::Enum::eRun))
            {
                mCurrentMotion = eScrabMotions::M_WalkToRun_16_4A8D60;
                mNextMotion = -1;
                MapFollowMe(TRUE);
                return;
            }

            if (Input().isPressed(InputCommands::Enum::eHop))
            {
                mCurrentMotion = eScrabMotions::M_HopBegin_5_4A96C0;
                mNextMotion = -1;
                MapFollowMe(TRUE);
                return;
            }

            MapFollowMe(TRUE);
            return;

        default:
            return;
    }
}

static const FP sScrabRunVelX_546EC4[14] = {
    FP_FromDouble(9.498733520507812),
    FP_FromDouble(9.733749389648438),
    FP_FromDouble(9.584152221679688),
    FP_FromDouble(9.141021728515625),
    FP_FromDouble(9.099105834960938),
    FP_FromDouble(9.576034545898438),
    FP_FromDouble(9.327011108398438),
    FP_FromDouble(8.654891967773438),
    FP_FromDouble(5.2911224365234375),
    FP_FromDouble(4.2801666259765625),
    FP_FromDouble(3.446380615234375),
    FP_FromDouble(3.3974456787109375),
    FP_FromDouble(3.8137969970703125),
    FP_FromDouble(5.15692138671875)};

void Scrab::M_Run_2_4A89C0()
{
    auto pTarget = static_cast<BaseAliveGameObject*>(sObjectIds.Find_Impl(field_120_obj_id));
    mVelX = mSpriteScale * sScrabRunVelX_546EC4[mAnim.mCurrentFrame];

    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mVelX = -mVelX;
    }

    FP gridSize = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        gridSize = -ScaleToGridSize(mSpriteScale);
    }
    else
    {
        gridSize = ScaleToGridSize(mSpriteScale);
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), gridSize * FP_FromDouble(0.5)))
    {
        KnockBack();
        return;
    }

    MoveOnLine();

    if (mCurrentMotion == eScrabMotions::M_Run_2_4A89C0)
    {
        KillTarget(pTarget);
        switch (mAnim.mCurrentFrame)
        {
            case 0:
            case 5:
            case 7:
            case 12:
                Scrab_SFX(ScrabSounds::eWalk1_6, Math_RandomRange(40, 50), 0x7FFF, 1);
                return;

            case 3:
            case 10:
                if (sControlledCharacter != this || mHealth <= FP_FromInteger(0))
                {
                    if (mNextMotion == eScrabMotions::M_Walk_1_4A84D0)
                    {
                        mCurrentMotion = eScrabMotions::M_RunToWalk_17_4A8D90;
                        mNextMotion = -1;
                        MapFollowMe(TRUE);
                        return;
                    }

                    if (mNextMotion != eScrabMotions::M_Stand_0_4A8220 && mNextMotion != eScrabMotions::M_Turn_3_4A91A0 && mNextMotion != eScrabMotions::M_HopMidair_6_4A9490 && mNextMotion != eScrabMotions::M_AttackLunge_37_4AA0B0 && mNextMotion != eScrabMotions::M_Empty_25_4A34D0 && mNextMotion != eScrabMotions::M_HowlBegin_26_4A9DA0 && mNextMotion != eScrabMotions::M_LegKick_38_4AA120 && mNextMotion != eScrabMotions::M_Shriek_30_4A9EA0)
                    {
                        if (mNextMotion == eScrabMotions::M_AttackSpin_32_4A8DC0)
                        {
                            mCurrentMotion = eScrabMotions::M_AttackSpin_32_4A8DC0;
                            MapFollowMe(TRUE);
                            return;
                        }

                        if (mNextMotion == eScrabMotions::M_ScrabBattleAnim_31_4A9F30)
                        {
                            ToStand();
                            mCurrentMotion = eScrabMotions::M_ScrabBattleAnim_31_4A9F30;
                            mNextMotion = -1;
                            MapFollowMe(TRUE);
                            return;
                        }

                        MapFollowMe(TRUE);
                        return;
                    }
                }
                else
                {
                    if ((mVelX <= FP_FromInteger(0) || !Input().isPressed(sInputKey_Left)) && (mVelX >= FP_FromInteger(0) || !Input().isPressed(sInputKey_Right)))
                    {
                        if (Input().isPressed(sInputKey_Left | sInputKey_Right))
                        {
                            if (!Input().isPressed(sInputKey_Run))
                            {
                                mCurrentMotion = eScrabMotions::M_RunToWalk_17_4A8D90;
                                MapFollowMe(TRUE);
                                return;
                            }

                            if (Input().isPressed(sInputKey_Hop))
                            {
                                ToJump();
                                MapFollowMe(TRUE);
                                return;
                            }

                            if (Input().isPressed(0xA0) && field_178_shred_power_active)
                            {
                                field_12C_timer = MakeTimer(field_174_possessed_max_whirl_attack_duration);
                                mCurrentMotion = eScrabMotions::M_AttackSpin_32_4A8DC0;
                                mNextMotion = -1;
                                MapFollowMe(TRUE);
                                return;
                            }

                            MapFollowMe(TRUE);
                            return;
                        }
                    }
                }
                mCurrentMotion = eScrabMotions::M_RunToStand_4_4A90C0;
                MapFollowMe(TRUE);
                return;

            default:
                return;
        }
    }
}

void Scrab::M_Turn_3_4A91A0()
{
    if (mAnim.mCurrentFrame == 5 || mAnim.mCurrentFrame == 9 || mAnim.mCurrentFrame == 11)
    {
        Scrab_SFX(ScrabSounds::eWalk1_6, Math_RandomRange(40, 50), 0x7FFF, 1);
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mAnim.mFlags.Toggle(AnimFlags::eBit5_FlipX);
        TryMoveOrStand();
    }
}

const FP dword_546EFC[10] = {
    490908,
    453112,
    254902,
    155230,
    49871,
    42004,
    46393,
    50715,
    47541,
    47764};

void Scrab::M_RunToStand_4_4A90C0()
{
    auto pTarget = static_cast<BaseAliveGameObject*>(sObjectIds.Find_Impl(field_120_obj_id));
    mVelX = mSpriteScale * dword_546EFC[mAnim.mCurrentFrame]; // TODO: check size
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mVelX = -mVelX;
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), mVelX))
    {
        KnockBack();
    }
    else
    {
        MoveOnLine();

        if (mCurrentMotion == eScrabMotions::M_RunToStand_4_4A90C0)
        {
            KillTarget(pTarget);

            if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                MapFollowMe(TRUE);
                TryMoveOrStand();
            }
        }
    }
}

const FP sScrabHopBeginVelX_546F24[4] = {
    FP_FromDouble(-1.0283050537109375),
    FP_FromDouble(-1.8401947021484375),
    FP_FromDouble(-0.2052764892578125),
    FP_FromDouble(2.77862548828125)};

void Scrab::M_HopBegin_5_4A96C0()
{
    EventBroadcast(kEventNoise, this);

    FP frameVelX = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        frameVelX = -sScrabHopBeginVelX_546F24[mAnim.mCurrentFrame];
    }
    else
    {
        frameVelX = sScrabHopBeginVelX_546F24[mAnim.mCurrentFrame];
    }

    mVelX = mSpriteScale * frameVelX;

    FP gridSize = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        gridSize = -ScaleToGridSize(mSpriteScale);
    }
    else
    {
        gridSize = ScaleToGridSize(mSpriteScale);
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), gridSize * FP_FromDouble(0.75)))
    {
        KnockBack();
        mCurrentMotion = eScrabMotions::M_JumpToFall_8_4A9220;
    }
    else
    {
        mXPos += mVelX;
        if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            SFX_Play_Pitch(SoundEffect::PickupItem_28, 50, -800);
            BaseAliveGameObjectLastLineYPos = mYPos;
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                mVelX = mSpriteScale * FP_FromDouble(-5.67);
            }
            else
            {
                mVelX = mSpriteScale * FP_FromDouble(5.67);
            }
            mVelY = mSpriteScale * FP_FromDouble(-9.8);
            mYPos += mVelY;
            VOnTrapDoorOpen();
            mCurrentMotion = eScrabMotions::M_HopMidair_6_4A9490;
            BaseAliveGameObjectCollisionLine = nullptr;
        }
    }
}

const FP sScrabHopMidAirVelX_546F34[8] = {
    FP_FromDouble(5.00537109375),
    FP_FromDouble(6.8809356689453125),
    FP_FromDouble(15.796859741210938),
    FP_FromDouble(18.309295654296875),
    FP_FromDouble(15.494369506835938),
    FP_FromDouble(15.187164306640625),
    FP_FromDouble(6.1830902099609375),
    FP_FromDouble(5.00537109375)};

void Scrab::M_HopMidair_6_4A9490()
{
    EventBroadcast(kEventNoise, this);

    FP frameVelX = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        frameVelX = -sScrabHopMidAirVelX_546F34[mAnim.mCurrentFrame];
    }
    else
    {
        frameVelX = sScrabHopMidAirVelX_546F34[mAnim.mCurrentFrame];
    }

    mVelX = mSpriteScale * frameVelX;

    FP kGridSize = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        kGridSize = -ScaleToGridSize(mSpriteScale);
    }
    else
    {
        kGridSize = ScaleToGridSize(mSpriteScale);
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), kGridSize * FP_FromDouble(0.75)))
    {
        KnockBack();
    }
    else
    {
        FP hitX = {};
        FP hitY = {};
        PathLine* pLine = nullptr;
        const auto bCollision = InAirCollision(&pLine, &hitX, &hitY, FP_FromDouble(1.8));

        if (sControlledCharacter == this)
        {
            SetActiveCameraDelayedFromDir();
        }

        if (bCollision)
        {
            switch (pLine->mLineType)
            {
                case eLineTypes::eFloor_0:
                case eLineTypes::eBackgroundFloor_4:
                case eLineTypes::eDynamicCollision_32:
                case eLineTypes::eBackgroundDynamicCollision_36:
                    BaseAliveGameObjectCollisionLine = pLine;
                    ToStand();
                    mCurrentMotion = eScrabMotions::M_HopLand_7_4A9890;
                    PlatformCollide();
                    mXPos = hitX;
                    mYPos = hitY;
                    return;

                case eLineTypes::eWallLeft_1:
                case eLineTypes::eWallRight_2:
                    mVelX = (-mVelX / FP_FromInteger(2));
                    return;
            }
        }

        if (mYPos - BaseAliveGameObjectLastLineYPos > FP_FromInteger(5))
        {
            field_134_falling_velx_scale_factor = FP_FromDouble(1.25);
            mCurrentMotion = eScrabMotions::M_JumpToFall_8_4A9220;
        }
    }
}

const FP sScrabHopLandVelX_546F54[4] = {
    FP_FromDouble(2.77862548828125),
    FP_FromDouble(0.9429779052734375),
    FP_FromDouble(0.4394683837890625),
    FP_FromDouble(0.388397216796875)};

void Scrab::M_HopLand_7_4A9890()
{
    if (mAnim.mCurrentFrame == 0)
    {
        Environment_SFX_457A40(EnvironmentSfx::eHitGroundSoft_6, 80, 400, this);
        Scrab_SFX(ScrabSounds::eHitCollision_4, 0, 0x7FFF, 1);
    }

    EventBroadcast(kEventNoise, this);

    FP frameVelX = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        frameVelX = -sScrabHopLandVelX_546F54[mAnim.mCurrentFrame];
    }
    else
    {
        frameVelX = sScrabHopLandVelX_546F54[mAnim.mCurrentFrame];
    }
    mVelX = (mSpriteScale * frameVelX);

    FP gridSize = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        gridSize = -ScaleToGridSize(mSpriteScale);
    }
    else
    {
        gridSize = ScaleToGridSize(mSpriteScale);
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), gridSize * FP_FromDouble(0.75)))
    {
        KnockBack();
    }
    else
    {
        MoveOnLine();
        if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            ToStand();
        }
    }
}

void Scrab::M_JumpToFall_8_4A9220()
{
    if (mVelX > FP_FromInteger(0))
    {
        mVelX = mVelX - (mSpriteScale * field_134_falling_velx_scale_factor);
        if (mVelX < FP_FromInteger(0))
        {
            mVelX = FP_FromInteger(0);
        }
    }
    else if (mVelX < FP_FromInteger(0))
    {
        mVelX = (mSpriteScale * field_134_falling_velx_scale_factor) + mVelX;
        if (mVelX > FP_FromInteger(0))
        {
            mVelX = FP_FromInteger(0);
        }
    }

    PathLine* pLine = nullptr;
    FP hitX = {};
    FP hitY = {};
    const auto bHit = InAirCollision(&pLine, &hitX, &hitY, FP_FromDouble(1.8));
    if (sControlledCharacter == this)
    {
        SetActiveCameraDelayedFromDir();
    }

    if (bHit)
    {
        switch (pLine->mLineType)
        {
            case eLineTypes::eFloor_0:
            case eLineTypes::eBackgroundFloor_4:
            case eLineTypes::eDynamicCollision_32:
            case eLineTypes::eBackgroundDynamicCollision_36:
                BaseAliveGameObjectCollisionLine = pLine;
                mCurrentMotion = eScrabMotions::M_Fall_20_4A93E0;
                PlatformCollide();
                mYPos = hitY;
                mXPos = hitX;
                MapFollowMe(TRUE);
                break;
            case eLineTypes::eWallLeft_1:
            case eLineTypes::eWallRight_2:
                mVelX = (-mVelX / FP_FromInteger(2));
                break;
            default:
                return;
        }
    }
}

const FP sStandToWalkVels_546E48[3] = {
    FP_FromDouble(1.12),
    FP_FromDouble(2.37),
    FP_FromDouble(3.20)};

void Scrab::M_StandToWalk_9_4A8450()
{
    mVelX = (mSpriteScale * sStandToWalkVels_546E48[mAnim.mCurrentFrame & 3]); // TODO: Check size of 3 is correct
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mVelX = -mVelX;
    }

    MoveOnLine();

    if (mCurrentMotion == eScrabMotions::M_StandToWalk_9_4A8450)
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            mCurrentMotion = eScrabMotions::M_Walk_1_4A84D0;
        }
    }
}

const FP sStandToRunVels_546EB8[3] = {
    FP_FromDouble(2.29),
    FP_FromDouble(2.69),
    FP_FromDouble(3.88)};

void Scrab::M_StandToRun_10_4A8900()
{
    mVelX = mSpriteScale * sStandToRunVels_546EB8[mAnim.mCurrentFrame & 3]; // TODO: Check size of 3 is correct
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mVelX = -mVelX;
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), mVelX))
    {
        KnockBack();
    }
    else
    {
        MoveOnLine();

        if (mCurrentMotion == eScrabMotions::M_StandToRun_10_4A8900)
        {
            if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                mCurrentMotion = eScrabMotions::M_Run_2_4A89C0;
            }
        }
    }
}

const FP sWalkToStandVels_546EAC[3] = {
    FP_FromDouble(1.04),
    FP_FromDouble(3.29),
    FP_FromDouble(2.86)};

void Scrab::M_WalkToStand_11_4A8880()
{
    mVelX = (mSpriteScale * sWalkToStandVels_546EAC[mAnim.mCurrentFrame & 3]); // TODO: Check size of 3 is correct
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mVelX = -mVelX;
    }

    MoveOnLine();

    if (mCurrentMotion == eScrabMotions::M_WalkToStand_11_4A8880)
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            TryMoveOrStand();
        }
    }
}

const FP dword_546F64[8] = {
    FP_FromDouble(11.25),
    FP_FromDouble(10.49),
    FP_FromDouble(9.67),
    FP_FromDouble(9.01),
    FP_FromDouble(7.47),
    FP_FromDouble(7.03),
    FP_FromDouble(6.69),
    FP_FromDouble(6.44)};

void Scrab::M_RunJumpBegin_12_4A99C0()
{
    if (mAnim.mCurrentFrame == 1)
    {
        SFX_Play_Pitch(SoundEffect::PickupItem_28, 50, -800);
    }

    EventBroadcast(kEventNoise, this);

    FP velX = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        velX = -dword_546F64[mAnim.mCurrentFrame & 8]; // TODO: Check size
    }
    else
    {
        velX = dword_546F64[mAnim.mCurrentFrame & 8]; // TODO: Check size
    }

    mVelX = mSpriteScale * velX;

    if (WallHit(mSpriteScale * FP_FromInteger(45), mVelX))
    {
        KnockBack();
    }
    else
    {
        FP hitX = {};
        FP hitY = {};
        PathLine* pLine = nullptr;
        const auto bHit = InAirCollision(&pLine, &hitX, &hitY, FP_FromDouble(1.8));
        if (sControlledCharacter == this)
        {
            SetActiveCameraDelayedFromDir();
        }

        if (bHit)
        {
            switch (pLine->mLineType)
            {
                case eLineTypes::eFloor_0:
                case eLineTypes::eBackgroundFloor_4:
                case eLineTypes::eDynamicCollision_32:
                case eLineTypes::eBackgroundDynamicCollision_36:
                    BaseAliveGameObjectCollisionLine = pLine;
                    ToStand();
                    mCurrentMotion = eScrabMotions::M_RunJumpEnd_13_4A9BE0;
                    PlatformCollide();
                    mXPos = hitX;
                    mYPos = hitY;
                    return;

                case eLineTypes::eWallLeft_1:
                case eLineTypes::eWallRight_2:
                    mVelX = (-mVelX / FP_FromInteger(2));
                    return;
                default:
                    break;
            }
        }

        if (mYPos - BaseAliveGameObjectLastLineYPos > FP_FromInteger(5))
        {
            field_134_falling_velx_scale_factor = FP_FromDouble(1.25);
            mCurrentMotion = eScrabMotions::M_JumpToFall_8_4A9220;
        }
    }
}

const FP sEndRunJumpVels_546F84[21] = {
    FP_FromDouble(2.68),
    FP_FromDouble(2.65),
    FP_FromDouble(2.72),
    FP_FromDouble(2.89),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0),
    FP_FromInteger(0)};


void Scrab::M_RunJumpEnd_13_4A9BE0()
{
    EventBroadcast(kEventNoise, this);
    mVelX = (mSpriteScale * sEndRunJumpVels_546F84[mAnim.mCurrentFrame & 21]); // TODO: Check size
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mVelX = -mVelX;
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), mVelX))
    {
        KnockBack();
    }
    else
    {
        MoveOnLine();
        if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            Scrab_SFX(ScrabSounds::eHitCollision_4, 0, 0x7FFF, 1);
            if (!ToNextMotion())
            {
                ToStand();
            }
        }
    }
}

void Scrab::M_WalkToFall_14_4A9460()
{
    M_JumpToFall_8_4A9220();
    if (mCurrentMotion == eScrabMotions::M_Stand_0_4A8220)
    {
        mCurrentMotion = eScrabMotions::M_HopLand_7_4A9890;
    }
}

void Scrab::M_RunToFall_15_4A9430()
{
    M_JumpToFall_8_4A9220();
    if (mCurrentMotion == eScrabMotions::M_Stand_0_4A8220)
    {
        mCurrentMotion = eScrabMotions::M_HopLand_7_4A9890;
    }
}

void Scrab::M_WalkToRun_16_4A8D60()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mCurrentMotion = eScrabMotions::M_Run_2_4A89C0;
        mNextMotion = -1;
    }
}

void Scrab::M_RunToWalk_17_4A8D90()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mCurrentMotion = eScrabMotions::M_Walk_1_4A84D0;
        mNextMotion = -1;
    }
}

void Scrab::M_Knockback_18_4AA490()
{
    if (BaseAliveGameObjectCollisionLine)
    {
        MoveOnLine();
    }
    else
    {
        M_JumpToFall_8_4A9220();
    }

    if (mAnim.mCurrentFrame == 0)
    {
        Environment_SFX_457A40(EnvironmentSfx::eHitGroundSoft_6, 80, 400, this);
    }

    else if (mAnim.mCurrentFrame == 2)
    {
        Scrab_SFX(ScrabSounds::eHitCollision_4, 0, 0x7FFF, 1);
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        if (BaseAliveGameObjectCollisionLine)
        {
            ToStand();
        }
        else
        {
            mCurrentMotion = eScrabMotions::M_JumpToFall_8_4A9220;
        }
    }
}

void Scrab::M_GetEaten_19_4AA3E0()
{
    if (mAnim.mCurrentFrame == 3)
    {
        SFX_Play_Pitch(SoundEffect::PickupItem_28, 120, -1000);
    }
}

void Scrab::M_Fall_20_4A93E0()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        Environment_SFX_457A40(EnvironmentSfx::eHitGroundSoft_6, 80, 400, this);
        Scrab_SFX(ScrabSounds::eHitCollision_4, 0, 0x7FFF, 1);
        ToStand();
        mNextMotion = -1;
    }
}

void Scrab::M_Stamp_21_4A9CC0()
{
    if (mAnim.mCurrentFrame == 9)
    {
        if (BrainIs(&Scrab::Brain_5_Possessed_4A6180))
        {
            KillTarget(nullptr);
        }

        Scrab_SFX(ScrabSounds::eHitCollision_4, 0, 0x7FFF, 1);
        SFX_Play_Pitch(SoundEffect::KillEffect_64, 60, Math_RandomRange(-255, 255));
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        Scrab_SFX(ScrabSounds::eHowl_0, 60, 511, 1);

        if (BrainIs(&Scrab::Brain_5_Possessed_4A6180))
        {
            ToStand();
        }
        else
        {
            ToNextMotion();
        }
    }
}

void Scrab::M_GetPossessed_22_4AA420()
{
    if (mAnim.mCurrentFrame == 9)
    {
        Scrab_SFX(ScrabSounds::eHitCollision_4, 0, 0x7FFF, 1);
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        if (mNextMotion)
        {
            field_1AA_flags.Set(Flags_1AA::eBit4_force_update_animation);
            mCurrentMotion = eScrabMotions::M_GetPossessed_22_4AA420;
        }
        else
        {
            ToStand();
        }
    }
}

void Scrab::M_Empty_23_4A9D80()
{
    
}

void Scrab::M_DeathEnd_24_4AA140()
{
    if (!BaseAliveGameObjectCollisionLine)
    {
        mVelX = FP_FromInteger(0);
        M_JumpToFall_8_4A9220();
        if (mCurrentMotion != eScrabMotions::M_DeathEnd_24_4AA140)
        {
            mCurrentMotion = eScrabMotions::M_DeathEnd_24_4AA140;
        }
    }
}

void Scrab::M_Empty_25_4A34D0()
{
    
}

void Scrab::M_HowlBegin_26_4A9DA0()
{
    if (mAnim.mCurrentFrame == 2)
    {
        if (sGnFrame & 1)
        {
            EventBroadcast(kEventLoudNoise, this);
            Scrab_SFX(ScrabSounds::eYell_8, 0, Math_RandomRange(-1600, -900), 1);
            if (BrainIs(&Scrab::Brain_5_Possessed_4A6180))
            {
                pEventSystem_5BC11C->PushEvent(GameSpeakEvents::Scrab_Howl_53);
            }
        }
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        field_178_shred_power_active = 1;

        if (mNextMotion == -1)
        {
            ToStand();
        }
        else
        {
            mCurrentMotion = eScrabMotions::M_HowlEnd_27_4A9E60;
        }
    }
}

void Scrab::M_HowlEnd_27_4A9E60()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        if (BrainIs(&Scrab::Brain_5_Possessed_4A6180))
        {
            if (!ToNextMotion())
            {
                ToStand();
            }
        }
        else
        {
            ToNextMotion();
        }
    }
}

void Scrab::M_GetDepossessedBegin_28_4AA200()
{
    field_178_shred_power_active = 0;

    if (sControlledCharacter == this)
    {
        if (mAnim.mCurrentFrame == 2)
        {
            Scrab_SFX(ScrabSounds::eYell_8, 0, Math_RandomRange(-1600, -900), 1);
        }

        if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            if (!Input_IsChanting_45F260())
            {
                mCurrentMotion = eScrabMotions::M_GetDepossessedEnd_29_4AA3C0;
            }
        }

        if (!(static_cast<s32>(sGnFrame) % 4))
        {
            const FP xRnd = FP_FromInteger(Math_RandomRange(-20, 20));
            const FP yRnd = FP_FromInteger(Math_RandomRange(20, 50));
            const FP ypos = mYPos - (mSpriteScale * yRnd);
            const FP xpos = (mSpriteScale * xRnd) + mXPos;
            New_TintChant_Particle(xpos, ypos, mSpriteScale, Layer::eLayer_0);
        }

        if (static_cast<s32>(sGnFrame) > field_130_depossession_timer || sActiveHero->mHealth <= FP_FromInteger(0))
        {
            sControlledCharacter = sActiveHero;
            mBaseAliveGameObjectFlags.Clear(Flags_114::e114_Bit4_bPossesed);
            field_1A2_speak_counter = 0;
            MusicController::static_PlayMusic(MusicController::MusicTypes::eNone_0, this, 0, 0);
            mCurrentMotion = eScrabMotions::M_GetDepossessedEnd_29_4AA3C0;
            ToPatrol();
            field_11C_brain_sub_state = Brain_0_Patrol::eBrain0_ToMoving_0;
            gMap.SetActiveCam(field_166_return_level, field_168_return_path, field_16A_return_camera, CameraSwapEffects::eInstantChange_0, 0, 0);
        }
    }
}

void Scrab::M_GetDepossessedEnd_29_4AA3C0()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToStand();
    }
}

void Scrab::M_Shriek_30_4A9EA0()
{
    if (mAnim.mCurrentFrame == 4)
    {
        if (sGnFrame & 1)
        {
            EventBroadcast(kEventLoudNoise, this);
            Scrab_SFX(ScrabSounds::eHowl_0, 0, 0x7FFF, 1);
            if (BrainIs(&Scrab::Brain_5_Possessed_4A6180))
            {
                pEventSystem_5BC11C->PushEvent(GameSpeakEvents::Scrab_Shriek_54);
            }
        }
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        if (!ToNextMotion())
        {
            ToStand();
        }
    }
}

void Scrab::M_ScrabBattleAnim_31_4A9F30()
{
    if (mAnim.mCurrentFrame == 0)
    {
        field_160_sfx_bitmask = Scrab_SFX(ScrabSounds::eShredding_5, 100, Math_RandomRange(-600, 200), 1);
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToNextMotion();
    }
}

void Scrab::M_AttackSpin_32_4A8DC0()
{
    auto pObj = static_cast<BaseAliveGameObject*>(sObjectIds.Find_Impl(field_120_obj_id));

    field_178_shred_power_active = 0;
    if (Expired(field_12C_timer))
    {
        mCurrentMotion = eScrabMotions::M_Run_2_4A89C0;
        return;
    }

    if (mAnim.mCurrentFrame == 0)
    {
        field_160_sfx_bitmask = Scrab_SFX(ScrabSounds::eShredding_5, 100, Math_RandomRange(-600, 200), 1);
    }

    const FP kGridSize = ScaleToGridSize(mSpriteScale);
    if (sControlledCharacter != this)
    {
        if (pObj)
        {
            if (pObj->mXPos <= mXPos)
            {
                mVelX = -(kGridSize / FP_FromDouble(3.5));
            }
            else
            {
                mVelX = (kGridSize / FP_FromDouble(3.5));
            }
        }
        else
        {
            mVelX = FP_FromInteger(0);
        }
    }
    else
    {
        if (Input().isPressed(InputCommands::Enum::eThrowItem | InputCommands::Enum::eDoAction))
        {
            if (!Input().isPressed(sInputKey_Left | sInputKey_Right))
            {
                mVelX = FP_FromInteger(0);
            }

            if (Input().isPressed(sInputKey_Run))
            {
                if (Input().isPressed(sInputKey_Left))
                {
                    mVelX = -(kGridSize / FP_FromDouble(3.5));
                }

                if (Input().isPressed(sInputKey_Right))
                {
                    mVelX = (kGridSize / FP_FromDouble(3.5));
                }
            }
            else
            {
                if (Input().isPressed(sInputKey_Left))
                {
                    mVelX = -(kGridSize / FP_FromInteger(7));
                }

                if (Input().isPressed(sInputKey_Right))
                {
                    mVelX = (kGridSize / FP_FromInteger(7));
                }
            }
        }
        else
        {
            mCurrentMotion = eScrabMotions::M_Run_2_4A89C0;
        }
    }

    MapFollowMe(0);

    if (WallHit(mSpriteScale * FP_FromInteger(45), mVelX))
    {
        mVelX = (-mVelX / FP_FromInteger(2));
    }
    else
    {
        MoveOnLine();
        if (mCurrentMotion == eScrabMotions::M_AttackSpin_32_4A8DC0)
        {
            KillTarget(pObj);
        }
    }
}

void Scrab::M_FeedToGulp_33_4A9FA0()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        if (mNextMotion == eScrabMotions::M_StandToFeed_35_4AA010)
        {
            mCurrentMotion = eScrabMotions::M_StandToFeed_35_4AA010;
        }
        else
        {
            mCurrentMotion = eScrabMotions::M_GulpToStand_34_4A9FF0;
        }
        mNextMotion = -1;
    }
}

void Scrab::M_GulpToStand_34_4A9FF0()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToStand();
    }
}

void Scrab::M_StandToFeed_35_4AA010()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mCurrentMotion = eScrabMotions::M_Feed_36_4AA030;
    }
}

void Scrab::M_Feed_36_4AA030()
{
    if (mAnim.mCurrentFrame)
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            mCurrentMotion = eScrabMotions::M_FeedToGulp_33_4A9FA0;
        }
    }
    else
    {
        SfxPlayMono((Math_NextRandom() & 1) ? SoundEffect::Eating2_66 : SoundEffect::Eating1_65, 0);
    }
}

void Scrab::M_AttackLunge_37_4AA0B0()
{
    auto pTarget = static_cast<BaseAliveGameObject*>(sObjectIds.Find_Impl(field_120_obj_id));
    KillTarget(pTarget);

    if (mAnim.mCurrentFrame == 4)
    {
        Scrab_SFX(ScrabSounds::eHowl_0, 0, 0x7FFF, 1);
    }
    else if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToStand();
    }
}

void Scrab::M_LegKick_38_4AA120()
{
    M_AttackLunge_37_4AA0B0();
}

void Scrab::M_DeathBegin_39_4AA190()
{
    if (!BaseAliveGameObjectCollisionLine)
    {
        mVelX = FP_FromInteger(0);
        M_JumpToFall_8_4A9220();
        if (mCurrentMotion != eScrabMotions::M_DeathBegin_39_4AA190)
        {
            mCurrentMotion = eScrabMotions::M_DeathBegin_39_4AA190;
        }
    }
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mCurrentMotion = eScrabMotions::M_DeathEnd_24_4AA140;
    }
}

s16 Scrab::VOnSameYLevel(BaseAnimatedWithPhysicsGameObject* pOther)
{
    const PSX_RECT ourRect = VGetBoundingRect();
    const PSX_RECT otherRect = pOther->VGetBoundingRect();

    const FP k10Scaled = (FP_FromInteger(10) * mSpriteScale);
    if (FP_FromInteger(ourRect.y) <= (FP_FromInteger(otherRect.h) - k10Scaled) && ourRect.y >= otherRect.y)
    {
        return TRUE;
    }

    if (ourRect.h <= otherRect.h)
    {
        if (FP_FromInteger(ourRect.h) >= (k10Scaled + FP_FromInteger(otherRect.y)))
        {
            return TRUE;
        }
    }

    if (ourRect.y >= otherRect.y)
    {
        if (ourRect.h <= otherRect.h)
        {
            return TRUE;
        }

        if (ourRect.y > otherRect.y)
        {
            return FALSE;
        }
    }

    if (ourRect.h < otherRect.h)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void Scrab::ToPatrol()
{
    SetBrain(&Scrab::Brain_0_Patrol_4AA630);
    field_138_unused = mXPos;
}

void Scrab::ToStand()
{
    field_134_falling_velx_scale_factor = FP_FromInteger(0);
    mVelX = FP_FromInteger(0);
    mVelY = FP_FromInteger(0);
    mCurrentMotion = eScrabMotions::M_Stand_0_4A8220;
    MapFollowMe(TRUE);
}

void Scrab::MoveOnLine()
{
    BaseGameObject* pObj = sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId);
    const FP oldXPos = mXPos;
    if (BaseAliveGameObjectCollisionLine)
    {
        BaseAliveGameObjectCollisionLine = BaseAliveGameObjectCollisionLine->MoveOnLine(&mXPos, &mYPos, mVelX);
        if (BaseAliveGameObjectCollisionLine)
        {
            if (pObj)
            {
                if (BaseAliveGameObjectCollisionLine->mLineType != eLineTypes::eDynamicCollision_32 &&
                    BaseAliveGameObjectCollisionLine->mLineType != eLineTypes::eBackgroundDynamicCollision_36)
                {
                    const auto oldMotion = mCurrentMotion;
                    VOnTrapDoorOpen();
                    mCurrentMotion = oldMotion;
                }
            }
            else if (BaseAliveGameObjectCollisionLine->mLineType == eLineTypes::eDynamicCollision_32 ||
                     BaseAliveGameObjectCollisionLine->mLineType == eLineTypes::eBackgroundDynamicCollision_36)
            {
                PlatformCollide();
            }
        }
        else
        {
            VOnTrapDoorOpen();
            BaseAliveGameObjectLastLineYPos = mYPos;
            field_134_falling_velx_scale_factor = FP_FromInteger(1);
            mXPos = oldXPos + mVelX;
            if (mCurrentMotion == eScrabMotions::M_Walk_1_4A84D0)
            {
                mCurrentMotion = eScrabMotions::M_WalkToFall_14_4A9460;
            }
            else
            {
                mCurrentMotion = eScrabMotions::M_RunToFall_15_4A9430;
            }
        }
    }
    else
    {
        mCurrentMotion = eScrabMotions::M_JumpToFall_8_4A9220;
        BaseAliveGameObjectLastLineYPos = mYPos;
    }
}

void Scrab::PlatformCollide()
{
    const PSX_RECT bRect = VGetBoundingRect();
    const PSX_Point xy = {bRect.x, static_cast<s16>(bRect.y + 5)};
    const PSX_Point wh = {bRect.w, static_cast<s16>(bRect.h + 5)};
    VOnCollisionWith(xy, wh, ObjList_5C1B78, 1, (TCollisionCallBack) &BaseAliveGameObject::OnTrapDoorIntersection);
}

BaseAliveGameObject* Scrab::Find_Fleech()
{
    for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
    {
        auto pObj = gBaseGameObjects->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj->Type() == ReliveTypes::eFleech)
        {
            auto pAliveObj = static_cast<BaseAliveGameObject*>(pObj);
            if (pAliveObj->mHealth > FP_FromInteger(0))
            {
                if (pAliveObj->VOnSameYLevel(pAliveObj))
                {
                    if (pAliveObj->VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromInteger(3), pAliveObj))
                    {
                        if (pAliveObj->VIsFacingMe(pAliveObj))
                        {
                            if (!WallHit(mSpriteScale * FP_FromInteger(45), pAliveObj->mXPos - mXPos) && gMap.Is_Point_In_Current_Camera(pAliveObj->mCurrentLevel, pAliveObj->mCurrentPath, pAliveObj->mXPos, pAliveObj->mYPos, 0) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0))
                            {
                                return pAliveObj;
                            }
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

void Scrab::VPossessed()
{
    mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit4_bPossesed);
    field_164_prevent_depossession = 1;
    field_178_shred_power_active = 0;
    mCurrentMotion = eScrabMotions::M_GetPossessed_22_4AA420;
    mNextMotion = -1;
    vUpdateAnim();
    SetBrain(&Scrab::Brain_5_Possessed_4A6180);
    field_11C_brain_sub_state = 0;
    field_120_obj_id = -1;
    field_124_fight_target_obj_id = -1;
    field_12C_timer = sGnFrame + 35;
    field_166_return_level = gMap.mCurrentLevel;
    field_168_return_path = gMap.mCurrentPath;
    field_16A_return_camera = gMap.mCurrentCamera;
}

u8** Scrab::ResBlockForMotion(s16 motion)
{
    if (motion < eScrabMotions::M_Stamp_21_4A9CC0)
    {
        field_140_motion_resource_block_index = 0;
    }
    else if (motion < eScrabMotions::M_Empty_23_4A9D80)
    {
        field_140_motion_resource_block_index = 6;
    }
    else if (motion < eScrabMotions::M_Empty_25_4A34D0)
    {
        field_140_motion_resource_block_index = 8;
    }
    else if (motion < eScrabMotions::M_HowlBegin_26_4A9DA0)
    {
        field_140_motion_resource_block_index = 10;
    }
    else if (motion < eScrabMotions::M_Shriek_30_4A9EA0)
    {
        field_140_motion_resource_block_index = 5;
    }
    else if (motion < eScrabMotions::M_ScrabBattleAnim_31_4A9F30)
    {
        field_140_motion_resource_block_index = 9;
    }
    else if (motion < eScrabMotions::M_FeedToGulp_33_4A9FA0)
    {
        field_140_motion_resource_block_index = 4;
    }
    else if (motion < eScrabMotions::M_StandToFeed_35_4AA010)
    {
        field_140_motion_resource_block_index = 11;
    }
    else if (motion < eScrabMotions::M_AttackLunge_37_4AA0B0)
    {
        field_140_motion_resource_block_index = 1;
    }
    else if (motion < eScrabMotions::M_LegKick_38_4AA120)
    {
        field_140_motion_resource_block_index = 2;
    }
    else if (motion >= eScrabMotions::M_DeathBegin_39_4AA190)
    {
        field_140_motion_resource_block_index = motion >= 40 ? 0 : 13; // 40 == max motions ??
    }
    else
    {
        field_140_motion_resource_block_index = 3;
    }
    return field_10_resources_array.ItemAt(field_140_motion_resource_block_index);
}

void Scrab::VScreenChanged()
{
    BaseGameObject* pChaseTarget = sObjectIds.Find_Impl(field_120_obj_id);

    if (gMap.mCurrentLevel != gMap.mNextLevel || gMap.mCurrentPath != gMap.mNextPath || gMap.mOverlayId != gMap.GetOverlayId())
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
    else if (pChaseTarget)
    {
        if (pChaseTarget->mBaseGameObjectFlags.Get(BaseGameObject::eDead))
        {
            field_120_obj_id = -1;
            mNextMotion = eScrabMotions::M_Stand_0_4A8220;
            ToPatrol();
            field_11C_brain_sub_state = Brain_0_Patrol::eBrain0_ToMoving_0;
        }
    }
}

void Scrab::VRender(PrimHeader** ppOt)
{
    if (UpdateDelay() == 0)
    {
        BaseAnimatedWithPhysicsGameObject::VRender(ppOt);
    }
}

void Scrab::TryMoveOrStand()
{
    if (!ToNextMotion())
    {
        ToStand();
    }
}

s16 Scrab::ToNextMotion()
{
    MapFollowMe(TRUE);

    if (sControlledCharacter == this && mHealth > FP_FromInteger(0) && field_11C_brain_sub_state != 0)
    {
        return PlayerControlled();
    }

    if (mNextMotion == eScrabMotions::M_Turn_3_4A91A0 || mNextMotion == eScrabMotions::M_Stamp_21_4A9CC0 || mNextMotion == eScrabMotions::M_Empty_25_4A34D0 || mNextMotion == eScrabMotions::M_HowlBegin_26_4A9DA0 || mNextMotion == eScrabMotions::M_Shriek_30_4A9EA0 || mNextMotion == eScrabMotions::M_ScrabBattleAnim_31_4A9F30 || mNextMotion == eScrabMotions::M_AttackLunge_37_4AA0B0 || mNextMotion == eScrabMotions::M_LegKick_38_4AA120 || mNextMotion == eScrabMotions::M_StandToFeed_35_4AA010)
    {
        mCurrentMotion = mNextMotion;
        mNextMotion = -1;
        return 1;
    }

    const FP k45Scaled = (mSpriteScale * FP_FromInteger(45));
    const FP kGridSize = ScaleToGridSize(mSpriteScale);

    // Check if going to run into a wall.
    if (mNextMotion == eScrabMotions::M_Run_2_4A89C0)
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            if (WallHit(k45Scaled, -kGridSize))
            {
                return 0;
            }
            else
            {
                mVelX = -(kGridSize / FP_FromDouble(3.5));
                mCurrentMotion = eScrabMotions::M_StandToRun_10_4A8900;
                mNextMotion = -1;
                return 1;
            }
        }
        else
        {
            if (WallHit(k45Scaled, kGridSize))
            {
                return 0;
            }
            else
            {
                mVelX = (kGridSize / FP_FromDouble(3.5));
                mCurrentMotion = eScrabMotions::M_StandToRun_10_4A8900;
                mNextMotion = -1;
                return 1;
            }
        }
    }

    if (mNextMotion != eScrabMotions::M_Walk_1_4A84D0)
    {
        if (mNextMotion == eScrabMotions::M_Stand_0_4A8220)
        {
            ToStand();
            return 1;
        }

        if (mNextMotion != eScrabMotions::M_HopMidair_6_4A9490)
        {
            return 0;
        }

        if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            if (WallHit(k45Scaled, -kGridSize))
            {
                return 0;
            }
            else
            {
                mCurrentMotion = eScrabMotions::M_HopBegin_5_4A96C0;
                mNextMotion = -1;
                return 1;
            }
        }
        else
        {
            if (WallHit(k45Scaled, kGridSize))
            {
                return 0;
            }
            else
            {
                mCurrentMotion = eScrabMotions::M_HopBegin_5_4A96C0;
                mNextMotion = -1;
                return 1;
            }
        }
    }
    else
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            if (WallHit(k45Scaled, -kGridSize))
            {
                return 0;
            }
            else
            {
                mCurrentMotion = eScrabMotions::M_StandToWalk_9_4A8450;
                mVelX = -(kGridSize / FP_FromInteger(7));
                mNextMotion = -1;
                return 1;
            }
        }
        else
        {
            if (WallHit(k45Scaled, kGridSize))
            {
                return 0;
            }
            else
            {
                mCurrentMotion = eScrabMotions::M_StandToWalk_9_4A8450;
                mVelX = (kGridSize / FP_FromInteger(7));
                mNextMotion = -1;
                return 1;
            }
        }
    }
}

s16 Scrab::PlayerControlled()
{
    const FP kGridSize = ScaleToGridSize(mSpriteScale);
    const FP k45Scaled = (mSpriteScale * FP_FromInteger(45));

    if (Input().isPressed(sInputKey_Right))
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            mCurrentMotion = eScrabMotions::M_Turn_3_4A91A0;
            return 1;
        }
        else
        {
            if (WallHit(k45Scaled, kGridSize))
            {
                return 0;
            }

            if (Input().isPressed(sInputKey_Run))
            {
                mVelX = (kGridSize / FP_FromDouble(3.5));
                mCurrentMotion = eScrabMotions::M_StandToRun_10_4A8900;
                return 1;
            }
            else
            {
                mVelX = (kGridSize / FP_FromInteger(7));
                mCurrentMotion = eScrabMotions::M_StandToWalk_9_4A8450;
                return 1;
            }
        }
    }
    else if (Input().isPressed(sInputKey_Left))
    {
        if (!mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            mCurrentMotion = eScrabMotions::M_Turn_3_4A91A0;
            return 1;
        }
        else
        {
            if (WallHit(k45Scaled, -kGridSize))
            {
                return 0;
            }

            if (Input().isPressed(sInputKey_Run))
            {
                mVelX = -(kGridSize / FP_FromDouble(3.5));
                mCurrentMotion = eScrabMotions::M_StandToRun_10_4A8900;
                return 1;
            }
            else
            {
                mVelX = -(kGridSize / FP_FromInteger(7));
                mCurrentMotion = eScrabMotions::M_StandToWalk_9_4A8450;
                return 1;
            }
        }
    }
    else
    {
        if (Input().isPressed(sInputKey_Up))
        {
            mCurrentMotion = eScrabMotions::M_StandToFeed_35_4AA010;
            return 1;
        }

        if (Input().isPressed(sInputKey_Down))
        {
            mCurrentMotion = eScrabMotions::M_Stamp_21_4A9CC0;
            return 1;
        }

        return 0;
    }
}

void Scrab::ToJump()
{
    BaseAliveGameObjectLastLineYPos = mYPos;

    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mVelX = mSpriteScale * -FP_FromDouble(-5.67);
    }
    else
    {
        mVelX = mSpriteScale * -FP_FromDouble(5.67);
    }

    mVelY = mSpriteScale * FP_FromDouble(-9.6);
    mYPos += mVelY;

    VOnTrapDoorOpen();

    mCurrentMotion = eScrabMotions::M_RunJumpBegin_12_4A99C0;
    BaseAliveGameObjectCollisionLine = nullptr;
}

s16 Scrab::VTakeDamage(BaseGameObject* pFrom)
{
    if (mHealth <= FP_FromInteger(0))
    {
        return 0;
    }

    switch (pFrom->Type())
    {
        case ReliveTypes::eFleech:
            mHealth = mHealth - FP_FromDouble(0.13);
            if (mHealth < FP_FromInteger(0))
            {
                mHealth = FP_FromInteger(0);
            }

            if (mHealth > FP_FromInteger(0))
            {
                return 1;
            }

            if (static_cast<BaseAliveGameObject*>(pFrom)->mXPos >= mXPos)
            {
                mAnim.mFlags.Clear(AnimFlags::eBit5_FlipX);
            }
            else
            {
                mAnim.mFlags.Set(AnimFlags::eBit5_FlipX);
            }

            EventBroadcast(kScrabOrParamiteDied, this);
            SetBrain(&Scrab::Brain_4_ShrinkDeath_4A6420);
            mCurrentMotion = eScrabMotions::M_GetEaten_19_4AA3E0;
            field_12C_timer = sGnFrame + 90;
            vUpdateAnim();
            if (sControlledCharacter == this)
            {
                SND_SEQ_Play(SeqId::DeathDrums_29, 1, 127, 127);
            }
            return 0;

        case ReliveTypes::eAbilityRing:
            return 0;

        case ReliveTypes::eBullet:
        //case ReliveTypes::eNeverSet:
        case ReliveTypes::eScrab:
            break;

        default:
            SfxPlayMono(SoundEffect::KillEffect_64, 127);
            SfxPlayMono(SoundEffect::FallingItemHit_47, 90);
            break;
    }

    EventBroadcast(kScrabOrParamiteDied, this);
    mHealth = FP_FromInteger(0);
    SetBrain(&Scrab::Brain_3_Death_4A62B0);
    field_12C_timer = sGnFrame + 90;
    mCurrentMotion = eScrabMotions::M_DeathBegin_39_4AA190;
    vUpdateAnim();

    if (sControlledCharacter == this)
    {
        SND_SEQ_Play(SeqId::DeathDrums_29, 1, 127, 127);
    }

    return 1;
}

void Scrab::KnockBack()
{
    field_134_falling_velx_scale_factor = FP_FromDouble(0.35);

    const FP kGridSize = ScaleToGridSize(mSpriteScale);

    FP v4 = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        v4 = (kGridSize / FP_FromDouble(3.5));
    }
    else
    {
        v4 = -(kGridSize / FP_FromDouble(3.5));
    }
    mVelX = (v4 / FP_FromInteger(2));

    MapFollowMe(TRUE);

    if (mVelY < FP_FromInteger(0))
    {
        mVelY = FP_FromInteger(0);
    }

    mCurrentMotion = eScrabMotions::M_Knockback_18_4AA490;
    field_130_depossession_timer = sGnFrame + 10;
}

const SfxDefinition getSfxDef(ScrabSounds effectId)
{
    return scrab_sScrabSfx_560330[static_cast<s32>(effectId)];
}

s32 Scrab::Scrab_SFX(ScrabSounds soundId, s32 vol, s32 pitch, s16 applyDirection)
{
    s16 volumeLeft = 0;
    s16 volumeRight = 0;
    const CameraPos direction = gMap.GetDirection(
        mCurrentLevel,
        mCurrentPath,
        mXPos,
        mYPos);

    const SfxDefinition effectDef = getSfxDef(soundId);
    const s16 defaultSndIdxVol = effectDef.field_3_default_volume;

    volumeRight = static_cast<s16>(vol);
    if (vol <= 0)
    {
        volumeRight = defaultSndIdxVol;
    }

    if (mSpriteScale == FP_FromDouble(0.5))
    {
        volumeRight /= 2;
    }

    if (applyDirection)
    {
        PSX_RECT pRect = {};
        gMap.Get_Camera_World_Rect(direction, &pRect);
        switch (direction)
        {
            case CameraPos::eCamCurrent_0:
                volumeLeft = volumeRight;
                break;
            case CameraPos::eCamTop_1:
            case CameraPos::eCamBottom_2:
            {
                volumeLeft = FP_GetExponent(FP_FromInteger(defaultSndIdxVol * 2) / FP_FromInteger(3));
                volumeRight = volumeLeft;
            }
            break;
            case CameraPos::eCamLeft_3:
            {
                const FP percentHowFar = (FP_FromInteger(pRect.w) - mXPos) / FP_FromInteger(368);
                volumeLeft = volumeRight - FP_GetExponent(percentHowFar * FP_FromInteger(volumeRight - (volumeRight / 3)));
                volumeRight = volumeRight - FP_GetExponent(percentHowFar * FP_FromInteger(volumeRight));
            }
            break;
            case CameraPos::eCamRight_4:
            {
                const FP percentHowFar = (mXPos - FP_FromInteger(pRect.x)) / FP_FromInteger(368);
                volumeLeft = volumeRight - FP_GetExponent(percentHowFar * FP_FromInteger(volumeRight));
                volumeRight = volumeRight - FP_GetExponent(percentHowFar * FP_FromInteger(volumeRight - (volumeRight / 3)));
            }
            break;
            default:
                return 0;
        }
    }
    else
    {
        volumeLeft = volumeRight;
    }

    return SFX_SfxDefinition_Play_Stereo(
        &effectDef,
        volumeLeft,
        volumeRight,
        static_cast<s16>(pitch),
        static_cast<s16>(pitch));
}

void Scrab::KillTarget(BaseAliveGameObject* pTarget)
{
    bool bKilledTarget = false;
    bool bKillSpecific = false;

    if (Is_In_Current_Camera() == CameraPos::eCamCurrent_0)
    {
        if (!BrainIs(&Scrab::Brain_2_Fighting_4A5840))
        {
            if (!BrainIs(&Scrab::Brain_5_Possessed_4A6180) || mCurrentMotion == eScrabMotions::M_LegKick_38_4AA120 || mCurrentMotion == eScrabMotions::M_Stamp_21_4A9CC0 || mCurrentMotion == eScrabMotions::M_AttackSpin_32_4A8DC0)
            {
                const PSX_RECT bOurRect = VGetBoundingRect();
                BaseAliveGameObject* pObj = pTarget;

                s32 list_idx = 0;
                if (pTarget)
                {
                    bKillSpecific = true;
                }
                else if (gBaseAliveGameObjects_5C1B7C->field_4_used_size > 0)
                {
                    list_idx = 1;
                    pObj = gBaseAliveGameObjects_5C1B7C->ItemAt(0);
                }
                else
                {
                    pObj = nullptr;
                }

                do
                {
                    if (pObj->mBaseGameObjectFlags.Get(BaseGameObject::eIsBaseAliveGameObject_Bit6))
                    {
                        if (pObj != this)
                        {
                            if ((pObj->Type() == ReliveTypes::eAbe ||
                                pObj->Type() == ReliveTypes::eRingOrLiftMud ||
                                pObj->Type() == ReliveTypes::eMudokon ||
                                //pObj->Type() == ReliveTypes::eNevetSet ||
                                pObj->Type() == ReliveTypes::eFleech ||
                                pObj->Type() == ReliveTypes::eScrab) &&
                                mScale == pObj->mScale && pObj->mHealth > FP_FromInteger(0))
                            {
                                const FP xDist = pObj->mXPos - mXPos;
                                if (!WallHit(mSpriteScale * FP_FromInteger(45), xDist))
                                {
                                    if (!pObj->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible))
                                    {
                                        if (pObj->Type() != ReliveTypes::eScrab ||
                                            !pObj->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit4_bPossesed) ||
                                            (pObj->mCurrentMotion != eScrabMotions::M_AttackSpin_32_4A8DC0 &&
                                            (pObj->Type() != ReliveTypes::eFleech || BrainIs(&Scrab::Brain_5_Possessed_4A6180) || field_1A8_bKill_enemy == Choice_short::eYes_1)))

                                        {
                                            const PSX_RECT objRect = pObj->VGetBoundingRect();

                                            if (PSX_Rects_overlap_no_adjustment(&objRect, &bOurRect))
                                            {
                                                if (pObj->VTakeDamage(this))
                                                {
                                                    bKilledTarget = true;
                                                    SfxPlayMono(SoundEffect::KillEffect_64, 0);
                                                    if (pObj->Type() == ReliveTypes::eAbe)
                                                    {
                                                        Mudokon_SFX(MudSounds::eHurt2_9, 0, 0, sActiveHero);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (bKillSpecific)
                    {
                        break;
                    }

                    if (bKilledTarget)
                    {
                        break;
                    }

                    if (list_idx >= gBaseAliveGameObjects_5C1B7C->Size())
                    {
                        break;
                    }

                    pObj = gBaseAliveGameObjects_5C1B7C->ItemAt(list_idx++);
                }
                while (pObj);
            }
        }
    }
}

s16 Scrab::FindAbeOrMud()
{
    if (CanSeeAbe(sActiveHero) && sActiveHero->mHealth > FP_FromInteger(0) && sActiveHero->mSpriteScale == mSpriteScale && !sActiveHero->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible))
    {
        if (!WallHit(mSpriteScale * FP_FromInteger(45), sActiveHero->mXPos - mXPos))
        {
            field_120_obj_id = sActiveHero->mBaseGameObjectId;
            return TRUE;
        }
    }

    for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
    {
        BaseGameObject* pObj = gBaseGameObjects->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj->mBaseGameObjectFlags.Get(BaseGameObject::eIsBaseAliveGameObject_Bit6))
        {
            auto pAliveObj = static_cast<BaseAliveGameObject*>(pObj);
            if ((pAliveObj->Type() == ReliveTypes::eRingOrLiftMud || pAliveObj->Type() == ReliveTypes::eMudokon || pAliveObj->Type() == ReliveTypes::eScrab) && (pAliveObj->Type() != ReliveTypes::eScrab || pAliveObj->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit4_bPossesed)) && CanSeeAbe(pAliveObj) && pAliveObj->mHealth > FP_FromInteger(0) && pAliveObj->mSpriteScale == mSpriteScale)
            {
                if (!WallHit(mSpriteScale * FP_FromInteger(45), pAliveObj->mXPos - mXPos))
                {
                    field_120_obj_id = pAliveObj->mBaseGameObjectId;
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

s16 Scrab::CanSeeAbe(BaseAliveGameObject* pObj)
{
    if (pObj->mSpriteScale != mSpriteScale)
    {
        return 0;
    }

    if (IsActiveHero(pObj))
    {
        if (sActiveHero->mCurrentMotion == eAbeMotions::Motion_67_LedgeHang_454E20 || sActiveHero->mCurrentMotion == eAbeMotions::Motion_69_LedgeHangWobble_454EF0)
        {
            return VOnSameYLevel(pObj);
        }
    }

    if (pObj->mYPos <= (mYPos - (mSpriteScale * FP_FromInteger(45))) || !LineOfSightTo(this, pObj))
    {
        return 0;
    }

    if (VOnSameYLevel(pObj))
    {
        FP offX = {};
        if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            offX = -ScaleToGridSize(mSpriteScale);
        }
        else
        {
            offX = ScaleToGridSize(mSpriteScale);
        }

        if (WallHit(mSpriteScale * FP_FromInteger(45), offX))
        {
            return 0;
        }
    }

    return 1;
}


bool Scrab::LineOfSightTo(Scrab* pThis, BaseAliveGameObject* pObj)
{
    const PSX_RECT objRect = pObj->VGetBoundingRect();
    const PSX_RECT bRect = pThis->VGetBoundingRect();

    PathLine* pLine = nullptr;
    FP hitX = {};
    FP hitY = {};
    return sCollisions->Raycast(
               pThis->mXPos,
               FP_FromInteger((bRect.y + bRect.h) / 2),
               pObj->mXPos,
               FP_FromInteger((objRect.y + objRect.h) / 2),
               &pLine,
               &hitX,
               &hitY,
               pThis->mScale == Scale::Fg ? kFgFloorWallOrCeiling
               : kBgFloorWallOrCeiling)
        != 1;
} 

Scrab* Scrab::FindScrabToFight()
{
    Scrab* pScrabIAmFightingAlready = nullptr;
    Scrab* pScrabNotInAFight = nullptr;
    Scrab* pScrabInFightWithSomeoneElse = nullptr;

    for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
    {
        BaseGameObject* pObj = gBaseGameObjects->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj->Type() == ReliveTypes::eScrab)
        {
            auto pScrab = static_cast<Scrab*>(pObj);

            if (pScrab != this && !pScrab->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit4_bPossesed) && !BrainIs(&Scrab::Brain_3_Death_4A62B0))
            {
                if (VOnSameYLevel(pScrab))
                {
                    if (!WallHit(mSpriteScale * FP_FromInteger(45), pScrab->mXPos - mXPos) && gMap.Is_Point_In_Current_Camera(pScrab->mCurrentLevel, pScrab->mCurrentPath, pScrab->mXPos, pScrab->mYPos, 0) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0))
                    {
                        if (pScrab->field_124_fight_target_obj_id == -1)
                        {
                            pScrabNotInAFight = pScrab;
                        }
                        else
                        {
                            if (pScrab->field_124_fight_target_obj_id == mBaseGameObjectId)
                            {
                                pScrabIAmFightingAlready = pScrab;
                            }
                            else
                            {
                                pScrabInFightWithSomeoneElse = pScrab;
                            }
                        }
                    }
                }
            }
        }
    }

    if (pScrabIAmFightingAlready)
    {
        return pScrabIAmFightingAlready;
    }

    if (pScrabNotInAFight)
    {
        return pScrabNotInAFight;
    }

    // This can be nullptr
    return pScrabInFightWithSomeoneElse;
}

s16 Scrab::Handle_SlamDoor_or_EnemyStopper(FP velX, s16 bCheckLeftRightBounds)
{
    if (velX < FP_FromInteger(0))
    {
        velX = -velX;
    }

    FP gridSize = ScaleToGridSize(mSpriteScale);
    if (velX > FP_FromInteger(5))
    {
        gridSize = (gridSize * FP_FromInteger(2));
    }

    TlvTypes objectType = {};
    Path_EnemyStopper::StopDirection stopDirection = Path_EnemyStopper::StopDirection::Both_2;
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        objectType = TlvTypes::ScrabLeftBound_43;
        stopDirection = Path_EnemyStopper::StopDirection::Left_0;
        gridSize = -gridSize;
    }
    else
    {
        objectType = TlvTypes::ScrabRightBound_44;
        stopDirection = Path_EnemyStopper::StopDirection::Right_1;
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), gridSize * FP_FromInteger(1)))
    {
        return 1;
    }

    BaseAliveGameObjectPathTLV = sPathInfo->TLV_Get_At_4DB4B0(
        FP_GetExponent(mXPos),
        FP_GetExponent(FP_Abs(mYPos)),
        FP_GetExponent(mXPos + gridSize),
        FP_GetExponent(mYPos - ScaleToGridSize(mSpriteScale)),
        TlvTypes::SlamDoor_85);

    auto pSlamDoorTlv = static_cast<Path_SlamDoor*>(BaseAliveGameObjectPathTLV);
    if (pSlamDoorTlv && ((pSlamDoorTlv->field_10_bStart_closed == Choice_short::eYes_1 && !SwitchStates_Get(pSlamDoorTlv->field_14_switch_id)) || (pSlamDoorTlv->field_10_bStart_closed == Choice_short::eNo_0 && SwitchStates_Get(pSlamDoorTlv->field_14_switch_id))))
    {
        return 1;
    }

    BaseAliveGameObjectPathTLV = sPathInfo->TLV_Get_At_4DB4B0(
        FP_GetExponent(mXPos),
        FP_GetExponent(mYPos),
        FP_GetExponent(mXPos + gridSize),
        FP_GetExponent(mYPos - ScaleToGridSize(mSpriteScale)),
        TlvTypes::EnemyStopper_47);

    auto pPathEnemyStopper = static_cast<Path_EnemyStopper*>(BaseAliveGameObjectPathTLV);
    if (pPathEnemyStopper && (pPathEnemyStopper->mStopDirection == stopDirection || pPathEnemyStopper->mStopDirection == Path_EnemyStopper::StopDirection::Both_2) && SwitchStates_Get(pPathEnemyStopper->mSwitchId))
    {
        return 1;
    }

    if (bCheckLeftRightBounds)
    {
        if (sPathInfo->TLV_Get_At_4DB4B0(
                FP_GetExponent(mXPos),
                FP_GetExponent(FP_Abs(mYPos)),
                FP_GetExponent(mXPos + gridSize),
                FP_GetExponent(mYPos - ScaleToGridSize(mSpriteScale)),
                objectType))
        {
            return 1;
        }
    }
    return 0;
}

GameSpeakEvents Scrab::LastSpeak()
{
    if (!gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 1))
    {
        return GameSpeakEvents::eNone_m1;
    }

    if (field_17C_last_event == pEventSystem_5BC11C->field_28_last_event_index)
    {
        if (pEventSystem_5BC11C->field_20_last_event == GameSpeakEvents::eNone_m1)
        {
            return GameSpeakEvents::eNone_m1;
        }
        else
        {
            return GameSpeakEvents::eSameAsLast_m2;
        }
    }
    else
    {
        field_17C_last_event = pEventSystem_5BC11C->field_28_last_event_index;
        return pEventSystem_5BC11C->field_20_last_event;
    }
}
