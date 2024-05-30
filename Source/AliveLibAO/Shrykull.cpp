#include "stdafx_ao.h"
#include "../relive_lib/Function.hpp"
#include "Shrykull.hpp"
#include "Map.hpp"
#include "../AliveLibAE/stdlib.hpp"
#include "Abe.hpp"
#include "../relive_lib/GameObjects/ZapLine.hpp"
#include "../relive_lib/Shadow.hpp"
#include "../relive_lib/ObjectIds.hpp"
#include "Sfx.hpp"
#include "../relive_lib/GameObjects/PossessionFlicker.hpp"
#include "../relive_lib/GameObjects/ParticleBurst.hpp"
#include "../relive_lib/GameObjects/Flash.hpp"
#include "Electrocute.hpp"
#include "AbilityRing.hpp"
#include "Math.hpp"
#include "Game.hpp"

namespace AO {

Shrykull::~Shrykull()
{
    BaseGameObject* pZapLine = sObjectIds.Find_Impl(mZapLineId);
    if (pZapLine)
    {
        pZapLine->SetDead(true);
        mZapLineId = Guid{};
    }

    mZapTargetId = Guid{};
}

void Shrykull::VScreenChanged()
{
    if (gMap.LevelChanged() || gMap.PathChanged())
    {
        SetDead(true);
    }
}

void Shrykull::LoadAnimations()
{
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::ShrykullStart));
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::ShrykullTransform));
    mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::ShrykullDetransform));
}

Shrykull::Shrykull()
    : BaseAliveGameObject(0)
{
    SetType(ReliveTypes::eShrykull);

    LoadAnimations();

    SetCanExplode(true);

    Animation_Init(GetAnimRes(AnimId::ShrykullStart));

    mXPos = gAbe->mXPos;
    mYPos = gAbe->mYPos;
    SetSpriteScale(gAbe->GetSpriteScale());
    SetScale(gAbe->GetScale());

    mState = State::eTransform_0;

    GetAnimation().SetFlipX(gAbe->GetAnimation().GetFlipX());

    CreateShadow();

    mResetRingTimer = false;
}

void Shrykull::VOnThrowableHit(BaseGameObject*)
{
    // Empty
}

bool Shrykull::CanKill(BaseAnimatedWithPhysicsGameObject* pObj)
{
    return (pObj->Type() == ReliveTypes::eTimedMine
            || pObj->Type() == ReliveTypes::eMine
            || pObj->Type() == ReliveTypes::eUXB
            || pObj->Type() == ReliveTypes::eSlig
            || pObj->Type() == ReliveTypes::eSlog
            || pObj->Type() == ReliveTypes::eBackgroundGlukkon
            || pObj->Type() == ReliveTypes::eSecurityClaw
            || pObj->Type() == ReliveTypes::eSecurityOrb)
        && pObj->GetAnimation().GetRender()
        && !pObj->GetDead()
        && gMap.Is_Point_In_Current_Camera(
            pObj->mCurrentLevel,
            pObj->mCurrentPath,
            pObj->mXPos,
            pObj->mYPos,
            0);
}

bool Shrykull::CanElectrocute(BaseGameObject* pObj) const
{
    switch (pObj->Type())
    {
        case ReliveTypes::eElum:
        case ReliveTypes::eBackgroundGlukkon:
        case ReliveTypes::eRingOrLiftMud:
        case ReliveTypes::eParamite:
        case ReliveTypes::eMudokon:
        case ReliveTypes::eScrab:
        case ReliveTypes::eSlig:
        case ReliveTypes::eSlog:
            return true;
        default:
            return false;
    }
}

