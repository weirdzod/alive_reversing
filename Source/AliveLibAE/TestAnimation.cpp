#include "stdafx.h"
#include "TestAnimation.hpp"
#include "Abe.hpp"
#include "ResourceManager.hpp"
#include "AnimResources.hpp"
#include "Function.hpp"
#include "stdlib.hpp"
#include "Game.hpp"


void TestAnimation::DelayLoad()
{
    // Trying to load on these lvls will result in a phat failure because they hardly have
    // any resource fiiles
    if (mLvlNumber == LevelIds::eMenu_0 || mLvlNumber == LevelIds::eCredits_16)
    {
        return;
    }

    LOG_INFO("Test anim loading...");

    mLoaded = true; // Only do code below once

    const AnimRecord& animRec = AnimRec(AnimId::Anim_Tester);

    if (!ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Animation, animRec.mResourceId, 0, 0))
    {
        ResourceManager::LoadResourceFile_49C170(animRec.mBanName, nullptr);
    }
    else
    {
        LOG_WARNING("Anim resource already loaded - BAN/BND name not verified by test animation");
    }

    u8** ppRes = Add_Resource(ResourceManager::Resource_Animation, animRec.mResourceId);
    Animation_Init(animRec.mFrameTableOffset, animRec.mMaxW, animRec.mMaxH, ppRes, 1, 1);
    mAnim.mAnimFlags.Set(AnimFlags::eBit8_Loop);

    if (animRec.mPalOverride != PalId::Default)
    {
        const PalRecord& palRec = PalRec(animRec.mPalOverride);

        if (!ResourceManager::GetLoadedResource_49C2A0(ResourceManager::Resource_Palt, palRec.mResourceId, 0, 0))
        {
            ResourceManager::LoadResourceFile_49C170(palRec.mBanName, nullptr);
        }
        else
        {
            LOG_WARNING("Pal resource already loaded - BAN/BND name not verified by test animation");
        }

        u8** ppPal = Add_Resource(ResourceManager::Resource_Palt, palRec.mResourceId);
        if (ppPal)
        {
            mAnim.Load_Pal(ppPal, 0);
        }
    }
}

TestAnimation::TestAnimation()
    : BaseAnimatedWithPhysicsGameObject(1)
{
    mLoaded = false;

    SetType(AETypes::eNone_0);

    mApplyShadows &= ~1u;

    mGameObjectFlags.Set(BaseGameObject::eDrawable_Bit4);
    mGameObjectFlags.Set(BaseGameObject::eSurviveDeathReset_Bit9);
}

void TestAnimation::SyncToAbePos()
{
    mXPos = sActiveHero->mXPos + FP_FromInteger(30);
    mYPos = sActiveHero->mYPos - FP_FromInteger(30);

    mAnim.mRenderLayer = sActiveHero->mAnim.mRenderLayer;
}

void TestAnimation::VUpdate()
{
    mPathNumber = gMap.mCurrentPath;
    mLvlNumber = gMap.mCurrentLevel;

    if (mLoaded)
    {
        SyncToAbePos();
    }
    else
    {
        DelayLoad();
    }
}

void TestAnimation::VScreenChanged()
{
    // Keep alive
}

void TestAnimation::VRender(PrimHeader** ppOt)
{
    if (mLoaded)
    {
        BaseAnimatedWithPhysicsGameObject::VRender(ppOt);
    }
}
