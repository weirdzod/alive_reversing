#include "stdafx.h"
#include "TimedMine.hpp"
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
#include "ObjectIds.hpp"
#include "BaseBomb.hpp"
#include "LiftPoint.hpp"
#include "Grid.hpp"

static TintEntry sTimedMineTint_550EB8[1] = {{LevelIds_s8::eNone, 127u, 127u, 127u}};


TimedMine::TimedMine(Path_TimedMine* pPath, TlvItemInfoUnion tlv)
    : BaseAliveGameObject(0)
{
    SetType(AETypes::eTimedMine_or_MovingBomb_10);

    const AnimRecord& rec = AnimRec(AnimId::TimedMine_Idle);
    u8** ppRes = Add_Resource(ResourceManager::Resource_Animation, rec.mResourceId);
    Animation_Init(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1, 1);

    mGameObjectFlags.Set(Options::eInteractive_Bit8);

    field_1C4_flags.Clear(TimedMine_Flags_1C4::eStickToLiftPoint_0);

    field_118_armed = 0;

    if (pPath->field_14_scale == Scale_short::eHalf_1)
    {
        mSpriteScale = FP_FromDouble(0.5);
        mScale = 0;
        mAnim.mRenderLayer = Layer::eLayer_BombMineCar_Half_16;
    }
    else
    {
        mSpriteScale = FP_FromDouble(1.0);
        mScale = 1;
        mAnim.mRenderLayer = Layer::eLayer_BombMineCar_35;
    }

    InitBlinkAnimation(&field_124_animation);

    field_11A_ticks_before_explosion = pPath->field_16_ticks_before_explosion;

    mXPos = FP_FromInteger((pPath->field_8_top_left.field_0_x + pPath->field_C_bottom_right.field_0_x) / 2);
    mYPos = FP_FromInteger(pPath->field_8_top_left.field_2_y);

    FP hitY = {};
    FP hitX = {};

    if (sCollisions_DArray_5C1128->Raycast(
            mXPos,
            mYPos,
            mXPos,
            mYPos + FP_FromInteger(24),
            &mCollisionLine,
            &hitX,
            &hitY,
            mScale != 0 ? 1 : 0x10))
    {
        mYPos = hitY;
    }

    field_11C_tlv = tlv.all;
    field_120_gnframe = sGnFrame_5C1B84;
    SetBaseAnimPaletteTint(sTimedMineTint_550EB8, gMap.mCurrentLevel, kBombResID);

    const FP gridSnap = ScaleToGridSize(mSpriteScale);
    mGameObjectFlags.Set(Options::eInteractive_Bit8);
    mApplyShadows |= 2u;

    mCollectionRect.x = mXPos - (gridSnap / FP_FromDouble(2.0));
    mCollectionRect.y = mYPos - gridSnap;
    mCollectionRect.w = (gridSnap / FP_FromDouble(2.0)) + mXPos;
    mCollectionRect.h = mYPos;

    mObjectId = -1;
}

void TimedMine::VUpdate()
{
    auto pPlatform = static_cast<LiftPoint*>(sObjectIds.Find_Impl(mObjectId));
    if (Event_Get(kEventDeathReset))
    {
        mGameObjectFlags.Set(Options::eDead);
    }

    if (!field_1C4_flags.Get(TimedMine_Flags_1C4::eStickToLiftPoint_0))
    {
        StickToLiftPoint();
    }

    if (pPlatform && pPlatform->vOnAnyFloor())
    {
        mCollectionRect.x = mXPos - (ScaleToGridSize(mSpriteScale) / FP_FromInteger(2));
        mCollectionRect.w = (ScaleToGridSize(mSpriteScale) / FP_FromInteger(2)) + mXPos;
        mCollectionRect.h = mYPos;
        mCollectionRect.y = mYPos - ScaleToGridSize(mSpriteScale);
    }

    if (field_118_armed == 1)
    {
        if (sGnFrame_5C1B84 > field_1BC_gnFrame_2 + field_1C0_detonation_timer)
        {
            field_1BC_gnFrame_2 = sGnFrame_5C1B84;
            const CameraPos soundDir = gMap.GetDirection_4811A0(mLvlNumber, mPathNumber, mXPos, mYPos);
            SFX_Play_Camera(SoundEffect::GreenTick_2, 50, soundDir);

            // TODO: Modulus ?
            if (((field_120_gnframe - sGnFrame_5C1B84) & 0xFFFFFFF8) >= 144)
            {
                field_1C0_detonation_timer = 18;
            }
            else
            {
                field_1C0_detonation_timer = (field_120_gnframe - sGnFrame_5C1B84) >> 3;
            }
        }

        if (sGnFrame_5C1B84 >= field_120_gnframe)
        {
            ae_new<BaseBomb>(mXPos, mYPos, 0, mSpriteScale);
            mGameObjectFlags.Set(Options::eDead);
        }
    }
}

void TimedMine::VRender(PrimHeader** ppOt)
{
    if (gMap.Is_Point_In_Current_Camera_4810D0(
            mLvlNumber,
            mPathNumber,
            mXPos,
            mYPos,
            0))
    {
        field_124_animation.VRender(
            FP_GetExponent((mXPos - pScreenManager_5BB5F4->field_20_pCamPos->field_0_x)),
            FP_GetExponent((mYPos - pScreenManager_5BB5F4->field_20_pCamPos->field_4_y - FP_NoFractional(mSpriteScale * FP_FromDouble(14)))),
            ppOt,
            0,
            0);

        PSX_RECT frameRect = {};
        field_124_animation.Get_Frame_Rect(&frameRect);

        pScreenManager_5BB5F4->InvalidateRect_40EC90(
            frameRect.x,
            frameRect.y,
            frameRect.w,
            frameRect.h,
            pScreenManager_5BB5F4->field_3A_idx);

        BaseAnimatedWithPhysicsGameObject::VRender(ppOt);
    }
}

