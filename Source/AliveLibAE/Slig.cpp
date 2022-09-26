#include "stdafx.h"
#include "Slig.hpp"
#include "Function.hpp"
#include "../relive_lib/Collisions.hpp"
#include "../relive_lib/Shadow.hpp"
#include "../relive_lib/ShadowZone.hpp"
#include "stdlib.hpp"
#include "Map.hpp"
#include "PathData.hpp"
#include "Abe.hpp"
#include "MusicController.hpp"
#include "DDCheat.hpp"
#include "../relive_lib/BaseGameObject.hpp"
#include "CrawlingSlig.hpp" // TODO: SFX playing only
#include "SnoozeParticle.hpp"
#include "../relive_lib/Events.hpp"
#include "Sfx.hpp"
#include "Blood.hpp"
#include "Gibs.hpp"
#include "../relive_lib/Particle.hpp"
#include "Sound/Midi.hpp"
#include "../relive_lib/ObjectIds.hpp"
#include "PlatformBase.hpp"
#include "ScreenShake.hpp"
#include "../relive_lib/ScreenManager.hpp"
#include "LiftPoint.hpp"
#include "Lever.hpp"
#include "SwitchStates.hpp"
#include "Bullet.hpp"
#include "Dove.hpp"
#include "BulletShell.hpp"
#include "GameSpeak.hpp"
#include "AmbientSound.hpp"
#include "../relive_lib/VRam.hpp"
#include "Electrocute.hpp"
#include "Grid.hpp"
#include "AnimationCallBacks.hpp"

const relive::SfxDefinition kSfxInfoTable_5607E0[17] = {
    {0u, 1u, 58u, 40u, -256, -256},
    {0u, 1u, 58u, 35u, 0, 0},
    {0u, 1u, 59u, 55u, 0, 0},
    {0u, 1u, 59u, 55u, 127, 127},
    {0u, 1u, 60u, 60u, 0, 0},
    {0u, 1u, 61u, 60u, 0, 0},
    {0u, 1u, 54u, 60u, 0, 0},
    {0u, 1u, 55u, 60u, 0, 0},
    {0u, 39u, 60u, 110u, -1000, -950},
    {0u, 1u, 48u, 90u, 0, 0},
    {0u, 1u, 49u, 90u, 0, 0},
    {0u, 1u, 50u, 90u, 0, 0},
    {0u, 0u, 66u, 50u, -512, -384},
    {0u, 0u, 67u, 50u, -512, -384},
    {0u, 3u, 36u, 60u, 0, 0},
    {0u, 3u, 37u, 60u, 0, 0},
    {0u, 3u, 38u, 60u, 0, 0}};

void Slig_SoundEffect_4BFFE0(SligSfx effect, BaseAliveGameObject* pObj)
{
    const relive::SfxDefinition& pEffect = kSfxInfoTable_5607E0[static_cast<s32>(effect)];
    s16 vLeft = 0;
    s16 vRight = 0;
    if (Calc_Slig_Sound_Direction_4C01B0(pObj, 0, pEffect, &vLeft, &vRight))
    {
        s16 pitch = 0;
        if (effect == SligSfx::ePropeller1_9 || effect == SligSfx::ePropeller2_10 || effect == SligSfx::ePropeller3_11)
        {
            FP sndDistance = FP_FromInteger(abs(Math_Distance(0, 0, FP_GetExponent(pObj->mVelX), FP_GetExponent(pObj->mVelY))));
            if (sndDistance > FP_FromInteger(8))
            {
                sndDistance = FP_FromInteger(8);
            }

            pitch = Math_RandomRange(-127, 127) + FP_GetExponent((sndDistance / FP_FromInteger(8)) * FP_FromInteger(768)) + 512;
        }
        else
        {
            pitch = Math_RandomRange(pEffect.field_E_pitch_min, pEffect.field_10_pitch_max);
        }
        SFX_SfxDefinition_Play_Stereo(pEffect, vLeft, vRight, pitch, pitch);
    }
}

void Animation_OnFrame_Slig_4C0600(BaseGameObject* pObj, u32&, const Point32& point)
{
    auto pSlig = reinterpret_cast<Slig*>(pObj);

    if (pSlig->UpdateDelay())
    {
        return;
    }

    BulletType bulletType = BulletType::eSligPossessedOrUnderGlukkonCommand_0;
    if (pSlig->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit4_bPossesed) || pSlig->vUnderGlukkonCommand_4B1760())
    {
        bulletType = BulletType::eSligPossessedOrUnderGlukkonCommand_0;
    }
    else
    {
        bulletType = BulletType::eNormalBullet_2;
    }

    const FP xOff = (pSlig->mSpriteScale * FP_FromInteger(point.x));
    const FP yOff = (pSlig->mSpriteScale * FP_FromInteger(point.y));

    Bullet* pBullet = nullptr;

    FP bullet_xDist = FP_FromInteger(0);
    s8 fireDirection = 0;
    FP fireXpos = FP_FromInteger(0);
    s16 shellDirection = 0;
    if (pSlig->mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        bullet_xDist = FP_FromInteger(-640);
        fireDirection = 1;
        fireXpos = pSlig->mXPos - xOff;
        shellDirection = 0;
    }
    else
    {
        bullet_xDist = FP_FromInteger(640);
        fireDirection = 0;
        fireXpos = xOff + pSlig->mXPos;
        shellDirection = 1;
    }

    pBullet = relive_new Bullet(pSlig, bulletType, pSlig->mXPos, yOff + pSlig->mYPos, bullet_xDist, pSlig->mSpriteScale, 0);
    if (pBullet)
    {
        pBullet->SetUpdateDelay(1);
    }

    New_ShootingFire_Particle(fireXpos, yOff + pSlig->mYPos, fireDirection, pSlig->mSpriteScale);

    if (pSlig->mSpriteScale == FP_FromDouble(0.5))
    {
        SfxPlayMono(relive::SoundEffects::SligShoot, 85);
    }
    else
    {
        relive_new BulletShell(pSlig->mXPos, yOff + pSlig->mYPos, shellDirection, pSlig->mSpriteScale);
        SfxPlayMono(relive::SoundEffects::SligShoot, 0);
    }

    EventBroadcast(kEventShooting, pSlig);
    EventBroadcast(kEventLoudNoise, pSlig);

    Dove::All_FlyAway(0);
    return;
}

TintEntry sSligTint_560570[15] = {
    {EReliveLevelIds::eMines, 102u, 127u, 118u},
    {EReliveLevelIds::eNecrum, 102u, 127u, 118u},
    {EReliveLevelIds::eMudomoVault, 102u, 127u, 118u},
    {EReliveLevelIds::eMudancheeVault, 102u, 127u, 118u},
    {EReliveLevelIds::eFeeCoDepot, 102u, 127u, 118u},
    {EReliveLevelIds::eBarracks, 102u, 127u, 118u},
    {EReliveLevelIds::eMudancheeVault_Ender, 102u, 127u, 118u},
    {EReliveLevelIds::eBonewerkz, 102u, 127u, 118u},
    {EReliveLevelIds::eBrewery, 102u, 127u, 118u},
    {EReliveLevelIds::eBrewery_Ender, 102u, 127u, 118u},
    {EReliveLevelIds::eMudomoVault_Ender, 102u, 127u, 118u},
    {EReliveLevelIds::eFeeCoDepot_Ender, 102u, 127u, 118u},
    {EReliveLevelIds::eBarracks_Ender, 102u, 127u, 118u},
    {EReliveLevelIds::eBonewerkz_Ender, 102u, 127u, 118u},
    {EReliveLevelIds::eNone, 102u, 127u, 118u}};

ALIVE_VAR(1, 0xBAF7E4, s32, unused_BAF7E4, 0);
ALIVE_VAR(1, 0xBAF7E8, s16, sSligsUnderControlCount_BAF7E8, 0);

const AnimId sSligAnimIdTable[52] = {
    AnimId::Slig_Idle,
    AnimId::Slig_StandToWalk,
    AnimId::Slig_Walking,
    AnimId::Slig_StandToRun,
    AnimId::Slig_Running,
    AnimId::Slig_TurnAroundStanding,
    AnimId::Slig_Shoot,
    AnimId::Slig_Idle,
    AnimId::Slig_SlidingToStand,
    AnimId::Slig_SlidingTurn,
    AnimId::Slig_SlidingTurnToWalk,
    AnimId::Slig_SlidingTurnToRun,
    AnimId::Slig_ReloadGun,
    AnimId::Slig_ShootToStand,
    AnimId::Slig_SteppingToStand,
    AnimId::Slig_StandingToStep,
    AnimId::Slig_Idle,
    AnimId::Slig_Gamespeak,
    AnimId::Slig_WalkToStand,
    AnimId::Slig_Recoil,
    AnimId::Slig_Speak1,
    AnimId::Slig_Speak2,
    AnimId::Slig_Speak3,
    AnimId::Slig_Speak4,
    AnimId::Slig_Speak1,
    AnimId::Slig_Speak2,
    AnimId::Slig_Speak3,
    AnimId::Slig_Speak4,
    AnimId::Slig_Speak1,
    AnimId::Slig_Speak2,
    AnimId::Slig_Speak3,
    AnimId::Slig_Speak4,
    AnimId::Slig_Sleeping,
    AnimId::Slig_SleepingToStand,
    AnimId::Slig_Knockback,
    AnimId::Slig_KnockbackToStand,
    AnimId::Slig_PossessShake,
    AnimId::Slig_PossessShake,
    AnimId::Slig_OutToFall,
    AnimId::Slig_FallingInitiate,
    AnimId::Slig_LandingSoft,
    AnimId::Slig_LandingFatal,
    AnimId::Slig_ShootZ,
    AnimId::Slig_ShootZtoStand,
    AnimId::Slig_Smash,
    AnimId::Slig_PullLever,
    AnimId::Slig_LiftGrip,
    AnimId::Slig_LiftUngrip,
    AnimId::Slig_LiftGripping,
    AnimId::Slig_LiftUp,
    AnimId::Slig_LiftDown,
    AnimId::Slig_Beat};

const TSligMotionFn sSlig_motion_table_5604A0[52] = {
    &Slig::M_StandIdle_0_4B4EC0,
    &Slig::M_StandToWalk_1_4B5F70,
    &Slig::M_Walking_2_4B5BC0,
    &Slig::M_StandToRun_3_4B62F0,
    &Slig::M_Running_4_4B6000,
    &Slig::M_TurnAroundStanding_5_4B6390,
    &Slig::M_Shoot_6_4B55A0,
    &Slig::M_Falling_7_4B42D0,
    &Slig::M_SlidingToStand_8_4B6520,
    &Slig::M_SlidingTurn_9_4B6680,
    &Slig::M_SlidingTurnToWalk_10_4B6800,
    &Slig::M_SlidingTurnToRun_11_4B6850,
    &Slig::M_ReloadGun_12_4B5530,
    &Slig::M_ShootToStand_13_4B5580,
    &Slig::M_SteppingToStand_14_4B8480,
    &Slig::M_StandingToStep_15_4B83B0,
    &Slig::M_DepossessingAbort_16_4B8250,
    &Slig::M_GameSpeak_17_4B5290,
    &Slig::M_WalkToStand_18_4B5FC0,
    &Slig::M_Recoil_19_4B8270,
    &Slig::M_SpeakHereBoy_20_4B5330,
    &Slig::M_SpeakHi_21_4B53D0,
    &Slig::M_SpeakFreeze_22_4B53F0,
    &Slig::M_SpeakGetHim_23_4B5410,
    &Slig::M_SpeakLaugh_24_4B5430,
    &Slig::M_SpeakBullshit1_25_4B5450,
    &Slig::M_SpeakLookOut_26_4B5470,
    &Slig::M_SpeakBullshit2_27_4B5490,
    &Slig::M_SpeakPanic_28_4B54B0,
    &Slig::M_SpeakWhat_29_4B54D0,
    &Slig::M_SpeakAIFreeze_30_4B54F0,
    &Slig::M_Blurgh_31_4B5510,
    &Slig::M_Sleeping_32_4B89A0,
    &Slig::M_SleepingToStand_33_4B8C50,
    &Slig::M_Knockback_34_4B68A0,
    &Slig::M_KnockbackToStand_35_4B6A30,
    &Slig::M_Depossessing_36_4B7F30,
    &Slig::M_Possess_37_4B72C0,
    &Slig::M_OutToFall_38_4B4570,
    &Slig::M_FallingInitiate_39_4B4640,
    &Slig::M_LandingSoft_40_4B4530,
    &Slig::M_LandingFatal_41_4B4680,
    &Slig::M_ShootZ_42_4B7560,
    &Slig::M_ShootZtoStand_43_4B77E0,
    &Slig::M_Smash_44_4B6B90,
    &Slig::M_PullLever_45_4B8950,
    &Slig::M_LiftGrip_46_4B3700,
    &Slig::M_LiftUngrip_47_4B3820,
    &Slig::M_LiftGripping_48_4B3850,
    &Slig::M_LiftUp_49_4B3930,
    &Slig::M_LiftDown_50_4B3960,
    &Slig::M_Beat_51_4B6C00};

const TSligBrainFn sSligBrainTable[36] = {
    &Slig::Brain_Death_0_4BBFB0,
    &Slig::Brain_ReturnControlToAbeAndDie_1_4BC410,
    &Slig::Brain_Possessed_2_4BBCF0,
    &Slig::Brain_DeathDropDeath_3_4BC1E0,
    &Slig::Brain_ListeningToGlukkon_4_4B9D20,
    &Slig::Brain_Empty_5_4B3220,
    &Slig::Brain_Empty_6_4B3420,
    &Slig::Brain_SpottedEnemy_7_4B3240,
    &Slig::Brain_Empty_8_4B3120,
    &Slig::Brain_Empty_9_4B3440,
    &Slig::Brain_EnemyDead_10_4B3460,
    &Slig::Brain_KilledEnemy_10_4B35C0,
    &Slig::Brain_PanicTurning_12_4BC490,
    &Slig::Brain_PanicRunning_13_4BC780,
    &Slig::Brain_PanicYelling_14_4BCA70,
    &Slig::Brain_Idle_15_4BD800,
    &Slig::Brain_StopChasing_16_4BCE30,
    &Slig::Brain_Chasing_17_4BCBD0,
    &Slig::Brain_StartChasing_18_4BCEB0,
    &Slig::Brain_Turning_19_4BDDD0,
    &Slig::Brain_StoppingNextToMudokon_20_4BF1E0,
    &Slig::Brain_Walking_21_4BE0C0,
    &Slig::Brain_GetAlertedTurn_22_4BE990,
    &Slig::Brain_GetAlerted_23_4BEC40,
    &Slig::Brain_BeatingUp_24_4BF2B0,
    &Slig::Brain_DiscussionWhat_25_4BF380,
    &Slig::Brain_Empty_26_4BF620,
    &Slig::Brain_Empty_27_4BF600,
    &Slig::Brain_ZShooting_28_4BFA70,
    &Slig::Brain_Shooting_29_4BF750,
    &Slig::Brain_ZSpottedEnemy_30_4BFA30,
    &Slig::Brain_WakingUp_31_4B9390,
    &Slig::Brain_Inactive_32_4B9430,
    &Slig::Brain_Paused_33_4B8DD0,
    &Slig::Brain_Sleeping_34_4B9170,
    &Slig::Brain_ChaseAndDisappear_35_4BF640};

AnimId Slig::MotionToAnimId(u32 motion)
{
    if (motion > ALIVE_COUNTOF(sSligAnimIdTable))
    {
        ALIVE_FATAL("Motion out of bounds");
    }

    if (motion == eSligMotions::M_Smash_44_4B6B90 && field_218_tlv_data.mData.mDeathMode == relive::Path_Slig_Data::DeathMode::StandIdle)
    {
        mCurrentMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        return AnimId::Slig_Idle;
    }
    return sSligAnimIdTable[motion];
}

void Slig::SetBrain(TSligBrainFn fn)
{
    field_154_brain_state = fn;
}

bool Slig::BrainIs(TSligBrainFn fn)
{
    return field_154_brain_state == fn;
}

void Slig::LoadAnimations()
{
    for (auto& animId : sSligAnimIdTable)
    {
        mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(animId));
    }
}

Slig::Slig(relive::Path_Slig* pTlv, const Guid& tlvId)
    : BaseAliveGameObject(17)
{
    field_160_last_event_index = -1;
    field_176_unused = -1;
    field_174_unused = 0;

    if (tlvId != Guid{})
    {
        mBaseGameObjectTlvInfo = tlvId;
    }

    LoadAnimations();
    Animation_Init(GetAnimRes(AnimId::Slig_Idle));

    SetType(ReliveTypes::eSlig);

    mBaseAliveGameObjectFlags.Clear(Flags_114::e114_Bit4_bPossesed);
    mBaseAliveGameObjectFlags.Clear(Flags_114::e114_Bit9_RestoredFromQuickSave);
    mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit3_Can_Be_Possessed);
    mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit6_SetOffExplosives);

    field_292_prevent_depossession &= ~1u;

    unused_BAF7E4 = 0;

    field_216_flags.Clear(Flags_216::eBit1_FollowGlukkon);
    field_216_flags.Clear(Flags_216::eBit3_GlukkonCalledAllOYa);
    field_216_flags.Clear(Flags_216::eBit4_HeardGlukkon);


    field_120_timer = 0;
    field_11E_pitch_min = 45 * (Math_NextRandom() % 5 - 2);
    mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    field_124_return_to_previous_motion = 0;
    field_150_explode_timer = 0;
    field_14C_death_by_being_shot_timer = 0;
    BaseAliveGameObjectPathTLV = pTlv;

    field_218_tlv_data = *pTlv;

    BaseAliveGameObject_PlatformId = Guid{};

    mCurrentMotion = eSligMotions::M_Falling_7_4B42D0;

    field_124_return_to_previous_motion = 0;
    mXPos = FP_FromInteger(pTlv->mTopLeftX);
    mYPos = FP_FromInteger(pTlv->mTopLeftY);
    field_130_falling_velx_scale_factor = FP_FromInteger(0);
    field_118_tlvInfo = tlvId;
    mInput = 0;
    field_158_num_times_to_shoot = 0;
    field_15A_unused = 0;
    field_15C_force_alive_state = 0;
    field_136_shot_motion = -1;
    field_208_glukkon_obj_id = Guid{};
    field_20C_state_after_speak = -1;
    field_20E_attention_timeout = 0;
    field_210_unused = 0;
    
    mAnim.mFnPtrArray = kSlig_Anim_Frame_Fns_55EFAC;

    if (pTlv->mData.mScale != relive::reliveScale::eFull)
    {
        if (pTlv->mData.mScale == relive::reliveScale::eHalf)
        {
            mSpriteScale = FP_FromDouble(0.5);
            mAnim.mRenderLayer = Layer::eLayer_SligGreeterFartsBat_Half_14;
            mScale = Scale::Bg;
        }
    }
    else
    {
        mSpriteScale = FP_FromInteger(1);
        mAnim.mRenderLayer = Layer::eLayer_SligGreeterFartsBats_33;
        mScale = Scale::Fg;
    }

    SetBaseAnimPaletteTint(&sSligTint_560570[0], gMap.mCurrentLevel, PalId::Slig_412);

    FP hitX = {};
    FP hitY = {};
    if (sCollisions->Raycast(
            mXPos,
            mYPos,
            mXPos,
            mYPos + FP_FromInteger(24),
            &BaseAliveGameObjectCollisionLine,
            &hitX,
            &hitY,
            mScale == Scale::Fg ? kFgFloor : kBgFloor)
        == 1)
    {
        mCurrentMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        mYPos = hitY;
    }

    MapFollowMe(TRUE);

    Init();

    VStackOnObjectsOfType(ReliveTypes::eSlig);

    if (gMap.mCurrentLevel == EReliveLevelIds::eBonewerkz && gMap.mCurrentPath == 2 && gMap.mCurrentCamera == 5)
    {
        mXOffset = 0;
    }

    mVisualFlags.Set(VisualFlags::eDoPurpleLightEffect);
    field_142_unused = 0;
    field_140_unused = 0;

    mShadow = relive_new Shadow();
}

void renderWithGlowingEyes(PrimHeader** ot, BaseAliveGameObject* actor, s16* pPalAlloc, s16 palSize, s16& r, s16& g, s16& b,
                           const s16* eyeColourIndices, s16 eyeColourIndicesSize)
{
    if (actor->mAnim.mFlags.Get(AnimFlags::eBit3_Render))
    {
        if (gMap.mCurrentPath == actor->mCurrentPath && gMap.mCurrentLevel == actor->mCurrentLevel && actor->Is_In_Current_Camera() == CameraPos::eCamCurrent_0)
        {
            actor->mAnim.field_14_scale = actor->mSpriteScale;

            const PSX_RECT boundingRect = actor->VGetBoundingRect();
            s16 rMod = actor->mRGB.r;
            s16 gMod = actor->mRGB.g;
            s16 bMod = actor->mRGB.b;
            ShadowZone::ShadowZones_Calculate_Colour(
                FP_GetExponent(actor->mXPos),
                (boundingRect.h + boundingRect.y) / 2,
                actor->mScale,
                &rMod,
                &gMod,
                &bMod);
            if (!actor->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit7_Electrocuted))
            {
                if (rMod != r || gMod != g || bMod != b)
                {
                    r = rMod;
                    g = gMod;
                    b = bMod;

                    for (s32 i = 0; i < palSize; i++)
                    {
                        s32 auxPalValue = actor->mAnim.mAnimRes.mTgaPtr->mPal.mPal[i] & 0x1F;
                        u16 resultR = static_cast<s16>(auxPalValue * r) >> 7;
                        if (resultR > 31)
                        {
                            resultR = 31;
                        }

                        auxPalValue = (actor->mAnim.mAnimRes.mTgaPtr->mPal.mPal[i] >> 5) & 0x1F;
                        u16 resultG = static_cast<s16>(auxPalValue * g) >> 7;
                        if (resultG > 31)
                        {
                            resultG = 31;
                        }

                        auxPalValue = (actor->mAnim.mAnimRes.mTgaPtr->mPal.mPal[i] >> 10) & 0x1F;
                        u16 resultB = static_cast<s16>(auxPalValue * b) >> 7;
                        if (resultB > 31)
                        {
                            resultB = 31;
                        }

                        s32 resultMixed = (actor->mAnim.mAnimRes.mTgaPtr->mPal.mPal[i] & 0x8000) | ((resultR & 31) + 32 * (resultG & 31) + 32 * 32 * (resultB & 31));
                        if (resultMixed <= 0 && actor->mAnim.mAnimRes.mTgaPtr->mPal.mPal[i])
                        {
                            resultMixed = 1;
                        }
                        pPalAlloc[i] = static_cast<u16>(resultMixed);
                    }
                    for (s32 i = 0; i < eyeColourIndicesSize; i++)
                    {
                        pPalAlloc[eyeColourIndices[i]] = actor->mAnim.mAnimRes.mTgaPtr->mPal.mPal[eyeColourIndices[i]];
                    }
                    /*
                    Pal_Set(
                        actor->mAnim.mPalVramXY,
                        actor->mAnim.mPalDepth,
                        reinterpret_cast<const u8*>(pPalAlloc),
                        palRect);
                        */
                }
                actor->mAnim.mRed = 127;
                actor->mAnim.mGreen = 127;
                actor->mAnim.mBlue = 127;
            }

            actor->mAnim.VRender(
                FP_GetExponent(FP_FromInteger(actor->mXOffset) + actor->mXPos - pScreenManager->CamXPos()),
                FP_GetExponent(FP_FromInteger(actor->mYOffset) + actor->mYPos - pScreenManager->CamYPos()),
                ot,
                0,
                0);

            PSX_RECT rectToInvalidate = {};
            actor->mAnim.Get_Frame_Rect(&rectToInvalidate);


            if (actor->mShadow)
            {
                actor->mShadow->Calculate_Position(actor->mXPos, actor->mYPos, &rectToInvalidate, actor->mSpriteScale, actor->mScale);
                actor->mShadow->Render(ot);
            }
        }
    }
}

void Slig::VRender(PrimHeader** ot)
{
    const s16 eyeIndices[] = {61, 62};
    renderWithGlowingEyes(ot, this, &field_178_pPalAlloc[0], ALIVE_COUNTOF(field_178_pPalAlloc),
                          field_200_red, field_202_green, field_204_blue, &eyeIndices[0], ALIVE_COUNTOF(eyeIndices));
}

s16 Slig::VIsFacingMe(BaseAnimatedWithPhysicsGameObject* pOther)
{
    return vIsFacingMe_4B23D0(pOther);
}

s16 Slig::VOnSameYLevel(BaseAnimatedWithPhysicsGameObject* pOther)
{
    return vOnSameYLevel_4BB6C0(pOther);
}

s32 Slig::VGetSaveState(u8* pSaveBuffer)
{
    if (mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit7_Electrocuted))
    {
        return 0;
    }

    auto pState = reinterpret_cast<Slig_State*>(pSaveBuffer);

    pState->field_0_type = AETypes::eSlig_125;

    pState->field_4_xpos = mXPos;
    pState->field_8_ypos = mYPos;

    pState->field_C_velx = mVelX;
    pState->field_10_vely = mVelY;

    pState->field_58_falling_velx_scale_factor = field_130_falling_velx_scale_factor;

    pState->field_14_path_number = mCurrentPath;
    pState->field_16_lvl_number = MapWrapper::ToAE(mCurrentLevel);
    pState->field_18_sprite_scale = mSpriteScale;

    pState->field_1C_scale = mScale;

    pState->field_1E_r = mRGB.r;
    pState->field_20_g = mRGB.g;
    pState->field_22_b = mRGB.b;

    if (mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit11_Electrocuting))
    {
        for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
        {
            BaseGameObject* pObj = gBaseGameObjects->ItemAt(i);
            if (!pObj)
            {
                break;
            }

            if (pObj->Type() == ReliveTypes::eElectrocute)
            {
                auto pElectrocute = static_cast<Electrocute*>(pObj);
                if (pElectrocute->field_20_target_obj_id == mBaseGameObjectId)
                {
                    pState->field_1E_r = pElectrocute->field_24_r;
                    pState->field_20_g = pElectrocute->field_26_g;
                    pState->field_22_b = pElectrocute->field_28_b;
                    break;
                }
            }
        }
    }
    pState->field_24_bFlipX = mAnim.mFlags.Get(AnimFlags::eBit5_FlipX);
    pState->field_26_current_motion = mCurrentMotion;
    pState->field_28_current_frame = static_cast<s16>(mAnim.mCurrentFrame);
    pState->field_2A_frame_change_counter = static_cast<s16>(mAnim.mFrameChangeCounter);
    pState->field_2D_bDrawable = mBaseGameObjectFlags.Get(BaseGameObject::eDrawable_Bit4);
    pState->field_2C_bRender = mAnim.mFlags.Get(AnimFlags::eBit3_Render);
    pState->field_30_health = mHealth;
    pState->field_34_current_motion = mCurrentMotion;
    pState->field_36_next_motion = mNextMotion;
    pState->field_38_last_line_ypos = FP_GetExponent(BaseAliveGameObjectLastLineYPos);
    pState->field_3A_collision_line_type = -1;

    if (BaseAliveGameObjectCollisionLine)
    {
        pState->field_3A_collision_line_type = BaseAliveGameObjectCollisionLine->mLineType;
    }

    pState->field_40_bActiveChar = this == sControlledCharacter;
    pState->field_42_brain_sub_state = field_11C_brain_sub_state;
    pState->field_44_pitch_min = field_11E_pitch_min;
    pState->field_48_timer = field_120_timer;
    pState->field_4C_return_to_previous_motion = field_124_return_to_previous_motion;
    pState->field_4E_checked_if_off_screen = field_126_checked_if_off_screen;
    pState->field_50_input = InputObject::KeyboardInputToPsxButtons_45EF70(mInput);
    pState->field_54_timer = field_12C_timer;
    pState->field_58_falling_velx_scale_factor = field_130_falling_velx_scale_factor;
    pState->field_5C_tlvInfo = field_118_tlvInfo;
    //pState->field_60_res_idx = field_134_res_idx;
    pState->field_62_shot_motion = field_136_shot_motion;
    pState->field_64_zone_rect = field_138_zone_rect;
    pState->field_6C_unused = field_140_unused;
    pState->field_6E_unused = field_142_unused;
    pState->field_70_unused = field_144_unused;
    pState->field_72_return_level = MapWrapper::ToAE(field_146_return_level);
    pState->field_74_return_path = field_148_return_path;
    pState->field_76_return_camera = field_14A_return_camera;
    pState->field_78_death_by_being_shot_timer = field_14C_death_by_being_shot_timer;
    pState->field_7C_explode_timer = field_150_explode_timer;
    pState->field_80_brain_state_idx = 0;

    s32 idx = 0;
    for (const auto& fn : sSligBrainTable)
    {
        if (BrainIs(fn))
        {
            pState->field_80_brain_state_idx = idx;
        }
        idx++;
    }

    pState->field_88_unused = unused_BAF7E4;
    pState->field_8C_num_times_to_shoot = field_158_num_times_to_shoot;
    pState->field_90_force_alive_state = field_15C_force_alive_state;
    pState->field_92_spotted_possessed_slig = field_15E_spotted_possessed_slig;
    pState->field_94_glukkon_id = Guid{};

    if (field_208_glukkon_obj_id != Guid{})
    {
        BaseGameObject* pGlukkon = sObjectIds.Find_Impl(field_208_glukkon_obj_id);
        if (pGlukkon)
        {
            pState->field_94_glukkon_id = pGlukkon->mBaseGameObjectTlvInfo;
        }
    }
    pState->field_98_state_after_speak = field_20C_state_after_speak;
    pState->field_9A_attention_timeout = field_20E_attention_timeout;

    pState->field_9C_unused = field_210_unused;

    pState->field_9E_next_command_arg1 = field_212_next_command_arg1;
    pState->field_A0_cmd_idx = field_214_cmd_idx;

    pState->field_A2_flags.Set(Slig_State::eBit2_FollowGlukkon, field_216_flags.Get(Flags_216::eBit1_FollowGlukkon));
    pState->field_A2_flags.Set(Slig_State::eBit3_StoppedForLeverOrLift, field_216_flags.Get(Flags_216::eBit2_StoppedForLeverOrLift));
    pState->field_A2_flags.Set(Slig_State::eBit4_GlukkonCalledAllOYa, field_216_flags.Get(Flags_216::eBit3_GlukkonCalledAllOYa));
    pState->field_A2_flags.Set(Slig_State::eBit5_HeardGlukkon, field_216_flags.Get(Flags_216::eBit4_HeardGlukkon));

    return sizeof(Slig_State);
}

