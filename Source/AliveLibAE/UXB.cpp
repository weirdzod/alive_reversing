#include "stdafx.h"
#include "UXB.hpp"
#include "Function.hpp"
#include "stdlib.hpp"
#include "SwitchStates.hpp"
#include "DebugHelpers.hpp"
#include "StringFormatters.hpp"
#include "Events.hpp"
#include "Sfx.hpp"
#include "VRam.hpp"
#include "Game.hpp"
#include "ScreenManager.hpp"
#include "Sound/Midi.hpp"
#include "Abe.hpp"
#include "BaseBomb.hpp"
#include "Grid.hpp"
#include <math.h>

const TintEntry sTintMap_UXB_563A3C[19] = {
    {LevelIds_s8::eMines_1, 127u, 127u, 127u},
    {LevelIds_s8::eNecrum_2, 137u, 137u, 137u},
    {LevelIds_s8::eMudomoVault_3, 127u, 127u, 127u},
    {LevelIds_s8::eMudancheeVault_4, 127u, 127u, 127u},
    {LevelIds_s8::eFeeCoDepot_5, 127u, 127u, 127u},
    {LevelIds_s8::eBarracks_6, 127u, 127u, 127u},
    {LevelIds_s8::eMudancheeVault_Ender_7, 127u, 127u, 127u},
    {LevelIds_s8::eBonewerkz_8, 127u, 127u, 127u},
    {LevelIds_s8::eBrewery_9, 127u, 127u, 127u},
    {LevelIds_s8::eBrewery_Ender_10, 127u, 127u, 127u},
    {LevelIds_s8::eMudomoVault_Ender_11, 127u, 127u, 127u},
    {LevelIds_s8::eFeeCoDepot_Ender_12, 127u, 127u, 127u},
    {LevelIds_s8::eBarracks_Ender_13, 127u, 127u, 127u},
    {LevelIds_s8::eBonewerkz_Ender_14, 127u, 127u, 127u},
    {LevelIds_s8::eNone, 127u, 127u, 127u}};

void UXB::InitBlinkAnim(Animation* pAnimation)
{
    const AnimRecord& rec = AnimRec(AnimId::Bomb_RedGreenTick);
    if (pAnimation->Init(rec.mFrameTableOffset, gObjList_animations_5C1A24, this, rec.mMaxW, rec.mMaxH, Add_Resource(ResourceManager::Resource_Animation, rec.mResourceId), 1u, 0, 0))
    {
        pAnimation->mRenderLayer = mAnim.mRenderLayer;
        pAnimation->mAnimFlags.Set(AnimFlags::eBit15_bSemiTrans);
        pAnimation->mAnimFlags.Set(AnimFlags::eBit16_bBlending);
        pAnimation->field_14_scale = mSpriteScale;
        pAnimation->mRed = 128;
        pAnimation->mGreen = 128;
        pAnimation->mBlue = 128;
        pAnimation->mRenderMode = TPageAbr::eBlend_1;
    }
    else
    {
        mGameObjectFlags.Set(Options::eListAddFailed_Bit1);
    }
}

void UXB::PlaySFX(SoundEffect sfxIdx)
{
    if (gMap.Is_Point_In_Current_Camera_4810D0(
            this->mLvlNumber,
            this->mPathNumber,
            this->mXPos,
            this->mYPos,
            0))
    {
        SFX_Play_Mono(sfxIdx, 35);
    }
}

s32 UXB::IsColliding()
{
    PSX_RECT uxbBound = {};
    VGetBoundingRect(&uxbBound, 1);

    for (s32 i = 0; i < gBaseAliveGameObjects_5C1B7C->Size(); i++)
    {
        BaseAliveGameObject* pObj = gBaseAliveGameObjects_5C1B7C->ItemAt(i);

        if (!pObj)
        {
            break;
        }

        if (pObj->mAliveGameObjectFlags.Get(e114_Bit6_SetOffExplosives) && pObj->mAnim.mAnimFlags.Get(AnimFlags::eBit3_Render))
        {
            PSX_RECT objBound = {};
            pObj->VGetBoundingRect(&objBound, 1);

            const s32 objX = FP_GetExponent(pObj->mXPos);
            const s32 objY = FP_GetExponent(pObj->mYPos);

            if (objX > uxbBound.x && objX < uxbBound.w && objY < uxbBound.h + 5 && uxbBound.x <= objBound.w && uxbBound.w >= objBound.x && uxbBound.h >= objBound.y && uxbBound.y <= objBound.h && pObj->mSpriteScale == mSpriteScale)
            {
                return 1;
            }
        }
    }

    return 0;
}



