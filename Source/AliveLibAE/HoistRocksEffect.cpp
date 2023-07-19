#include "stdafx.h"
#include "HoistRocksEffect.hpp"
#include "Game.hpp"
#include "stdlib.hpp"
#include "Map.hpp"
#include "../relive_lib/GameObjects/ScreenManager.hpp"
#include "Path.hpp"
#include "../relive_lib/GameObjects/BaseAnimatedWithPhysicsGameObject.hpp"
#include "../relive_lib/Collisions.hpp"
#include "Math.hpp"
#include "../relive_lib/FixedPoint.hpp"

const static AnimId HoistRocksAnimIdTable[4] = 
{AnimId::HoistRock1,
AnimId::HoistRock2,
AnimId::HoistRock3,
AnimId::HoistRock1};

const static s16 sRndValues[12] = {5, 0, 10, 0, 30, 0, 5, 0, 0, 0, 0, 0};

void HoistRocksEffect::LoadAnimations()
{
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::HoistRock1));
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::HoistRock2));
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::HoistRock3));
}

HoistRocksEffect::HoistRocksEffect(relive::Path_Hoist* pTlv, const Guid& tlvId)
    : BaseGameObject(true, 0)
{
    mTlvId = tlvId;

    LoadAnimations();

    mTlvXPos = (pTlv->mTopLeftX + pTlv->mBottomRightX) / 2;
    mTlvYPos = pTlv->mTopLeftY;
    mTimer = 0;

    if (pTlv->mScale == relive::reliveScale::eHalf)
    {
        mSpriteScale = FP_FromDouble(0.5);
    }
    else
    {
        mSpriteScale = FP_FromDouble(1.0);
    }

    if (gObjListDrawables->Push_Back(this))
    {
        SetDrawable(true);
    }

    for (HoistRockParticle& particle : mRocks)
    {
        particle.mAnim.Init(GetAnimRes(AnimId::HoistRock1), this);

        particle.mAnim.SetRGB(255, 255, 255);

        if (pTlv->mScale == relive::reliveScale::eHalf)
        {
            particle.mAnim.SetRenderLayer(Layer::eLayer_BeforeShadow_Half_6);
        }
        else
        {
            particle.mAnim.SetRenderLayer(Layer::eLayer_BeforeShadow_25);
        }

        particle.mAnim.SetBlendMode(relive::TBlendModes::eBlend_0);
        particle.mAnim.SetBlending(false);
        particle.mAnim.SetSemiTrans(true);
        particle.mAnim.SetSpriteScale(mSpriteScale);

        particle.mState = 0;
    }
}

HoistRocksEffect::~HoistRocksEffect()
{
    gObjListDrawables->Remove_Item(this);

    for (HoistRockParticle& particle : mRocks)
    {
        particle.mAnim.VCleanUp();
    }

    Path::TLV_Reset(mTlvId, -1, 0, 0);
}

void HoistRocksEffect::VUpdate()
{
    if (mTimer <= static_cast<s32>(sGnFrame))
    {
        s32 idx = 0;
        while (mRocks[idx].mState != 0)
        {
            if (++idx >= 4)
            {
                break;
            }
        }

        if (idx < 4)
        {
            s32 randomXScaled = 0;
            if (mSpriteScale == FP_FromDouble(1.0))
            {
                randomXScaled = Math_RandomRange(-8, 8);
            }
            else
            {
                randomXScaled = Math_RandomRange(-4, 4);
            }

            mRocks[idx].mXPos = FP_FromInteger(mTlvXPos + randomXScaled);
            mRocks[idx].mYPos = FP_FromInteger(mTlvYPos + Math_RandomRange(-4, 4));

            mRocks[idx].mVelY = FP_FromInteger(0);
            mRocks[idx].mState = 1;

            const s32 randomAnimAndUpdate = 2 * Math_RandomRange(0, 3);
            auto rndAnimIdx = randomAnimAndUpdate / 2;
            if (rndAnimIdx >= mLoadedAnims.size())
            {
                rndAnimIdx = 0;
            }
            mRocks[idx].mAnim.Set_Animation_Data(mLoadedAnims[rndAnimIdx]);
            mTimer = sGnFrame + Math_RandomRange(sRndValues[randomAnimAndUpdate], 2 * sRndValues[randomAnimAndUpdate]);
        }
    }

    for (HoistRockParticle& particle : mRocks)
    {
        if (particle.mState)
        {
            if (particle.mVelY >= FP_FromInteger(10))
            {
                if (!gMap.Is_Point_In_Current_Camera(
                        gMap.mCurrentLevel,
                        gMap.mCurrentPath,
                        particle.mXPos,
                        particle.mYPos,
                        0))
                {
                    particle.mState = 0;
                }
            }
            else
            {
                particle.mVelY += FP_FromDouble(0.6);
            }

            particle.mYPos += particle.mVelY;

            if (particle.mState == 1)
            {
                PathLine* pLine = nullptr;
                FP hitX = {};
                FP hitY = {};
                if (gCollisions->Raycast(
                        particle.mXPos,
                        particle.mYPos - particle.mVelY,
                        particle.mXPos,
                        particle.mYPos,
                        &pLine,
                        &hitX,
                        &hitY,
                        mSpriteScale > FP_FromDouble(0.5) ? kFgFloor : kBgFloor))
                {
                    particle.mYPos = hitY;
                    particle.mVelY = particle.mVelY * FP_FromDouble(-0.3);
                    particle.mState = 2;
                }
            }
        }
    }
}

void HoistRocksEffect::VRender(OrderingTable& ot)
{
    for (HoistRockParticle& particle : mRocks)
    {
        if (particle.mState)
        {
            particle.mAnim.VRender(
                FP_GetExponent(particle.mXPos - gScreenManager->CamXPos()),
                FP_GetExponent(particle.mYPos - gScreenManager->CamYPos()),
                ot,
                0,
                0);
        }
    }
}

void HoistRocksEffect::VScreenChanged()
{
    SetDead(true);
}