s32 Slig::CreateFromSaveState(const u8* pBuffer)
{
    auto pState = reinterpret_cast<const Slig_State*>(pBuffer);
    auto pTlv = static_cast<relive::Path_Slig*>(sPathInfo->TLV_From_Offset_Lvl_Cam(pState->field_5C_tlvInfo));

    auto pSlig = relive_new Slig(pTlv, pState->field_5C_tlvInfo);
    if (pSlig)
    {
        if (pState->field_40_bActiveChar)
        {
            sControlledCharacter = pSlig;
            pSlig->mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit4_bPossesed);
        }

        pSlig->BaseAliveGameObjectPathTLV = nullptr;
        pSlig->BaseAliveGameObjectCollisionLine = nullptr;
        pSlig->mXPos = pState->field_4_xpos;
        pSlig->mYPos = pState->field_8_ypos;
        pSlig->mVelX = pState->field_C_velx;
        pSlig->mVelY = pState->field_10_vely;
        pSlig->mCurrentPath = pState->field_14_path_number;
        pSlig->mCurrentLevel = MapWrapper::FromAESaveData(pState->field_16_lvl_number);
        pSlig->mSpriteScale = pState->field_18_sprite_scale;

        if (pSlig->mSpriteScale == FP_FromInteger(1))
        {
            pSlig->mAnim.mRenderLayer = Layer::eLayer_SligGreeterFartsBats_33;
        }
        else
        {
            pSlig->mAnim.mRenderLayer = Layer::eLayer_SligGreeterFartsBat_Half_14;
        }

        pSlig->mScale = pState->field_1C_scale;

        pSlig->field_200_red = pState->field_1E_r;
        pSlig->field_202_green = pState->field_20_g;
        pSlig->field_204_blue = pState->field_22_b;

        pSlig->mRGB.SetRGB(pState->field_1E_r, pState->field_20_g, pState->field_22_b);

        pSlig->mCurrentMotion = pState->field_26_current_motion;
        //u8** ppRes = pSlig->ResForMotion_4B1E90(pSlig->mCurrentMotion);
        pSlig->mAnim.Set_Animation_Data(pSlig->GetAnimRes(pSlig->MotionToAnimId(pState->field_26_current_motion)));


        pSlig->mAnim.mCurrentFrame = pState->field_28_current_frame;
        pSlig->mAnim.mFrameChangeCounter = pState->field_2A_frame_change_counter;

        pSlig->mBaseGameObjectFlags.Set(BaseGameObject::eDrawable_Bit4, pState->field_2D_bDrawable & 1);

        pSlig->mAnim.mFlags.Set(AnimFlags::eBit5_FlipX, pState->field_24_bFlipX & 1);
        pSlig->mAnim.mFlags.Set(AnimFlags::eBit3_Render, pState->field_2C_bRender & 1);

        if (IsLastFrame(&pSlig->mAnim))
        {
            pSlig->mAnim.mFlags.Set(AnimFlags::eBit18_IsLastFrame);
        }

        pSlig->mHealth = pState->field_30_health;
        pSlig->mCurrentMotion = pState->field_34_current_motion;
        pSlig->mNextMotion = pState->field_36_next_motion;
        pSlig->BaseAliveGameObjectLastLineYPos = FP_FromInteger(pState->field_38_last_line_ypos);

        pSlig->mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit9_RestoredFromQuickSave);

        pSlig->BaseAliveGameObjectCollisionLineType = pState->field_3A_collision_line_type;
        pSlig->field_11C_brain_sub_state = pState->field_42_brain_sub_state;
        pSlig->field_11E_pitch_min = pState->field_44_pitch_min;
        pSlig->field_120_timer = pState->field_48_timer;
        pSlig->field_124_return_to_previous_motion = pState->field_4C_return_to_previous_motion;
        pSlig->field_126_checked_if_off_screen = pState->field_4E_checked_if_off_screen;

        pSlig->mInput = InputObject::PsxButtonsToKeyboardInput_45EE40(pState->field_50_input);

        pSlig->field_12C_timer = pState->field_54_timer;
        pSlig->field_130_falling_velx_scale_factor = pState->field_58_falling_velx_scale_factor;
        pSlig->field_118_tlvInfo = pState->field_5C_tlvInfo;
        //pSlig->field_134_res_idx = pState->field_60_res_idx;
        pSlig->field_136_shot_motion = pState->field_62_shot_motion;

        pSlig->field_138_zone_rect = pState->field_64_zone_rect;

        pSlig->field_140_unused = pState->field_6C_unused;
        pSlig->field_142_unused = pState->field_6E_unused;
        pSlig->field_144_unused = pState->field_70_unused;

        pSlig->field_146_return_level = MapWrapper::FromAESaveData(pState->field_72_return_level);
        pSlig->field_148_return_path = pState->field_74_return_path;
        pSlig->field_14A_return_camera = pState->field_76_return_camera;

        pSlig->field_14C_death_by_being_shot_timer = pState->field_78_death_by_being_shot_timer;
        pSlig->field_150_explode_timer = pState->field_7C_explode_timer;

        pSlig->SetBrain(sSligBrainTable[pState->field_80_brain_state_idx]);

        unused_BAF7E4 = pState->field_88_unused;

        pSlig->field_158_num_times_to_shoot = static_cast<s16>(pState->field_8C_num_times_to_shoot); // TODO: Wrong type ??
        pSlig->field_15C_force_alive_state = pState->field_90_force_alive_state;
        pSlig->field_15E_spotted_possessed_slig = pState->field_92_spotted_possessed_slig;
        pSlig->field_208_glukkon_obj_id = pState->field_94_glukkon_id;
        pSlig->field_20C_state_after_speak = pState->field_98_state_after_speak;
        pSlig->field_20E_attention_timeout = pState->field_9A_attention_timeout;

        pSlig->field_210_unused = pState->field_9C_unused;
        pSlig->field_212_next_command_arg1 = pState->field_9E_next_command_arg1;

        pSlig->field_214_cmd_idx = pState->field_A0_cmd_idx;

        pSlig->field_216_flags.Set(Flags_216::eBit1_FollowGlukkon, pState->field_A2_flags.Get(Slig_State::eBit2_FollowGlukkon));
        pSlig->field_216_flags.Set(Flags_216::eBit2_StoppedForLeverOrLift, pState->field_A2_flags.Get(Slig_State::eBit3_StoppedForLeverOrLift));
        pSlig->field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa, pState->field_A2_flags.Get(Slig_State::eBit4_GlukkonCalledAllOYa));
        pSlig->field_216_flags.Set(Flags_216::eBit4_HeardGlukkon, pState->field_A2_flags.Get(Slig_State::eBit5_HeardGlukkon));

        sSligsUnderControlCount_BAF7E8 = 0;
    }

    return sizeof(Slig_State);
}

void Slig::M_StandIdle_0_4B4EC0()
{
    FP xOff = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        xOff = FP_FromInteger(10) * mSpriteScale;
    }
    else
    {
        xOff = FP_FromInteger(-10) * mSpriteScale;
    }

    PathLine* pLine = nullptr;
    FP hitX = {};
    FP hitY = {};
    if (!sCollisions->Raycast(
            xOff + mXPos,
            mYPos - FP_FromInteger(10),
            xOff + mXPos,
            mYPos + FP_FromInteger(10),
            &pLine,
            &hitX,
            &hitY,
            mScale == Scale::Fg ? kFgFloor : kBgFloor))
    {
        VOnTrapDoorOpen();
        mCurrentMotion = eSligMotions::M_FallingInitiate_39_4B4640;
        BaseAliveGameObjectLastLineYPos = mYPos;
        field_130_falling_velx_scale_factor = FP_FromDouble(0.3);
        return;
    }

    if (!MainMovement_4B4720())
    {
        if (sControlledCharacter == this && mHealth > FP_FromInteger(0))
        {
            if (Input_IsChanting_45F260())
            {
                if (!(field_292_prevent_depossession & 1))
                {
                    mCurrentMotion = eSligMotions::M_Depossessing_36_4B7F30;
                    field_12C_timer = sGnFrame + 30;
                    SfxPlayMono(relive::SoundEffects::PossessEffect, 0);
                    return;
                }
            }
            else
            {
                const auto inputHeld = Input().mPads[sCurrentControllerIndex].mHeld;
                const auto gameSpeakCmds = InputCommands::Enum::eChant | InputCommands::Enum::eGameSpeak8 | InputCommands::Enum::eGameSpeak7 | InputCommands::Enum::eGameSpeak6 | InputCommands::Enum::eGameSpeak5 | InputCommands::Enum::eGameSpeak4 | InputCommands::Enum::eGameSpeak3 | InputCommands::Enum::eGameSpeak2 | InputCommands::Enum::eGameSpeak1;

                if (inputHeld & gameSpeakCmds)
                {
                    mInput = inputHeld;
                    mCurrentMotion = eSligMotions::M_GameSpeak_17_4B5290;
                    return;
                }
            }
        }

        if (static_cast<s32>(sGnFrame) >= field_12C_timer)
        {
            mCurrentMotion = eSligMotions::M_ReloadGun_12_4B5530;
        }
    }
}

void Slig::M_StandToWalk_1_4B5F70()
{
    mInput |= Input().mPads[sCurrentControllerIndex].mHeld;
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mCurrentMotion = eSligMotions::M_Walking_2_4B5BC0;
    }
    MoveOnLine_4B4C40();
}

void Slig::M_Walking_2_4B5BC0()
{
    if (gMap.GetDirection(
            mCurrentLevel,
            mCurrentPath,
            mXPos,
            mYPos)
            >= CameraPos::eCamCurrent_0
        && MusicController::static_GetMusicType(0, 0, 0) <= MusicController::MusicTypes::eTension_4)
    {
        MusicController::static_PlayMusic(MusicController::MusicTypes::eTension_4, this, 0, 0);
    }

    mInput |= Input().mPads[sCurrentControllerIndex].mHeld;

    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
    }
    else
    {
        mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), mVelX * FP_FromInteger(2)))
    {
        ToStand_4B4A20();
        return;
    }

    MoveOnLine_4B4C40();

    if (mCurrentMotion == eSligMotions::M_Walking_2_4B5BC0)
    {
        if (mAnim.mCurrentFrame == 5 || mAnim.mCurrentFrame == 14)
        {
            Slig_SoundEffect_4BFFE0(SligSfx::eWalkingStep_2, this);

            if (!field_126_checked_if_off_screen)
            {
                field_126_checked_if_off_screen = 1;
                MapFollowMe(TRUE);
            }

            if (sControlledCharacter == this && mHealth > FP_FromInteger(0))
            {
                if (sInputKey_Run & Input().mPads[sCurrentControllerIndex].mPressed)
                {
                    field_124_return_to_previous_motion = 1;
                    mPreviousMotion = eSligMotions::M_Running_4_4B6000;
                    mBaseAliveGameObjectLastAnimFrame = (mAnim.mCurrentFrame - 5) != 0 ? 13 : 5;
                    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
                    {
                        mInput = 0;
                        mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
                        return;
                    }
                    mVelX = ScaleToGridSize(mSpriteScale) / FP_FromInteger(4);
                }
                mInput = 0;
            }
        }
        else if (mAnim.mCurrentFrame == 11)
        {
            FP gridSize = {};
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                gridSize = -ScaleToGridSize(mSpriteScale);
            }
            else
            {
                gridSize = ScaleToGridSize(mSpriteScale);
            }

            if (WallHit(mSpriteScale * FP_FromInteger(45), gridSize * FP_FromDouble(2.5)))
            {
                mCurrentMotion = eSligMotions::M_WalkToStand_18_4B5FC0;
            }
            else if (sControlledCharacter != this || mHealth <= FP_FromInteger(0))
            {
                if (mNextMotion == eSligMotions::M_Shoot_6_4B55A0)
                {
                    mCurrentMotion = eSligMotions::M_WalkToStand_18_4B5FC0;
                    field_12C_timer = sGnFrame + 60;
                }
                else if (mNextMotion != -1)
                {
                    mCurrentMotion = eSligMotions::M_WalkToStand_18_4B5FC0;
                }
            }
            else
            {
                PlayerControlStopWalkingIfRequired_4B8540();
            }
        }
        else
        {
            field_126_checked_if_off_screen = 0;
        }
    }
}

void Slig::M_StandToRun_3_4B62F0()
{
    mInput |= Input().mPads[sCurrentControllerIndex].mHeld;
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mCurrentMotion = eSligMotions::M_Running_4_4B6000;
    }

    EventBroadcast(kEventNoise, this);

    if (WallHit(mSpriteScale * FP_FromInteger(45), mVelX))
    {
        FallKnockBackOrSmash_4B4A90();
    }
    else
    {
        MoveOnLine_4B4C40();
    }
}

void Slig::M_Running_4_4B6000()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
    }
    else
    {
        mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
    }

    if (gMap.GetDirection(mCurrentLevel, mCurrentPath, mXPos, mYPos) >= CameraPos::eCamCurrent_0 && MusicController::static_GetMusicType(0, 0, 0) != MusicController::MusicTypes::ePossessed_9)
    {
        MusicController::static_PlayMusic(MusicController::MusicTypes::eSoftChase_8, this, 0, 0);
    }

    mInput |= Input().mPads[sCurrentControllerIndex].mHeld;

    EventBroadcast(kEventNoise, this);

    if (WallHit(mSpriteScale * FP_FromInteger(45), mVelX))
    {
        FallKnockBackOrSmash_4B4A90();
    }
    else
    {
        MoveOnLine_4B4C40();

        if (mCurrentMotion == eSligMotions::M_Running_4_4B6000)
        {
            if (mAnim.mCurrentFrame == 4 || mAnim.mCurrentFrame == 12)
            {
                Slig_SoundEffect_4BFFE0(SligSfx::eRunningStep_3, this);

                if (field_126_checked_if_off_screen == 0)
                {
                    field_126_checked_if_off_screen = 1;
                    MapFollowMe(TRUE);
                }

                if (sControlledCharacter == this && mHealth > FP_FromInteger(0))
                {
                    if (mAnim.mCurrentFrame == 4)
                    {
                        PlayerControlRunningSlideStopOrTurnFrame4_4B85D0();
                    }
                    else
                    {
                        PlayerControlRunningSlideStopOrTurnFrame12_4B8790();
                    }
                }
                else
                {
                    if (mNextMotion == eSligMotions::M_Walking_2_4B5BC0)
                    {
                        mCurrentMotion = eSligMotions::M_Walking_2_4B5BC0;
                        mNextMotion = -1;
                        if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
                        {
                            mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
                        }
                        else
                        {
                            mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
                        }
                    }
                    else if (mNextMotion == eSligMotions::M_Shoot_6_4B55A0)
                    {
                        mCurrentMotion = eSligMotions::M_Shoot_6_4B55A0;
                        mNextMotion = -1;
                        field_12C_timer = sGnFrame + 60;
                    }
                    else if (mNextMotion != -1)
                    {
                        if (mVelX >= FP_FromInteger(0))
                        {
                            mVelX = (mSpriteScale * FP_FromDouble(13.2));
                        }
                        else
                        {
                            mVelX = (mSpriteScale * FP_FromDouble(-13.2));
                        }

                        mCurrentMotion = eSligMotions::M_SlidingToStand_8_4B6520;
                    }
                }
            }
            else
            {
                field_126_checked_if_off_screen = 0;
            }
        }
    }
}

void Slig::M_TurnAroundStanding_5_4B6390()
{
    if (gMap.GetDirection(
            mCurrentLevel,
            mCurrentPath,
            mXPos,
            mYPos)
            >= CameraPos::eCamCurrent_0
        && MusicController::static_GetMusicType(0, 0, 0) <= MusicController::MusicTypes::eTension_4)
    {
        MusicController::static_PlayMusic(MusicController::MusicTypes::eTension_4, this, 0, 0);
    }

    Slig_SoundEffect_4BFFE0(SligSfx::eStandingTurn_1, this);

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mAnim.mFlags.Toggle(AnimFlags::eBit5_FlipX);
        ToStand_4B4A20();
    }
}

void Slig::M_Shoot_6_4B55A0()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        if (sControlledCharacter == this && mHealth > FP_FromInteger(0))
        {
            if (Input().isPressed(sInputKey_ThrowItem))
            {
                const FP k45Scaled = mSpriteScale * FP_FromInteger(45);
                const FP kGridSize = ScaleToGridSize(mSpriteScale);
                const FP k8 = FP_FromInteger(8);

                // Recoil right
                if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX) && Input().isPressed(sInputKey_Right))
                {
                    PathLine* pLine = nullptr;
                    FP hitX = {};
                    FP hitY = {};
                    if (sCollisions->Raycast(
                            mXPos,
                            mYPos,
                            mXPos + (k8 * (kGridSize / k8)),
                            mYPos,
                            &pLine,
                            &hitX,
                            &hitY,
                            mScale == Scale::Fg ? kFgWalls : kBgWalls)
                        || sCollisions->Raycast(
                            mXPos,
                            mYPos - k45Scaled,
                            mXPos + (k8 * (kGridSize / k8)),
                            mYPos - k45Scaled,
                            &pLine,
                            &hitX,
                            &hitY,
                            mScale == Scale::Fg ? kFgWalls : kBgWalls))
                    {
                        return;
                    }
                    mVelX = (kGridSize / k8);
                    mCurrentMotion = eSligMotions::M_Recoil_19_4B8270;
                    return;
                }

                // Recoil left
                if (!mAnim.mFlags.Get(AnimFlags::eBit5_FlipX) && Input().isPressed(sInputKey_Left))
                {
                    PathLine* pLine = nullptr;
                    FP hitX = {};
                    FP hitY = {};
                    if (sCollisions->Raycast(
                            mXPos,
                            mYPos,
                            mXPos - (k8 * (kGridSize / k8)),
                            mYPos,
                            &pLine,
                            &hitX,
                            &hitY,
                            mScale == Scale::Fg ? kFgWalls : kBgWalls)
                        || sCollisions->Raycast(
                            mXPos,
                            mYPos - k45Scaled,
                            mXPos - (k8 * (kGridSize / k8)),
                            mYPos - k45Scaled,
                            &pLine,
                            &hitX,
                            &hitY,
                            mScale == Scale::Fg ? kFgWalls : kBgWalls))
                    {
                        return;
                    }
                    mVelX = -(kGridSize / k8);
                    mCurrentMotion = eSligMotions::M_Recoil_19_4B8270;
                    return;
                }

                // General recoil
                if (!Input().isPressed(sInputKey_Down) || mSpriteScale != FP_FromDouble(0.5))
                {
                    if (field_12C_timer > static_cast<s32>(sGnFrame))
                    {
                        return;
                    }

                    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
                    {
                        mVelX = (kGridSize / k8);
                    }
                    else
                    {
                        mVelX = -(kGridSize / k8);
                    }

                    PathLine* pLine = nullptr;
                    FP hitX = {};
                    FP hitY = {};
                    if (sCollisions->Raycast(
                            mXPos,
                            mYPos,
                            mXPos + (k8 * mVelX),
                            mYPos,
                            &pLine,
                            &hitX,
                            &hitY,
                            mScale == Scale::Fg ? kFgWalls : kBgWalls)
                        || sCollisions->Raycast(
                            mXPos, mYPos - k45Scaled,
                            mXPos + (k8 * mVelX),
                            mYPos - k45Scaled,
                            &pLine,
                            &hitX,
                            &hitY,
                            mScale == Scale::Fg ? kFgWalls : kBgWalls))
                    {
                        mVelX = FP_FromInteger(0);
                    }
                    else
                    {
                        mCurrentMotion = eSligMotions::M_Recoil_19_4B8270;
                    }
                    return;
                }
            }
            else
            {
                SND_SEQ_PlaySeq(SeqId::HitBottomOfDeathPit_9, 1, 1);
                mCurrentMotion = eSligMotions::M_ShootToStand_13_4B5580;
                return;
            }
        }

        if (mNextMotion == eSligMotions::M_StandIdle_0_4B4EC0)
        {
            SND_SEQ_PlaySeq(SeqId::HitBottomOfDeathPit_9, 1, 1);
            mCurrentMotion = eSligMotions::M_ShootToStand_13_4B5580;
            mNextMotion = -1;
            return;
        }

        if (mNextMotion != -1 && mNextMotion != eSligMotions::M_Shoot_6_4B55A0)
        {
            SND_SEQ_PlaySeq(SeqId::HitBottomOfDeathPit_9, 1, 1);
            mCurrentMotion = eSligMotions::M_ShootToStand_13_4B5580;
            return;
        }
    }
}

void Slig::M_Falling_7_4B42D0()
{
    if (mVelX <= FP_FromInteger(0))
    {
        if (mVelX < FP_FromInteger(0))
        {
            mVelX += (mSpriteScale * field_130_falling_velx_scale_factor);
            if (mVelX < (ScaleToGridSize(mSpriteScale) / FP_FromInteger(4)))
            {
                mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
            }

            if (mVelX > FP_FromInteger(0))
            {
                mVelX = FP_FromInteger(0);
            }
        }
    }
    else
    {
        mVelX -= (mSpriteScale * field_130_falling_velx_scale_factor);
        if (mVelX > (ScaleToGridSize(mSpriteScale) / FP_FromInteger(4)))
        {
            mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
        }

        if (mVelX < FP_FromInteger(0))
        {
            mVelX = FP_FromInteger(0);
        }
    }

    PathLine* ppPathLine = nullptr;
    FP hitX = {};
    FP hitY = {};
    const auto bCollision = InAirCollision(&ppPathLine, &hitX, &hitY, FP_FromDouble(1.8));
    SetActiveCameraDelayedFromDir();
    if (bCollision)
    {
        switch (ppPathLine->mLineType)
        {
            case eLineTypes::eFloor_0:
            case eLineTypes::eBackgroundFloor_4:
            case 32u:
            case 36u:
                BaseAliveGameObjectCollisionLine = ppPathLine;
                mYPos = hitY;
                mXPos = hitX;
                MapFollowMe(TRUE);
                ToStand_4B4A20();
                PlatformCollide_4B4E00();
                mCurrentMotion = eSligMotions::M_LandingSoft_40_4B4530;
                break;
            case eLineTypes::eWallLeft_1:
            case eLineTypes::eWallRight_2:
            case eLineTypes::eBackgroundWallLeft_5:
            case eLineTypes::eBackgroundWallRight_6:
                mXPos = hitX;
                mYPos = hitY;
                FallKnockBackOrSmash_4B4A90();
                break;
            default:
                return;
        }
    }
}

void Slig::M_SlidingToStand_8_4B6520()
{
    EventBroadcast(kEventNoise, this);
    if (WallHit(mSpriteScale * FP_FromInteger(45), mVelX))
    {
        FallKnockBackOrSmash_4B4A90();
    }
    else
    {
        SlowDown_4B6450(FP_FromDouble(2.125));
        if (mCurrentMotion == eSligMotions::M_SlidingToStand_8_4B6520)
        {
            if (mAnim.mCurrentFrame < 6 && sControlledCharacter == this && mHealth > FP_FromInteger(0))
            {
                if ((mAnim.mFlags.Get(AnimFlags::eBit5_FlipX) && Input().isPressed(sInputKey_Right)) || ((!(mAnim.mFlags.Get(AnimFlags::eBit5_FlipX)) && Input().isPressed(sInputKey_Left))))
                {
                    mBaseAliveGameObjectLastAnimFrame = mAnim.mCurrentFrame;
                    mPreviousMotion = eSligMotions::M_SlidingTurn_9_4B6680;
                    field_124_return_to_previous_motion = 1;
                }
            }
            else if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                Environment_SFX_457A40(EnvironmentSfx::eSlideStop_0, 0, 0x7FFF, this);
                MapFollowMe(TRUE);
                MainMovement_4B4720();
            }
        }
    }
}

void Slig::M_SlidingTurn_9_4B6680()
{
    EventBroadcast(kEventNoise, this);

    if (WallHit(mSpriteScale * FP_FromInteger(45), mVelX))
    {
        FallKnockBackOrSmash_4B4A90();
    }
    else
    {
        SlowDown_4B6450(FP_FromDouble(2.125));

        if (mCurrentMotion == eSligMotions::M_SlidingTurn_9_4B6680)
        {
            if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                MapFollowMe(TRUE);

                if (sControlledCharacter == this && mHealth > FP_FromInteger(0))
                {
                    if (Input().isPressed(sInputKey_Run))
                    {
                        mNextMotion = eSligMotions::M_Running_4_4B6000;
                    }
                    else
                    {
                        mNextMotion = eSligMotions::M_Walking_2_4B5BC0;
                    }
                }

                if (mNextMotion == eSligMotions::M_Running_4_4B6000)
                {
                    mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
                    mCurrentMotion = eSligMotions::M_SlidingTurnToRun_11_4B6850;
                }
                else
                {
                    mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
                    mCurrentMotion = eSligMotions::M_SlidingTurnToWalk_10_4B6800;
                }

                if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
                {
                    mVelX = -mVelX;
                }
            }
        }
    }
}

void Slig::M_SlidingTurnToWalk_10_4B6800()
{
    EventBroadcast(kEventNoise, this);
    MoveOnLine_4B4C40();

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mAnim.mFlags.Toggle(AnimFlags::eBit5_FlipX);
        mCurrentMotion = eSligMotions::M_Walking_2_4B5BC0;
    }
}

void Slig::M_SlidingTurnToRun_11_4B6850()
{
    EventBroadcast(kEventNoise, this);
    MoveOnLine_4B4C40();

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mAnim.mFlags.Toggle(AnimFlags::eBit5_FlipX);
        mCurrentMotion = eSligMotions::M_Running_4_4B6000;
    }
}

void Slig::M_ReloadGun_12_4B5530()
{
    if (mAnim.mCurrentFrame == 1)
    {
        Slig_SoundEffect_4BFFE0(SligSfx::eReload1_6, this);
    }
    else if (mAnim.mCurrentFrame == 4)
    {
        Slig_SoundEffect_4BFFE0(SligSfx::eReload2_7, this);
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToStand_4B4A20();
    }
}

void Slig::M_ShootToStand_13_4B5580()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        MainMovement_4B4720();
    }
}

void Slig::M_SteppingToStand_14_4B8480()
{
    if (mAnim.mCurrentFrame == 0)
    {
        Slig_SoundEffect_4BFFE0(SligSfx::eWalkingStep_2, this);
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), mVelX))
    {
        FallKnockBackOrSmash_4B4A90();
    }
    else
    {
        MoveOnLine_4B4C40();
        if (mCurrentMotion == eSligMotions::M_SteppingToStand_14_4B8480)
        {
            if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                MapFollowMe(TRUE);
                ToStand_4B4A20();
            }
        }
    }
}

void Slig::M_StandingToStep_15_4B83B0()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        if (Input().isPressed(sInputKey_Left | sInputKey_Right))
        {
            mCurrentMotion = eSligMotions::M_StandToWalk_1_4B5F70;
        }
        else if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            mCurrentMotion = eSligMotions::M_SteppingToStand_14_4B8480;
            mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(6));
        }
        else
        {
            mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(6));
            mCurrentMotion = eSligMotions::M_SteppingToStand_14_4B8480;
        }
    }
}

void Slig::M_DepossessingAbort_16_4B8250()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToStand_4B4A20();
    }
}

void Slig::M_GameSpeak_17_4B5290()
{
    mInput |= Input().mPads[sCurrentControllerIndex].mHeld;
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mCurrentMotion = GetNextMotionIncGameSpeak_4B5080(mInput);
        if (mCurrentMotion == -1)
        {
            ToStand_4B4A20();
            mInput = 0;
        }
        else
        {
            if (!BrainIs(&Slig::Brain_ListeningToGlukkon_4_4B9D20))
            {
                EventBroadcast(kEventSpeaking, this);
            }
            mInput = 0;
        }
    }
}