UXB::UXB(Path_UXB* tlv_params, TlvItemInfoUnion itemInfo)
    : BaseAliveGameObject(0)
{
    SetType(AETypes::eUXB_143);

    const AnimRecord& activeRec = AnimRec(AnimId::UXB_Active);
    auto pResource = BaseGameObject::Add_Resource(ResourceManager::Resource_Animation, activeRec.mResourceId);
    Animation_Init(activeRec.mFrameTableOffset, activeRec.mMaxW, activeRec.mMaxH, pResource, 1, 1u);

    mAnim.mAnimFlags.Set(AnimFlags::eBit15_bSemiTrans);
    mAnim.mRenderMode = TPageAbr::eBlend_0;

    SetTint(sTintMap_UXB_563A3C, gMap.mCurrentLevel);

    mGameObjectFlags.Set(BaseGameObject::Options::eInteractive_Bit8);
    field_1C8_flags.Clear(UXB_Flags_1C8::eUnused_Bit0);
    field_118_state = UXBState::eDelay_0;

    field_1C0_pattern_length = tlv_params->field_10_pattern_length;
    if (tlv_params->field_10_pattern_length < 1 || tlv_params->field_10_pattern_length > 4)
    {
        field_1C0_pattern_length = 1;
    }


    field_1C4_pattern = tlv_params->field_12_pattern;
    if (!tlv_params->field_12_pattern) // If no pattern set, go to a default one.
    {
        field_1C4_pattern = 11111;
    }

    field_1C2_pattern_index = 0;
    // Single out a single digit, and use that digit as the new amount of red blinks before a green one.
    field_1C6_red_blink_count = (field_1C4_pattern / static_cast<s32>(pow(10, field_1C0_pattern_length - 1))) % 10;

    if (tlv_params->field_14_scale != Scale_short::eFull_0)
    {
        if (tlv_params->field_14_scale == Scale_short::eHalf_1)
        {
            mSpriteScale = FP_FromDouble(0.5);
            mAnim.mRenderLayer = Layer::eLayer_BombMineCar_Half_16;
            mScale = 0;
        }
    }
    else
    {
        mSpriteScale = FP_FromDouble(1.0);
        mAnim.mRenderLayer = Layer::eLayer_BombMineCar_35;
        mScale = 1;
    }

    InitBlinkAnim(&field_128_animation);
    if (tlv_params->field_1_tlv_state) // Stores the activated/deactivated state for UXB.
    {
        if (tlv_params->field_16_start_state == Path_UXB::StartState::eOn_0)
        {
            field_128_animation.Load_Pal(ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Palt, AEResourceID::kGrenflshResID, 0, 0), 0);
            field_1C8_flags.Clear(UXB_Flags_1C8::eIsRed_Bit1);
            const AnimRecord& flashRec = AnimRec(AnimId::Bomb_RedGreenTick);
            field_128_animation.Set_Animation_Data(flashRec.mFrameTableOffset, 0);
            PlaySFX(SoundEffect::GreenTick_2);

            const AnimRecord& animRec = AnimRec(AnimId::UXB_Disabled);
            mAnim.Set_Animation_Data(animRec.mFrameTableOffset, nullptr);
            field_118_state = UXBState::eDeactivated_3;
            field_11A_starting_state = UXBState::eDelay_0;
        }
        else
        {
            field_11A_starting_state = UXBState::eDeactivated_3;
        }
    }
    else
    {
        if (tlv_params->field_16_start_state == Path_UXB::StartState::eOn_0)
        {
            field_11A_starting_state = UXBState::eDelay_0;
        }
        else
        {
            field_128_animation.Load_Pal(ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Palt, AEResourceID::kGrenflshResID, 0, 0), 0);
            field_1C8_flags.Clear(UXB_Flags_1C8::eIsRed_Bit1);
            const AnimRecord& flashRec = AnimRec(AnimId::Bomb_RedGreenTick);
            field_128_animation.Set_Animation_Data(flashRec.mFrameTableOffset, 0);

            const AnimRecord& animRec = AnimRec(AnimId::UXB_Disabled);
            mAnim.Set_Animation_Data(animRec.mFrameTableOffset, nullptr);
            field_11A_starting_state = UXBState::eDeactivated_3;
            field_118_state = UXBState::eDeactivated_3;
        }
    }

    FP hitX = {};
    FP hitY = {};

    mXPos = FP_FromInteger((tlv_params->field_8_top_left.field_0_x + tlv_params->field_C_bottom_right.field_0_x) / 2);
    mYPos = FP_FromInteger(tlv_params->field_8_top_left.field_2_y);

    // Raycasts on ctor to place perfectly on the floor.
    if (sCollisions_DArray_5C1128->Raycast(
            mXPos,
            FP_FromInteger(tlv_params->field_8_top_left.field_2_y),
            mXPos,
            FP_FromInteger(tlv_params->field_8_top_left.field_2_y + 24),
            &mCollisionLine,
            &hitX,
            &hitY,
            mScale != 0 ? 1 : 16)
        == 1)
    {
        mYPos = hitY;
    }

    field_120_tlv = itemInfo;
    field_124_next_state_frame = sGnFrame_5C1B84;
    field_11C_disabled_resources = static_cast<u16>(tlv_params->field_18_disabled_resources);

    Add_Resource(ResourceManager::Resource_Animation, AEResourceID::kAbebombResID);
    Add_Resource(ResourceManager::Resource_Animation, AEResourceID::kDebrisID00ResID);
    Add_Resource(ResourceManager::Resource_Animation, AEResourceID::kBgexpldResID);
    Add_Resource(ResourceManager::Resource_Palt, AEResourceID::kGrenflshResID);

    if (!(field_11C_disabled_resources & 1))
    {
        Add_Resource(ResourceManager::Resource_Animation, AEResourceID::kAbeblowResID);
    }
    if (!(field_11C_disabled_resources & 2))
    {
        Add_Resource(ResourceManager::Resource_Animation, AEResourceID::kSlogBlowResID);
    }

    const FP gridSnap = ScaleToGridSize(mSpriteScale);
    mGameObjectFlags.Set(Options::eInteractive_Bit8);
    mApplyShadows |= 2u;

    mCollectionRect.x = mXPos - (gridSnap / FP_FromDouble(2.0));
    mCollectionRect.y = mYPos - gridSnap;
    mCollectionRect.w = (gridSnap / FP_FromDouble(2.0)) + mXPos;
    mCollectionRect.h = mYPos;
}


