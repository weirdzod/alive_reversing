#include "stdafx_ao.h"
#include "Function.hpp"
#include "BoomMachine.hpp"
#include "../AliveLibAE/stdlib.hpp"
#include "../relive_lib/Events.hpp"
#include "Sfx.hpp"
#include "ThrowableArray.hpp"
#include "Game.hpp"
#include "Grenade.hpp"
#include "Grid.hpp"
#include "../relive_lib/ObjectIds.hpp"
#include "../AliveLibAE/FixedPoint.hpp"

namespace AO {

class GrenadeMachineNozzle final : public BaseAnimatedWithPhysicsGameObject
{
public:
    GrenadeMachineNozzle()
        : BaseAnimatedWithPhysicsGameObject(0)
    {
        LoadAnimations();
    }

    void LoadAnimations()
    {
        mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::BoomMachine_Nozzle_DropGrenade));
        mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::BoomMachine_Nozzle_Idle));
    }

	void DropGrenadeAnimation()
	{
        if (mState == BoomMachineStates::eInactive_0)
        {
            mState = BoomMachineStates::eDropGrenadeAnimation_2;
            mTimer = sGnFrame + 10;
        }
	}

	void AlreadyUsed()
	{
	    if (mState == BoomMachineStates::eInactive_0)
        {
            mState = BoomMachineStates::eAlreadyUsed_1;
            mTimer = sGnFrame + 10;
        }
	}

    virtual void VUpdate() override
    {
        switch (mState)
        {
            case BoomMachineStates::eAlreadyUsed_1:
                if (static_cast<s32>(sGnFrame) > mTimer)
                {
                    SFX_Play_Pitch(relive::SoundEffects::ZPop, 60, -1800);
                    mState = BoomMachineStates::eInactive_0;
                }
                break;

            case BoomMachineStates::eDropGrenadeAnimation_2:
                if (static_cast<s32>(sGnFrame) > mTimer)
                {
                    mState = BoomMachineStates::eDropGrenade_3;
                    GetAnimation().Set_Animation_Data(GetAnimRes(AnimId::BoomMachine_Nozzle_DropGrenade));
                }
                break;

            case BoomMachineStates::eDropGrenade_3:
                if (GetAnimation().mFlags.Get(AnimFlags::eIsLastFrame))
                {
                    SFX_Play_Pitch(relive::SoundEffects::PickupItem, 127, -900);
                    if (!gThrowableArray)
                    {
                        gThrowableArray = relive_new ThrowableArray();
                    }

                    gThrowableArray->Add(mGrenadeCount);

                    FP directedScale = {};
                    if (GetAnimation().mFlags.Get(AnimFlags::eFlipX))
                    {
                        directedScale = -GetSpriteScale();
                    }
                    else
                    {
                        directedScale = GetSpriteScale();
                    }
                    auto pGrenade = relive_new Grenade(
                        mXPos + (FP_FromInteger(6) * directedScale),
                        mYPos + (-FP_FromInteger(6) * directedScale),
                        mGrenadeCount);
                    if (pGrenade)
                    {
                        pGrenade->VThrow(GetAnimation().mFlags.Get(AnimFlags::eFlipX) ? FP_FromDouble(-0.75) : FP_FromDouble(0.75), FP_FromInteger(3));
                    }

                    GetAnimation().Set_Animation_Data(GetAnimRes(AnimId::BoomMachine_Nozzle_Idle));
                    mState = BoomMachineStates::eInactive_0;
                }
                break;
            default:
                return;
        }
    }

    virtual void VScreenChanged() override
    {
        // Empty
    }

    BoomMachineStates mState = BoomMachineStates::eInactive_0;
    s32 mTimer = 0;
    s16 mGrenadeCount = 0;
};

void BoomMachine::LoadAnimations()
{
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::BoomMachine_Button_Off));
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::BoomMachine_Button_On));
}