void Slig::M_WalkToStand_18_4B5FC0()
{
    MoveOnLine_4B4C40();

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        MapFollowMe(TRUE);
        if (!Slig::MainMovement_4B4720())
        {
            ToStand_4B4A20();
        }
    }
}

void Slig::M_Recoil_19_4B8270()
{
    MoveOnLine_4B4C40();

    if (mCurrentMotion == eSligMotions::M_Recoil_19_4B8270)
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            MapFollowMe(TRUE);
            if (sControlledCharacter == this && mHealth > FP_FromInteger(0))
            {
                if (Input().isPressed(sInputKey_ThrowItem))
                {
                    mVelX = FP_FromInteger(0);
                    mPreviousMotion = eSligMotions::M_Shoot_6_4B55A0;
                    field_12C_timer = sGnFrame + 60;
                    mBaseAliveGameObjectLastAnimFrame = 2;
                    field_124_return_to_previous_motion = 1;
                }
                else
                {
                    SND_SEQ_PlaySeq(SeqId::HitBottomOfDeathPit_9, 1, 1);
                    mCurrentMotion = eSligMotions::M_ShootToStand_13_4B5580;
                }
            }
            else if (mNextMotion != -1)
            {
                SND_SEQ_PlaySeq(SeqId::HitBottomOfDeathPit_9, 1, 1);
                mCurrentMotion = eSligMotions::M_ShootToStand_13_4B5580;
                mNextMotion = -1;
            }
        }
    }
    else if (mSpriteScale == FP_FromDouble(0.5))
    {
        SfxPlayMono(relive::SoundEffects::SligShoot, 85);
    }
    else
    {
        SfxPlayMono(relive::SoundEffects::SligShoot, 0);
    }
}

void Slig::M_SpeakHereBoy_20_4B5330()
{
    mInput |= Input().mPads[sCurrentControllerIndex].mHeld;
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mCurrentMotion = GetNextMotionIncGameSpeak_4B5080(mInput);
        if (mCurrentMotion == -1)
        {
            ToStand_4B4A20();
            mInput = 0;
        }
        else
        {
            if (!BrainIs(&Slig::Brain_ListeningToGlukkon_4_4B9D20))
            {
                EventBroadcast(kEventSpeaking, this);
            }
            mInput = 0;
        }
    }
}

void Slig::M_SpeakHi_21_4B53D0()
{
    M_SpeakHereBoy_20_4B5330();
}

void Slig::M_SpeakFreeze_22_4B53F0()
{
    M_SpeakHereBoy_20_4B5330();
}

void Slig::M_SpeakGetHim_23_4B5410()
{
    M_SpeakHereBoy_20_4B5330();
}

void Slig::M_SpeakLaugh_24_4B5430()
{
    M_SpeakHereBoy_20_4B5330();
}

void Slig::M_SpeakBullshit1_25_4B5450()
{
    M_SpeakHereBoy_20_4B5330();
}

void Slig::M_SpeakLookOut_26_4B5470()
{
    M_SpeakHereBoy_20_4B5330();
}

void Slig::M_SpeakBullshit2_27_4B5490()
{
    M_SpeakHereBoy_20_4B5330();
}

void Slig::M_SpeakPanic_28_4B54B0()
{
    M_SpeakHereBoy_20_4B5330();
}

void Slig::M_SpeakWhat_29_4B54D0()
{
    M_SpeakHereBoy_20_4B5330();
}

void Slig::M_SpeakAIFreeze_30_4B54F0()
{
    M_SpeakHereBoy_20_4B5330();
}

void Slig::M_Blurgh_31_4B5510()
{
    M_SpeakHereBoy_20_4B5330();
}

void Slig::M_Sleeping_32_4B89A0()
{
    if (mNextMotion == eSligMotions::M_SleepingToStand_33_4B8C50)
    {
        // Out of sleep
        mCurrentMotion = eSligMotions::M_SleepingToStand_33_4B8C50;
        mNextMotion = -1;
    }
    else if (static_cast<s32>(sGnFrame % 60))
    {
        if (!(static_cast<s32>(sGnFrame - 20) % 60))
        {
            Slig_SoundEffect_4BFFE0(SligSfx::eSnooze1_5, this);
            if (gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0))
            {
                FP xOff = {};
                if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
                {
                    xOff = -(mSpriteScale * FP_FromInteger(-20));
                }
                else
                {
                    xOff = (mSpriteScale * FP_FromInteger(-20));
                }
                relive_new SnoozeParticle(
                    xOff + mXPos,
                    mYPos + (mSpriteScale * FP_FromInteger(-10)),
                    mAnim.mRenderLayer,
                    mAnim.field_14_scale);
            }
        }
    }
    else
    {
        Slig_SoundEffect_4BFFE0(SligSfx::eSnooze2_4, this);

        if (gMap.Is_Point_In_Current_Camera(
                mCurrentLevel,
                mCurrentPath,
                mXPos,
                mYPos,
                0))
        {
            FP xOff = {};
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                xOff = -(mSpriteScale * FP_FromInteger(-20));
            }
            else
            {
                xOff = (mSpriteScale * FP_FromInteger(-20));
            }

            relive_new SnoozeParticle(
                xOff + mXPos,
                mYPos + (mSpriteScale * FP_FromInteger(-10)),
                mAnim.mRenderLayer,
                mAnim.field_14_scale);
        }
    }
}

void Slig::M_SleepingToStand_33_4B8C50()
{
    // OWI hack - kill all particles, even if they're not ours!
    for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
    {
        auto pObj = gBaseGameObjects->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj->Type() == ReliveTypes::eSnoozeParticle)
        {
            static_cast<SnoozeParticle*>(pObj)->mState = SnoozeParticle::SnoozeParticleState::eBlowingUp_2;
        }
    }

    if (mAnim.mCurrentFrame >= 2 && mAnim.mCurrentFrame <= 10)
    {
        Slig_SoundEffect_4BFFE0(SligSfx::eToStand_0, this);
    }

    if (mAnim.mCurrentFrame == 9)
    {
        Slig_SoundEffect_4BFFE0(SligSfx::eWalkingStep_2, this);
    }

    switch (mAnim.mCurrentFrame)
    {
        case 9:
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                mXPos -= (mSpriteScale * FP_FromInteger(7));
                return;
            }
            else
            {
                mXPos += (mSpriteScale * FP_FromInteger(7));
                return;
            }
            break;

        case 10:
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                mXPos -= (mSpriteScale * FP_FromInteger(12));
                return;
            }
            else
            {
                mXPos += (mSpriteScale * FP_FromInteger(12));
                return;
            }
            break;

        case 11:
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                mXPos -= (mSpriteScale * FP_FromInteger(6));
                return;
            }
            else
            {
                mXPos += (mSpriteScale * FP_FromInteger(6));
                return;
            }
            break;

        default:
            if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
            {
                MusicController::static_PlayMusic(MusicController::MusicTypes::eTension_4, this, 0, 0);
                MainMovement_4B4720();
            }
            break;
    }
}

void Slig::M_Knockback_34_4B68A0()
{
    if (BaseAliveGameObjectCollisionLine)
    {
        SlowDown_4B6450(FP_FromDouble(0.39));
    }
    else
    {
        M_Falling_7_4B42D0();

        if (mCurrentMotion == eSligMotions::M_LandingSoft_40_4B4530)
        {
            mCurrentMotion = eSligMotions::M_Knockback_34_4B68A0;
            field_12C_timer = sGnFrame + 10;
            if (mYPos - BaseAliveGameObjectLastLineYPos > FP_FromInteger(180)
                && !sPathInfo->TLV_Get_At_4DB4B0(
                    FP_GetExponent(mXPos),
                    FP_GetExponent(mYPos),
                    FP_GetExponent(mXPos),
                    FP_GetExponent(mYPos),
                    ReliveTypes::eSoftLanding))
            {
                mCurrentMotion = eSligMotions::M_LandingFatal_41_4B4680;
                field_12C_timer = sGnFrame + 30;
                mHealth = FP_FromInteger(0);
                EventBroadcast(kEventMudokonComfort, this);
            }
        }
    }

    EventBroadcast(kEventNoise, this);
    if (mAnim.mCurrentFrame == 4)
    {
        Environment_SFX_457A40(EnvironmentSfx::eHitGroundSoft_6, 80, -200, this);
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit12_ForwardLoopCompleted))
    {
        if (BaseAliveGameObjectCollisionLine)
        {
            mVelX = FP_FromInteger(0);

            if (mHealth > FP_FromInteger(0) && field_12C_timer <= static_cast<s32>(sGnFrame) && sActiveHero->mHealth > FP_FromInteger(0))
            {
                mCurrentMotion = eSligMotions::M_KnockbackToStand_35_4B6A30;
            }
        }
    }
}

void Slig::M_KnockbackToStand_35_4B6A30()
{
    EventBroadcast(kEventNoise, this);

    if (mAnim.mCurrentFrame >= 2 && mAnim.mCurrentFrame <= 10)
    {
        Slig_SoundEffect_4BFFE0(SligSfx::eToStand_0, this);
    }

    if (mAnim.mCurrentFrame == 9)
    {
        Slig_SoundEffect_4BFFE0(SligSfx::eWalkingStep_2, this);
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

    if (!WallHit(mSpriteScale * FP_FromInteger(45), gridSize))
    {
        s32 xOff = 0;
        switch (mAnim.mCurrentFrame)
        {
            case 9:
                xOff = 3;
                break;
            case 10:
                xOff = 13;
                break;
            case 11:
                xOff = 9;
                break;
        }

        if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            xOff = -xOff;
        }

        mXPos += (mSpriteScale * mSpriteScale) * FP_FromInteger(xOff);
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToStand_4B4A20();
    }
}

void Slig::M_Depossessing_36_4B7F30()
{
    if (sControlledCharacter == this)
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            if (!Input_IsChanting_45F260())
            {
                mCurrentMotion = eSligMotions::M_DepossessingAbort_16_4B8250;
            }
        }

        if (!(static_cast<s32>(sGnFrame) % 4))
        {
            const FP xRand = FP_FromInteger(Math_RandomRange(-20, 20));
            const FP yRand = FP_FromInteger(Math_RandomRange(20, 50));
            New_TintChant_Particle(
                (mSpriteScale * xRand) + mXPos,
                mYPos - (mSpriteScale * yRand),
                mSpriteScale,
                Layer::eLayer_0);
        }

        if (static_cast<s32>(sGnFrame) > field_12C_timer)
        {
            BlowToGibs_4B8020();
        }
    }
}

void Slig::M_Possess_37_4B72C0()
{
    if (!BaseAliveGameObjectCollisionLine)
    {
        M_Falling_7_4B42D0();
        if (mCurrentMotion != eSligMotions::M_Possess_37_4B72C0)
        {
            mCurrentMotion = eSligMotions::M_Possess_37_4B72C0;
        }
    }

    if (static_cast<s32>(sGnFrame) >= field_150_explode_timer)
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit3_Render))
        {
            FP xOff = (FP_FromInteger(20) * mSpriteScale);
            if (mVelX < FP_FromInteger(0))
            {
                xOff = -xOff;
            }

            relive_new Gibs(
                GibType::Slig_1,
                mXPos,
                mYPos,
                xOff,
                FP_FromInteger(0),
                mSpriteScale,
                0);

            New_Smoke_Particles(
                mXPos,
                mYPos - (FP_FromInteger(30) * mSpriteScale),
                mSpriteScale,
                3,
                0x80u,
                0x80u,
                0x80u);

            if (mSpriteScale == FP_FromDouble(0.5))
            {
                SfxPlayMono(relive::SoundEffects::KillEffect, 80);
                SfxPlayMono(relive::SoundEffects::FallingItemHit, 60);
            }
            else
            {
                SfxPlayMono(relive::SoundEffects::KillEffect, 127);
                SfxPlayMono(relive::SoundEffects::FallingItemHit, 90);
            }

            mAnim.mFlags.Clear(AnimFlags::eBit3_Render);
            mCurrentMotion = eSligMotions::M_StandIdle_0_4B4EC0;
            mVelY = FP_FromInteger(0);
            mVelX = FP_FromInteger(0);
            mHealth = FP_FromInteger(0);
            MusicController::static_PlayMusic(MusicController::MusicTypes::eNone_0, this, 0, 0);
            SetUpdateDelay(40);
            field_120_timer = sGnFrame + 60;
            SetBrain(&Slig::Brain_Death_0_4BBFB0);
        }
    }

    if (static_cast<s32>(sGnFrame) > field_14C_death_by_being_shot_timer)
    {
        if (mVelX >= FP_FromInteger(0))
        {
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                mCurrentMotion = eSligMotions::M_Knockback_34_4B68A0;
            }
        }
        else if (!mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            mCurrentMotion = eSligMotions::M_Knockback_34_4B68A0;
        }
        mCurrentMotion = eSligMotions::M_Smash_44_4B6B90;
    }
}

// When slig is "air walking" in a death fall
void Slig::M_OutToFall_38_4B4570()
{
    M_Falling_7_4B42D0();

    const FP fallDepth = mYPos - BaseAliveGameObjectLastLineYPos;
    if (fallDepth > FP_FromInteger(16 * 640))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    if (mCurrentMotion == eSligMotions::M_LandingSoft_40_4B4530 && fallDepth > FP_FromInteger(180) && !sPathInfo->TLV_Get_At_4DB4B0(FP_GetExponent(mXPos), FP_GetExponent(mYPos), FP_GetExponent(mXPos), FP_GetExponent(mYPos), ReliveTypes::eSoftLanding))
    {
        mCurrentMotion = eSligMotions::M_LandingFatal_41_4B4680;
        field_12C_timer = sGnFrame + 30;
        mHealth = FP_FromInteger(0);
        EventBroadcast(kEventMudokonComfort, this);
    }
}

void Slig::M_FallingInitiate_39_4B4640()
{
    EventBroadcast(kEventNoise, this);

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mCurrentMotion = eSligMotions::M_OutToFall_38_4B4570;
    }

    M_Falling_7_4B42D0();
}

void Slig::M_LandingSoft_40_4B4530()
{
    if (mAnim.mCurrentFrame == 0)
    {
        Environment_SFX_457A40(EnvironmentSfx::eHitGroundSoft_6, 0, 0x7FFF, 0);
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToStand_4B4A20();
    }
}

void Slig::M_LandingFatal_41_4B4680()
{
    if (mAnim.mCurrentFrame == 0)
    {
        SND_SEQ_Play(SeqId::HitBottomOfDeathPit_9, 1, 65, 65);
        SfxPlayMono(relive::SoundEffects::KillEffect, 80);
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        if (static_cast<s32>(sGnFrame) >= field_12C_timer && !BrainIs(&Slig::Brain_Death_0_4BBFB0))
        {
            mHealth = FP_FromInteger(0);
            SetBrain(&Slig::Brain_Death_0_4BBFB0);
            field_120_timer = sGnFrame + 60;
        }
    }
}

void Slig::M_ShootZ_42_4B7560()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        // Controlled by player and no longer pressing down or shoot
        if (sControlledCharacter == this && (!Input().isPressed(sInputKey_ThrowItem) || !Input().isPressed(sInputKey_Down)))
        {
            SND_SEQ_PlaySeq(SeqId::HitBottomOfDeathPit_9, 1, 1);
            mCurrentMotion = eSligMotions::M_ShootZtoStand_43_4B77E0;
        }
        // Controlled by AI and need to leave Z-shoot motion
        else if (mNextMotion != -1)
        {
            SND_SEQ_PlaySeq(SeqId::HitBottomOfDeathPit_9, 1, 1);
            mNextMotion = -1;
            mCurrentMotion = eSligMotions::M_ShootZtoStand_43_4B77E0;
        }
    }
    else if (mAnim.mCurrentFrame == 7)
    {
        BulletType bulletType = BulletType::eSligPossessedOrUnderGlukkonCommand_0;
        if (sControlledCharacter == this)
        {
            bulletType = BulletType::ePossessedSligZBullet_1;
        }
        else
        {
            bulletType = BulletType::eZBullet_3;
        }

        relive_new Bullet(
            this,
            bulletType,
            mXPos,
            mYPos - FP_FromInteger(12),
            FP_FromInteger(640),
            mSpriteScale,
            field_218_tlv_data.mData.mNumTimesToShoot - field_158_num_times_to_shoot - 1);

        New_ShootingZFire_Particle(mXPos, mYPos - FP_FromInteger(12), mSpriteScale);

        if (mSpriteScale == FP_FromDouble(0.5))
        {
            SfxPlayMono(relive::SoundEffects::SligShoot, 85);
        }
        else
        {
            SfxPlayMono(relive::SoundEffects::SligShoot, 0);
        }

        // The doves don't like bullets
        Dove::All_FlyAway(0);

        EventBroadcast(kEventShooting, this);
        EventBroadcast(kEventLoudNoise, this);
    }
}

void Slig::M_ShootZtoStand_43_4B77E0()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToStand_4B4A20();
    }
}

void Slig::M_Smash_44_4B6B90()
{
    if (BaseAliveGameObjectCollisionLine)
    {
        if (mAnim.mCurrentFrame == 4)
        {
            Environment_SFX_457A40(EnvironmentSfx::eHitGroundSoft_6, 80, -200, this);
        }
    }
    else
    {
        M_Falling_7_4B42D0();

        if (mCurrentMotion != eSligMotions::M_Smash_44_4B6B90)
        {
            mCurrentMotion = eSligMotions::M_Smash_44_4B6B90;
        }
    }
}

void Slig::M_PullLever_45_4B8950()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        if (mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit10_Teleporting))
        {
            Slig_GameSpeak_SFX_4C04F0(SligSpeak::eBlurgh_11, 0, field_11E_pitch_min, this);
            mCurrentMotion = eSligMotions::M_Blurgh_31_4B5510;
        }
        else
        {
            ToStand_4B4A20();
        }
    }
}

void Slig::M_LiftGrip_46_4B3700()
{
    auto pLiftPoint = static_cast<LiftPoint*>(sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId));
    if (pLiftPoint)
    {
        pLiftPoint->vMove_4626A0(FP_FromInteger(0), FP_FromInteger(0), 0);
        mVelY = FP_FromInteger(0);
        if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            if (sControlledCharacter == this)
            {
                if (Input().isPressed(sInputKey_Up))
                {
                    if (pLiftPoint->vOnTopFloor())
                    {
                        mCurrentMotion = eSligMotions::M_LiftGripping_48_4B3850;
                    }
                    else
                    {
                        mCurrentMotion = eSligMotions::M_LiftUp_49_4B3930;
                    }
                    return;
                }

                if (Input().isPressed(sInputKey_Down))
                {
                    if (pLiftPoint->vOnBottomFloor())
                    {
                        mCurrentMotion = eSligMotions::M_LiftGripping_48_4B3850;
                    }
                    else
                    {
                        mCurrentMotion = eSligMotions::M_LiftDown_50_4B3960;
                    }
                    return;
                }

                mCurrentMotion = eSligMotions::M_LiftGripping_48_4B3850;
            }
            else
            {
                if (pLiftPoint->vOnTopFloor())
                {
                    mCurrentMotion = eSligMotions::M_LiftDown_50_4B3960;
                }

                if (pLiftPoint->vOnBottomFloor())
                {
                    mCurrentMotion = eSligMotions::M_LiftUp_49_4B3930;
                }
            }
        }
    }
}

void Slig::M_LiftUngrip_47_4B3820()
{
    mVelY = FP_FromInteger(0);

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ToStand_4B4A20();
    }
}

void Slig::M_LiftGripping_48_4B3850()
{
    auto pLiftPoint = static_cast<LiftPoint*>(sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId));
    if (pLiftPoint)
    {
        CheckPlatformVanished_4B3640();

        pLiftPoint->vMove_4626A0(FP_FromInteger(0), FP_FromInteger(0), 0);
        mVelY = FP_FromInteger(0);

        if (Input().isPressed(sInputKey_Up))
        {
            if (!pLiftPoint->vOnTopFloor())
            {
                mCurrentMotion = eSligMotions::M_LiftUp_49_4B3930;
            }
        }
        else if (Input().isPressed(sInputKey_Down))
        {
            if (!pLiftPoint->vOnBottomFloor())
            {
                mCurrentMotion = eSligMotions::M_LiftDown_50_4B3960;
            }
        }
        else if (pLiftPoint->vOnAnyFloor())
        {
            mCurrentMotion = eSligMotions::M_LiftUngrip_47_4B3820;
        }
    }
}

void Slig::M_LiftUp_49_4B3930()
{
    mCurrentMotion = MoveLift_4B3990(FP_FromInteger(-4));
}

void Slig::M_LiftDown_50_4B3960()
{
    mCurrentMotion = MoveLift_4B3990(FP_FromInteger(4));
}


inline PSX_RECT MakeRectFromFP(FP x, FP y, FP w, FP h)
{
    PSX_RECT r = {};
    r.x = FP_GetExponent(x);
    r.w = FP_GetExponent(w);
    r.y = FP_GetExponent(y);
    r.h = FP_GetExponent(h);
    return r;
}

inline PSX_RECT MakeMinMaxRect(FP x, FP y, FP w, FP h, bool flipToMaxMin = false)
{
    if (flipToMaxMin)
    {
        return MakeRectFromFP(
            std::max(x, w),
            std::max(y, h),
            std::min(x, w),
            std::min(y, h));
    }
    else
    {
        return MakeRectFromFP(
            std::min(x, w),
            std::min(y, h),
            std::max(x, w),
            std::max(y, h));
    }
}

void Slig::M_Beat_51_4B6C00()
{
    if (mAnim.mCurrentFrame == 5)
    {
        SFX_Play_Pitch(relive::SoundEffects::AirStream, 90, -300);
    }

    if (mAnim.mCurrentFrame == 8)
    {
        const FP kGridSize = ScaleToGridSize(mSpriteScale);
        const FP k2Scaled = FP_FromInteger(2) * kGridSize;

        PSX_RECT hitRect = {};
        hitRect = MakeMinMaxRect(
            mAnim.mFlags.Get(AnimFlags::eBit5_FlipX) ? mXPos - kGridSize : mXPos + kGridSize,
            mYPos,
            mXPos,
            mYPos - k2Scaled);

        for (s32 i = 0; i < gBaseAliveGameObjects->Size(); i++)
        {
            BaseAliveGameObject* pObj = gBaseAliveGameObjects->ItemAt(i);
            if (!pObj)
            {
                break;
            }

            if (pObj != this)
            {
                if (pObj->Type() == ReliveTypes::eMudokon || pObj->Type() == ReliveTypes::eCrawlingSlig)
                {
                    const PSX_RECT bRect = pObj->VGetBoundingRect();

                    if (pObj->mHealth > FP_FromInteger(0) && pObj->mScale == mScale && PSX_Rects_overlap_no_adjustment(&hitRect, &bRect))
                    {
                        pObj->VTakeDamage(this);
                        EventBroadcast(kEventNoise, this);
                        SfxPlayMono(relive::SoundEffects::FallingItemHit, 60);
                        return;
                    }
                }
            }
        }
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        mCurrentMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    }
}

enum Brain_2_Possessed
{
    eBrain2_StartPossession_0 = 0,
    eBrain2_PossessionShaking_1 = 1,
    eBrain2_PossessionInactive_2 = 2,
    eBrain2_PossessionSpeak_3 = 3,
    eBrain2_ControlledByPlayer_4 = 4
};

enum Brain_35_ChaseAndDisappear
{
    eBrain35_Summoned_0 = 0,
    eBrain35_Running_1 = 1,
    eBrain35_ReachedDestination_2 = 2
};

s16 Slig::Brain_Death_0_4BBFB0()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit3_Render) && field_120_timer < (static_cast<s32>(sGnFrame) + 30))
    {
        mSpriteScale -= FP_FromDouble(0.01);
        mRGB.r -= 2;
        mRGB.g -= 2;
        mRGB.b -= 2;

        if (mSpriteScale >= FP_FromDouble(0.3))
        {
            DeathSmokeEffect(true);
        }
    }
    else if (!mAnim.mFlags.Get(AnimFlags::eBit3_Render))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    if (sControlledCharacter == this)
    {
        if (field_120_timer < static_cast<s32>(sGnFrame))
        {
            sControlledCharacter = sActiveHero;
            MusicController::static_PlayMusic(MusicController::MusicTypes::eNone_0, this, 0, 0);
            gMap.SetActiveCam(field_146_return_level, field_148_return_path, field_14A_return_camera, CameraSwapEffects::eInstantChange_0, 0, 0);
        }
    }

    if (sControlledCharacter != this)
    {
        if (!gMap.Is_Point_In_Current_Camera(
                mCurrentLevel,
                mCurrentPath,
                mXPos,
                mYPos,
                0))
        {
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        }
    }

    if (mSpriteScale < FP_FromInteger(0))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    return 116;
}

s16 Slig::Brain_ReturnControlToAbeAndDie_1_4BC410()
{
    if (sControlledCharacter == this)
    {
        sControlledCharacter = sActiveHero;
        MusicController::static_PlayMusic(MusicController::MusicTypes::eNone_0, this, 0, 0);
        gMap.SetActiveCam(field_146_return_level, field_148_return_path, field_14A_return_camera, CameraSwapEffects::eInstantChange_0, 0, 0);
    }

    mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    return 117;
}

s16 Slig::Brain_Possessed_2_4BBCF0()
{
    switch (field_11C_brain_sub_state)
    {
        case Brain_2_Possessed::eBrain2_StartPossession_0:
            Slig_GameSpeak_SFX_4C04F0(SligSpeak::eHelp_10, 0, field_11E_pitch_min, this);
            field_11C_brain_sub_state = 1;
            mHealth = FP_FromInteger(0);
            mCurrentMotion = eSligMotions::M_Possess_37_4B72C0;
            field_120_timer = sGnFrame + 30;
            field_150_explode_timer = sGnFrame + 1000;
            if (field_14C_death_by_being_shot_timer == 0)
            {
                field_14C_death_by_being_shot_timer = sGnFrame + 1000;
            }
            return field_11C_brain_sub_state;

        case Brain_2_Possessed::eBrain2_PossessionShaking_1:
            if (static_cast<s32>(sGnFrame) >= field_120_timer)
            {
                field_11C_brain_sub_state = Brain_2_Possessed::eBrain2_PossessionInactive_2;
                field_120_timer = sGnFrame + 20;
                mCurrentMotion = eSligMotions::M_StandIdle_0_4B4EC0;
                return field_11C_brain_sub_state;
            }
            break;

        case Brain_2_Possessed::eBrain2_PossessionInactive_2:
            if (static_cast<s32>(sGnFrame) >= field_120_timer)
            {
                if (Math_NextRandom() & 1)
                {
                    field_120_timer = sGnFrame + 20;
                    mCurrentMotion = eSligMotions::M_SpeakHi_21_4B53D0;
                    Slig_GameSpeak_SFX_4C04F0(SligSpeak::eHi_0, 0, field_11E_pitch_min, this);
                }
                else
                {
                    field_120_timer = sGnFrame + 20;
                    mCurrentMotion = eSligMotions::M_SpeakLaugh_24_4B5430;
                    Slig_GameSpeak_SFX_4C04F0(SligSpeak::eLaugh_3, 0, field_11E_pitch_min, this);
                }
                field_11C_brain_sub_state = Brain_2_Possessed::eBrain2_PossessionSpeak_3;
                return field_11C_brain_sub_state;
            }
            break;

        case Brain_2_Possessed::eBrain2_PossessionSpeak_3:
            if (EventGet(kEventDeathReset))
            {
                if (sControlledCharacter != this)
                {
                    mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                }
            }

            if (static_cast<s32>(sGnFrame) >= field_120_timer)
            {
                field_11C_brain_sub_state = Brain_2_Possessed::eBrain2_ControlledByPlayer_4;
                mHealth = FP_FromInteger(1);
                return field_11C_brain_sub_state;
            }
            break;

        case Brain_2_Possessed::eBrain2_ControlledByPlayer_4:
            if (EventGet(kEventDeathReset))
            {
                if (sControlledCharacter != this)
                {
                    mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                }
            }
            if (mHealth <= FP_FromInteger(0))
            {
                SetBrain(&Slig::Brain_Death_0_4BBFB0);
                field_120_timer = sGnFrame + 60;
                return field_11C_brain_sub_state;
            }
            break;

        default:
            return field_11C_brain_sub_state;
    }

    if (static_cast<s32>(sGnFrame) < field_120_timer)
    {
        if (mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit1_bShot))
        {
            BlowToGibs_4B8020();
        }
    }

    return field_11C_brain_sub_state;
}

enum Brain_3_DeathDropDeath
{
    eBrain3_SayHelpOnce_0 = 0,
    eBrain3_SayHelpAndDie_1 = 1,
    eBrain3_SwitchCamToAbe_2 = 2
};