void TimedMine::InitBlinkAnimation(Animation* pAnimation)
{
    const AnimRecord& tickRec = AnimRec(AnimId::Bomb_RedGreenTick);
    if (pAnimation->Init(tickRec.mFrameTableOffset, gObjList_animations_5C1A24, this, tickRec.mMaxW, tickRec.mMaxH, Add_Resource(ResourceManager::Resource_Animation, tickRec.mResourceId), 1, 0, 0))
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

void TimedMine::StickToLiftPoint()
{
    FP hitX = {};
    FP hitY = {};
    PathLine* pLine = nullptr;
    if (sCollisions_DArray_5C1128->Raycast(
            mXPos,
            mYPos - FP_FromInteger(20),
            mXPos, mYPos + FP_FromInteger(20),
            &pLine, &hitX, &hitY,
            (mScale == 1) ? 15 : 240))
    {
        if (pLine->field_8_type == eLineTypes::eUnknown_32 ||
            pLine->field_8_type == eLineTypes::eUnknown_36)
        {
            if (ObjList_5C1B78)
            {
                for (s32 i = 0; i < ObjList_5C1B78->Size(); i++)
                {
                    BaseGameObject* pObj = ObjList_5C1B78->ItemAt(i);
                    if (!pObj)
                    {
                        break;
                    }

                    if (pObj->Type() == AETypes::eLiftPoint_78)
                    {
                        PSX_RECT bRect = {};
                        auto pLiftPoint = static_cast<LiftPoint*>(pObj);
                        pLiftPoint->VGetBoundingRect(&bRect, 1);
                        if (FP_GetExponent(mXPos) > bRect.x && FP_GetExponent(mXPos) < bRect.w && FP_GetExponent(mYPos) < bRect.h)
                        {
                            pLiftPoint->VAdd(this);
                            mObjectId = pObj->field_8_object_id;
                            return;
                        }
                    }
                }
            }
        }
    }
}

TimedMine::~TimedMine()
{
    auto pPlatform = static_cast<LiftPoint*>(sObjectIds.Find_Impl(mObjectId));
    if (field_118_armed != 1 || sGnFrame_5C1B84 < field_120_gnframe)
    {
        Path::TLV_Reset(field_11C_tlv, -1, 0, 0);
    }
    else
    {
        Path::TLV_Reset(field_11C_tlv, -1, 0, 1);
    }

    field_124_animation.VCleanUp();

    if (pPlatform)
    {
        pPlatform->VRemove(this);
        mObjectId = -1;
    }

    mGameObjectFlags.Clear(BaseGameObject::eInteractive_Bit8);
}

void TimedMine::VScreenChanged()
{
    if (gMap.mCurrentLevel != gMap.mLevel || gMap.mCurrentPath != gMap.mPath)
    {
        mGameObjectFlags.Set(BaseGameObject::eDead);
    }
    if (field_118_armed != 1)
    {
        mGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

s16 TimedMine::VTakeDamage(BaseGameObject* pFrom)
{
    if (mGameObjectFlags.Get(BaseGameObject::eDead))
    {
        return 0;
    }

    switch (pFrom->Type())
    {
        case AETypes::eAbe_69:
        case AETypes::eAbilityRing_104:
        case AETypes::eExplosion_109:
        case AETypes::eShrykull_121:
        {
            mGameObjectFlags.Set(BaseGameObject::eDead);
            ae_new<BaseBomb>(mXPos, mYPos, 0, mSpriteScale);
            field_118_armed = 1;
            field_120_gnframe = sGnFrame_5C1B84;
            return 1;
        }

        default:
            return 0;
    }
}

void TimedMine::VOnThrowableHit(BaseGameObject* /*pHitBy*/)
{
    ae_new<BaseBomb>(mXPos, mYPos, 0, mSpriteScale);
    
    field_118_armed = 1;
    mGameObjectFlags.Set(BaseGameObject::eDead);
    field_120_gnframe = sGnFrame_5C1B84;
}

void TimedMine::VOnPickUpOrSlapped()
{
    if (field_118_armed != 1)
    {
        field_118_armed = 1;
        if ((s32)(field_11A_ticks_before_explosion & 0xFFFC) >= 72)
        {
            field_1C0_detonation_timer = 18;
        }
        else
        {
            field_1C0_detonation_timer = field_11A_ticks_before_explosion >> 2;
        }
        field_1BC_gnFrame_2 = sGnFrame_5C1B84;
        const AnimRecord& animRec = AnimRec(AnimId::TimedMine_Activated);
        mAnim.Set_Animation_Data(animRec.mFrameTableOffset, nullptr);
        const AnimRecord& flashRec = AnimRec(AnimId::Bomb_Flash);
        field_120_gnframe = sGnFrame_5C1B84 + field_11A_ticks_before_explosion;
        field_124_animation.Set_Animation_Data(flashRec.mFrameTableOffset, 0);
        SFX_Play_Mono(SoundEffect::GreenTick_2, 0);
    }
}