void UXB::VOnPickUpOrSlapped()
{
    if (field_118_state != UXBState::eExploding_2)
    {
        if (field_118_state != UXBState::eDeactivated_3 || field_124_next_state_frame > sGnFrame_5C1B84)
        {
            if (field_1C6_red_blink_count)
            {
                field_118_state = UXBState::eExploding_2;
                field_124_next_state_frame = sGnFrame_5C1B84 + 2;
            }
            else
            {
                const AnimRecord& flashRec = AnimRec(AnimId::Bomb_RedGreenTick);
                field_128_animation.Set_Animation_Data(flashRec.mFrameTableOffset, 0);
                PlaySFX(SoundEffect::GreenTick_2);

                const AnimRecord& animRec = AnimRec(AnimId::UXB_Toggle);
                mAnim.Set_Animation_Data(animRec.mFrameTableOffset, nullptr);
                field_118_state = UXBState::eDeactivated_3;

                field_124_next_state_frame = sGnFrame_5C1B84 + 10;
            }
        }
        else
        {
            field_118_state = UXBState::eDelay_0;
            SetUpdateDelay(6);
            const AnimRecord& animRec = AnimRec(AnimId::UXB_Active);
            mAnim.Set_Animation_Data(animRec.mFrameTableOffset, nullptr);
            PlaySFX(SoundEffect::RedTick_3);
        }
    }
}