s16 Slig::Brain_DeathDropDeath_3_4BC1E0()
{
    switch (field_11C_brain_sub_state)
    {
        case Brain_3_DeathDropDeath::eBrain3_SayHelpOnce_0:
            Slig_GameSpeak_SFX_4C04F0(SligSpeak::eHelp_10, 0, field_11E_pitch_min, this);
            field_120_timer = sGnFrame + 60;
            return Brain_3_DeathDropDeath::eBrain3_SayHelpAndDie_1;

        case Brain_3_DeathDropDeath::eBrain3_SayHelpAndDie_1:
        {
            if (static_cast<s32>(sGnFrame) < field_120_timer)
            {
                if (!((field_120_timer - sGnFrame) % 15))
                {
                    Slig_GameSpeak_SFX_4C04F0(
                        SligSpeak::eHelp_10,
                        static_cast<s16>(2 * ((field_120_timer & 0xFFFF) - sGnFrame)),
                        field_11E_pitch_min,
                        this);
                }

                if (static_cast<s32>(sGnFrame) == field_120_timer - 6)
                {
                    SND_SEQ_Play(SeqId::HitBottomOfDeathPit_9, 1, 65, 65);
                }

                return field_11C_brain_sub_state;
            }

            Environment_SFX_457A40(EnvironmentSfx::eFallingDeathScreamHitGround_15, 0, 0x7FFF, this);

            relive_new ScreenShake(0, 0);

            field_120_timer = sGnFrame + 30;
            return Brain_3_DeathDropDeath::eBrain3_SwitchCamToAbe_2;
        }

        case Brain_3_DeathDropDeath::eBrain3_SwitchCamToAbe_2:
            if (static_cast<s32>(sGnFrame) > field_120_timer)
            {
                if (sControlledCharacter == this)
                {
                    MusicController::static_PlayMusic(MusicController::MusicTypes::eNone_0, this, 0, 0);
                    sControlledCharacter = sActiveHero;
                    gMap.SetActiveCam(field_146_return_level, field_148_return_path, field_14A_return_camera, CameraSwapEffects::eInstantChange_0, 0, 0);
                }
                mBaseGameObjectFlags.Set(BaseGameObject::eDead);
            }
            return field_11C_brain_sub_state;

        default:
            return field_11C_brain_sub_state;
    }
}

const s16 sGlukkonResponseTable_560768[8][6] = {
    {eSligMotions::M_StandIdle_0_4B4EC0, eSligMotions::M_SpeakHi_21_4B53D0, -1, 0, 0, 0},
    {eSligMotions::M_StandIdle_0_4B4EC0, eSligMotions::M_SpeakWhat_29_4B54D0, -1, 0, 0, 0},
    {eSligMotions::M_StandIdle_0_4B4EC0, eSligMotions::M_SpeakBullshit1_25_4B5450, -1, 0, 0, 0},
    {eSligMotions::M_StandIdle_0_4B4EC0, eSligMotions::M_SpeakBullshit2_27_4B5490, -1, 0, 0, 0},
    {eSligMotions::M_Walking_2_4B5BC0, -1, 0, 0, 0, 0},
    {eSligMotions::M_Running_4_4B6000, -1, 0, 0, 0, 0},
    {eSligMotions::M_StandIdle_0_4B4EC0, eSligMotions::M_SpeakLaugh_24_4B5430, -1, 0, 0, 0},
    {eSligMotions::M_StandIdle_0_4B4EC0, eSligMotions::M_ReloadGun_12_4B5530, eSligMotions::M_ReloadGun_12_4B5530, eSligMotions::M_ReloadGun_12_4B5530, eSligMotions::M_ReloadGun_12_4B5530, -1}};


enum Brain_ListeningToGlukkon_States
{
    GettingAttention_0 = 0,
    IdleListening_1 = 1,
    Moving_2 = 2,
    NextToLeverThroughComeHere_3 = 3,
    Speaking_4 = 4,
    StoppingNextToLever_5 = 5,
    PullingLever_6 = 6,
    StoppingOnLift_7 = 7,
    Shooting_8 = 8,
    LostAttention_9 = 9,
};

enum Brain_ListeningToGlukkon_GlukkonCommands
{
    HeyOrAllYa_0 = 0,
    What_1 = 1,
    Stay_2 = 2,
    Bs_3 = 3,
    Walk_4 = 4,
    Run_5 = 5,
    AfterShootOrLaugh_6 = 6,
    DoitGunReload_7 = 7,
};

s16 Slig::Brain_ListeningToGlukkon_4_4B9D20()
{
    auto pGlukkonObj = static_cast<BaseAliveGameObject*>(sObjectIds.Find_Impl(field_208_glukkon_obj_id));
    auto pPlatformObj = static_cast<LiftPoint*>(sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId));

    field_216_flags.Clear(Flags_216::eBit4_HeardGlukkon);

    if (!pGlukkonObj || !pGlukkonObj->mBaseGameObjectFlags.Get(BaseGameObject::eDrawable_Bit4))
    {
        PauseALittle_4BDD00();
        mNextMotion = eSligMotions::M_SpeakLaugh_24_4B5430;
        field_216_flags.Clear(Flags_216::eBit1_FollowGlukkon);
        --sSligsUnderControlCount_BAF7E8;
        return Brain_ListeningToGlukkon_States::GettingAttention_0;
    }

    switch (field_11C_brain_sub_state)
    {
        case Brain_ListeningToGlukkon_States::GettingAttention_0:
            return Brain_ListenToGlukkon_GettingAttention(pGlukkonObj);
        case Brain_ListeningToGlukkon_States::IdleListening_1:
            return Brain_ListenToGlukkon_IdleListen(pGlukkonObj, pPlatformObj);
        case Brain_ListeningToGlukkon_States::Moving_2:
            return Brain_ListenToGlukkon_Moving(pGlukkonObj);
        case Brain_ListeningToGlukkon_States::NextToLeverThroughComeHere_3:
            return Brain_ListenToGlukkon_NextLever(pGlukkonObj);
        case Brain_ListeningToGlukkon_States::Speaking_4:
            return Brain_ListenToGlukkon_Speaking();
        case Brain_ListeningToGlukkon_States::StoppingNextToLever_5:
            return Brain_ListenToGlukkon_StoppingNextToLever();
        case Brain_ListeningToGlukkon_States::PullingLever_6:
            return Brain_ListeningToGlukkon_PullingLever();
        case Brain_ListeningToGlukkon_States::StoppingOnLift_7:
            return Brain_ListeningToGlukkon_StoppingOnLift(pPlatformObj);
        case Brain_ListeningToGlukkon_States::Shooting_8:
            return Brain_ListeningToGlukkon_Shooting();
        case Brain_ListeningToGlukkon_States::LostAttention_9:
            return Brain_ListeningToGlukkon_LostAttention();
        default:
            field_11C_brain_sub_state = Brain_ListeningToGlukkon_States::GettingAttention_0;
            return field_11C_brain_sub_state;
    }
}

s16 Slig::Brain_ListeningToGlukkon_LostAttention()
{
    SetBrain(&Slig::Brain_Paused_33_4B8DD0);
    return field_11C_brain_sub_state;
}

s16 Slig::Brain_ListeningToGlukkon_Shooting()
{
    LastGlukkonSpeak_4B3090();
    if (static_cast<s32>(sGnFrame) == field_120_timer - 15)
    {
        mNextMotion = eSligMotions::M_Shoot_6_4B55A0;
    }

    if (static_cast<s32>(sGnFrame) <= field_120_timer)
    {
        return field_11C_brain_sub_state;
    }
    NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::AfterShootOrLaugh_6, Brain_ListeningToGlukkon_States::IdleListening_1);
    return Brain_ListeningToGlukkon_States::Speaking_4;
}

s16 Slig::Brain_ListeningToGlukkon_StoppingOnLift(LiftPoint* pPlatformObj)
{
    const FP gridSize = ScaleToGridSize(mSpriteScale);

    if (mCurrentMotion == eSligMotions::M_Knockback_34_4B68A0)
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        return Brain_ListeningToGlukkon_States::IdleListening_1;
    }

    if (mCurrentMotion != eSligMotions::M_StandIdle_0_4B4EC0)
    {
        return field_11C_brain_sub_state;
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        if (!pPlatformObj)
        {
            return Brain_ListeningToGlukkon_States::IdleListening_1;
        }

        if (pPlatformObj->Type() != ReliveTypes::eLiftPoint)
        {
            return Brain_ListeningToGlukkon_States::IdleListening_1;
        }

        const FP gridSizeHalved = (gridSize / FP_FromInteger(2));
        const FP v84 = mXPos - gridSize - FP_FromInteger((BaseAliveGameObjectCollisionLine->mRect.x + BaseAliveGameObjectCollisionLine->mRect.w) / 2) >= FP_FromInteger(0) ? mXPos - gridSize - FP_FromInteger((BaseAliveGameObjectCollisionLine->mRect.x + BaseAliveGameObjectCollisionLine->mRect.w) / 2) : FP_FromInteger((BaseAliveGameObjectCollisionLine->mRect.x + BaseAliveGameObjectCollisionLine->mRect.w) / 2) - (mXPos - gridSize);
        if (v84 >= gridSizeHalved)
        {
            return Brain_ListeningToGlukkon_States::IdleListening_1;
        }
        mNextMotion = eSligMotions::M_SteppingToStand_14_4B8480;
        mVelX = -(gridSize / FP_FromInteger(6));
        return field_11C_brain_sub_state;
    }

    if (!pPlatformObj)
    {
        return Brain_ListeningToGlukkon_States::IdleListening_1;
    }

    if (pPlatformObj->Type() != ReliveTypes::eLiftPoint)
    {
        return Brain_ListeningToGlukkon_States::IdleListening_1;
    }

    const FP v90 = mXPos + gridSize - FP_FromInteger((BaseAliveGameObjectCollisionLine->mRect.x + BaseAliveGameObjectCollisionLine->mRect.w) / 2) >= FP_FromInteger(0) ? mXPos + gridSize
                                                                                                                                                                                   - FP_FromInteger((BaseAliveGameObjectCollisionLine->mRect.x + BaseAliveGameObjectCollisionLine->mRect.w) / 2)
                                                                                                                                                                             : FP_FromInteger((BaseAliveGameObjectCollisionLine->mRect.x + BaseAliveGameObjectCollisionLine->mRect.w) / 2)
                                                                                                                                                                                   - (mXPos + gridSize);
    if (v90 >= gridSize / FP_FromInteger(2))
    {
        return Brain_ListeningToGlukkon_States::IdleListening_1;
    }
    mVelX = (gridSize / FP_FromInteger(6));
    mNextMotion = eSligMotions::M_SteppingToStand_14_4B8480;
    return field_11C_brain_sub_state;
}

s16 Slig::Brain_ListeningToGlukkon_PullingLever()
{
    const FP gridSize = ScaleToGridSize(mSpriteScale);

    Lever* pLever = nullptr;
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        pLever = static_cast<Lever*>(FindObjectOfType(ReliveTypes::eLever, mXPos - gridSize, mYPos - FP_FromInteger(5)));
    }
    else
    {
        pLever = static_cast<Lever*>(FindObjectOfType(ReliveTypes::eLever, mXPos + gridSize, mYPos - FP_FromInteger(5)));
    }

    if (pLever)
    {
        if (mCurrentMotion == eSligMotions::M_PullLever_45_4B8950)
        {
            pLever->VPull(mXPos < pLever->mXPos);
            mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
            return Brain_ListeningToGlukkon_States::IdleListening_1;
        }
        else
        {
            mNextMotion = eSligMotions::M_PullLever_45_4B8950;
            return field_11C_brain_sub_state;
        }
    }
    else
    {
        mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
        return field_11C_brain_sub_state;
    }
}

s16 Slig::Brain_ListenToGlukkon_StoppingNextToLever()
{
    const FP gridSize = ScaleToGridSize(mSpriteScale);

    if (mCurrentMotion == eSligMotions::M_Knockback_34_4B68A0)
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        return Brain_ListeningToGlukkon_States::IdleListening_1;
    }

    if (mCurrentMotion != eSligMotions::M_StandIdle_0_4B4EC0)
    {
        return field_11C_brain_sub_state;
    }

    if (!FindObjectOfType(ReliveTypes::eLever, gridSize + mXPos, mYPos - FP_FromInteger(5)))
    {
        if (!FindObjectOfType(ReliveTypes::eLever, mXPos - gridSize, mYPos - FP_FromInteger(5)))
        {
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                mNextMotion = eSligMotions::M_SteppingToStand_14_4B8480;
                mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(6));
                return field_11C_brain_sub_state;
            }
            mVelX = (gridSize / FP_FromInteger(6));
            mNextMotion = eSligMotions::M_SteppingToStand_14_4B8480;
            return field_11C_brain_sub_state;
        }
    }

    if ((!FindObjectOfType(ReliveTypes::eLever, gridSize + mXPos, mYPos - FP_FromInteger(5)) || !(mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))) && (!FindObjectOfType(ReliveTypes::eLever, mXPos - gridSize, mYPos - FP_FromInteger(5)) || mAnim.mFlags.Get(AnimFlags::eBit5_FlipX)))
    {
        return Brain_ListeningToGlukkon_States::IdleListening_1;
    }

    mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
    return field_11C_brain_sub_state;
}

s16 Slig::Brain_ListenToGlukkon_Speaking()
{
    if (static_cast<s32>(sGnFrame) <= field_120_timer)
    {
        return field_11C_brain_sub_state;
    }

    if (mNextMotion == eSligMotions::M_TurnAroundStanding_5_4B6390)
    {
        return field_11C_brain_sub_state;
    }

    field_120_timer = sGnFrame + 8;

    mNextMotion = sGlukkonResponseTable_560768[field_212_next_command_arg1][field_214_cmd_idx];

    // keeps going till it hits -1
    field_214_cmd_idx++;

    if (sGlukkonResponseTable_560768[field_212_next_command_arg1][field_214_cmd_idx] != -1)
    {
        return field_11C_brain_sub_state;
    }

    return field_20C_state_after_speak;
}

s16 Slig::Brain_ListenToGlukkon_NextLever(BaseAliveGameObject* pGlukkonObj)
{
    if (!VIsFacingMe(pGlukkonObj))
    {
        mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
        return field_11C_brain_sub_state;
    }

    if (mCurrentMotion != eSligMotions::M_StandIdle_0_4B4EC0)
    {
        return field_11C_brain_sub_state;
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(6));
    }
    else
    {
        mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(6));
    }

    mNextMotion = eSligMotions::M_SteppingToStand_14_4B8480;
    return Brain_ListeningToGlukkon_States::IdleListening_1;
}

s16 Slig::Brain_ListenToGlukkon_Moving(BaseAliveGameObject* pGlukkonObj)
{
    FP xOffset = (ScaleToGridSize(mSpriteScale) * FP_FromInteger(mCurrentMotion != eSligMotions::M_Running_4_4B6000 ? 1 : 3));
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        xOffset = -xOffset;
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), xOffset))
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        return Brain_ListeningToGlukkon_States::IdleListening_1;
    }

    if (mCurrentMotion == eSligMotions::M_StandIdle_0_4B4EC0)
    {
        if (mNextMotion != eSligMotions::M_Walking_2_4B5BC0 && mNextMotion != eSligMotions::M_Running_4_4B6000)
        {
            return Brain_ListeningToGlukkon_States::IdleListening_1;
        }
    }

    if (mCurrentMotion == eSligMotions::M_Knockback_34_4B68A0)
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        return Brain_ListeningToGlukkon_States::IdleListening_1;
    }

    if (mCurrentMotion != eSligMotions::M_Walking_2_4B5BC0 && mCurrentMotion != eSligMotions::M_Running_4_4B6000)
    {
        return field_11C_brain_sub_state;
    }

    if (mCurrentMotion != eSligMotions::M_Running_4_4B6000 && FindSwitch_4B9A50())
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        field_216_flags.Clear(Flags_216::eBit1_FollowGlukkon);
        field_216_flags.Set(Flags_216::eBit2_StoppedForLeverOrLift);
        return Brain_ListeningToGlukkon_States::StoppingNextToLever_5;
    }

    if (mCurrentMotion != eSligMotions::M_Running_4_4B6000 && FindLiftPoint_4B9B40())
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        field_216_flags.Clear(Flags_216::eBit1_FollowGlukkon);
        field_216_flags.Set(Flags_216::eBit2_StoppedForLeverOrLift);
        return Brain_ListeningToGlukkon_States::StoppingOnLift_7;
    }

    if (!VIsFacingMe(pGlukkonObj))
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        return Brain_ListeningToGlukkon_States::IdleListening_1;
    }

    if (VIsObjNearby(ScaleToGridSize(mSpriteScale), pGlukkonObj))
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        return Brain_ListeningToGlukkon_States::IdleListening_1;
    }

    if (field_160_last_event_index != pEventSystem_5BC11C->field_28_last_event_index)
    {
        field_160_last_event_index = pEventSystem_5BC11C->field_28_last_event_index;
        if (pEventSystem_5BC11C->field_20_last_event == GameSpeakEvents::Glukkon_StayHere_38 && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0))
        {
            field_216_flags.Clear(Flags_216::eBit1_FollowGlukkon);
            NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::Stay_2, Brain_ListeningToGlukkon_States::IdleListening_1);
            return Brain_ListeningToGlukkon_States::Speaking_4;
        }
    }

    if (!VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromInteger(5), pGlukkonObj))
    {
        if (mCurrentMotion == eSligMotions::M_Walking_2_4B5BC0)
        {
            mNextMotion = eSligMotions::M_Running_4_4B6000;
            return Brain_ListeningToGlukkon_States::Moving_2;
        }
    }

    if (!VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromInteger(5), pGlukkonObj) || mCurrentMotion != eSligMotions::M_Running_4_4B6000)
    {
        return field_11C_brain_sub_state;
    }

    mNextMotion = eSligMotions::M_Walking_2_4B5BC0;
    return Brain_ListeningToGlukkon_States::Moving_2;
}

s16 Slig::Brain_ListenToGlukkon_IdleListen(BaseAliveGameObject* pGlukkonObj, LiftPoint* pPlatformObj)
{
    if (mCurrentMotion != eSligMotions::M_StandIdle_0_4B4EC0)
    {
        field_20E_attention_timeout = 0;
        return field_11C_brain_sub_state;
    }

    if (field_216_flags.Get(Flags_216::eBit1_FollowGlukkon))
    {
        field_20E_attention_timeout++;
    }

    field_216_flags.Clear(Flags_216::eBit2_StoppedForLeverOrLift);

    FP directedGridSize = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        directedGridSize = -ScaleToGridSize(mSpriteScale);
    }
    else
    {
        directedGridSize = ScaleToGridSize(mSpriteScale);
    }

    const FP gridSize = ScaleToGridSize(mSpriteScale);

    const s32 bWallHit = WallHit(mSpriteScale * FP_FromInteger(45), directedGridSize);
    if (field_20E_attention_timeout <= 450 && (gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 1) || field_216_flags.Get(Flags_216::eBit1_FollowGlukkon)))
    {
        if (field_216_flags.Get(Flags_216::eBit1_FollowGlukkon))
        {
            if (!VIsObjNearby(gridSize, pGlukkonObj))
            {
                if (!VIsFacingMe(pGlukkonObj))
                {
                    mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
                    return field_11C_brain_sub_state;
                }

                if (VIsObjNearby(gridSize * FP_FromInteger(5), pGlukkonObj) && pGlukkonObj->mCurrentMotion != eSligMotions::M_Running_4_4B6000)
                {
                    if (!bWallHit)
                    {
                        if (FindLiftPoint_4B9B40())
                        {
                            field_216_flags.Clear(Flags_216::eBit1_FollowGlukkon);
                            return Brain_ListeningToGlukkon_States::StoppingOnLift_7;
                        }
                        else
                        {
                            NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::Walk_4, Brain_ListeningToGlukkon_States::Moving_2);
                            return Brain_ListeningToGlukkon_States::Speaking_4;
                        }
                    }
                }
                else
                {
                    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
                    {
                        directedGridSize = -ScaleToGridSize(mSpriteScale);
                    }
                    else
                    {
                        directedGridSize = ScaleToGridSize(mSpriteScale);
                    }

                    if (!WallHit(mSpriteScale * FP_FromInteger(45), directedGridSize * FP_FromInteger(3)))
                    {
                        NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::Run_5, Brain_ListeningToGlukkon_States::Moving_2);
                        return Brain_ListeningToGlukkon_States::Speaking_4;
                    }
                }
            }

            if (sSligsUnderControlCount_BAF7E8 > 1)
            {
                auto pOtherSlig = static_cast<Slig*>(FindObjectOfType(ReliveTypes::eSlig, mXPos, mYPos - FP_FromInteger(5)));
                if (pOtherSlig)
                {
                    if (pOtherSlig != this && !VIsFacingMe(pOtherSlig) && pOtherSlig->mCurrentMotion != eSligMotions::M_TurnAroundStanding_5_4B6390)
                    {
                        mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
                        return field_11C_brain_sub_state;
                    }
                }
            }
        }

        const GameSpeakEvents glukkonSpeak = LastGlukkonSpeak_4B3090();
        if (glukkonSpeak == GameSpeakEvents::eNone_m1 || glukkonSpeak == GameSpeakEvents::eSameAsLast_m2)
        {
            return field_11C_brain_sub_state;
        }

        if (glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40)
        {
            NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::HeyOrAllYa_0, Brain_ListeningToGlukkon_States::IdleListening_1);
        }
        else
        {
            if (glukkonSpeak == GameSpeakEvents::Glukkon_Laugh_43)
            {
                NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::AfterShootOrLaugh_6, Brain_ListeningToGlukkon_States::IdleListening_1);
            }
            else
            {
                if (!field_216_flags.Get(Flags_216::eBit3_GlukkonCalledAllOYa) || glukkonSpeak == GameSpeakEvents::Glukkon_Hey_36)
                {
                    if (!HeardGlukkonToListenTo_4B9690(glukkonSpeak))
                    {
                        field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa, glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40);
                        return field_11C_brain_sub_state;
                    }
                }

                switch (glukkonSpeak)
                {
                    case GameSpeakEvents::Glukkon_Hey_36:
                        if (!VIsFacingMe(pGlukkonObj))
                        {
                            mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
                        }
                        NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::HeyOrAllYa_0, Brain_ListeningToGlukkon_States::IdleListening_1);
                        break;

                    case GameSpeakEvents::Glukkon_DoIt_37:
                        if (pPlatformObj && pPlatformObj->Type() == ReliveTypes::eLiftPoint)
                        {
                            const FP scaled_2 = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(2));
                            const FP lineMidX = FP_FromInteger((BaseAliveGameObjectCollisionLine->mRect.x + BaseAliveGameObjectCollisionLine->mRect.w) / 2);
                            const FP v33 = mXPos - lineMidX >= FP_FromInteger(0) ? mXPos - lineMidX : lineMidX - mXPos;
                            if (v33 < scaled_2 && (pPlatformObj->vOnTopFloor() || pPlatformObj->vOnBottomFloor()))
                            {
                                mNextMotion = eSligMotions::M_LiftGrip_46_4B3700;

                                for (s32 i = 0; i < gBaseAliveGameObjects->Size(); i++)
                                {
                                    BaseAliveGameObject* pFoundSlig = gBaseAliveGameObjects->ItemAt(i);
                                    if (!pFoundSlig)
                                    {
                                        break;
                                    }

                                    if (pFoundSlig->Type() == ReliveTypes::eSlig && pFoundSlig != this && pFoundSlig->mNextMotion == eSligMotions::M_LiftGrip_46_4B3700)
                                    {
                                        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
                                    }
                                }

                                field_11C_brain_sub_state = Brain_ListeningToGlukkon_States::IdleListening_1;
                                field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa, glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40);
                                return field_11C_brain_sub_state;
                            }
                        }

                        if (FindObjectOfType(ReliveTypes::eLever, ScaleToGridSize(mSpriteScale) + mXPos, mYPos - FP_FromInteger(5)) || FindObjectOfType(ReliveTypes::eLever, mXPos - ScaleToGridSize(mSpriteScale), mYPos - FP_FromInteger(5)))
                        {
                            field_11C_brain_sub_state = Brain_ListeningToGlukkon_States::PullingLever_6;
                            field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa, glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40);
                            return field_11C_brain_sub_state;
                        }

                        if (!FindObjectOfType(ReliveTypes::eMudokon, mXPos, mYPos - FP_FromInteger(5)))
                        {
                            const FP scaled = mAnim.mFlags.Get(AnimFlags::eBit5_FlipX) ? -ScaleToGridSize(mSpriteScale) : ScaleToGridSize(mSpriteScale);

                            if (!FindObjectOfType(ReliveTypes::eMudokon, scaled + mXPos, mYPos - FP_FromInteger(5)))
                            {
                                NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::DoitGunReload_7, Brain_ListeningToGlukkon_States::LostAttention_9);
                                field_11C_brain_sub_state = Brain_ListeningToGlukkon_States::Speaking_4;
                                --sSligsUnderControlCount_BAF7E8;
                                field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa, glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40);
                                return field_11C_brain_sub_state;
                            }
                        }

                        mNextMotion = eSligMotions::M_Beat_51_4B6C00;

                        field_11C_brain_sub_state = Brain_ListeningToGlukkon_States::IdleListening_1;
                        field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa, glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40);
                        return field_11C_brain_sub_state;

                    case GameSpeakEvents::Glukkon_StayHere_38:
                        field_216_flags.Clear(Flags_216::eBit1_FollowGlukkon);
                        NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::Stay_2, Brain_ListeningToGlukkon_States::IdleListening_1);
                        break;

                    case GameSpeakEvents::Glukkon_Commere_39:
                        field_216_flags.Set(Flags_216::eBit1_FollowGlukkon);
                        if (VIsObjNearby(gridSize, pGlukkonObj))
                        {
                            if (!VIsObjNearby(gridSize * FP_FromDouble(0.5), pGlukkonObj))
                            {
                                field_11C_brain_sub_state = Brain_ListeningToGlukkon_States::NextToLeverThroughComeHere_3;
                                field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa, glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40);
                                return field_11C_brain_sub_state;
                            }
                        }

                        if (FindLiftPoint_4B9B40())
                        {
                            field_216_flags.Clear(Flags_216::eBit1_FollowGlukkon);
                            field_11C_brain_sub_state = Brain_ListeningToGlukkon_States::StoppingOnLift_7;
                            field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa, glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40);
                            return field_11C_brain_sub_state;
                        }

                        if (bWallHit)
                        {
                            NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::Bs_3, Brain_ListeningToGlukkon_States::IdleListening_1);
                        }
                        else
                        {
                            if (!VIsFacingMe(pGlukkonObj))
                            {
                                mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
                            }
                            NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::Stay_2, Brain_ListeningToGlukkon_States::IdleListening_1);
                        }
                        break;

                    case GameSpeakEvents::Glukkon_KillEm_44:
                        field_11C_brain_sub_state = Brain_ListeningToGlukkon_States::Shooting_8;
                        field_120_timer = sGnFrame + 25;
                        field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa, glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40);
                        return field_11C_brain_sub_state;

                    default:
                        field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa, glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40);
                        return field_11C_brain_sub_state;
                }
            }
        }
        field_11C_brain_sub_state = Brain_ListeningToGlukkon_States::Speaking_4;
        field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa, glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40);
        return field_11C_brain_sub_state;
    }

    PauseALittle_4BDD00();
    field_216_flags.Clear(Flags_216::eBit1_FollowGlukkon);
    --sSligsUnderControlCount_BAF7E8;
    return Brain_ListeningToGlukkon_States::GettingAttention_0;
}

s16 Slig::Brain_ListenToGlukkon_GettingAttention(BaseAliveGameObject* pGlukkonObj)
{
    mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    field_20E_attention_timeout = 0;
    field_216_flags.Clear(Flags_216::eBit1_FollowGlukkon);
    field_216_flags.Clear(Flags_216::eBit2_StoppedForLeverOrLift);
    if (!VIsFacingMe(pGlukkonObj))
    {
        mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
    }
    NextCommand_4B9A00(Brain_ListeningToGlukkon_GlukkonCommands::HeyOrAllYa_0, Brain_ListeningToGlukkon_States::IdleListening_1);
    return Brain_ListeningToGlukkon_States::Speaking_4;
}

s16 Slig::Brain_Empty_5_4B3220()
{
    
    return 0;
}

s16 Slig::Brain_Empty_6_4B3420()
{
    
    return 0;
}

s16 Slig::Brain_SpottedEnemy_7_4B3240()
{
    if (gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) || field_218_tlv_data.mData.mChaseAbeWhenSpotted == relive::reliveChoice::eNo)
    {
        if (VOnSameYLevel(sControlledCharacter) && VIsObj_GettingNear_On_X(sControlledCharacter) && VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromInteger(3), sControlledCharacter) && !EventGet(kEventResetting) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible))
        {
            ToShoot_4BF9A0();
        }
        else if (
            VOnSameYLevel(sControlledCharacter) && EventGet(kEventAbeOhm) && VIsFacingMe(sControlledCharacter))
        {
            ToShoot_4BF9A0();
        }
        else if (field_120_timer > static_cast<s32>(sGnFrame))
        {
            if (sActiveHero->mHealth <= FP_FromInteger(0))
            {
                ToAbeDead_4B3580();
            }
            else
            {
                ShouldStillBeAlive_4BBC00();
            }
        }
        else if (VIsFacingMe(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible))
        {
            ToShoot_4BF9A0();
        }
        else
        {
            ToTurn_4BE090();
        }
    }
    else
    {
        ToChase_4BCFF0();
    }

    return 302;
}

