#include "stdafx_ao.h"
#include "Function.hpp"
#include "Throwable.hpp"
#include "Map.hpp"
#include "../AliveLibAE/stdlib.hpp"
#include "Grenade.hpp"
#include "Rock.hpp"
#include "Meat.hpp"
#include "../relive_lib/Collisions.hpp"
#include "Game.hpp"
#include "PlatformBase.hpp"

namespace AO {

// Overlay to throwable type table ??
const AOTypes gThrowableFromOverlayId[54] = {
    AOTypes::eNone_0, AOTypes::eNone_0, AOTypes::eNone_0, AOTypes::eGrenade_40, AOTypes::eNone_0, AOTypes::eNone_0, AOTypes::eRock_70, AOTypes::eNone_0, AOTypes::eRock_70, AOTypes::eRock_70,
    AOTypes::eNone_0, AOTypes::eRock_70, AOTypes::eNone_0, AOTypes::eRock_70, AOTypes::eNone_0, AOTypes::eRock_70, AOTypes::eNone_0, AOTypes::eNone_0, AOTypes::eRock_70, AOTypes::eNone_0,
    AOTypes::eMeat_54, AOTypes::eNone_0, AOTypes::eMeat_54, AOTypes::eNone_0, AOTypes::eRock_70, AOTypes::eNone_0, AOTypes::eRock_70, AOTypes::eRock_70, AOTypes::eRock_70, AOTypes::eNone_0,
    AOTypes::eRock_70, AOTypes::eNone_0, AOTypes::eRock_70, AOTypes::eNone_0, AOTypes::eRock_70, AOTypes::eNone_0, AOTypes::eRock_70, AOTypes::eNone_0, AOTypes::eNone_0, AOTypes::eGrenade_40,
    AOTypes::eNone_0, AOTypes::eGrenade_40, AOTypes::eGrenade_40, AOTypes::eGrenade_40, AOTypes::eGrenade_40, AOTypes::eGrenade_40, AOTypes::eGrenade_40, AOTypes::eGrenade_40, AOTypes::eNone_0, AOTypes::eGrenade_40,
    AOTypes::eNone_0, AOTypes::eNone_0, AOTypes::eNone_0, AOTypes::eNone_0};

BaseThrowable* Make_Throwable(FP xpos, FP ypos, s16 count)
{
    switch (gThrowableFromOverlayId[gMap.mOverlayId])
    {
        case AOTypes::eGrenade_40:
        {
            auto pGrenade = relive_new Grenade(xpos, ypos, count);
            if (pGrenade)
            {
                return pGrenade;
            }
            break;
        }

        case AOTypes::eMeat_54:
        {
            auto pMeat = relive_new Meat(xpos, ypos, count);
            if (pMeat)
            {
                return pMeat;
            }
            break;
        }

        case AOTypes::eRock_70:
        {
            auto pRock = relive_new Rock(xpos, ypos, count);
            if (pRock)
            {
                return pRock;
            }
            break;
        }

        default:
            break;
    }
    return nullptr;
}


void BaseThrowable::VToDead()
{
    mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    mIsDead = TRUE;
}

s16 BaseThrowable::VGetCount()
{
    return mThrowableCount;
}

void BaseThrowable::VOnPickUpOrSlapped()
{
    VToDead();
}

void BaseThrowable::BaseAddToPlatform()
{
    const FP scale = GetSpriteScale() - FP_FromDouble(0.5);

    PathLine* pLine = nullptr;
    FP hitX = {};
    FP hitY = {};
    if (sCollisions->Raycast(
            mXPos,
            mYPos - FP_FromInteger(20),
            mXPos,
            mYPos + FP_FromInteger(20),
            &pLine,
            &hitX,
            &hitY,
            scale != FP_FromInteger(0) ? kFgWallsOrFloor : kBgWallsOrFloor))
    {
        if (pLine->mLineType == eLineTypes::eDynamicCollision_32 ||
            pLine->mLineType == eLineTypes::eBackgroundDynamicCollision_36)
        {
            for (s32 i = 0; i < gPlatformsArray->Size(); i++)
            {
                BaseGameObject* pObjIter = gPlatformsArray->ItemAt(i);
                if (!pObjIter)
                {
                    break;
                }

                if (pObjIter->Type() == ReliveTypes::eLiftPoint || pObjIter->Type() == ReliveTypes::eTrapDoor)
                {
                    auto pPlatformBase = static_cast<PlatformBase*>(pObjIter);

                    const PSX_RECT objRect = pPlatformBase->VGetBoundingRect();

                    if (FP_GetExponent(mXPos) > objRect.x && FP_GetExponent(mXPos) < objRect.w && FP_GetExponent(mYPos) < objRect.h)
                    {
                        if (mLiftPoint)
                        {
                            if (mLiftPoint == pPlatformBase)
                            {
                                return;
                            }
                            mLiftPoint->VRemove(this);
                            mLiftPoint->mBaseGameObjectRefCount--;
                            mLiftPoint = nullptr;
                        }

                        mLiftPoint = pPlatformBase;
                        mLiftPoint->VAdd(this);
                        mLiftPoint->mBaseGameObjectRefCount++;
                        return;
                    }
                }
            }
        }
    }
}

} // namespace AO
