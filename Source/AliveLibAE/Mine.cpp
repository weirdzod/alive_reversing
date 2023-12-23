#include "stdafx.h"
#include "Mine.hpp"
#include "../relive_lib/Function.hpp"
#include "stdlib.hpp"
#include "../relive_lib/Events.hpp"
#include "Sfx.hpp"
#include "Game.hpp"
#include "../relive_lib/GameObjects/ScreenManager.hpp"
#include "../relive_lib/GameObjects/GroundExplosion.hpp"
#include "../relive_lib/Grid.hpp"
#include "Map.hpp"
#include "Path.hpp"
#include "../relive_lib/Collisions.hpp"
#include "../relive_lib/FixedPoint.hpp"
#include "../relive_lib/GameType.hpp"

static Mine* sMinePlayingSound = nullptr;

void Mine::LoadAnimations()
{
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::Mine_Flash));
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::Mine));
}

Mine::Mine(relive::Path_Mine* pTlv, const Guid& tlvId)
    : BaseAliveGameObject(0)
{
    SetType(ReliveTypes::eMine);

    LoadAnimations();
    Animation_Init(GetAnimRes(AnimId::Mine));

    SetInteractive(true);
    SetCanExplode(true);

    if (pTlv->mScale == relive::reliveScale::eHalf)
    {
        SetSpriteScale(FP_FromDouble(0.5));
        GetAnimation().SetRenderLayer(Layer::eLayer_RollingBallBombMineCar_Half_16);
        SetScale(Scale::Bg);
    }
    else
    {
        SetSpriteScale(FP_FromInteger(1));
        GetAnimation().SetRenderLayer(Layer::eLayer_RollingBallBombMineCar_35);
        SetScale(Scale::Fg);
    }

    if (GetGameType() == GameType::eAe)
    {
        const FP tlvMidPointX = FP_FromInteger(pTlv->MidPointX());
        mXPos = tlvMidPointX;
        const FP tlvTopLeftY = FP_FromInteger(pTlv->mTopLeftY);
        mYPos = tlvTopLeftY;

        FP hitY;
        FP hitX;

        if (gCollisions->Raycast(
                tlvMidPointX,
                tlvTopLeftY,
                tlvMidPointX,
                tlvTopLeftY + FP_FromInteger(24),
                &BaseAliveGameObjectCollisionLine,
                &hitX,
                &hitY,
                GetScale() == Scale::Fg ? kFgFloor : kBgFloor)
            == 1)
        {
            mYPos = hitY;
        }
    }
    else
    {
        mXPos = FP_FromInteger(pTlv->mTopLeftX + 12);
        mYPos = FP_FromInteger(pTlv->mTopLeftY + 24);
    }

    mTlvId = tlvId;
    mExplosionTimer = sGnFrame;
    mFlashAnim.Init(GetAnimRes(AnimId::Mine_Flash), this);

    mFlashAnim.SetSemiTrans(true);
    mFlashAnim.SetBlending(true);

    mFlashAnim.SetRenderLayer(GetAnimation().GetRenderLayer());
    mFlashAnim.SetSpriteScale(GetSpriteScale());
    mFlashAnim.SetRGB(128, 128, 128);

    if (pTlv->mPersistOffscreen == relive::reliveChoice::eYes)
    {
        mPersistOffscreen = true;
    }

    if (GetMap().mCurrentLevel == EReliveLevelIds::eStockYards || GetMap().mCurrentLevel == EReliveLevelIds::eStockYardsReturn)
    {
        mRGB.SetRGB(50, 50, 50);
        // TODO: Set the pal
        // ResourceManager::GetLoadedResource(ResourceManager::Resource_Palt, AOResourceID::kAbeblowAOResID, 1, 0);
        // ResourceManager::GetLoadedResource(ResourceManager::Resource_Palt, AOResourceID::kSlogBlowAOResID, 1, 0);
    }

    const FP gridSnap = ScaleToGridSize(GetSpriteScale());
    SetInteractive(true);
    if (GetGameType() == GameType::eAe)
    {
        SetDoPurpleLightEffect(true);
    }

    mCollectionRect.x = mXPos - (gridSnap / FP_FromInteger(2));
    mCollectionRect.y = mYPos - gridSnap;
    mCollectionRect.w = mXPos + (gridSnap / FP_FromInteger(2));
    mCollectionRect.h = mYPos;
}