s16 Slig::Brain_Empty_8_4B3120()
{
    
    return 0;
}

s16 Slig::Brain_Empty_9_4B3440()
{
    
    return 0;
}

s16 Slig::Brain_EnemyDead_10_4B3460()
{
    if (EventGet(kEventDeathReset) && !gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        return 113;
    }

    if (field_120_timer >= static_cast<s32>(sGnFrame))
    {
        ShouldStillBeAlive_4BBC00();
    }
    else
    {
        const auto rnd = Math_NextRandom();

        // Say this often
        if (rnd < 25)
        {
            mNextMotion = eSligMotions::M_SpeakBullshit1_25_4B5450;
            field_120_timer = sGnFrame + 15;
            return 113;
        }

        // Say this less often
        if (rnd < 50)
        {
            mNextMotion = eSligMotions::M_SpeakBullshit2_27_4B5490;
            field_120_timer = sGnFrame + 15;
            return 113;
        }

        // And turn even less often
        if (sActiveHero->mHealth > FP_FromInteger(0))
        {
            TurnOrWalk_4BD6A0(0);
        }
    }
    return 113;
}

s16 Slig::Brain_KilledEnemy_10_4B35C0()
{
    if (field_120_timer > static_cast<s32>(sGnFrame))
    {
        ShouldStillBeAlive_4BBC00();
    }
    else
    {
        ToAbeDead_4B3580();
    }
    return 114;
}

s16 Slig::Brain_PanicTurning_12_4BC490()
{
    if (EventGet(kEventDeathReset) && !gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        return 107;
    }

    if (ListenToGlukkonCommands_4B95D0())
    {
        ToUnderGlukkonCommand_4B9660();
        return 107;
    }

    bool checkTurn = false;
    if (mCurrentMotion != eSligMotions::M_TurnAroundStanding_5_4B6390)
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
            return 107;
        }
        checkTurn = true;
    }
    else if (mCurrentMotion == eSligMotions::M_TurnAroundStanding_5_4B6390)
    {
        if (!mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
        {
            checkTurn = true;
        }
    }

    if (checkTurn)
    {
        if (mAnim.mCurrentFrame == 4)
        {
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                if (sControlledCharacter->mVelX >= FP_FromInteger(0) && (sControlledCharacter->mVelX != FP_FromInteger(0) || sControlledCharacter->mXPos >= mXPos))
                {
                    ShouldStillBeAlive_4BBC00();
                    return 107;
                }
            }
            else
            {
                if (sControlledCharacter->mVelX <= FP_FromInteger(0) && (sControlledCharacter->mVelX != FP_FromInteger(0) || sControlledCharacter->mXPos <= mXPos))
                {
                    ShouldStillBeAlive_4BBC00();
                    return 107;
                }
            }

            const PSX_RECT bRect = VGetBoundingRect();
            const PSX_RECT charRect = sControlledCharacter->VGetBoundingRect();

            if (sControlledCharacter->Type() != ReliveTypes::eGlukkon && sControlledCharacter->Type() != ReliveTypes::eSlig && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible))
            {
                if (charRect.x <= bRect.w && charRect.w >= bRect.x && charRect.h >= bRect.y && charRect.y <= bRect.h)
                {
                    mAnim.mFlags.Toggle(AnimFlags::eBit5_FlipX);
                    return 107;
                }
            }
        }
        ShouldStillBeAlive_4BBC00();
        return 107;
    }

    if (field_120_timer > static_cast<s32>(sGnFrame))
    {
        ToPanicRunning_4BCA30();
    }
    else
    {
        WaitOrWalk_4BE870();
    }
    return 107;
}

s16 Slig::Brain_PanicRunning_13_4BC780()
{
    if (mVelX > FP_FromInteger(4) && ((ScaleToGridSize(mSpriteScale) * FP_FromInteger(4)) + mXPos) > FP_FromInteger(field_138_zone_rect.w))
    {
        ToPanicTurn_4BC750();
    }
    else if (mVelX < FP_FromInteger(-4) && (mXPos - (ScaleToGridSize(mSpriteScale) * FP_FromInteger(4))) < FP_FromInteger(field_138_zone_rect.x))
    {
        ToPanicTurn_4BC750();
    }
    else if (HandleEnemyStopper_4BBA00(4))
    {
        ToPanicTurn_4BC750();
    }
    else if (ListenToGlukkonCommands_4B95D0())
    {
        ToUnderGlukkonCommand_4B9660();
    }
    else if (!mCurrentMotion && mNextMotion == -1)
    {
        ToPanicRunning_4BCA30();
    }
    else if (VOnSameYLevel(sControlledCharacter) && sControlledCharacter->Type() != ReliveTypes::eGlukkon && VIsFacingMe(sControlledCharacter) && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && !IsWallBetween_4BB8B0(this, sControlledCharacter) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting))
    {
        ToShoot_4BF9A0();
    }
    else if (sActiveHero->mHealth <= FP_FromInteger(0))
    {
        ToAbeDead_4B3580();
    }
    else
    {
        if (field_120_timer <= static_cast<s32>(sGnFrame))
        {
            WaitOrWalk_4BE870();
        }
        else
        {
            ShouldStillBeAlive_4BBC00();
        }
    }
    return 109;
}

s16 Slig::Brain_PanicYelling_14_4BCA70()
{
    if (ListenToGlukkonCommands_4B95D0())
    {
        ToUnderGlukkonCommand_4B9660();
    }

    if (mCurrentMotion != eSligMotions::M_SpeakPanic_28_4B54B0)
    {
        if (mNextMotion != eSligMotions::M_SpeakPanic_28_4B54B0)
        {
            mNextMotion = eSligMotions::M_SpeakPanic_28_4B54B0;
        }

        if (mCurrentMotion != eSligMotions::M_SpeakPanic_28_4B54B0)
        {
            ShouldStillBeAlive_4BBC00();
            return 115;
        }
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        EventBroadcast(kEventAlarm, this);

        const bool kFlipX = mAnim.mFlags.Get(AnimFlags::eBit5_FlipX);
        field_120_timer = sGnFrame + field_218_tlv_data.mData.mPanicTimeout;
        if ((!kFlipX || mXPos >= FP_FromInteger((field_138_zone_rect.x + field_138_zone_rect.w) / 2)) && (kFlipX || mXPos <= FP_FromInteger((field_138_zone_rect.x + field_138_zone_rect.w) / 2)))
        {
            ToPanicRunning_4BCA30();
        }
        else
        {
            ToPanicTurn_4BC750();
        }
    }
    else
    {
        ShouldStillBeAlive_4BBC00();
    }
    return 115;
}

s16 Slig::Brain_Idle_15_4BD800()
{
    if ((EventGet(kEventAbeOhm) || EventGet(kEventAlarm)) && !EventGet(kEventResetting) && field_218_tlv_data.mData.mPanicTimeout)
    {
        ToPanicYelling_4BCBA0();
        return 104;
    }

    if (ListenToGlukkonCommands_4B95D0())
    {
        ToUnderGlukkonCommand_4B9660();
        return 104;
    }

    if (VOnSameYLevel(sControlledCharacter) && 
        VIsFacingMe(sControlledCharacter) && 
        !IsInInvisibleZone(sControlledCharacter) &&
        !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) &&
        !IsWallBetween_4BB8B0(this, sControlledCharacter) && (!field_15E_spotted_possessed_slig || sControlledCharacter->Type() != ReliveTypes::eSlig) &&
        !IsAbeEnteringDoor_4BB990(sControlledCharacter) &&
        !EventGet(kEventResetting) &&
        gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && sControlledCharacter->Type() != ReliveTypes::eGlukkon)
    {
        RespondToEnemyOrPatrol_4B3140();
        return 104;
    }

    if (sActiveHero->mHealth <= FP_FromInteger(0))
    {
        ToAbeDead_4B3580();
        return 104;
    }

    auto pShooter = static_cast<BaseAliveGameObject*>(EventGet(kEventShooting));
    if (pShooter && pShooter->mSpriteScale == mSpriteScale && gMap.Is_Point_In_Current_Camera(pShooter->mCurrentLevel, pShooter->mCurrentPath, pShooter->mXPos, pShooter->mYPos, 0) && pShooter == sControlledCharacter && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting))
    {
        if (VIsFacingMe(pShooter))
        {
            RespondToEnemyOrPatrol_4B3140();
            return 104;
        }
        else
        {
            ToTurn_4BE090();
            return 104;
        }
    }

    auto pNoiseOrSpeaking = static_cast<BaseAliveGameObject*>(EventGet(kEventSuspiciousNoise));
    if (!pNoiseOrSpeaking)
    {
        pNoiseOrSpeaking = static_cast<BaseAliveGameObject*>(EventGet(kEventSpeaking));
    }

    if (pNoiseOrSpeaking && pNoiseOrSpeaking->mSpriteScale == mSpriteScale && gMap.Is_Point_In_Current_Camera(pNoiseOrSpeaking->mCurrentLevel, pNoiseOrSpeaking->mCurrentPath, pNoiseOrSpeaking->mXPos, pNoiseOrSpeaking->mYPos, 0) && pNoiseOrSpeaking != this && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting))
    {
        if (pNoiseOrSpeaking != sControlledCharacter)
        {
            if (Math_NextRandom() >= 192u)
            {
                return 104;
            }
        }

        if (VIsFacingMe(sControlledCharacter))
        {
            GoAlertedOrSayWhat_4BF140();
        }
        else
        {
            TurnOrSayWhat_4BEBC0();
        }
        return 104;
    }
    else
    {
        if (sControlledCharacter->mSpriteScale > mSpriteScale && VIsFacingMe(sControlledCharacter) && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !InZCover_4BB7C0(sControlledCharacter) && !InZCover_4BB7C0(this) && !EventGet(kEventResetting))
        {
            ToZShoot_4BF9E0();
            return 104;
        }

        if (mCurrentMotion == eSligMotions::M_StandIdle_0_4B4EC0 && field_120_timer <= static_cast<s32>(sGnFrame))
        {
            ToTurn_4BE090();
            return 104;
        }
        else
        {
            ShouldStillBeAlive_4BBC00();
            return 104;
        }
    }
}

s16 Slig::Brain_StopChasing_16_4BCE30()
{
    if (field_120_timer > static_cast<s32>(sGnFrame))
    {
        if (HandleEnemyStopper_4BBA00(4))
        {
            mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
            SetBrain(&Slig::Brain_Idle_15_4BD800);
            field_120_timer = sGnFrame + 1;
        }
    }
    else
    {
        field_15C_force_alive_state = 0;
        ToTurn_4BE090();
    }
    return 119;
}

s16 Slig::Brain_Chasing_17_4BCBD0()
{
    if (gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && VOnSameYLevel(sControlledCharacter) && VIsFacingMe(sControlledCharacter) && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && !IsWallBetween_4BB8B0(this, sControlledCharacter) && !RenderLayerIs_4BBBC0(sControlledCharacter) && !EventGet(kEventResetting) && sControlledCharacter->Type() != ReliveTypes::eGlukkon)
    {
        field_15C_force_alive_state = 0;
        RespondToEnemyOrPatrol_4B3140();
        return 118;
    }

    if (HandleEnemyStopper_4BBA00(4))
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        SetBrain(&Slig::Brain_Idle_15_4BD800);
        field_120_timer = sGnFrame + 1;
        return 118;
    }

    if (mCurrentPath != gMap.mCurrentPath || mCurrentLevel != gMap.mCurrentLevel || (EventGet(kEventDeathReset) && !gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0)))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
    else if (gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0))
    {
        SetBrain(&Slig::Brain_StopChasing_16_4BCE30);
        field_120_timer = sGnFrame + field_218_tlv_data.mData.mStopChaseDelay;
        return 118;
    }
    return 118;
}

s16 Slig::Brain_StartChasing_18_4BCEB0()
{
    if (field_120_timer > static_cast<s32>(sGnFrame))
    {
        if (gMap.Is_Point_In_Current_Camera(
                mCurrentLevel,
                mCurrentPath,
                mXPos,
                mYPos,
                0)
            && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && sControlledCharacter->Type() != ReliveTypes::eGlukkon)
        {
            field_15C_force_alive_state = 0;
            ToShoot_4BF9A0();
        }
    }
    else
    {
        if (mCurrentPath != gMap.mCurrentPath || mCurrentLevel != gMap.mCurrentLevel)
        {
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        }

        if (!VIsFacingMe(sControlledCharacter))
        {
            mAnim.mFlags.Toggle(AnimFlags::eBit5_FlipX);
        }

        field_15C_force_alive_state = 1;
        mNextMotion = eSligMotions::M_Running_4_4B6000;
        SetBrain(&Slig::Brain_Chasing_17_4BCBD0);
        field_120_timer = field_218_tlv_data.mData.mPauseTime;
    }
    return 122;
}

s16 Slig::Brain_Turning_19_4BDDD0()
{
    if (EventGet(kEventDeathReset) && !gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        return 106;
    }

    if (ListenToGlukkonCommands_4B95D0())
    {
        ToUnderGlukkonCommand_4B9660();
        return 106;
    }

    if ((mCurrentMotion == eSligMotions::M_TurnAroundStanding_5_4B6390 && mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame)) || (mCurrentMotion == eSligMotions::M_StandIdle_0_4B4EC0 && mNextMotion == -1))
    {
        WaitOrWalk_4BE870();
        return 106;
    }

    if (mAnim.mCurrentFrame != 4)
    {
        ShouldStillBeAlive_4BBC00();
        return 106;
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        if (sControlledCharacter->mVelX >= FP_FromInteger(0) && (sControlledCharacter->mVelX > FP_FromInteger(0) || sControlledCharacter->mXPos >= mXPos))
        {
            ShouldStillBeAlive_4BBC00();
            return 106;
        }
    }
    else
    {
        if (sControlledCharacter->mVelX <= FP_FromInteger(0) && (sControlledCharacter->mVelX != FP_FromInteger(0) || sControlledCharacter->mXPos <= mXPos))
        {
            ShouldStillBeAlive_4BBC00();
            return 106;
        }
    }

    const PSX_RECT bRect = VGetBoundingRect();
    const PSX_RECT charRect = sControlledCharacter->VGetBoundingRect();


    if (sControlledCharacter->Type() != ReliveTypes::eGlukkon && sControlledCharacter->mScale == mScale && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && !IsWallBetween_4BB8B0(this, sControlledCharacter) && PSX_Rects_overlap_no_adjustment(&charRect, &bRect) && sControlledCharacter->Type() != ReliveTypes::eSlig)
    {
        mAnim.mFlags.Toggle(AnimFlags::eBit5_FlipX);
        return 106;
    }

    ShouldStillBeAlive_4BBC00();
    return 106;
}

s16 Slig::Brain_StoppingNextToMudokon_20_4BF1E0()
{
    if (mCurrentMotion != eSligMotions::M_StandIdle_0_4B4EC0)
    {
        return 128;
    }

    BaseAliveGameObject* pBeatTarget = FindBeatTarget_4BD070(ReliveTypes::eRingOrLiftMud, 1);
    if (!pBeatTarget || pBeatTarget->mHealth <= FP_FromInteger(0))
    {
        WaitOrWalk_4BE870();
        return 128;
    }

    if (Math_NextRandom() >= 100)
    {
        if (Math_NextRandom() & 1)
        {
            mNextMotion = eSligMotions::M_SpeakBullshit1_25_4B5450;
        }
        else
        {
            mNextMotion = eSligMotions::M_SpeakBullshit2_27_4B5490;
        }
    }
    else
    {
        mNextMotion = eSligMotions::M_Beat_51_4B6C00;
        SetBrain(&Slig::Brain_BeatingUp_24_4BF2B0);
    }

    return 128;
}

s16 Slig::Brain_Walking_21_4BE0C0()
{
    if (unused_BAF7E4) // NOTE: This is never set to true, so this branch is never executed.
    {
        ToPanicRunning_4BCA30();
        return 108;
    }

    if (EventGet(kEventAlarm) && field_218_tlv_data.mData.mPanicTimeout)
    {
        ToPanicYelling_4BCBA0();
        return 108;
    }

    if (mVelX > FP_FromInteger(0))
    {
        if ((ScaleToGridSize(mSpriteScale) * FP_FromInteger(2) + mXPos) >= FP_FromInteger(field_138_zone_rect.w))
        {
            PauseALittle_4BDD00();
            return 108;
        }
    }

    if (mVelX < FP_FromInteger(0))
    {
        if ((mXPos - (ScaleToGridSize(mSpriteScale) * FP_FromInteger(2))) <= FP_FromInteger(field_138_zone_rect.x))
        {
            PauseALittle_4BDD00();
            return 108;
        }
    }

    if (mVelX == FP_FromInteger(0) && mCurrentMotion == eSligMotions::M_StandIdle_0_4B4EC0 && mNextMotion != eSligMotions::M_Walking_2_4B5BC0)
    {
        PauseALittle_4BDD00();
    }
    else if (HandleEnemyStopper_4BBA00(2))
    {
        PauseALittle_4BDD00();
    }
    else if (ListenToGlukkonCommands_4B95D0())
    {
        ToUnderGlukkonCommand_4B9660();
    }
    else if (VOnSameYLevel(sControlledCharacter) &&
        VIsFacingMe(sControlledCharacter) &&
        !IsInInvisibleZone(sControlledCharacter) &&
        !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) &&
        !IsWallBetween_4BB8B0(this, sControlledCharacter) &&
        gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) &&
        gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && (!field_15E_spotted_possessed_slig || sControlledCharacter->Type() != ReliveTypes::eSlig) &&
        !IsAbeEnteringDoor_4BB990(sControlledCharacter) && !EventGet(kEventResetting) && sControlledCharacter->Type() != ReliveTypes::eGlukkon)
    {
        RespondToEnemyOrPatrol_4B3140();
    }
    else
    {
        auto pShooter = static_cast<BaseAliveGameObject*>(EventGet(kEventShooting));
        if (pShooter && pShooter->mSpriteScale == mSpriteScale && gMap.Is_Point_In_Current_Camera(pShooter->mCurrentLevel, pShooter->mCurrentPath, pShooter->mXPos, pShooter->mYPos, 0) && pShooter == sControlledCharacter && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting))
        {
            if (VIsFacingMe(pShooter))
            {
                RespondToEnemyOrPatrol_4B3140();
            }
            else
            {
                ToTurn_4BE090();
            }
        }
        else if (VOnSameYLevel(sControlledCharacter) && VIsFacingMe(sControlledCharacter) && !IsWallBetween_4BB8B0(this, sControlledCharacter) && EventGet(kEventAbeOhm) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting))
        {
            ToShoot_4BF9A0();
        }
        else if (EventGet(kEventAbeOhm) && !EventGet(kEventResetting) && field_218_tlv_data.mData.mPanicTimeout)
        {
            ToPanicYelling_4BCBA0();
        }
        else if (sActiveHero->mHealth <= FP_FromInteger(0))
        {
            ToAbeDead_4B3580();
        }
        else
        {
            auto pNoiseOrSpeaker = static_cast<BaseAliveGameObject*>(EventGet(kEventSuspiciousNoise));
            if (!pNoiseOrSpeaker)
            {
                pNoiseOrSpeaker = static_cast<BaseAliveGameObject*>(EventGet(kEventSpeaking));
            }

            if (pNoiseOrSpeaker && pNoiseOrSpeaker->mSpriteScale == mSpriteScale && gMap.Is_Point_In_Current_Camera(pNoiseOrSpeaker->mCurrentLevel, pNoiseOrSpeaker->mCurrentPath, pNoiseOrSpeaker->mXPos, pNoiseOrSpeaker->mYPos, 0) && pNoiseOrSpeaker != this && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting))
            {
                if (VIsFacingMe(sControlledCharacter))
                {
                    GoAlertedOrSayWhat_4BF140();
                }
                else
                {
                    TurnOrSayWhat_4BEBC0();
                }
            }
            else if (sControlledCharacter->mSpriteScale > mSpriteScale && VIsFacingMe(sControlledCharacter) && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !InZCover_4BB7C0(sControlledCharacter) && !InZCover_4BB7C0(this) && !EventGet(kEventResetting))
            {
                ToZShoot_4BF9E0();
            }
            else
            {
                if (Math_NextRandom() < field_218_tlv_data.mData.mPercentBeatMud && FindBeatTarget_4BD070(ReliveTypes::eRingOrLiftMud, 2) && mCurrentMotion)
                {
                    mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
                    SetBrain(&Slig::Brain_StoppingNextToMudokon_20_4BF1E0);
                    field_120_timer = sGnFrame + 30;
                }
                else
                {
                    ShouldStillBeAlive_4BBC00();
                }
            }
        }
    }
    return 108;
}

s16 Slig::Brain_GetAlertedTurn_22_4BE990()
{
    if (mCurrentMotion == eSligMotions::M_TurnAroundStanding_5_4B6390 && mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        GoAlertedOrSayWhat_4BF140();
        return 123;
    }

    if (ListenToGlukkonCommands_4B95D0())
    {
        ToUnderGlukkonCommand_4B9660();
        return 123;
    }

    if (mAnim.mCurrentFrame == 4)
    {
        bool tryTurningToPlayer = true;
        if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            if (sControlledCharacter->mVelX >= FP_FromInteger(0) && (sControlledCharacter->mVelX > FP_FromInteger(0) || sControlledCharacter->mXPos >= mXPos))
            {
                tryTurningToPlayer = false;
            }
        }
        else if (sControlledCharacter->mVelX <= FP_FromInteger(0) && (sControlledCharacter->mVelX > FP_FromInteger(0) || sControlledCharacter->mXPos <= mXPos))
        {
            tryTurningToPlayer = false;
        }

        if (tryTurningToPlayer)
        {
            if (sControlledCharacter->Type() != ReliveTypes::eGlukkon && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible))
            {
                const PSX_RECT bRect = VGetBoundingRect();
                const PSX_RECT bRectChar = sControlledCharacter->VGetBoundingRect();

                if (PSX_Rects_overlap_no_adjustment(&bRectChar, &bRect))
                {
                    mAnim.mFlags.Toggle(AnimFlags::eBit5_FlipX);
                    return 123;
                }
            }
        }
    }

    if (mCurrentMotion != eSligMotions::M_StandIdle_0_4B4EC0)
    {
        ShouldStillBeAlive_4BBC00();
    }
    else
    {
        mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
    }
    return 123;
}

s16 Slig::Brain_GetAlerted_23_4BEC40()
{
    if (field_120_timer != (field_218_tlv_data.mData.mAlertedListenTime + static_cast<s32>(sGnFrame) - 2) || Math_RandomRange(0, 100) >= field_218_tlv_data.mData.mPercentSayWhat)
    {
        if (ListenToGlukkonCommands_4B95D0())
        {
            ToUnderGlukkonCommand_4B9660();
        }
    }
    else
    {
        mNextMotion = eSligMotions::M_SpeakWhat_29_4B54D0;
    }

    // Check if we should be killing Abe
    if (VOnSameYLevel(sControlledCharacter) &&
        VIsFacingMe(sControlledCharacter) &&
        !IsInInvisibleZone(sControlledCharacter) &&
        !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) &&
        !IsWallBetween_4BB8B0(this, sControlledCharacter) &&
        gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && (sControlledCharacter->Type() != ReliveTypes::eSlig && !field_15E_spotted_possessed_slig) &&
        !IsAbeEnteringDoor_4BB990(sControlledCharacter) && !EventGet(kEventResetting) && sControlledCharacter->Type() != ReliveTypes::eGlukkon)
    {
        RespondToEnemyOrPatrol_4B3140();
    }
    // Panic?
    else if ((EventGet(kEventAbeOhm) || EventGet(kEventAlarm)) && !EventGet(kEventResetting) && field_218_tlv_data.mData.mPanicTimeout)
    {
        ToPanicYelling_4BCBA0();
    }
    else
    {
        // If some fool is trying to shoot us then shoot back
        auto pShootingSlig = static_cast<BaseAliveGameObject*>(EventGet(kEventShooting));
        if (pShootingSlig && pShootingSlig->mSpriteScale == mSpriteScale && gMap.Is_Point_In_Current_Camera(pShootingSlig->mCurrentLevel, pShootingSlig->mCurrentPath, pShootingSlig->mXPos, pShootingSlig->mYPos, 0) && pShootingSlig == sControlledCharacter && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting))
        {
            if (VIsFacingMe(pShootingSlig))
            {
                RespondToEnemyOrPatrol_4B3140();
            }
            else
            {
                ToTurn_4BE090();
            }
        }
        else
        {
            // Is a mud being noisy?
            auto pNoisyMud = static_cast<BaseAliveGameObject*>(EventGet(kEventSuspiciousNoise));
            if (!pNoisyMud)
            {
                pNoisyMud = static_cast<BaseAliveGameObject*>(EventGet(kEventSpeaking));
            }

            // Then kill them
            if (pNoisyMud && gMap.Is_Point_In_Current_Camera(pNoisyMud->mCurrentLevel, pNoisyMud->mCurrentPath, pNoisyMud->mXPos, pNoisyMud->mYPos, 0) && (pNoisyMud == sControlledCharacter || pNoisyMud->Type() == ReliveTypes::eMudokon) && VOnSameYLevel(pNoisyMud) && VIsFacingMe(pNoisyMud) && (pNoisyMud != sControlledCharacter || (!sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting))))
            {
                ToShoot_4BF9A0();
            }
            else
            {
                // Is a slig being noisy?
                auto pNoisySlig = static_cast<BaseAliveGameObject*>(EventGet(kEventSuspiciousNoise));
                if (!pNoisySlig)
                {
                    pNoisySlig = static_cast<BaseAliveGameObject*>(EventGet(kEventSpeaking));
                }

                // Then mSay what, stop walking to respond or try to kill them.
                if (pNoisySlig && gMap.Is_Point_In_Current_Camera(pNoisySlig->mCurrentLevel, pNoisySlig->mCurrentPath, pNoisySlig->mXPos, pNoisySlig->mYPos, 0) && (pNoisySlig == sControlledCharacter || pNoisySlig->Type() != ReliveTypes::eSlig) && !VIsFacingMe(pNoisySlig) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting))
                {
                    TurnOrSayWhat_4BEBC0();
                }
                else
                {
                    if (field_120_timer > static_cast<s32>(sGnFrame))
                    {
                        ShouldStillBeAlive_4BBC00();
                    }
                    else
                    {
                        WaitOrWalk_4BE870();
                    }
                }
            }
        }
    }
    return 124;
}

s16 Slig::Brain_BeatingUp_24_4BF2B0()
{
    if (mCurrentMotion != eSligMotions::M_StandIdle_0_4B4EC0 || field_120_timer >= static_cast<s32>(sGnFrame))
    {
        return 129;
    }

    if (Math_NextRandom() < 100)
    {
        mNextMotion = eSligMotions::M_Beat_51_4B6C00;
        return 129;
    }

    if (Math_NextRandom() < 64)
    {
        mNextMotion = eSligMotions::M_SpeakBullshit1_25_4B5450;
        return 129;
    }

    if (Math_NextRandom() < 64)
    {
        mNextMotion = eSligMotions::M_SpeakBullshit2_27_4B5490;
        return 129;
    }

    WaitOrWalk_4BE870();
    return 129;
}

s16 Slig::Brain_DiscussionWhat_25_4BF380()
{
    if (ListenToGlukkonCommands_4B95D0())
    {
        ToUnderGlukkonCommand_4B9660();
    }

    if (mCurrentMotion == eSligMotions::M_StandIdle_0_4B4EC0 && field_120_timer == static_cast<s32>(sGnFrame))
    {
        mNextMotion = static_cast<s16>(field_294_next_gamespeak_motion);
    }

    if (field_120_timer < static_cast<s32>(sGnFrame - 5))
    {
        WaitOrWalk_4BE870();
    }

    return 128;
}

s16 Slig::Brain_Empty_26_4BF620()
{
    
    return 0;
}

s16 Slig::Brain_Empty_27_4BF600()
{
    
    return 0;
}

s16 Slig::Brain_ZShooting_28_4BFA70()
{
    if (mCurrentMotion != eSligMotions::M_ShootZ_42_4B7560 || !mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        ShouldStillBeAlive_4BBC00();
        return 127;
    }

    field_158_num_times_to_shoot++;

    if (field_158_num_times_to_shoot < field_218_tlv_data.mData.mNumTimesToShoot)
    {
        return 127;
    }

    field_158_num_times_to_shoot = 0;

    if (sActiveHero->mHealth <= FP_FromInteger(0))
    {
        ToAbeDead_4B3580();
    }
    else
    {
        if (sControlledCharacter->mHealth > FP_FromInteger(0))
        {
            WaitOrWalk_4BE870();
        }
        else
        {
            ToKilledAbe_4B3600();
        }
    }
    return 127;
}