void UXB::VOnThrowableHit(BaseGameObject* /*pFrom*/)
{
    ae_new<BaseBomb>(mXPos,
                                  mYPos,
                                  0,
                                  mSpriteScale);
    field_118_state = UXBState::eExploding_2;
    mGameObjectFlags.Set(BaseGameObject::eDead);
    field_124_next_state_frame = sGnFrame_5C1B84;
}

s16 UXB::VTakeDamage(BaseGameObject* pFrom)
{
    if (mGameObjectFlags.Get(BaseGameObject::eDead))
    {
        return 0;
    }

    switch (pFrom->Type())
    {
        case AETypes::eAbe_69:
        case AETypes::eMudokon_110:
            if (field_118_state == UXBState::eDeactivated_3)
            {
                return 0;
            }
            break;

        case AETypes::eMineCar_89:
        case AETypes::eAbilityRing_104:
        case AETypes::eExplosion_109:
        case AETypes::eShrykull_121:
            break;

        default:
            return 0;
    }

    mGameObjectFlags.Set(BaseGameObject::eDead);

    ae_new<BaseBomb>(mXPos,
                                 mYPos,
                                 0,
                                 mSpriteScale);
    field_118_state = UXBState::eExploding_2;
    field_124_next_state_frame = sGnFrame_5C1B84;

    return 1;
}

UXB::~UXB()
{
    if (field_118_state != UXBState::eExploding_2 || sGnFrame_5C1B84 < field_124_next_state_frame)
    {
        Path::TLV_Reset(field_120_tlv.all, -1, 0, 0);
    }
    else
    {
        Path::TLV_Reset(field_120_tlv.all, -1, 0, 1);
    }

    field_128_animation.VCleanUp();

    mGameObjectFlags.Clear(Options::eInteractive_Bit8);
}

