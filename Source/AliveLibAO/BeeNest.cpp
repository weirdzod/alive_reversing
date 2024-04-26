#include "stdafx_ao.h"
#include "../relive_lib/Function.hpp"
#include "BeeNest.hpp"
#include "BeeSwarm.hpp"
#include "../AliveLibAE/stdlib.hpp"
#include "../relive_lib/SwitchStates.hpp"
#include "Abe.hpp"
#include "Path.hpp"
#include "../relive_lib/ObjectIds.hpp"

namespace AO {

BeeNest::BeeNest(relive::Path_BeeNest* pTlv, const Guid& tlvId)
    : BaseGameObject(true, 0)
{
    SetType(ReliveTypes::eBeeNest);

    mTlvInfo = tlvId;

    mSwarmSize = pTlv->mSwarmSize;

    mSwitchId = pTlv->mSwitchId;

    mBeeSwarmX = FP_FromInteger(pTlv->mTopLeftX);
    mBeeSwarmY = FP_FromInteger(pTlv->mTopLeftY);

    mTotalChaseTime = pTlv->mChaseTime;
    mSpeed = FP_FromRaw(pTlv->mSpeed << 8);

    mState = BeeNestStates::eWaitForTrigger_0;

    // The "idle" swarm that hovers around the nest
    relive_new BeeSwarm(mBeeSwarmX, mBeeSwarmY, FP_FromInteger(0), pTlv->mBeesAmount, 0);
}

BeeNest::~BeeNest()
{

}

void BeeNest::VScreenChanged()
{
    if (gMap.LevelChanged() || gMap.PathChanged() || !mBeeSwarm.IsValid())
    {
        Path::TLV_Reset(mTlvInfo);
        mBeeSwarm = Guid{};
        SetDead(true);
    }
}

void BeeNest::VUpdate()
{
    switch (mState)
    {
        case BeeNestStates::eWaitForTrigger_0:
            if (SwitchStates_Get(mSwitchId))
            {
                auto pBeeSwarm = relive_new BeeSwarm(
                    mBeeSwarmX,
                    mBeeSwarmY,
                    mSpeed,
                    mSwarmSize,
                    mTotalChaseTime);

                mBeeSwarm = pBeeSwarm->mBaseGameObjectId;
                if (pBeeSwarm)
                {
                    pBeeSwarm->Chase(gAbe);
                    mState = BeeNestStates::eResetIfDead_1;
                }
            }
            break;

        case BeeNestStates::eResetIfDead_1:
        {
            auto pBeeSwarm = sObjectIds.Find(mBeeSwarm, ReliveTypes::eBeeSwarm);
            if (!pBeeSwarm || pBeeSwarm->GetDead())
            {
                mState = BeeNestStates::eWaitForTrigger_0;
                mBeeSwarm = Guid{};
                SwitchStates_Set(mSwitchId, 0);
            }
            break;
        }

        default:
            break;
    }
}

} // namespace AO