s16 Slig::Brain_Shooting_29_4BF750()
{
    if (mCurrentMotion == 6 && mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        field_158_num_times_to_shoot++;

        if (field_158_num_times_to_shoot < field_218_tlv_data.mData.mNumTimesToShoot)
        {
            mNextMotion = eSligMotions::M_Shoot_6_4B55A0;
            return 111;
        }

        if (sActiveHero->mHealth <= FP_FromInteger(0))
        {
            ToKilledAbe_4B3600();
            return 111;
        }

        if (sControlledCharacter->mHealth <= FP_FromInteger(0))
        {
            ToKilledAbe_4B3600();
            return 111;
        }
        if (!VOnSameYLevel(sControlledCharacter) || !VIsFacingMe(sControlledCharacter) || IsInInvisibleZone(sControlledCharacter) || sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) || IsWallBetween_4BB8B0(this, sControlledCharacter) || !gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) || !gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) || EventGet(kEventResetting))
        {
            PauseALittle_4BDD00();
            return 111;
        }

        if (!VIsFacingMe(sControlledCharacter))
        {
            ToTurn_4BE090();
            return 111;
        }

        if (!gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && field_218_tlv_data.mData.mChaseAbeWhenSpotted == relive::reliveChoice::eYes)
        {
            ToChase_4BCFF0();
            return 111;
        }
    }

    ShouldStillBeAlive_4BBC00();
    return 111;
}

s16 Slig::Brain_ZSpottedEnemy_30_4BFA30()
{
    if (field_120_timer > static_cast<s32>(sGnFrame))
    {
        return 126;
    }
    mNextMotion = eSligMotions::M_ShootZ_42_4B7560;
    SetBrain(&Slig::Brain_ZShooting_28_4BFA70);
    return 126;
}

s16 Slig::Brain_WakingUp_31_4B9390()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame))
    {
        WaitOrWalk_4BE870();
    }
    return 0;
}

s16 Slig::Brain_Inactive_32_4B9430()
{
    if (ListenToGlukkonCommands_4B95D0())
    {
        ToUnderGlukkonCommand_4B9660();
    }
    else if (field_120_timer > static_cast<s32>(sGnFrame))
    {
        if (sActiveHero->mHealth <= FP_FromInteger(0))
        {
            ToAbeDead_4B3580();
        }
        else if (VOnSameYLevel(sControlledCharacter) && VIsFacingMe(sControlledCharacter) && VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromInteger(1), sControlledCharacter) && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && !IsWallBetween_4BB8B0(this, sControlledCharacter) && !EventGet(kEventResetting) && sControlledCharacter->Type() != ReliveTypes::eGlukkon)
        {
            ToShoot_4BF9A0();
        }
        else
        {
            ShouldStillBeAlive_4BBC00();
        }
    }
    else
    {
        WaitOrWalk_4BE870();
    }
    return 103;
}

s16 Slig::Brain_Paused_33_4B8DD0()
{
    if (ListenToGlukkonCommands_4B95D0())
    {
        ToUnderGlukkonCommand_4B9660();
        return 101;
    }

    if (VOnSameYLevel(sControlledCharacter) && VIsFacingMe(sControlledCharacter) && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && !IsWallBetween_4BB8B0(this, sControlledCharacter) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && (!field_15E_spotted_possessed_slig || sControlledCharacter->Type() != ReliveTypes::eSlig) && !EventGet(kEventResetting) && sControlledCharacter->Type() != ReliveTypes::eGlukkon)
    {
        RespondToEnemyOrPatrol_4B3140();
        return 101;
    }

    if (VOnSameYLevel(sControlledCharacter) && VIsFacingMe(sControlledCharacter) && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && !IsWallBetween_4BB8B0(this, sControlledCharacter) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting) && sControlledCharacter->Type() != ReliveTypes::eGlukkon)
    {
        ToShoot_4BF9A0();
    }
    else if (sActiveHero->mHealth <= FP_FromInteger(0))
    {
        ToAbeDead_4B3580();
    }
    else
    {
        auto pNoiseOrSpeaking = static_cast<BaseAliveGameObject*>(EventGet(kEventSuspiciousNoise));
        if (!pNoiseOrSpeaking)
        {
            pNoiseOrSpeaking = static_cast<BaseAliveGameObject*>(EventGet(kEventSpeaking));
        }

        if (pNoiseOrSpeaking && pNoiseOrSpeaking->mSpriteScale == mSpriteScale && pNoiseOrSpeaking != this && field_120_timer <= static_cast<s32>(sGnFrame) && !EventGet(kEventResetting))
        {
            ToTurn_4BE090();
        }
        else if (sControlledCharacter->mSpriteScale > mSpriteScale && VIsFacingMe(sControlledCharacter) && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !InZCover_4BB7C0(sControlledCharacter) && !InZCover_4BB7C0(this) && !EventGet(kEventResetting))
        {
            ToZShoot_4BF9E0();
        }
        else
        {
            ShouldStillBeAlive_4BBC00();
        }
    }
    return 101;
}

s16 Slig::Brain_Sleeping_34_4B9170()
{
    auto pEvent = IsEventInRange(kEventSuspiciousNoise, mXPos, mYPos, AsEventScale(mScale));
    if (pEvent)
    {
        auto pNoise = IsEventInRange(kEventNoise, mXPos, mYPos, AsEventScale(mScale));
        if (pNoise)
        {
            pEvent = pNoise;
        }

        const FP wakeUpDistance = ScaleToGridSize(mSpriteScale) * FP_FromInteger(field_218_tlv_data.mData.mNoiseWakeUpDistance);
        if (VIsObjNearby(wakeUpDistance, static_cast<BaseAnimatedWithPhysicsGameObject*>(pEvent)) && field_120_timer <= static_cast<s32>(sGnFrame) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting))
        {
            WakeUp_4B93B0();
            return 102;
        }
    }

    if (EventGet(kEventSpeaking) || EventGet(kEventLoudNoise))
    {
        if (pEvent != this && field_120_timer <= static_cast<s32>(sGnFrame) && gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 0) && !EventGet(kEventResetting))
        {
            WakeUp_4B93B0();
            return 102;
        }
    }

    if (EventGet(kEventAlarm) && !EventGet(kEventResetting))
    {
        WakeUp_4B93B0();
        return 102;
    }

    ShouldStillBeAlive_4BBC00();

    if (mBaseGameObjectFlags.Get(BaseGameObject::eDead))
    {
        const CameraPos direction = gMap.GetDirection(mCurrentLevel, mCurrentPath, mXPos, mYPos);
        Start_Slig_sounds(direction, 0);
    }
    return 102;
}

// A leftover function from Abe's Oddysee.
s16 Slig::Brain_ChaseAndDisappear_35_4BF640()
{
    if (EventGet(kEventDeathReset))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    if (field_11C_brain_sub_state == Brain_35_ChaseAndDisappear::eBrain35_Summoned_0)
    {
        if (sNum_CamSwappers_5C1B66 > 0 || sActiveHero->field_1AC_flags.Get(Abe::e1AC_Bit5_shrivel))
        {
            return field_11C_brain_sub_state;
        }

        mCurrentMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        field_120_timer = sGnFrame + field_218_tlv_data.mData.mPauseTime;
        return Brain_35_ChaseAndDisappear::eBrain35_Running_1;
    }
    else if (field_11C_brain_sub_state == Brain_35_ChaseAndDisappear::eBrain35_Running_1)
    {
        if (static_cast<s32>(sGnFrame) < field_120_timer)
        {
            return field_11C_brain_sub_state;
        }

        mNextMotion = eSligMotions::M_Running_4_4B6000;
        return Brain_35_ChaseAndDisappear::eBrain35_ReachedDestination_2;
    }
    else
    {
        if (field_11C_brain_sub_state == Brain_35_ChaseAndDisappear::eBrain35_ReachedDestination_2 
            && gMap.Is_Point_In_Current_Camera(
                mCurrentLevel,
                mCurrentPath,
                mXPos,
                mYPos,
                0))
        {
            return field_11C_brain_sub_state;
        }
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        mHealth = FP_FromInteger(0);
        return field_11C_brain_sub_state;
    }
}

void Slig::Init()
{
    field_15E_spotted_possessed_slig = 0;
    field_120_timer = field_218_tlv_data.mData.mPauseTime + sGnFrame;

    switch (field_218_tlv_data.mData.mStartState)
    {
        case relive::Path_Slig_Data::StartState::Patrol:
            SetBrain(&Slig::Brain_Inactive_32_4B9430);
            break;

        case relive::Path_Slig_Data::StartState::Sleeping:
            if (field_218_tlv_data.mTlvSpecificMeaning && field_218_tlv_data.mData.mStayAwake == relive::reliveChoice::eYes)
            {
                SetBrain(&Slig::Brain_Inactive_32_4B9430);
            }
            else
            {
                SetBrain(&Slig::Brain_Sleeping_34_4B9170);
                mCurrentMotion = eSligMotions::M_Sleeping_32_4B89A0;
                vUpdateAnim_4B1320();
            }
            break;

        case relive::Path_Slig_Data::StartState::Chase:
            SetBrain(&Slig::Brain_StartChasing_18_4BCEB0);
            field_120_timer = sGnFrame + field_218_tlv_data.mData.mTimeToWaitBeforeChase;
            break;

        case relive::Path_Slig_Data::StartState::ChaseAndDisappear:
            SetBrain(&Slig::Brain_ChaseAndDisappear_35_4BF640);
            field_120_timer = sGnFrame + field_218_tlv_data.mData.mPauseTime;
            break;;

        case relive::Path_Slig_Data::StartState::ListeningToGlukkon:
            for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
            {
                BaseGameObject* pObj = gBaseGameObjects->ItemAt(i);
                if (!pObj)
                {
                    break;
                }

                if (pObj->Type() == ReliveTypes::eGlukkon)
                {
                    auto pGlukkon = static_cast<BaseAliveGameObject*>(pObj);
                    if (gMap.Is_Point_In_Current_Camera(
                            pGlukkon->mCurrentLevel,
                            pGlukkon->mCurrentPath,
                            pGlukkon->mXPos,
                            pGlukkon->mYPos,
                            0))
                    {
                        field_208_glukkon_obj_id = pGlukkon->mBaseGameObjectId;
                        sSligsUnderControlCount_BAF7E8++;
                        field_216_flags.Set(Flags_216::eBit1_FollowGlukkon);
                        SetBrain(&Slig::Brain_ListeningToGlukkon_4_4B9D20);
                        field_11C_brain_sub_state = Brain_ListeningToGlukkon_States::IdleListening_1;
                        break;
                    }
                }
            }

            if (field_208_glukkon_obj_id != Guid{})
            {
                SetBrain(&Slig::Brain_Inactive_32_4B9430);
            }
            break;

        default:
            SetBrain(&Slig::Brain_Paused_33_4B8DD0);
            break;
    }

    if (field_218_tlv_data.mData.mFacing == relive::reliveXDirection::eLeft)
    {
        mAnim.mFlags.Set(AnimFlags::eBit5_FlipX);
    }

    field_290_points_count = 0;

    field_268_points[field_290_points_count].x = FP_GetExponent(mXPos);
    field_268_points[field_290_points_count].y = FP_GetExponent(mYPos);
    field_290_points_count++;

    for (s16 yCam = -3; yCam < 4; yCam++)
    {
        for (s16 xCam = -3; xCam < 4; xCam++)
        {
            relive::Path_TLV* pTlvIter = sPathInfo->Get_First_TLV_For_Offsetted_Camera(xCam, yCam);
            while (pTlvIter)
            {
                bool addPoint = false;
                if (pTlvIter->mTlvType == ReliveTypes::eSligBoundLeft)
                {
                    if (static_cast<relive::Path_SligBoundLeft*>(pTlvIter)->mSligBoundId == field_218_tlv_data.mData.mSligBoundId)
                    {
                        field_138_zone_rect.x = pTlvIter->mTopLeftX;
                        addPoint = true;
                    }
                }
                else if (pTlvIter->mTlvType == ReliveTypes::eSligBoundRight)
                {
                    if (static_cast<relive::Path_SligBoundRight*>(pTlvIter)->mSligBoundId == field_218_tlv_data.mData.mSligBoundId)
                    {
                        field_138_zone_rect.w = pTlvIter->mTopLeftX;
                        addPoint = true;
                    }
                }
                else if (pTlvIter->mTlvType == ReliveTypes::eSligPersist)
                {
                    if (static_cast<relive::Path_SligPersist*>(pTlvIter)->mSligBoundId == field_218_tlv_data.mData.mSligBoundId)
                    {
                        addPoint = true;
                    }
                }

                if (addPoint)
                {
                    if (field_290_points_count < ALIVE_COUNTOF(field_268_points))
                    {
                        field_268_points[field_290_points_count].x = pTlvIter->mTopLeftX;
                        field_268_points[field_290_points_count].y = pTlvIter->mTopLeftY;
                        field_290_points_count++;
                    }
                }

                pTlvIter = Path::Next_TLV(pTlvIter);
            }
        }
    }
}

Slig::~Slig()
{
    if (sControlledCharacter == this)
    {
        sControlledCharacter = sActiveHero;

        MusicController::static_PlayMusic(MusicController::MusicTypes::eNone_0, this, 0, 0);

        if (gMap.mNextLevel != EReliveLevelIds::eMenu)
        {
            gMap.SetActiveCam(
                field_146_return_level,
                field_148_return_path,
                field_14A_return_camera,
                CameraSwapEffects::eInstantChange_0,
                0,
                0);
        }
    }

    relive::Path_TLV* pTlv = sPathInfo->TLV_From_Offset_Lvl_Cam(field_118_tlvInfo);
    if (pTlv)
    {
        if (pTlv->mTlvType != ReliveTypes::eSligGetPants && pTlv->mTlvType != ReliveTypes::eSligSpawner)
        {
            if (mHealth <= FP_FromInteger(0))
            {
                pTlv->mTlvFlags.Clear(relive::TlvFlags::eBit1_Created);
                pTlv->mTlvFlags.Set(relive::TlvFlags::eBit2_Destroyed);
            }
            else
            {
                pTlv->mTlvFlags.Clear(relive::TlvFlags::eBit1_Created);
                pTlv->mTlvFlags.Clear(relive::TlvFlags::eBit2_Destroyed);
            }
        }
    }
}

const FP dword_5473E8[8] = {
    FP_FromInteger(4),
    FP_FromInteger(4),
    FP_FromInteger(0),
    FP_FromInteger(-4),
    FP_FromInteger(-4),
    FP_FromInteger(-4),
    FP_FromInteger(0),
    FP_FromInteger(4)};

// TODO: Size 8 ??
const FP dword_547408[22] = {
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

void Slig::VUpdate()
{
    if (mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit9_RestoredFromQuickSave))
    {
        if (BaseAliveGameObjectCollisionLineType != -1)
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

            if (BaseAliveGameObjectCollisionLine)
            {
                if (BaseAliveGameObjectCollisionLine->mLineType == eLineTypes::eDynamicCollision_32 || BaseAliveGameObjectCollisionLine->mLineType == eLineTypes::eBackgroundDynamicCollision_36)
                {
                    const PSX_RECT bRect = VGetBoundingRect();
                    VOnCollisionWith(
                        {bRect.x, static_cast<s16>(bRect.y + 5)},
                        {bRect.w, static_cast<s16>(bRect.h + 5)},
                        ObjList_5C1B78,
                        (TCollisionCallBack) &BaseAliveGameObject::OnTrapDoorIntersection);
                }
            }
        }
        field_208_glukkon_obj_id = BaseGameObject::RefreshId(field_208_glukkon_obj_id);
        if (BrainIs(&Slig::Brain_ListeningToGlukkon_4_4B9D20))
        {
            sSligsUnderControlCount_BAF7E8++;
        }
        mBaseAliveGameObjectFlags.Clear(Flags_114::e114_Bit9_RestoredFromQuickSave);
        BaseAliveGameObjectCollisionLineType = 0;
    }

    if (!Input_IsChanting_45F260())
    {
        field_292_prevent_depossession &= ~1u;
    }

    if (sControlledCharacter == this && mHealth > FP_FromInteger(0))
    {
        MusicController::static_PlayMusic(MusicController::MusicTypes::ePossessed_9, this, 0, 0);
    }

    if (sDDCheat_FlyingEnabled_5C2C08 && sControlledCharacter == this)
    {
        BaseAliveGameObjectCollisionLine = nullptr;

        // TODO: InputCommand constants
        if (Input().mPads[sCurrentControllerIndex].mPressed & 0xF)
        {
            mVelX = dword_5473E8[Input().mPads[sCurrentControllerIndex].mDir >> 5];
            mVelY = dword_547408[Input().mPads[sCurrentControllerIndex].mDir >> 5];

            if (Input().mPads[sCurrentControllerIndex].mPressed & 0x10)
            {
                mVelX += dword_5473E8[Input().mPads[sCurrentControllerIndex].mDir >> 5];
                mVelX += dword_5473E8[Input().mPads[sCurrentControllerIndex].mDir >> 5];
                mVelY += dword_547408[Input().mPads[sCurrentControllerIndex].mDir >> 5];
            }

            mXPos += mVelX;
            mYPos += mVelY;

            // Keep in the map bounds
            PSX_Point mapBounds = {};
            gMap.Get_map_size(&mapBounds);

            if (mXPos < FP_FromInteger(0))
            {
                mXPos = FP_FromInteger(0);
            }

            if (mXPos >= FP_FromInteger(mapBounds.x))
            {
                mXPos = FP_FromInteger(mapBounds.x) - FP_FromInteger(1);
            }

            if (mYPos < FP_FromInteger(0))
            {
                mYPos = FP_FromInteger(0);
            }

            if (mYPos >= FP_FromInteger(mapBounds.y))
            {
                mYPos = FP_FromInteger(mapBounds.y) - FP_FromInteger(1);
            }
        }
        else
        {
            mVelX = FP_FromInteger(0);
            mVelY = FP_FromInteger(0);
        }

        SetActiveCameraDelayedFromDir();

        BaseAliveGameObjectLastLineYPos = mYPos;
    }
    else
    {
        const auto oldMotion = mCurrentMotion;
        field_11C_brain_sub_state = (this->*field_154_brain_state)();

        if (mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit1_bShot))
        {
            vShot_4B2EA0();
        }

        if (sDDCheat_ShowAI_Info)
        {
            DDCheat::DebugStr("Slig %d %d %d %d\n", field_11C_brain_sub_state, field_120_timer, mCurrentMotion, mNextMotion);
        }

        const FP oldXPos = mXPos;
        const FP oldYPos = mYPos;

        (this->*sSlig_motion_table_5604A0[mCurrentMotion])();

        if (oldXPos != mXPos || oldYPos != mYPos)
        {
            BaseAliveGameObjectPathTLV = sPathInfo->TlvGetAt(
                nullptr,
                mXPos,
                mYPos,
                mXPos,
                mYPos);
            VOnTlvCollision(BaseAliveGameObjectPathTLV);
        }

        if (oldMotion != mCurrentMotion || mBaseAliveGameObjectFlags.Get(Flags_114::e114_MotionChanged_Bit2))
        {
            mBaseAliveGameObjectFlags.Clear(Flags_114::e114_MotionChanged_Bit2);
            vUpdateAnim_4B1320();
        }
        else if (field_124_return_to_previous_motion)
        {
            mCurrentMotion = mPreviousMotion;
            vUpdateAnim_4B1320();
            mAnim.SetFrame(mBaseAliveGameObjectLastAnimFrame);
            field_124_return_to_previous_motion = 0;
        }
    }
}

void Slig::VScreenChanged()
{
    if (gMap.mCurrentLevel != gMap.mNextLevel || gMap.mOverlayId != gMap.GetOverlayId() || (gMap.mCurrentPath != gMap.mNextPath && this != sControlledCharacter))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

void Slig::VPossessed()
{
    mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit4_bPossesed);
    field_292_prevent_depossession |= 1u;
    if (mNextMotion != eSligMotions::M_KnockbackToStand_35_4B6A30 && mNextMotion != eSligMotions::M_Knockback_34_4B68A0)
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    }
    SetBrain(&Slig::Brain_Possessed_2_4BBCF0);
    field_11C_brain_sub_state = Brain_2_Possessed::eBrain2_StartPossession_0;

    field_146_return_level = gMap.mCurrentLevel;
    field_148_return_path = gMap.mCurrentPath;
    field_14A_return_camera = gMap.mCurrentCamera;

    MusicController::static_PlayMusic(MusicController::MusicTypes::ePossessed_9, this, 1, 0);
}

void Slig::VUnPosses()
{
    mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    field_120_timer = sGnFrame + 180;
    MusicController::static_PlayMusic(MusicController::MusicTypes::eNone_0, this, 0, 0);
}

void Slig::VOnTlvCollision(relive::Path_TLV* pTlv)
{
    while (pTlv)
    {
        if (pTlv->mTlvType == ReliveTypes::eDeathDrop)
        {
            if (mHealth > FP_FromInteger(0))
            {
                mHealth = FP_FromInteger(0);
                field_11C_brain_sub_state = 0;
                SetBrain(&Slig::Brain_DeathDropDeath_3_4BC1E0);
                mVelY = FP_FromInteger(0);
                mVelX = FP_FromInteger(0);
                VSetMotion(eSligMotions::M_Falling_7_4B42D0);
                EventBroadcast(kEventMudokonComfort, this);
            }
        }
        pTlv = sPathInfo->TlvGetAt(pTlv, mXPos, mYPos, mXPos, mYPos);
    }
}

void Slig::vShot_4B2EA0()
{
    if (field_136_shot_motion != -1)
    {
        mCurrentMotion = field_136_shot_motion;
    }

    mBaseAliveGameObjectFlags.Clear(Flags_114::e114_Bit1_bShot);
    mNextMotion = -1;
    field_136_shot_motion = -1;
    SetBrain(&Slig::Brain_Death_0_4BBFB0);
    field_120_timer = sGnFrame + 60;
}

void Slig::vUpdateAnim_4B1320()
{
    // note: OG was falling back to eSligMotions::M_StandIdle_0_4B4EC0 if 
    // ResForMotion_4B1E90 didnt return a resource (which can happen depending on disabled resources value)
    mAnim.Set_Animation_Data(GetAnimRes(MotionToAnimId(mCurrentMotion)));
}

bool Slig::vUnderGlukkonCommand_4B1760()
{
    return BrainIs(&Slig::Brain_ListeningToGlukkon_4_4B9D20);
}

void Slig::WakeUp_4B93B0()
{
    mNextMotion = eSligMotions::M_SleepingToStand_33_4B8C50;
    SetBrain(&Slig::Brain_WakingUp_31_4B9390);

    MusicController::static_PlayMusic(MusicController::MusicTypes::eTension_4, this, 0, 0);
    relive::Path_TLV* pTlv = sPathInfo->TLV_Get_At_4DB4B0(
        field_218_tlv_data.mTopLeftX,
        field_218_tlv_data.mTopLeftY,
        field_218_tlv_data.mTopLeftX,
        field_218_tlv_data.mTopLeftY,
        ReliveTypes::eSlig);
    if (pTlv)
    {
        pTlv->mTlvSpecificMeaning = 1; // TODO: Keep track of these, 1 must keep slig awake ??
    }
}

void Slig::ShouldStillBeAlive_4BBC00()
{
    if (!field_15C_force_alive_state)
    {
        // Check not falling and not in the current screen
        if (mCurrentMotion != eSligMotions::M_Falling_7_4B42D0
            && mCurrentMotion != eSligMotions::M_OutToFall_38_4B4570
            && !gMap.Is_Point_In_Current_Camera(
                mCurrentLevel,
                mCurrentPath,
                mXPos,
                mYPos,
                0))
        {
            if (field_290_points_count <= 0)
            {
                // No patrol points, die
                mBaseGameObjectFlags.Set(BaseGameObject::eDead);
            }
            else
            {
                s32 i = 0;
                while (!gMap.Is_Point_In_Current_Camera(
                    mCurrentLevel,
                    mCurrentPath,
                    FP_FromInteger(field_268_points[i].x),
                    FP_FromInteger(field_268_points[i].y),
                    0))
                {
                    if (i >= field_290_points_count)
                    {
                        // No within any out our patrol points, die
                        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                        break;
                    }
                    i++;
                }
            }
        }
    }
}

void Slig::ToTurn_4BE090()
{
    mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
    SetBrain(&Slig::Brain_Turning_19_4BDDD0);
}

void Slig::RespondWithWhat_4BF400()
{
    field_120_timer = sGnFrame + 20;
    if (VIsFacingMe(sControlledCharacter))
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    }
    else
    {
        mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
    }
    SetBrain(&Slig::Brain_DiscussionWhat_25_4BF380);
}

void Slig::ToShoot_4BF9A0()
{
    mNextMotion = eSligMotions::M_Shoot_6_4B55A0;
    SetBrain(&Slig::Brain_Shooting_29_4BF750);
    field_158_num_times_to_shoot = 0;
    field_15A_unused = 0;
    MusicController::static_PlayMusic(MusicController::MusicTypes::eSoftChase_8, this, 0, 0);
}

void Slig::ToZShoot_4BF9E0()
{
    mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    field_120_timer = sGnFrame + field_218_tlv_data.mData.mZShootDelay;
    SetBrain(&Slig::Brain_ZSpottedEnemy_30_4BFA30);
    MusicController::static_PlayMusic(MusicController::MusicTypes::eSoftChase_8, this, 0, 0);
}

void Slig::PauseALittle_4BDD00()
{
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        field_120_timer = field_218_tlv_data.mData.mPauseLeftMin + sGnFrame;
        if (field_218_tlv_data.mData.mPauseLeftMax > field_218_tlv_data.mData.mPauseLeftMin)
        {
            field_120_timer += Math_NextRandom() % (field_218_tlv_data.mData.mPauseLeftMax - field_218_tlv_data.mData.mPauseLeftMin);
        }
    }
    else
    {
        field_120_timer = field_218_tlv_data.mData.mPauseRightMin + sGnFrame;
        if (field_218_tlv_data.mData.mPauseRightMax > field_218_tlv_data.mData.mPauseRightMin)
        {
            field_120_timer += Math_NextRandom() % (field_218_tlv_data.mData.mPauseRightMax - field_218_tlv_data.mData.mPauseRightMin);
        }
    }
    mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    SetBrain(&Slig::Brain_Idle_15_4BD800);
}

void Slig::ToStand_4B4A20()
{
    field_130_falling_velx_scale_factor = FP_FromInteger(0);
    mVelX = FP_FromInteger(0);
    mVelY = FP_FromInteger(0);
    mCurrentMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    mInput = 0;
    field_12C_timer = Math_RandomRange(0, 60) + sGnFrame + 120;
    MapFollowMe(TRUE);
}

void Slig::BlowToGibs_4B8020()
{
    relive_new Gibs(
        GibType::Slig_1,
        mXPos,
        mYPos,
        mVelX,
        mVelY,
        mSpriteScale,
        0);

    relive_new Blood(
        mXPos,
        mYPos - (FP_FromInteger(30) * mSpriteScale),
        FP_FromInteger(0),
        FP_FromInteger(0),
        mSpriteScale,
        20);

    New_Smoke_Particles(
        mXPos,
        mYPos - (FP_FromInteger(30) * mSpriteScale),
        mSpriteScale,
        3,
        128,
        128,
        128);

    if (mSpriteScale == FP_FromDouble(0.5))
    {
        SfxPlayMono(relive::SoundEffects::KillEffect, 80);
        SfxPlayMono(relive::SoundEffects::FallingItemHit, 60);
    }
    else
    {
        SfxPlayMono(relive::SoundEffects::KillEffect, 127);
        SfxPlayMono(relive::SoundEffects::FallingItemHit, 90);
    }

    mAnim.mFlags.Clear(AnimFlags::eBit3_Render);

    mCurrentMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    mVelY = FP_FromInteger(0);
    mVelX = FP_FromInteger(0);
    mHealth = FP_FromInteger(0);
    MusicController::static_PlayMusic(MusicController::MusicTypes::eNone_0, this, 0, 0);
    field_120_timer = sGnFrame + 60;
    SetUpdateDelay(40);
    SetBrain(&Slig::Brain_Death_0_4BBFB0);
    EventBroadcast(kEventMudokonComfort, this);
}