void UXB::VUpdate()
{
    switch (field_118_state)
    {
        case UXBState::eDelay_0:
            if (IsColliding())
            {
                field_118_state = UXBState::eExploding_2;
                field_124_next_state_frame = sGnFrame_5C1B84 + 2;
            }
            else if (field_124_next_state_frame <= sGnFrame_5C1B84)
            {
                const AnimRecord& flashRec = AnimRec(AnimId::Bomb_Flash);
                field_118_state = UXBState::eActive_1;
                field_128_animation.Set_Animation_Data(flashRec.mFrameTableOffset, 0);
                field_124_next_state_frame = sGnFrame_5C1B84 + 2;
            }
            break;

        case UXBState::eDeactivated_3:
            // Do nothing
            break;

        case UXBState::eActive_1:
            if (IsColliding())
            {
                field_118_state = UXBState::eExploding_2;
                field_124_next_state_frame = sGnFrame_5C1B84 + 2;
            }
            else if (field_124_next_state_frame <= sGnFrame_5C1B84)
            {
                if (field_1C6_red_blink_count)
                {
                    field_1C6_red_blink_count--;
                    if (field_1C6_red_blink_count == 0)
                    {
                        field_128_animation.Load_Pal(ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Palt, AEResourceID::kGrenflshResID, 0, 0), 0);
                        field_1C8_flags.Clear(UXB_Flags_1C8::eIsRed_Bit1);
                    }
                }
                else
                {
                    const FrameInfoHeader* pFrameInfo = field_128_animation.Get_FrameHeader(-1);
                    const FrameHeader* pFrameHeader = reinterpret_cast<const FrameHeader*>(&(*field_128_animation.field_20_ppBlock)[pFrameInfo->field_0_frame_header_offset]);
                    field_128_animation.Load_Pal(field_128_animation.field_20_ppBlock, pFrameHeader->field_0_clut_offset);

                    field_1C8_flags.Set(UXB_Flags_1C8::eIsRed_Bit1);

                    field_1C2_pattern_index++;

                    if (field_1C2_pattern_index >= field_1C0_pattern_length)
                    {
                        field_1C2_pattern_index = 0;
                    }

                    // Single out a single digit, and use that digit as the new amount of red blinks before a green one.
                    field_1C6_red_blink_count = (field_1C4_pattern / static_cast<s32>(pow(10, field_1C0_pattern_length - field_1C2_pattern_index - 1))) % 10;
                }

                const AnimRecord& rec = AnimRec(AnimId::Bomb_RedGreenTick);
                field_128_animation.Set_Animation_Data(rec.mFrameTableOffset, 0);

                if (field_1C8_flags.Get(UXB_Flags_1C8::eIsRed_Bit1))
                {
                    PlaySFX(SoundEffect::RedTick_3);
                }
                else
                {
                    PlaySFX(SoundEffect::GreenTick_2);
                }

                field_118_state = UXBState::eDelay_0;
                field_124_next_state_frame = sGnFrame_5C1B84 + 10; // UXB change color delay.
            }
            break;

        case UXBState::eExploding_2:
            if (sGnFrame_5C1B84 >= field_124_next_state_frame)
            {
                ae_new<BaseBomb>(mXPos, mYPos, 0, mSpriteScale);
                mGameObjectFlags.Set(Options::eDead);
            }
            break;
    }

    if (field_118_state != UXBState::eExploding_2)
    {
        if (Event_Get(kEventDeathReset))
        {
            if (field_11A_starting_state != UXBState::eDeactivated_3 || field_118_state == UXBState::eDeactivated_3)
            {
                if (field_11A_starting_state != UXBState::eDelay_0 || field_118_state != UXBState::eDeactivated_3)
                {
                    Path::TLV_Reset(field_120_tlv.all, 0, 1, 0);
                }
                else
                {
                    Path::TLV_Reset(field_120_tlv.all, 1, 1, 0);
                }
            }
            else
            {
                Path::TLV_Reset(field_120_tlv.all, 1, 1, 0);
            }
            mGameObjectFlags.Set(Options::eDead);
        }
    }
}

void UXB::VRender(PrimHeader** ppOt)
{
    if (mAnim.mAnimFlags.Get(AnimFlags::eBit3_Render))
    {
        if (gMap.Is_Point_In_Current_Camera_4810D0(
                mLvlNumber,
                mPathNumber,
                mXPos,
                mYPos,
                0))
        {
            field_128_animation.VRender(
                FP_GetExponent((mXPos - pScreenManager_5BB5F4->field_20_pCamPos->field_0_x)),
                FP_GetExponent((mYPos - pScreenManager_5BB5F4->field_20_pCamPos->field_4_y - FP_NoFractional(mSpriteScale * FP_FromInteger(17)))),
                ppOt,
                0,
                0);

            PSX_RECT frameRect;
            field_128_animation.Get_Frame_Rect(&frameRect);

            pScreenManager_5BB5F4->InvalidateRect_40EC90(
                frameRect.x,
                frameRect.y,
                frameRect.w,
                frameRect.h,
                pScreenManager_5BB5F4->field_3A_idx);

            BaseAnimatedWithPhysicsGameObject::VRender(ppOt);
        }
    }
}

void UXB::VScreenChanged()
{
    BaseGameObject::VScreenChanged();

    const FP x_distance = FP_Abs(sControlledCharacter->mXPos - mXPos);
    const FP y_distance = FP_Abs(sControlledCharacter->mYPos - mYPos);

    if (y_distance > FP_FromInteger(520) || x_distance > FP_FromInteger(750))
    {
        if (field_11A_starting_state != UXBState::eDeactivated_3 || field_118_state == UXBState::eDeactivated_3)
        {
            if (field_11A_starting_state != UXBState::eDelay_0 || field_118_state != UXBState::eDeactivated_3)
            {
                Path::TLV_Reset(field_120_tlv.all, 0, 1, 0);
                mGameObjectFlags.Set(Options::eDead);
            }
            else
            {
                Path::TLV_Reset(field_120_tlv.all, 1, 1, 0);
                mGameObjectFlags.Set(Options::eDead);
            }
        }
        else
        {
            Path::TLV_Reset(field_120_tlv.all, 1, 1, 0);
            mGameObjectFlags.Set(Options::eDead);
        }
    }
}