void Shrykull::VUpdate()
{
    auto pExistingBeingZappedObj = static_cast<BaseAliveGameObject*>(sObjectIds.Find_Impl(mZapTargetId));
    auto pExistingZapLine = static_cast<ZapLine*>(sObjectIds.Find_Impl(mZapLineId));

    switch (mState)
    {
        case State::eTransform_0:
            if (GetAnimation().GetCurrentFrame() == 0)
            {
                SFX_Play_Pitch(relive::SoundEffects::Shrykull1, 127, -2000);
                SFX_Play_Pitch(relive::SoundEffects::Shrykull2, 127, 0);
                SfxPlayMono(relive::SoundEffects::IngameTransition, 127);
            }

            if (GetAnimation().GetForwardLoopCompleted())
            {
                GetAnimation().Set_Animation_Data(GetAnimRes(AnimId::ShrykullTransform));
                mState = State::eZapTargets_1;
            }
            break;

        case State::eZapTargets_1:
            if (GetAnimation().GetCurrentFrame() == 0)
            {
                if (Math_NextRandom() >= 128u)
                {
                    SFX_Play_Pitch(relive::SoundEffects::Shrykull2, 127, 0);
                }
                else
                {
                    SFX_Play_Pitch(relive::SoundEffects::Shrykull1, 127, 0);
                }
            }

            for (s32 i = 0; i < gBaseAliveGameObjects->Size(); i++)
            {
                BaseAliveGameObject* pObj = gBaseAliveGameObjects->ItemAt(i);
                if (!pObj)
                {
                    break;
                }

                if (CanKill(pObj) && !pObj->GetZappedByShrykull())
                {
                    mZapTargetId = pObj->mBaseGameObjectId;

                    const PSX_RECT objRect = pObj->VGetBoundingRect();
                    const PSX_RECT ourRect = VGetBoundingRect();
                    
                    if (pExistingZapLine)
                    {
                        pExistingZapLine->CalculateSourceAndDestinationPositions(
                            FP_FromInteger((ourRect.x + ourRect.w) / 2),
                            FP_FromInteger((ourRect.y + ourRect.h) / 2),
                            FP_FromInteger((objRect.x + objRect.w) / 2),
                            FP_FromInteger((objRect.y + objRect.h) / 2));
                    }
                    else
                    {
                        auto pZapLine = relive_new ZapLine(
                            FP_FromInteger((ourRect.x + ourRect.w) / 2),
                            FP_FromInteger((ourRect.y + ourRect.h) / 2),
                            FP_FromInteger((objRect.x + objRect.w) / 2),
                            FP_FromInteger((objRect.y + objRect.h) / 2),
                            0, ZapLineType::eThin_1,
                            Layer::eLayer_ZapLinesElumMuds_28);
                        if (pZapLine)
                        {
                            mZapLineId = pZapLine->mBaseGameObjectId;
                        }
                    }

                    mCanElectrocute = CanElectrocute(pObj);
                    if (mCanElectrocute)
                    {
                        relive_new Electrocute(pObj, 0);
                        mFlashTimer = MakeTimer(3);

                        if (pObj->Type() == ReliveTypes::eBackgroundGlukkon)
                        {
                            pObj->VTakeDamage(this);
                        }
                    }

                    relive_new PossessionFlicker(pObj, 8, 255, 255, 255);

                    relive_new AbilityRing(
                        FP_FromInteger((objRect.x + objRect.w) / 2),
                        FP_FromInteger((objRect.y + objRect.h) / 2),
                        RingTypes::eShrykull_Pulse_Large_5,
                        FP_FromInteger(1));

                    relive_new PossessionFlicker(this, 8, 255, 255, 255);

                    relive_new AbilityRing(
                        FP_FromInteger((ourRect.x + ourRect.w) / 2),
                        FP_FromInteger((ourRect.y + ourRect.h) / 2),
                        RingTypes::eShrykull_Pulse_Large_5,
                        FP_FromInteger(1));

                    pObj->SetZappedByShrykull(true);

                    SFX_Play_Pitch(relive::SoundEffects::Respawn, 100, 2000);
                    SfxPlayMono(relive::SoundEffects::Zap1, 0);

                    mState = State::eKillTargets_4;
                    mZapIntervalTimer = MakeTimer(12);
                    mResetRingTimer = true;
                    return;
                }
            }

            if (pExistingZapLine)
            {
                pExistingZapLine->SetDead(true);
                mZapLineId = Guid{};
            }
            mState = State::eDetransform_2;
            break;

        case State::eDetransform_2:
            if (GetAnimation().GetIsLastFrame())
            {
                GetAnimation().Set_Animation_Data(GetAnimRes(AnimId::ShrykullDetransform));
                mState = State::eFinish_3;
            }
            break;

        case State::eFinish_3:
            if (GetAnimation().GetCurrentFrame() == 0)
            {
                SFX_Play_Pitch(relive::SoundEffects::Shrykull1, 127, -2000);
                SFX_Play_Pitch(relive::SoundEffects::Shrykull2, 127, 0);
            }

            if (GetAnimation().GetForwardLoopCompleted())
            {
                gAbe->ExitShrykull(mResetRingTimer);
                SetDead(true);
            }
            break;

        case State::eKillTargets_4:
            if (GetAnimation().GetCurrentFrame() == 0)
            {
                if (Math_NextRandom() >= 128u)
                {
                    SFX_Play_Pitch(relive::SoundEffects::Shrykull2, 127, 0);
                }
                else
                {
                    SFX_Play_Pitch(relive::SoundEffects::Shrykull1, 127, 0);
                }
            }

            if (pExistingBeingZappedObj)
            {
                if (pExistingBeingZappedObj->GetDead())
                {
                    mZapTargetId = Guid{};
                }
                else
                {
                    const PSX_RECT zapRect = pExistingBeingZappedObj->VGetBoundingRect();
                    const PSX_RECT ourRect = VGetBoundingRect();
                    if (static_cast<s32>(sGnFrame) == mFlashTimer)
                    {
                        relive_new ParticleBurst(
                            FP_FromInteger((zapRect.x + zapRect.w) / 2),
                            FP_FromInteger((zapRect.y + zapRect.h) / 2),
                            20,
                            GetSpriteScale(),
                            BurstType::eBigPurpleSparks);

                       relive_new Flash(Layer::eLayer_Above_FG1_39, 255, 255, 255);
                    }
                    pExistingZapLine->CalculateSourceAndDestinationPositions(
                        FP_FromInteger((ourRect.x + ourRect.w) / 2),
                        FP_FromInteger((ourRect.y + ourRect.h) / 2),
                        FP_FromInteger((zapRect.x + zapRect.w) / 2),
                        FP_FromInteger((zapRect.y + zapRect.h) / 2));
                }
            }

            if (static_cast<s32>(sGnFrame) > mZapIntervalTimer)
            {
                mState = State::eZapTargets_1;

                if (pExistingBeingZappedObj)
                {
                    if (!mCanElectrocute)
                    {
                        pExistingBeingZappedObj->VTakeDamage(this);
                    }

                    mZapTargetId = Guid{};
                }
            }
            break;

        default:
            return;
    }
}

} // namespace AO