s16 Slig::MainMovement_4B4720()
{
    if (sControlledCharacter == this && mHealth > FP_FromInteger(0))
    {
        return HandlePlayerControlled_4B7800();
    }

    if (mNextMotion == eSligMotions::M_TurnAroundStanding_5_4B6390)
    {
        mCurrentMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
        mNextMotion = -1;
        return 1;
    }

    if (mNextMotion != eSligMotions::M_Walking_2_4B5BC0)
    {
        if (mNextMotion == eSligMotions::M_Running_4_4B6000)
        {
            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
            }
            else
            {
                mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
            }
            mCurrentMotion = eSligMotions::M_StandToRun_3_4B62F0;
            mNextMotion = -1;
            return 1;
        }

        if (mNextMotion == eSligMotions::M_Shoot_6_4B55A0 || mNextMotion == eSligMotions::M_ShootZ_42_4B7560)
        {
            mCurrentMotion = mNextMotion;
            mNextMotion = -1;
            field_12C_timer = sGnFrame + 60;
            field_130_falling_velx_scale_factor = FP_FromInteger(0);
            mVelX = FP_FromInteger(0);
            mVelY = FP_FromInteger(0);
        }
        else
        {
            if (mNextMotion < eSligMotions::M_SpeakHereBoy_20_4B5330 || mNextMotion > eSligMotions::M_Blurgh_31_4B5510)
            {
                if (mNextMotion == eSligMotions::M_ReloadGun_12_4B5530 || mNextMotion == eSligMotions::M_PullLever_45_4B8950 || mNextMotion == eSligMotions::M_SteppingToStand_14_4B8480 || mNextMotion == eSligMotions::M_LiftGrip_46_4B3700 || mNextMotion == eSligMotions::M_Beat_51_4B6C00)
                {
                    mCurrentMotion = mNextMotion;
                    mNextMotion = -1;
                    return 1;
                }
                else
                {
                    ToStand_4B4A20();
                    return 0;
                }
            }

            mCurrentMotion = GetNextMotionIncGameSpeak_4B5080(0);
            if (mCurrentMotion == -1)
            {
                ToStand_4B4A20();
                return 0;
            }

            if (!BrainIs(&Slig::Brain_ListeningToGlukkon_4_4B9D20))
            {
                EventBroadcast(kEventSpeaking, this);
                return 1;
            }
        }
        return 1;
    }

    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        if (WallHit(mSpriteScale * FP_FromInteger(45), -ScaleToGridSize(mSpriteScale)))
        {
            return 0;
        }
        mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
    }
    else
    {
        if (WallHit(mSpriteScale * FP_FromInteger(45), ScaleToGridSize(mSpriteScale)))
        {
            return 0;
        }
        mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
    }

    mCurrentMotion = eSligMotions::M_StandToWalk_1_4B5F70;
    mNextMotion = -1;

    return 1;
}

s16 Slig::LeftRigtMovement(MovementDirection direction)
{
    FP offX1 = {};
    FP offX2 = {};
    FP StandToRunVelX = {};
    FP StandingToStepVelX = {};
    FP SteppingToStandVelX = {};

    switch (direction)
    {
        case MovementDirection::eLeft:
            StandToRunVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
            StandingToStepVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
            SteppingToStandVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(6));
            offX1 = -(ScaleToGridSize(mSpriteScale) * FP_FromInteger(2));
            offX2 = -(ScaleToGridSize(mSpriteScale));
            break;

        case MovementDirection::eRight:
            StandToRunVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
            StandingToStepVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
            SteppingToStandVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(6));
            offX1 = (ScaleToGridSize(mSpriteScale) * FP_FromInteger(2));
            offX2 = (ScaleToGridSize(mSpriteScale));
            break;

        default:
            break;
    }

    if ((direction == MovementDirection::eLeft && !mAnim.mFlags.Get(AnimFlags::eBit5_FlipX)) || (direction == MovementDirection::eRight && mAnim.mFlags.Get(AnimFlags::eBit5_FlipX)))
    {
        mCurrentMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
        return 1;
    }

    if (Input().isPressed(sInputKey_Run))
    {
        mVelX = StandToRunVelX;
        mCurrentMotion = eSligMotions::M_StandToRun_3_4B62F0;
        return 1;
    }

    if (!WallHit(mSpriteScale * FP_FromInteger(45), offX1))
    {
        mVelX = StandingToStepVelX;
        mCurrentMotion = eSligMotions::M_StandingToStep_15_4B83B0;
        return 1;
    }

    if (WallHit(mSpriteScale * FP_FromInteger(45), offX2))
    {
        return 0;
    }

    mVelX = SteppingToStandVelX;
    mCurrentMotion = eSligMotions::M_SteppingToStand_14_4B8480;
    return 1;
}

void Slig::PullLever()
{
    FP switchXPos = {};
    FP switchYPos = {};
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        switchXPos = mYPos - FP_FromInteger(5);
        switchYPos = mXPos - ScaleToGridSize(mSpriteScale);
    }
    else
    {
        switchXPos = mYPos - FP_FromInteger(5);
        switchYPos = ScaleToGridSize(mSpriteScale) + mXPos;
    }

    auto pSwitch = static_cast<Lever*>(FindObjectOfType(ReliveTypes::eLever, switchYPos, switchXPos));
    if (pSwitch && !mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit10_Teleporting))
    {
        pSwitch->VPull(mXPos < pSwitch->mXPos);
        mCurrentMotion = eSligMotions::M_PullLever_45_4B8950;
        return;
    }

    Slig_GameSpeak_SFX_4C04F0(SligSpeak::eWhat_9, 0, field_11E_pitch_min, this);
    mCurrentMotion = eSligMotions::M_SpeakWhat_29_4B54D0;
}

s16 Slig::ToShootZ()
{
    if (Input().isPressed(sInputKey_ThrowItem) && mSpriteScale == FP_FromDouble(0.5) && !mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit10_Teleporting))
    {
        mCurrentMotion = eSligMotions::M_ShootZ_42_4B7560;
        mNextMotion = -1;
        field_12C_timer = sGnFrame + 60;
        return 1;
    }
    return 0;
}

void Slig::ShootOrShootZ()
{
    if (!Input().isPressed(sInputKey_Down) || mSpriteScale != FP_FromDouble(0.5) || mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit10_Teleporting))
    {
        mCurrentMotion = eSligMotions::M_Shoot_6_4B55A0;
    }
    else
    {
        mCurrentMotion = eSligMotions::M_ShootZ_42_4B7560;
    }

    mNextMotion = -1;
    field_12C_timer = sGnFrame + 60;
}

s16 Slig::GrabNearbyLift()
{
    auto pLiftPoint = static_cast<LiftPoint*>(sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId));
    if (pLiftPoint && pLiftPoint->Type() == ReliveTypes::eLiftPoint)
    {
        const FP halfGrid = ScaleToGridSize(mSpriteScale) / FP_FromInteger(2);
        const FP midSwitchX = FP_FromInteger((BaseAliveGameObjectCollisionLine->mRect.x + BaseAliveGameObjectCollisionLine->mRect.w) / 2);
        const FP distToSwitchMid = (mXPos - midSwitchX) >= FP_FromInteger(0) ? mXPos - midSwitchX : midSwitchX - mXPos;
        if (distToSwitchMid < halfGrid)
        {
            mCurrentMotion = eSligMotions::M_LiftGrip_46_4B3700;
            return 1;
        }
    }
    return 0;
}

s16 Slig::HandlePlayerControlled_4B7800()
{
    if (Input().isPressed(sInputKey_Right))
    {
        return LeftRigtMovement(MovementDirection::eRight);
    }

    if (Input().isPressed(sInputKey_Left))
    {
        return LeftRigtMovement(MovementDirection::eLeft);
    }

    if (Input().isHeld(sInputKey_DoAction))
    {
        PullLever();
        return 1;
    }

    if (Input().isPressed(sInputKey_ThrowItem))
    {
        ShootOrShootZ();
        return 1;
    }

    if (Input().isPressed(sInputKey_FartRoll))
    {
        mCurrentMotion = eSligMotions::M_Beat_51_4B6C00;
        field_12C_timer = sGnFrame + 60;
        return 1;
    }

    if (Input().isPressed(sInputKey_Down))
    {
        const auto ShouldGrabLift = GrabNearbyLift();
        if (ShouldGrabLift)
        {
            return ShouldGrabLift;
        }

        const auto ShouldShootZ = ToShootZ();
        if (ShouldShootZ)
        {
            return ShouldShootZ;
        }
    }
    else if (Input().isPressed(sInputKey_Up))
    {
        const auto GrabLift = GrabNearbyLift();
        if (GrabLift)
        {
            return GrabLift;
        }

        if (Input().isHeld(sInputKey_Up))
        {
            Slig_GameSpeak_SFX_4C04F0(SligSpeak::eWhat_9, 0, field_11E_pitch_min, this);
            mCurrentMotion = eSligMotions::M_SpeakWhat_29_4B54D0;
            return 1;
        }
    }
    else if (Input().isHeld(sInputKey_FartRoll | sInputKey_Hop))
    {
        Slig_GameSpeak_SFX_4C04F0(SligSpeak::eBlurgh_11, 0, field_11E_pitch_min, this);
        mCurrentMotion = eSligMotions::M_Blurgh_31_4B5510;
        return 1;
    }
    ToStand_4B4A20();
    return 0;
}

s16 Slig::GetNextMotionIncGameSpeak_4B5080(s32 input)
{
    if (sControlledCharacter == this && mHealth > FP_FromInteger(0))
    {
        if (Input_IsChanting_45F260())
        {
            if (field_292_prevent_depossession & 1)
            {
                return -1;
            }
            field_12C_timer = sGnFrame + 30;
            SfxPlayMono(relive::SoundEffects::PossessEffect, 0);
            return eSligMotions::M_Depossessing_36_4B7F30;
        }

        if (sInputKey_GameSpeak2 & input)
        {
            mNextMotion = eSligMotions::M_SpeakHereBoy_20_4B5330;
        }
        else if (input & sInputKey_GameSpeak1)
        {
            mNextMotion = eSligMotions::M_SpeakHi_21_4B53D0;
        }
        else if (input & sInputKey_GameSpeak3)
        {
            if (Input().Is_Demo_Playing_45F220() != 0)
            {
                mNextMotion = eSligMotions::M_SpeakGetHim_23_4B5410;
            }
            else
            {
                mNextMotion = eSligMotions::M_SpeakFreeze_22_4B53F0;
            }
        }
        else if (input & sInputKey_GameSpeak4)
        {
            if (Input().Is_Demo_Playing_45F220() != 0)
            {
                mNextMotion = eSligMotions::M_SpeakFreeze_22_4B53F0;
            }
            else
            {
                mNextMotion = eSligMotions::M_SpeakGetHim_23_4B5410;
            }
        }
        else if (input & sInputKey_GameSpeak8)
        {
            mNextMotion = eSligMotions::M_SpeakLaugh_24_4B5430;
        }
        else if (input & sInputKey_GameSpeak6)
        {
            mNextMotion = eSligMotions::M_SpeakBullshit1_25_4B5450;
        }
        else if (input & sInputKey_GameSpeak7)
        {
            mNextMotion = eSligMotions::M_SpeakLookOut_26_4B5470;
        }
        else if (input & sInputKey_GameSpeak5)
        {
            mNextMotion = eSligMotions::M_SpeakBullshit2_27_4B5490;
        }
        else
        {
            return -1;
        }

        GameSpeakEvents event = GameSpeakEvents::eNone_m1;
        switch (mNextMotion)
        {
            case eSligMotions::M_SpeakHereBoy_20_4B5330:
                event = GameSpeakEvents::Slig_HereBoy_28;
                break;
            case eSligMotions::M_SpeakHi_21_4B53D0:
                event = GameSpeakEvents::Slig_Hi_27;
                break;
            case eSligMotions::M_SpeakFreeze_22_4B53F0:
                event = GameSpeakEvents::Slig_Freeze_31;
                break;
            case eSligMotions::M_SpeakGetHim_23_4B5410:
                event = GameSpeakEvents::Slig_GetEm_29;
                break;
            case eSligMotions::M_SpeakLaugh_24_4B5430:
                event = GameSpeakEvents::Slig_Laugh_8;
                break;
            case eSligMotions::M_SpeakBullshit1_25_4B5450:
                event = GameSpeakEvents::Slig_BS_5;
                break;
            case eSligMotions::M_SpeakLookOut_26_4B5470:
                event = GameSpeakEvents::Slig_LookOut_6;
                break;
            case eSligMotions::M_SpeakBullshit2_27_4B5490:
                event = GameSpeakEvents::Slig_BS2_7;
                break;
        }

        pEventSystem_5BC11C->PushEvent(event);
    }

    if (mNextMotion >= eSligMotions::M_SpeakHereBoy_20_4B5330 && mNextMotion <= eSligMotions::M_Blurgh_31_4B5510)
    {
        SligSpeak speak = SligSpeak::eNone;
        switch (mNextMotion)
        {
            case eSligMotions::M_SpeakHereBoy_20_4B5330:
                speak = SligSpeak::eHereBoy_1;
                break;
            case eSligMotions::M_SpeakHi_21_4B53D0:
                speak = SligSpeak::eHi_0;
                break;
            case eSligMotions::M_SpeakFreeze_22_4B53F0:
                speak = SligSpeak::eFreeze_8;
                break;
            case eSligMotions::M_SpeakGetHim_23_4B5410:
                speak = SligSpeak::eGetHim_2;
                break;
            case eSligMotions::M_SpeakLaugh_24_4B5430:
                speak = SligSpeak::eLaugh_3;
                break;
            case eSligMotions::M_SpeakBullshit1_25_4B5450:
                speak = SligSpeak::eBullshit_5;
                break;
            case eSligMotions::M_SpeakLookOut_26_4B5470:
                speak = SligSpeak::eLookOut_6;
                break;
            case eSligMotions::M_SpeakBullshit2_27_4B5490:
                speak = SligSpeak::eBullshit2_7;
                break;
            case eSligMotions::M_SpeakPanic_28_4B54B0:
                speak = SligSpeak::eHelp_10;
                break;
            case eSligMotions::M_SpeakWhat_29_4B54D0:
                speak = SligSpeak::eWhat_9;
                break;
            case eSligMotions::M_SpeakAIFreeze_30_4B54F0:
                speak = SligSpeak::eFreeze_8;
                break;
            case eSligMotions::M_Blurgh_31_4B5510:
                speak = SligSpeak::eBlurgh_11;
                break;
        }

        Slig_GameSpeak_SFX_4C04F0(speak, 0, field_11E_pitch_min, this);
        mCurrentMotion = mNextMotion;
        mNextMotion = -1;
        return mCurrentMotion;
    }

    return -1;
}

void Slig::WaitOrWalk_4BE870()
{
    mNextMotion = eSligMotions::M_Walking_2_4B5BC0;
    SetBrain(&Slig::Brain_Walking_21_4BE0C0);

    // Right rect bound
    if (!IsFacingEffectiveLeft_4BB780(this) && (ScaleToGridSize(mSpriteScale) * FP_FromInteger(2)) + mXPos >= FP_FromInteger(field_138_zone_rect.w))
    {
        PauseALittle_4BDD00();
        MusicController::static_PlayMusic(MusicController::MusicTypes::eTension_4, this, 0, 0);
        return;
    }

    // Left rect bound
    if (IsFacingEffectiveLeft_4BB780(this) && (mXPos - (ScaleToGridSize(mSpriteScale) * FP_FromInteger(2))) <= FP_FromInteger(field_138_zone_rect.x))
    {
        PauseALittle_4BDD00();
        MusicController::static_PlayMusic(MusicController::MusicTypes::eTension_4, this, 0, 0);
        return;
    }

    Brain_Walking_21_4BE0C0();
    MusicController::static_PlayMusic(MusicController::MusicTypes::eTension_4, this, 0, 0);
}

void Slig::ToAbeDead_4B3580()
{
    mNextMotion = eSligMotions::M_SpeakLaugh_24_4B5430;
    SetBrain(&Slig::Brain_EnemyDead_10_4B3460);
    field_120_timer = sGnFrame + 45;
}

void Slig::ToUnderGlukkonCommand_4B9660()
{
    SetBrain(&Slig::Brain_ListeningToGlukkon_4_4B9D20);
    field_11C_brain_sub_state = Brain_ListeningToGlukkon_States::GettingAttention_0;
}

void Slig::ToKilledAbe_4B3600()
{
    mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    SetBrain(&Slig::Brain_KilledEnemy_10_4B35C0);
    field_120_timer = sGnFrame + 15;
}

bool Slig::IsWallBetween_4BB8B0(BaseAliveGameObject* pLeft, BaseAliveGameObject* pRight)
{
    const PSX_RECT thisBRect =  VGetBoundingRect();
    const PSX_RECT rightBRect = pRight->VGetBoundingRect();

    PathLine* pLine = nullptr;
    FP hitX = {};
    FP hitY = {};
    return sCollisions->Raycast(
               pLeft->mXPos,
               FP_FromInteger(thisBRect.h - 25),
               pRight->mXPos,
               FP_FromInteger(rightBRect.h - 25),
               &pLine, &hitX, &hitY, pLeft->mScale == Scale::Fg ? kFgWalls : kBgWalls)
        == 1;
}

GameSpeakEvents Slig::LastGlukkonSpeak_4B3090()
{
    if (!gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, mYPos, 1))
    {
        return GameSpeakEvents::eNone_m1;
    }

    const s32 last_event_idx = pEventSystem_5BC11C->field_28_last_event_index;
    if (field_160_last_event_index == last_event_idx)
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

    field_160_last_event_index = last_event_idx;

    return pEventSystem_5BC11C->field_20_last_event;
}

s16 Slig::ListenToGlukkonCommands_4B95D0()
{
    const GameSpeakEvents glukkonSpeak = LastGlukkonSpeak_4B3090();
    if (sControlledCharacter->mSpriteScale != mSpriteScale)
    {
        return 0;
    }

    if (glukkonSpeak == GameSpeakEvents::Glukkon_Hey_36)
    {
        if (!HeardGlukkonToListenTo_4B9690(GameSpeakEvents::Glukkon_Hey_36))
        {
            return 0;
        }
    }
    else if (glukkonSpeak != GameSpeakEvents::Glukkon_AllOYa_40)
    {
        return 0;
    }

    sSligsUnderControlCount_BAF7E8++;

    field_208_glukkon_obj_id = sControlledCharacter->mBaseGameObjectId;

    if (glukkonSpeak == GameSpeakEvents::Glukkon_AllOYa_40)
    {
        field_216_flags.Set(Flags_216::eBit3_GlukkonCalledAllOYa);
    }

    return 1;
}

void Slig::PlatformCollide_4B4E00()
{
    PSX_Point xy;
    xy.x = FP_GetExponent(mXPos - FP_FromInteger(5));
    xy.y = FP_GetExponent(mYPos - FP_FromInteger(5));

    PSX_Point wh;
    wh.x = FP_GetExponent(mXPos + FP_FromInteger(5));
    wh.y = FP_GetExponent(mYPos + FP_FromInteger(5));

    VOnCollisionWith(
        xy,
        wh,
        ObjList_5C1B78,
        (TCollisionCallBack) &BaseAliveGameObject::OnTrapDoorIntersection);
}

// TODO: rename to ToKnockBack() to match AO
void Slig::FallKnockBackOrSmash_4B4A90()
{
    Environment_SFX_457A40(EnvironmentSfx::eKnockback_13, 0, 0x7FFF, this);

    mXPos -= mVelX;

    if (BaseAliveGameObjectCollisionLine)
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            if (!WallHit((mSpriteScale * FP_FromInteger(45)), (ScaleToGridSize(mSpriteScale) * FP_FromInteger(2))))
            {
                mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
            }
            else
            {
                mVelX = FP_FromInteger(0);
            }
        }
        else
        {
            if (!WallHit((mSpriteScale * FP_FromInteger(45)), (-ScaleToGridSize(mSpriteScale) * FP_FromInteger(2))))
            {
                mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
            }
            else
            {
                mVelX = FP_FromInteger(0);
            }
        }
    }
    else
    {
        mVelX = FP_FromInteger(0);
    }

    if (mNextMotion == eSligMotions::M_Smash_44_4B6B90)
    {
        mVelX = -mVelX;
    }

    MapFollowMe(TRUE);

    if (mVelY < FP_FromInteger(0))
    {
        mVelY = FP_FromInteger(0);
    }

    mCurrentMotion = eSligMotions::M_Knockback_34_4B68A0;
    field_12C_timer = sGnFrame + 10;
}

void Slig::TurnOrSayWhat_4BEBC0()
{
    if (EventGet(kEventSpeaking) && !IsInInvisibleZone(sControlledCharacter) && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible))
    {
        GameSpeakResponse_4BF470();
        RespondWithWhat_4BF400();
    }
    else
    {
        mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
        SetBrain(&Slig::Brain_GetAlertedTurn_22_4BE990);
    }
}

void Slig::GameSpeakResponse_4BF470()
{
    const s32 lastIdx = pEventSystem_5BC11C->field_28_last_event_index;

    GameSpeakEvents speak = GameSpeakEvents::eNone_m1;

    if (field_160_last_event_index == lastIdx)
    {
        if (pEventSystem_5BC11C->field_20_last_event == GameSpeakEvents::eNone_m1)
        {
            speak = GameSpeakEvents::eNone_m1;
        }
        else
        {
            speak = GameSpeakEvents::eSameAsLast_m2;
        }
    }
    else
    {
        field_160_last_event_index = lastIdx;
        speak = pEventSystem_5BC11C->field_20_last_event;
    }

    switch (speak)
    {
        case GameSpeakEvents::eUnknown_1:
        case GameSpeakEvents::eUnknown_2:
            if (!(Math_NextRandom() & 4))
            {
                field_294_next_gamespeak_motion = eSligMotions::M_SpeakBullshit1_25_4B5450;
            }
            else
            {
                field_294_next_gamespeak_motion = eSligMotions::M_SpeakBullshit2_27_4B5490;
            }
            break;

        case GameSpeakEvents::eFart_3:
        case GameSpeakEvents::eUnknown_4:
        case GameSpeakEvents::Slig_Laugh_8:
        case GameSpeakEvents::eUnknown_14:
        case GameSpeakEvents::eUnknown_15:
        case GameSpeakEvents::Slig_GetEm_29:
        case GameSpeakEvents::eUnknown_34:
            field_294_next_gamespeak_motion = eSligMotions::M_Blurgh_31_4B5510;
            return;

        case GameSpeakEvents::Slig_BS_5:
            field_294_next_gamespeak_motion = eSligMotions::M_SpeakBullshit1_25_4B5450;
            break;

        case GameSpeakEvents::Slig_LookOut_6:
            if (!(Math_NextRandom() & 4))
            {
                field_294_next_gamespeak_motion = eSligMotions::M_SpeakWhat_29_4B54D0;
            }
            else
            {
                field_294_next_gamespeak_motion = eSligMotions::M_SpeakPanic_28_4B54B0;
            }
            return;

        case GameSpeakEvents::Slig_BS2_7:
            field_294_next_gamespeak_motion = eSligMotions::M_SpeakBullshit2_27_4B5490;
            break;

        case GameSpeakEvents::eHello_9:
        case GameSpeakEvents::Slig_Hi_27:
            field_294_next_gamespeak_motion = eSligMotions::M_SpeakHi_21_4B53D0;
            break;

        case GameSpeakEvents::eFollowMe_10:
        case GameSpeakEvents::eWait_12:
            if (Math_NextRandom() & 8)
            {
                field_294_next_gamespeak_motion = eSligMotions::M_SpeakBullshit1_25_4B5450;
            }
            else
            {
                field_294_next_gamespeak_motion = eSligMotions::M_SpeakBullshit2_27_4B5490;
            }
            break;

        case GameSpeakEvents::eAnger_11:
        case GameSpeakEvents::eUnknown_13:
        case GameSpeakEvents::eUnknown_16:
        case GameSpeakEvents::eUnknown_17:
        case GameSpeakEvents::eUnknown_18:
        case GameSpeakEvents::eUnknown_19:
        case GameSpeakEvents::eUnknown_20:
        case GameSpeakEvents::eUnknown_25:
        case GameSpeakEvents::eUnknown_26:
        case GameSpeakEvents::eUnknown_30:
        case GameSpeakEvents::Slig_Freeze_31:
        case GameSpeakEvents::eDynamicCollision_32:
        case GameSpeakEvents::eUnknown_35:
            field_294_next_gamespeak_motion = eSligMotions::M_SpeakLaugh_24_4B5430;
            break;

        default:
            field_294_next_gamespeak_motion = eSligMotions::M_SpeakWhat_29_4B54D0;
            break;
    }
}

void Slig::GoAlertedOrSayWhat_4BF140()
{
    if (EventGet(kEventSpeaking)
        && !IsInInvisibleZone(sControlledCharacter)
        && !sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible))
    {
        GameSpeakResponse_4BF470();
        RespondWithWhat_4BF400();
    }
    else
    {
        mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
        field_294_next_gamespeak_motion = eSligMotions::M_SpeakWhat_29_4B54D0;
        SetBrain(&Slig::Brain_GetAlerted_23_4BEC40);
        field_120_timer = sGnFrame + field_218_tlv_data.mData.mAlertedListenTime;
    }
}

s32 Slig::IsFacingEffectiveLeft_4BB780(Slig* pSlig)
{
    s32 flipX = pSlig->mAnim.mFlags.Get(AnimFlags::eBit5_FlipX);
    if (pSlig->mCurrentMotion == eSligMotions::M_TurnAroundStanding_5_4B6390 && pSlig->mAnim.mCurrentFrame > 4)
    {
        // Flip the result
        return flipX == 0;
    }
    else
    {
        return flipX;
    }
}

void Slig::MoveOnLine_4B4C40()
{
    auto pPlatform = static_cast<PlatformBase*>(sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId));

    const FP xpos = mXPos;
    if (BaseAliveGameObjectCollisionLine)
    {
        BaseAliveGameObjectCollisionLine = BaseAliveGameObjectCollisionLine->MoveOnLine(
            &mXPos,
            &mYPos,
            mVelX);
        if (BaseAliveGameObjectCollisionLine)
        {
            if (pPlatform)
            {
                if (BaseAliveGameObjectCollisionLine->mLineType != eLineTypes::eDynamicCollision_32 && BaseAliveGameObjectCollisionLine->mLineType != eLineTypes::eBackgroundDynamicCollision_36)
                {
                    const auto oldMotion = mCurrentMotion;
                    VOnTrapDoorOpen();
                    mCurrentMotion = oldMotion;
                }
            }
            else if (BaseAliveGameObjectCollisionLine->mLineType == eLineTypes::eDynamicCollision_32 || BaseAliveGameObjectCollisionLine->mLineType == eLineTypes::eBackgroundDynamicCollision_36)
            {
                PlatformCollide_4B4E00();
            }
        }
        else
        {
            VOnTrapDoorOpen();
            mXPos = xpos + mVelX;
            BaseAliveGameObjectLastLineYPos = mYPos;
            mCurrentMotion = eSligMotions::M_FallingInitiate_39_4B4640;
            field_130_falling_velx_scale_factor = FP_FromDouble(0.3);

            if (mVelX > (FP_FromInteger(6) * mSpriteScale))
            {
                mVelX = (FP_FromInteger(6) * mSpriteScale);
            }

            if (mVelX < (-FP_FromInteger(6) * mSpriteScale))
            {
                mVelX = (-FP_FromInteger(6) * mSpriteScale);
            }
        }
    }
    else
    {
        VOnTrapDoorOpen();
        mCurrentMotion = eSligMotions::M_OutToFall_38_4B4570;
        BaseAliveGameObjectLastLineYPos = mYPos;
    }
}

void Slig::PlayerControlStopWalkingIfRequired_4B8540()
{
    // Pressing other direction to facing or not pressing a direction at all?
    if ((mVelX > FP_FromInteger(0) && Input().isPressed(sInputKey_Left)) || (mVelX < FP_FromInteger(0) && Input().isPressed(sInputKey_Right)) || !Input().isPressed(sInputKey_Left | sInputKey_Right))
    {
        // Then go to standing
        mCurrentMotion = eSligMotions::M_WalkToStand_18_4B5FC0;
    }
    mInput = 0;
}

void Slig::CheckPlatformVanished_4B3640()
{
    BaseGameObject* pLiftPoint = sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId);
    if (pLiftPoint)
    {
        if (pLiftPoint->mBaseGameObjectFlags.Get(BaseGameObject::eDead))
        {
            // Platform is somehow gone, fall.
            const auto savedMotion = mCurrentMotion;
            VOnTrapDoorOpen();
            mCurrentMotion = savedMotion;
        }
        SetActiveCameraDelayedFromDir();
    }
}

void Slig::VOnTrapDoorOpen()
{
    auto pPlatform = static_cast<PlatformBase*>(sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId));
    if (pPlatform)
    {
        pPlatform->VRemove(this);
        BaseAliveGameObject_PlatformId = Guid{};
        BaseAliveGameObjectLastLineYPos = mYPos;
        VSetMotion(eSligMotions::M_OutToFall_38_4B4570);
    }
}