Mine::~Mine()
{
    if (mDetonating)
    {
        Path::TLV_Reset(mTlvId, -1, 0, 1);
    }
    else
    {
        Path::TLV_Reset(mTlvId, -1, 0, 0);
    }

    mFlashAnim.VCleanUp();
    SetInteractive(false);

    if (sMinePlayingSound == this)
    {
        sMinePlayingSound = nullptr;
    }
}

void Mine::VScreenChanged()
{
    if (GetMap().LevelChanged() || GetMap().PathChanged() || !mPersistOffscreen)
    {
        SetDead(true);
    }
}

bool Mine::VTakeDamage(BaseGameObject* pFrom)
{
    if (GetDead())
    {
        return false;
    }

    switch (pFrom->Type())
    {
        case ReliveTypes::eGreeter:
        case ReliveTypes::eAbe:
        case ReliveTypes::eMineCar:
        case ReliveTypes::eAbilityRing:
        case ReliveTypes::eAirExplosion:
        case ReliveTypes::eMudokon: // might cause issues in AO?
        case ReliveTypes::eShrykull:
        {
            relive_new GroundExplosion(mXPos, mYPos, GetSpriteScale());
            SetDead(true);
            mDetonating = true;
            mExplosionTimer = sGnFrame;
            return true;
        }

        default:
            return false;
    }
}

void Mine::VOnThrowableHit(BaseGameObject* /*pFrom*/)
{
    relive_new GroundExplosion(mXPos, mYPos, GetSpriteScale());
    if (GetGameType() == GameType::eAe)
    {
        SetDead(true);
    }
    mDetonating = true;
}

void Mine::VOnAbeInteraction()
{
    if (!mDetonating)
    {
        mDetonating = true;
        mExplosionTimer = MakeTimer(5);
    }
}

void Mine::VRender(OrderingTable& ot)
{
    if (GetAnimation().GetRender())
    {
        if (GetMap().Is_Point_In_Current_Camera(
                mCurrentLevel,
                mCurrentPath,
                mXPos,
                mYPos,
                0))
        {
            mFlashAnim.VRender(
                FP_GetExponent(mXPos - gScreenManager->CamXPos()),
                FP_GetExponent(FP_FromInteger(mYOffset) + mYPos - gScreenManager->CamYPos()),
                ot,
                0,
                0);
            BaseAnimatedWithPhysicsGameObject::VRender(ot);
        }
    }
}

void Mine::VUpdate()
{
    const s16 bInCamera = GetMap().Is_Point_In_Current_Camera(
        mCurrentLevel,
        mCurrentPath,
        mXPos,
        mYPos,
        0);

    if (mDetonating)
    {
        if (mDetonating && sGnFrame >= mExplosionTimer)
        {
            relive_new GroundExplosion(mXPos, mYPos, GetSpriteScale());
            SetDead(true);
        }
    }
    else
    {
        if (GetAnimation().GetCurrentFrame() == 1 && (!sMinePlayingSound || sMinePlayingSound == this))
        {
            if (bInCamera)
            {
                SfxPlayMono(relive::SoundEffects::RedTick, 35);
                sMinePlayingSound = this;
            }
            else
            {
                sMinePlayingSound = nullptr;
            }
        }
        if (IsColliding())
        {
            mDetonating = true;
            mExplosionTimer = sGnFrame;
        }
    }
    if (!mDetonating)
    {
        if (EventGet(kEventDeathReset) || mCurrentLevel != GetMap().mCurrentLevel || mCurrentPath != GetMap().mCurrentPath)
        {
            SetDead(true);
        }
    }
}

bool Mine::IsColliding()
{
    const PSX_RECT mineBound = VGetBoundingRect();
    for (s32 i = 0; i < gBaseAliveGameObjects->Size(); i++)
    {
        IBaseAliveGameObject* pObj = gBaseAliveGameObjects->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj->GetCanSetOffExplosives() && pObj->GetAnimation().GetRender())
        {
            const PSX_RECT objBound = pObj->VGetBoundingRect();
            const s32 objX = FP_GetExponent(pObj->mXPos);
            const s32 objY = FP_GetExponent(pObj->mYPos);

            // TODO: add to conversion instead?
            s16 boundAdjustment = GetGameType() == GameType::eAe ? 12 : 5;

            if (objX > mineBound.x && objX < mineBound.w && objY < mineBound.h + boundAdjustment && mineBound.x <= objBound.w && mineBound.w >= objBound.x && mineBound.h >= objBound.y && mineBound.y <= objBound.h && pObj->GetSpriteScale() == GetSpriteScale())
            {
                return true;
            }
        }
    }
    return false;
}