s32 UXB::VGetSaveState(u8* __pSaveBuffer)
{
    SaveState_UXB* pSaveState = reinterpret_cast<SaveState_UXB*>(__pSaveBuffer);

    pSaveState->field_0_id = AETypes::eUXB_143;
    pSaveState->field_4_tlv = field_120_tlv;
    pSaveState->field_8_next_state_frame = field_124_next_state_frame;
    pSaveState->field_C_state = field_118_state;
    pSaveState->field_E_starting_state = field_11A_starting_state;
    pSaveState->field_10_disabled_resources = field_11C_disabled_resources;
    pSaveState->field_12_pattern_index = field_1C2_pattern_index;
    pSaveState->field_14_red_blink_count = field_1C6_red_blink_count;
    pSaveState->field_16_is_red = field_1C8_flags.Get(UXB_Flags_1C8::eIsRed_Bit1);

    return sizeof(SaveState_UXB);
}

s32 UXB::CreateFromSaveState(const u8* __pSaveState)
{
    const SaveState_UXB* pSaveState = reinterpret_cast<const SaveState_UXB*>(__pSaveState);

    Path_UXB* uxbPath = reinterpret_cast<Path_UXB*>(sPath_dword_BB47C0->TLV_From_Offset_Lvl_Cam(pSaveState->field_4_tlv.all));

    if (!(uxbPath->field_18_disabled_resources & 1) && !ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Animation, AEResourceID::kAbeblowResID, 0, 0))
    {
        ResourceManager::LoadResourceFile_49C170("ABEBLOW.BAN", 0);
    }
    if (!(uxbPath->field_18_disabled_resources & 2) && !ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Animation, AEResourceID::kSlogBlowResID, 0, 0))
    {
        ResourceManager::LoadResourceFile_49C170("DOGBLOW.BAN", 0);
    }
    if (!ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Animation, AEResourceID::kUXBResID, 0, 0))
    {
        ResourceManager::LoadResourceFile_49C170("UXB.BND", 0);
    }
    if (!ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Animation, AEResourceID::kBgexpldResID, 0, 0))
    {
        ResourceManager::LoadResourceFile_49C170("EXPLODE.BND", 0);
    }

    UXB* pUXB = ae_new<UXB>(uxbPath, pSaveState->field_4_tlv);

    if (pSaveState->field_C_state == UXBState::eDeactivated_3)
    {
        pUXB->field_128_animation.Load_Pal(ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Palt, AEResourceID::kGrenflshResID, 0, 0), 0);
        const AnimRecord& tickRec = AnimRec(AnimId::Bomb_RedGreenTick);
        pUXB->field_128_animation.Set_Animation_Data(tickRec.mFrameTableOffset, 0);
        const AnimRecord& animRec = AnimRec(AnimId::UXB_Disabled);
        pUXB->mAnim.Set_Animation_Data(animRec.mFrameTableOffset, nullptr);
    }

    pUXB->field_124_next_state_frame = pSaveState->field_8_next_state_frame;
    pUXB->field_118_state = pSaveState->field_C_state;
    pUXB->field_11A_starting_state = pSaveState->field_E_starting_state;
    pUXB->field_11C_disabled_resources = pSaveState->field_10_disabled_resources;
    pUXB->field_1C2_pattern_index = pSaveState->field_12_pattern_index;
    pUXB->field_1C6_red_blink_count = pSaveState->field_14_red_blink_count;

    pUXB->field_1C8_flags.Clear(UXB_Flags_1C8::eIsRed_Bit1);

    if (pSaveState->field_16_is_red)
    {
        pUXB->field_1C8_flags.Set(UXB_Flags_1C8::eIsRed_Bit1);
    }

    return sizeof(SaveState_UXB);
}