BoomMachine::BoomMachine(relive::Path_BoomMachine* pTlv, const Guid& tlvId)
    : BaseAnimatedWithPhysicsGameObject(0)
{
    SetType(ReliveTypes::eBoomMachine);

    LoadAnimations();

    Animation_Init(GetAnimRes(AnimId::BoomMachine_Button_On));

    mVisualFlags.Clear(VisualFlags::eApplyShadowZoneColour);
    mTlvId = tlvId;
    GetAnimation().SetRenderMode(TPageAbr::eBlend_1);

    if (pTlv->mScale == relive::reliveScale::eHalf)
    {
        SetSpriteScale(FP_FromDouble(0.5));
    }
    else
    {
        SetSpriteScale(FP_FromInteger(1));
    }

    mXPos = FP_FromInteger(pTlv->mTopLeftX) + (ScaleToGridSize(GetSpriteScale()) / FP_FromInteger(2));
    mYPos = FP_FromInteger(pTlv->mTopLeftY);

    auto pNozzle = relive_new GrenadeMachineNozzle();
    if (pNozzle)
    {
        FP directedScale = GetSpriteScale();
        if (pTlv->mNozzleSide == relive::Path_BoomMachine::NozzleSide::eLeft)
        {
            directedScale = -directedScale;
        }

        pNozzle->Animation_Init(pNozzle->GetAnimRes(AnimId::BoomMachine_Nozzle_Idle));

        pNozzle->GetAnimation().mFlags.Clear(AnimFlags::eSemiTrans);
        pNozzle->SetSpriteScale(GetSpriteScale());
        pNozzle->mVisualFlags.Clear(VisualFlags::eApplyShadowZoneColour);
        pNozzle->mState = BoomMachineStates::eInactive_0;
        pNozzle->mXPos = mXPos + (directedScale * FP_FromInteger(30));
        pNozzle->mYPos = mYPos + (GetSpriteScale() * FP_FromInteger(-30));
        pNozzle->mGrenadeCount = static_cast<s16>(pTlv->mGrenadeAmount);
        pNozzle->GetAnimation().mFlags.Set(AnimFlags::eFlipX, pTlv->mNozzleSide == relive::Path_BoomMachine::NozzleSide::eLeft);

        mNozzleId = pNozzle->mBaseGameObjectId;
    }

    if (gThrowableArray && gThrowableArray->mCount)
    {
        mIsButtonOn = true;
        GetAnimation().Set_Animation_Data(GetAnimRes(AnimId::BoomMachine_Button_On));
    }
    else
    {
        mIsButtonOn = false;
    }

    mYOffset = 0;
}

void BoomMachine::VUpdate()
{
    if (EventGet(kEventDeathReset))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    if (!mIsButtonOn)
    {
        if (!gThrowableArray || gThrowableArray->mCount == 0)
        {
            mIsButtonOn = true;
            GetAnimation().Set_Animation_Data(GetAnimRes(AnimId::BoomMachine_Button_On));
        }
    }
    else if (mIsButtonOn)
    {
        if (gThrowableArray && gThrowableArray->mCount > 0)
        {
            mIsButtonOn = false;
            GetAnimation().Set_Animation_Data(GetAnimRes(AnimId::BoomMachine_Button_Off));
        }

        if (GetAnimation().GetCurrentFrame() == 3)
        {
            SFX_Play_Pitch(relive::SoundEffects::RedTick, 25, -1200);
        }
    }
}

void BoomMachine::VScreenChanged()
{
    mBaseGameObjectFlags.Set(BaseGameObject::eDead);
}

bool BoomMachine::VIsButtonOn()
{
    return mIsButtonOn == true;
}

void BoomMachine::VHandleButton()
{
    auto pNozzle = static_cast<GrenadeMachineNozzle*>(sObjectIds.Find_Impl(mNozzleId));
    if (pNozzle)
    {
        if (VIsButtonOn())
        {
            pNozzle->DropGrenadeAnimation();
        }
        else
        {
            pNozzle->AlreadyUsed();
        }
    }
}

BoomMachine::~BoomMachine()
{
    BaseGameObject* pObj = sObjectIds.Find_Impl(mNozzleId);
    if (pObj)
    {
        pObj->mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
    Path::TLV_Reset(mTlvId, -1, 0, 0);
}
} // namespace AO