s16 Slig::MoveLift_4B3990(FP ySpeed)
{
    auto pLiftPoint = static_cast<LiftPoint*>(sObjectIds.Find_Impl(BaseAliveGameObject_PlatformId));
    if (!pLiftPoint)
    {
        return eSligMotions::M_LiftGripping_48_4B3850;
    }

    pLiftPoint->vMove_4626A0(FP_FromInteger(0), ySpeed, 0);
    CheckPlatformVanished_4B3640();
    mVelY = pLiftPoint->mVelY;

    if (!mAnim.mFlags.Get(AnimFlags::eBit18_IsLastFrame) && mAnim.mCurrentFrame != 5)
    {
        return mCurrentMotion;
    }

    if (ySpeed < FP_FromInteger(0))
    {
        if (pLiftPoint->vOnTopFloor())
        {
            return eSligMotions::M_LiftGripping_48_4B3850;
        }

        if (sControlledCharacter != this)
        {
            return mCurrentMotion;
        }

        if (Input().isPressed(sInputKey_Up))
        {
            return eSligMotions::M_LiftUp_49_4B3930;
        }

        if (Input().isPressed(sInputKey_Down))
        {
            return eSligMotions::M_LiftDown_50_4B3960;
        }
    }
    else if (ySpeed > FP_FromInteger(0))
    {
        if (pLiftPoint->vOnBottomFloor())
        {
            return eSligMotions::M_LiftGripping_48_4B3850;
        }

        if (sControlledCharacter != this)
        {
            return mCurrentMotion;
        }

        if (Input().isPressed(sInputKey_Down))
        {
            return eSligMotions::M_LiftDown_50_4B3960;
        }

        if (Input().isPressed(sInputKey_Up))
        {
            return eSligMotions::M_LiftUp_49_4B3930;
        }
    }

    // Strange how this isn't "if nothing pressed and on a floor then let go ??"
    if (Input().mPads[sCurrentControllerIndex].mPressed && pLiftPoint->vOnAnyFloor())
    {
        return eSligMotions::M_LiftUngrip_47_4B3820;
    }

    pLiftPoint->vMove_4626A0(FP_FromInteger(0), FP_FromInteger(0), 0);
    return eSligMotions::M_LiftGripping_48_4B3850;
}

void Slig::SlowDown_4B6450(FP speed)
{
    if (FP_GetExponent(mVelX))
    {
        MoveOnLine_4B4C40();

        if (mVelX <= FP_FromInteger(0))
        {
            if (mVelX < FP_FromInteger(0))
            {
                mVelX = (mSpriteScale * speed) + mVelX;
                if (mVelX > FP_FromInteger(0))
                {
                    mVelX = FP_FromInteger(0);
                }
            }
        }
        else
        {
            mVelX = mVelX - (mSpriteScale * speed);
            if (mVelX < FP_FromInteger(0))
            {
                mVelX = FP_FromInteger(0);
            }
        }
    }
}

void Slig::ToChase_4BCFF0()
{
    field_120_timer = sGnFrame + field_218_tlv_data.mData.mTimeToWaitBeforeChase;

    if (!VIsFacingMe(sControlledCharacter))
    {
        mAnim.mFlags.Toggle(AnimFlags::eBit5_FlipX);
    }

    mNextMotion = eSligMotions::M_StandIdle_0_4B4EC0;
    SetBrain(&Slig::Brain_StartChasing_18_4BCEB0);
    MusicController::static_PlayMusic(MusicController::MusicTypes::eSoftChase_8, this, 0, 0);
}

s16 Slig::HandleEnemyStopper_4BBA00(s32 gridBlocks)
{
    s32 directedGirdBlocks = gridBlocks;
    const auto bFacingLeft = IsFacingEffectiveLeft_4BB780(this);
    if (bFacingLeft)
    {
        directedGirdBlocks = -gridBlocks;
    }

    const FP width = ScaleToGridSize(mSpriteScale) * FP_FromInteger(directedGirdBlocks) + mXPos;
    auto pTlv = static_cast<relive::Path_EnemyStopper*>(sPathInfo->TLV_Get_At_4DB4B0(
        FP_GetExponent(mXPos),
        FP_GetExponent(mYPos),
        FP_GetExponent(width),
        FP_GetExponent(mYPos - ScaleToGridSize(mSpriteScale)),
        ReliveTypes::eEnemyStopper));

    if (!pTlv)
    {
        return 0;
    }

    if (!SwitchStates_Get(pTlv->mSwitchId))
    {
        return 0;
    }

    if (pTlv->mStopDirection == relive::Path_EnemyStopper::StopDirection::Both)
    {
        return 1;
    }

    if (bFacingLeft && pTlv->mStopDirection == relive::Path_EnemyStopper::StopDirection::Left)
    {
        return 1;
    }

    if (!bFacingLeft && pTlv->mStopDirection == relive::Path_EnemyStopper::StopDirection::Right)
    {
        return 1;
    }

    return 0;
}

void Slig::ToPanicYelling_4BCBA0()
{
    mNextMotion = eSligMotions::M_SpeakPanic_28_4B54B0;
    SetBrain(&Slig::Brain_PanicYelling_14_4BCA70);
}

void Slig::ToPanicTurn_4BC750()
{
    mNextMotion = eSligMotions::M_TurnAroundStanding_5_4B6390;
    SetBrain(&Slig::Brain_PanicTurning_12_4BC490);
}

ALIVE_VAR(1, 0xBAF7EC, s16, sDelayIdx_BAF7EC, 0);
const s16 sDelayTable_56075C[6] = {0, 6, 10, 8, 4, 12};


s16 Slig::SligStableDelay_4B99B0()
{
    // Similar to the mudokon delay stuff

    if (sSligsUnderControlCount_BAF7E8 <= 1)
    {
        return 0;
    }

    if (sDelayIdx_BAF7EC >= ALIVE_COUNTOF(sDelayTable_56075C))
    {
        sDelayIdx_BAF7EC = 0;
    }

    return sDelayTable_56075C[sDelayIdx_BAF7EC++];
}

void Slig::PlayerControlRunningSlideStopOrTurnFrame12_4B8790()
{
    const FP curVelX = mVelX;

    if (curVelX > FP_FromInteger(0) && Input().isPressed(sInputKey_Left))
    {
        mVelX = mSpriteScale * FP_FromDouble(13.2);
        mCurrentMotion = eSligMotions::M_SlidingTurn_9_4B6680;
        mInput = 0;
        return;
    }

    if (curVelX < FP_FromInteger(0) && Input().isPressed(sInputKey_Right))
    {
        mVelX = (mSpriteScale * -FP_FromDouble(13.2));
        mCurrentMotion = eSligMotions::M_SlidingTurn_9_4B6680;
        mInput = 0;
        return;
    }

    if (!Input().isPressed(sInputKey_Left | sInputKey_Right))
    {
        if (curVelX >= FP_FromInteger(0))
        {
            mVelX = mSpriteScale * FP_FromDouble(13.2);
        }
        else
        {
            mVelX = (mSpriteScale * -FP_FromDouble(13.2));
        }
        mCurrentMotion = eSligMotions::M_SlidingToStand_8_4B6520;
        mInput = 0;
        return;
    }

    if (Input().isPressed(sInputKey_Run))
    {
        mInput = 0;
        return;
    }

    mPreviousMotion = eSligMotions::M_Walking_2_4B5BC0;
    mBaseAliveGameObjectLastAnimFrame = 15;
    field_124_return_to_previous_motion = 1;

    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mInput = 0;
        mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
    }
    else
    {
        mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
        mInput = 0;
    }
}

void Slig::PlayerControlRunningSlideStopOrTurnFrame4_4B85D0()
{
    const FP curVelX = mVelX;
    if (curVelX > FP_FromInteger(0) && Input().isPressed(sInputKey_Left))
    {
        mVelX = (mSpriteScale * FP_FromDouble(13.2));
        mCurrentMotion = eSligMotions::M_SlidingTurn_9_4B6680;
        mInput = 0;
        return;
    }

    if (curVelX < FP_FromInteger(0) && Input().isPressed(sInputKey_Right))
    {
        mVelX = (mSpriteScale * -FP_FromDouble(13.2));
        mCurrentMotion = eSligMotions::M_SlidingTurn_9_4B6680;
        mInput = 0;
        return;
    }

    if (!Input().isPressed(sInputKey_Left | sInputKey_Right))
    {
        if (curVelX >= FP_FromInteger(0))
        {
            mVelX = mSpriteScale * FP_FromDouble(13.2);
        }
        else
        {
            mVelX = (mSpriteScale * -FP_FromDouble(13.2));
        }
        mCurrentMotion = eSligMotions::M_SlidingToStand_8_4B6520;
        mInput = 0;
        return;
    }

    if (Input().isPressed(sInputKey_Run))
    {
        mInput = 0;
        return;
    }

    mPreviousMotion = eSligMotions::M_Walking_2_4B5BC0;
    mBaseAliveGameObjectLastAnimFrame = 6;
    field_124_return_to_previous_motion = 1;

    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        mInput = 0;
        mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
    }
    else
    {
        mInput = 0;
        mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(9));
    }
}

BaseAliveGameObject* Slig::FindBeatTarget_4BD070(ReliveTypes /*typeToFind*/, s32 gridBlocks)
{
    const FP kGridSize = ScaleToGridSize(mSpriteScale);
    const FP k2Scaled = FP_FromInteger(2) * kGridSize;
    const FP kGridBlocksScaled = FP_FromInteger(gridBlocks) * kGridSize;

    const FP xAndW = mAnim.mFlags.Get(AnimFlags::eBit5_FlipX) ? mXPos - kGridBlocksScaled : mXPos + kGridBlocksScaled;

    PSX_RECT hitRect = MakeMinMaxRect(
        xAndW,
        mYPos,
        xAndW,
        mYPos - k2Scaled,
        true);

    for (s32 i = 0; i < gBaseAliveGameObjects->Size(); i++)
    {
        BaseAliveGameObject* pObj = gBaseAliveGameObjects->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj != this && pObj->Type() == ReliveTypes::eMudokon)
        {
            const PSX_RECT bRect = pObj->VGetBoundingRect();

            if (hitRect.w <= bRect.w && hitRect.x >= bRect.x && hitRect.y >= bRect.y && hitRect.h <= bRect.h && pObj->mScale == mScale && !IsInInvisibleZone(pObj) && !pObj->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) && pObj->mHealth > FP_FromInteger(0))
            {
                return pObj;
            }
        }
    }
    return nullptr;
}

void Slig::TurnOrWalk_4BD6A0(s32 a2)
{
    if (a2 == 1)
    {
        if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            if (mXPos > FP_FromInteger(field_138_zone_rect.x) + (ScaleToGridSize(mSpriteScale) * FP_FromInteger(4)))
            {
                ToTurn_4BE090();
                return;
            }
        }
        else
        {
            if (mXPos < FP_FromInteger(field_138_zone_rect.w) - (ScaleToGridSize(mSpriteScale) * FP_FromInteger(4)))
            {
                ToTurn_4BE090();
                return;
            }
        }
    }

    if (!(mAnim.mFlags.Get(AnimFlags::eBit5_FlipX)))
    {
        if (mXPos > FP_FromInteger(field_138_zone_rect.w) - (ScaleToGridSize(mSpriteScale) * FP_FromInteger(4)))
        {
            ToTurn_4BE090();
            return;
        }
    }
    else
    {
        if (mXPos < FP_FromInteger(field_138_zone_rect.x) + (ScaleToGridSize(mSpriteScale) * FP_FromInteger(4)))
        {
            ToTurn_4BE090();
            return;
        }
    }

    WaitOrWalk_4BE870();
}

void Slig::ToPanicRunning_4BCA30()
{
    mNextMotion = eSligMotions::M_Running_4_4B6000;
    SetBrain(&Slig::Brain_PanicRunning_13_4BC780);
    Brain_PanicRunning_13_4BC780();
    MusicController::static_PlayMusic(MusicController::MusicTypes::eSoftChase_8, this, 0, 0);
}

void Slig::RespondToEnemyOrPatrol_4B3140()
{
    field_144_unused = 0;

    if (field_218_tlv_data.mData.mShootOnSightDelay || sControlledCharacter->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible))
    {
        if (sControlledCharacter->Type() != ReliveTypes::eSlig ||
            field_218_tlv_data.mData.mShootPossessedSligs != relive::reliveChoice::eNo)
        {
            SetBrain(&Slig::Brain_SpottedEnemy_7_4B3240);
            mNextMotion = eSligMotions::M_SpeakAIFreeze_30_4B54F0;
            field_120_timer = sGnFrame + field_218_tlv_data.mData.mShootOnSightDelay;
        }
        else
        {
            field_15E_spotted_possessed_slig = 1;
            TurnOrWalk_4BD6A0(0);
        }
    }
    else if (VIsFacingMe(sControlledCharacter))
    {
        if (mSpriteScale == sControlledCharacter->mSpriteScale)
        {
            ToShoot_4BF9A0();
        }
        else
        {
            ToZShoot_4BF9E0();
        }
    }
    else
    {
        ToTurn_4BE090();
    }
}

s16 Slig::IsAbeEnteringDoor_4BB990(BaseAliveGameObject* pThis)
{
    if (((pThis->Type() == ReliveTypes::eAbe) && (pThis->mCurrentMotion == eAbeMotions::Motion_114_DoorEnter_459470 && pThis->mAnim.mCurrentFrame > 7)) || (pThis->mCurrentMotion == eAbeMotions::Motion_115_DoorExit_459A40 && pThis->mAnim.mCurrentFrame < 4))
    {
        return 1;
    }
    return 0;
}

s16 Slig::FindSwitch_4B9A50()
{
    const s16 yPos = FP_GetExponent(mYPos - FP_FromInteger(5));
    if (sPathInfo->TLV_Get_At_4DB4B0(FP_GetExponent(mXPos), yPos, FP_GetExponent(mXPos), yPos, ReliveTypes::eLever))
    {
        return 0;
    }

    FP xOff = ScaleToGridSize(mSpriteScale);
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        xOff = -xOff;
    }

    return sPathInfo->TLV_Get_At_4DB4B0(
               FP_GetExponent(FP_Abs(mXPos) + xOff),
               yPos,
               FP_GetExponent(FP_Abs(mXPos) + xOff),
               yPos,
               ReliveTypes::eLever)
        != 0;
}

s16 Slig::NearOrFacingActiveChar_4B9930(BaseAliveGameObject* pObj)
{
    if (pObj->VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromDouble(0.5), sControlledCharacter) || sControlledCharacter->VIsFacingMe(pObj))
    {
        return 1;
    }

    return 0;
}

static s16 IsInZCover(relive::Path_TLV* pTlv, const PSX_RECT* pRect)
{
    if (pTlv->mTlvType == ReliveTypes::eZSligCover)
    {
        if (pRect->x >= pTlv->mTopLeftX && pRect->x <= pTlv->mBottomRightX &&

            pRect->y >= pTlv->mTopLeftY && pRect->y <= pTlv->mBottomRightY &&

            pRect->w >= pTlv->mTopLeftX && pRect->w <= pTlv->mBottomRightX &&

            pRect->h >= pTlv->mTopLeftY && pRect->h <= pTlv->mBottomRightY)
        {
            return TRUE;
        }
    }
    return FALSE;
}

s16 Slig::InZCover_4BB7C0(BaseAliveGameObject* pObj)
{
    const PSX_RECT bRect = pObj->VGetBoundingRect();
    return Bullet::InZBulletCover(pObj->mXPos, FP_FromInteger(bRect.y), bRect);
}

bool Slig::RenderLayerIs_4BBBC0(BaseAliveGameObject* pThis)
{
    return pThis->mAnim.mRenderLayer == Layer::eLayer_BeforeWell_Half_3 || pThis->mAnim.mRenderLayer == Layer::eLayer_BeforeWell_22;
}

void Slig::NextCommand_4B9A00(s16 speakTableIndex, s16 responseState)
{
    field_212_next_command_arg1 = speakTableIndex;
    field_214_cmd_idx = 0;
    field_120_timer = sGnFrame + SligStableDelay_4B99B0();
    field_20C_state_after_speak = responseState;
}

s16 Slig::HeardGlukkonToListenTo_4B9690(GameSpeakEvents glukkonSpeak)
{
    const s32 distToPlayer = Math_Distance(
        FP_GetExponent(sControlledCharacter->mXPos),
        FP_GetExponent(sControlledCharacter->mYPos),
        FP_GetExponent(mXPos),
        FP_GetExponent(mYPos));

    if (!NearOrFacingActiveChar_4B9930(this))
    {
        return 0;
    }

    for (s32 i = 0; i < gBaseAliveGameObjects->Size(); i++)
    {
        BaseAliveGameObject* pObj = gBaseAliveGameObjects->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj != this && pObj->Type() == ReliveTypes::eSlig)
        {
            auto* pOtherSlig = static_cast<Slig*>(pObj);
            if (pOtherSlig->mSpriteScale == sControlledCharacter->mSpriteScale && gMap.Is_Point_In_Current_Camera(pOtherSlig->mCurrentLevel, pOtherSlig->mCurrentPath, pOtherSlig->mXPos, pOtherSlig->mYPos, 0) && NearOrFacingActiveChar_4B9930(pOtherSlig) && (glukkonSpeak == GameSpeakEvents::Glukkon_Hey_36 || pOtherSlig->BrainIs(&Slig::Brain_ListeningToGlukkon_4_4B9D20)))
            {
                if (VIsObjNearby(ScaleToGridSize(mSpriteScale) * FP_FromDouble(0.5), pOtherSlig))
                {
                    if ((pOtherSlig->VIsFacingMe(sControlledCharacter) && !VIsFacingMe(sControlledCharacter)) || pOtherSlig->field_216_flags.Get(Flags_216::eBit4_HeardGlukkon))
                    {
                        return 0;
                    }
                }
                else if (distToPlayer > Math_Distance(
                             FP_GetExponent(sControlledCharacter->mXPos),
                             FP_GetExponent(sControlledCharacter->mYPos),
                             FP_GetExponent(pOtherSlig->mXPos),
                             FP_GetExponent(pOtherSlig->mYPos)))
                {
                    return 0;
                }
            }
        }
    }

    field_216_flags.Set(Flags_216::eBit4_HeardGlukkon);
    return 1;
}

s16 Slig::VTakeDamage(BaseGameObject* pFrom)
{
    if (mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit10_Teleporting))
    {
        return 0;
    }

    switch (pFrom->Type())
    {
        case ReliveTypes::eBullet:
        {
            if (mHealth <= FP_FromInteger(0) && sControlledCharacter != this)
            {
                return 0;
            }

            auto pBullet = static_cast<Bullet*>(pFrom);
            switch (pBullet->mBulletType)
            {
                case BulletType::eSligPossessedOrUnderGlukkonCommand_0:
                case BulletType::eNormalBullet_2:
                {
                    {
                        const FP yOff = FP_FromInteger(Math_NextRandom() % 16) - FP_FromInteger(8);
                        const FP xOff = ((pBullet->mXDistance <= FP_FromInteger(0) ? FP_FromInteger(-1) : FP_FromInteger(1)) * FP_FromInteger(Math_NextRandom() & 15)) + FP_FromInteger(16);
                        const FP yPos = mYPos - (FP_FromInteger(25) * mSpriteScale);
                        const FP xPos = mSpriteScale * (pBullet->mXDistance <= FP_FromInteger(0) ? FP_FromInteger(-6) : FP_FromInteger(6));
                        relive_new Blood(xPos + mXPos, yPos, xOff, yOff, mSpriteScale, 12);
                    }

                    {
                        const FP xOff = pBullet->mXDistance <= FP_FromInteger(0) ? FP_FromInteger(-6) : FP_FromInteger(6);
                        const FP yPos = mYPos - (FP_FromInteger(25) * mSpriteScale);
                        const FP xPos = mSpriteScale * (pBullet->mXDistance <= FP_FromInteger(0) ? FP_FromInteger(-12) : FP_FromInteger(12));
                        relive_new Blood(xPos + mXPos, yPos, xOff, FP_FromInteger(0), mSpriteScale, 8);
                    }
                    break;
                }

                case BulletType::ePossessedSligZBullet_1:
                case BulletType::eZBullet_3:
                {
                    const PSX_RECT myRect = VGetBoundingRect();
                    const FP rectY = FP_FromInteger(myRect.y);

                    relive::Path_TLV* pTlvIter = nullptr;
                    for (;;)
                    {
                        pTlvIter = sPathInfo->TlvGetAt(pTlvIter, mXPos, rectY, mXPos, rectY);
                        if (!pTlvIter)
                        {
                            break;
                        }

                        if (IsInZCover(pTlvIter, &myRect))
                        {
                            mBaseAliveGameObjectFlags.Clear(Flags_114::e114_Bit1_bShot);
                            mHealth = FP_FromInteger(1);
                            return 0;
                        }
                    }

                    if (!gMap.Is_Point_In_Current_Camera(mCurrentLevel, mCurrentPath, mXPos, rectY, 0))
                    {
                        mBaseAliveGameObjectFlags.Clear(Flags_114::e114_Bit1_bShot);
                        mHealth = FP_FromInteger(1);
                        return 0;
                    }

                    relive_new Blood(mXPos, mYPos - (FP_FromInteger(25) * mSpriteScale), FP_FromInteger(0), FP_FromInteger(0), mSpriteScale, 25);
                }

                default:
                    break;
            }

            mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit1_bShot);
            SetBrain(&Slig::Brain_Death_0_4BBFB0);

            field_14C_death_by_being_shot_timer = sGnFrame + 5;
            if (mCurrentMotion != eSligMotions::M_Possess_37_4B72C0)
            {
                if (field_150_explode_timer > static_cast<s32>(sGnFrame) + 20)
                {
                    field_150_explode_timer = sGnFrame + 20;
                }

                if (mCurrentMotion != eSligMotions::M_Possess_37_4B72C0 && mCurrentMotion != eSligMotions::M_Smash_44_4B6B90 && mCurrentMotion != eSligMotions::M_Knockback_34_4B68A0)
                {
                    mNextMotion = eSligMotions::M_Possess_37_4B72C0;
                    field_136_shot_motion = eSligMotions::M_Possess_37_4B72C0;
                    field_150_explode_timer = sGnFrame + 20;
                    vShot_4B2EA0();
                    mBaseAliveGameObjectFlags.Set(Flags_114::e114_MotionChanged_Bit2);
                    if (static_cast<Bullet*>(pFrom)->mXDistance < FP_FromInteger(0))
                    {
                        mVelX = FP_FromDouble(-0.001);
                        mHealth = FP_FromInteger(0);
                        EventBroadcast(kEventMudokonComfort, this);
                        return 1;
                    }
                    mVelX = FP_FromDouble(0.001);
                }
            }

            mHealth = FP_FromInteger(0);
            EventBroadcast(kEventMudokonComfort, this);
            return 1;
        }

        case ReliveTypes::eDrill:
        case ReliveTypes::eBaseBomb:
        case ReliveTypes::eExplosion:
        {
            if (!mAnim.mFlags.Get(AnimFlags::eBit3_Render))
            {
                return 1;
            }

            relive_new Gibs(GibType::Slig_1, mXPos, mYPos, mVelX, mVelY, mSpriteScale, 0);
            mHealth = FP_FromInteger(0);
            SfxPlayMono(relive::SoundEffects::FallingItemHit, 90);
            mAnim.mFlags.Clear(AnimFlags::eBit2_Animate);
            mAnim.mFlags.Clear(AnimFlags::eBit3_Render);
            SetUpdateDelay(40);
            SetBrain(&Slig::Brain_ReturnControlToAbeAndDie_1_4BC410);
            mCurrentMotion = eSligMotions::M_StandIdle_0_4B4EC0;
            vUpdateAnim_4B1320();
            EventBroadcast(kEventMudokonComfort, this);
            return 1;
        }

        case ReliveTypes::eElectricWall:
            Slig_GameSpeak_SFX_4C04F0(SligSpeak::eHelp_10, 0, field_11E_pitch_min, this);
            return 1;

        case ReliveTypes::eAbe:
        {
            if (mHealth <= FP_FromInteger(0))
            {
                return 1;
            }

            if (mCurrentMotion == eSligMotions::M_Knockback_34_4B68A0 || mCurrentMotion == eSligMotions::M_Smash_44_4B6B90 || mCurrentMotion == eSligMotions::M_KnockbackToStand_35_4B6A30)
            {
                return 1;
            }

            if (!VIsFacingMe(sActiveHero) || IsInInvisibleZone(sActiveHero) || sActiveHero->mBaseAliveGameObjectFlags.Get(Flags_114::e114_Bit8_bInvisible) || IsAbeEnteringDoor_4BB990(sControlledCharacter))
            {
                GoAlertedOrSayWhat_4BF140();
            }
            else
            {
                RespondToEnemyOrPatrol_4B3140();
            }

            mBaseAliveGameObjectFlags.Set(Flags_114::e114_MotionChanged_Bit2);

            if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
            {
                mXPos = ScaleToGridSize(mSpriteScale) + mXPos;
            }
            else
            {
                mXPos = mXPos - ScaleToGridSize(mSpriteScale);
            }
            FallKnockBackOrSmash_4B4A90();
            mVelX = FP_FromInteger(0);
            return 1;
        }

        case ReliveTypes::eAbilityRing:
            return 1;

        case ReliveTypes::eSlog:
        {
            if (mHealth <= FP_FromInteger(0) && (mCurrentMotion == eSligMotions::M_Knockback_34_4B68A0 || mCurrentMotion == eSligMotions::M_Smash_44_4B6B90))
            {
                return 1;
            }

            mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit1_bShot);
            mHealth = FP_FromInteger(0);

            SetBrain(&Slig::Brain_Death_0_4BBFB0);

            Environment_SFX_457A40(EnvironmentSfx::eKnockback_13, 0, 0x7FFF, this);

            EventBroadcast(kEventMudokonComfort, this);

            if (VIsFacingMe(static_cast<BaseAliveGameObject*>(pFrom)))
            {
                if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
                {
                    mVelX = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
                }
                else
                {
                    mVelX = -(ScaleToGridSize(mSpriteScale) / FP_FromInteger(4));
                }

                mBaseAliveGameObjectFlags.Set(Flags_114::e114_MotionChanged_Bit2);
                field_12C_timer = sGnFrame + 10;
                mCurrentMotion = eSligMotions::M_Knockback_34_4B68A0;
                mNextMotion = eSligMotions::M_Knockback_34_4B68A0;
                field_136_shot_motion = eSligMotions::M_Knockback_34_4B68A0;
            }
            else
            {
                mNextMotion = eSligMotions::M_Smash_44_4B6B90;
                field_136_shot_motion = eSligMotions::M_Smash_44_4B6B90;
            }
            return 1;
        }

        case ReliveTypes::eElectrocute:
            if (mHealth <= FP_FromInteger(0))
            {
                return 1;
            }
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
            mHealth = FP_FromInteger(0);
            EventBroadcast(kEventMudokonComfort, this);
            return 1;

        default:
            if (mHealth <= FP_FromInteger(0))
            {
                return 1;
            }
            SfxPlayMono(relive::SoundEffects::KillEffect, 127);
            SfxPlayMono(relive::SoundEffects::FallingItemHit, 90);
            break;
    }

    if (mHealth <= FP_FromInteger(0))
    {
        return 1;
    }

    mBaseAliveGameObjectFlags.Set(Flags_114::e114_Bit1_bShot);
    mHealth = FP_FromInteger(0);
    mNextMotion = eSligMotions::M_Smash_44_4B6B90;
    field_136_shot_motion = eSligMotions::M_Smash_44_4B6B90;
    EventBroadcast(kEventMudokonComfort, this);
    return 1;
}

s16 Slig::vIsFacingMe_4B23D0(BaseAnimatedWithPhysicsGameObject* pWho)
{
    if (mCurrentMotion != eSligMotions::M_TurnAroundStanding_5_4B6390 || mAnim.mCurrentFrame < 6)
    {
        if (pWho->mXPos <= mXPos && mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            return 1;
        }

        if (pWho->mXPos >= mXPos && !mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            return 1;
        }
    }
    else
    {
        if (pWho->mXPos <= mXPos && !mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            return 1;
        }

        if (pWho->mXPos >= mXPos && mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
        {
            return 1;
        }
    }
    return 0;
}

s16 Slig::vOnSameYLevel_4BB6C0(BaseAnimatedWithPhysicsGameObject* pOther)
{
    const PSX_RECT bOurRect = VGetBoundingRect();
    const PSX_RECT bTheirRect = pOther->VGetBoundingRect();

    return ((bTheirRect.h + bTheirRect.y) / 2) <= bOurRect.h && bTheirRect.h >= bOurRect.y && mScale == pOther->mScale;
}

s16 Slig::FindLiftPoint_4B9B40()
{
    const FP k2Scaled = ScaleToGridSize(mSpriteScale) * FP_FromInteger(2);

    PlatformBase* pLift = nullptr;
    if (mAnim.mFlags.Get(AnimFlags::eBit5_FlipX))
    {
        pLift = static_cast<PlatformBase*>(FindObjectOfType(ReliveTypes::eLiftPoint, mXPos - k2Scaled, mYPos + FP_FromInteger(5)));
    }
    else
    {
        pLift = static_cast<PlatformBase*>(FindObjectOfType(ReliveTypes::eLiftPoint, k2Scaled + mXPos, mYPos + FP_FromInteger(5)));
    }

    if (pLift)
    {
        const PSX_RECT rect = pLift->VGetBoundingRect();

        const FP liftMidXPos = FP_FromInteger((rect.x + rect.w) / 2);
        return FP_Abs(mXPos - liftMidXPos) < k2Scaled;
    }

    return FALSE;
}
