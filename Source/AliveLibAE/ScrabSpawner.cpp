#include "stdafx.h"
#include "ScrabSpawner.hpp"
#include "Function.hpp"
#include "stdlib.hpp"
#include "ObjectIds.hpp"
#include "Events.hpp"
#include "SwitchStates.hpp"
#include "Sfx.hpp"

ScrabSpawner::ScrabSpawner(Path_ScrabSpawner* pTlv, s32 tlvInfo)
    : BaseGameObject(TRUE, 0)
{
    field_20_tlvInfo = tlvInfo;
    SetType(ReliveTypes::eScrabSpawner);

    field_28_tlv_data.mTlvFlags = pTlv->mTlvFlags;
    field_28_tlv_data.mTlvState = pTlv->mTlvState;
    field_28_tlv_data.mLength = pTlv->mLength;
    field_28_tlv_data.mTlvType32 = pTlv->mTlvType32;
    field_28_tlv_data.mTopLeft = pTlv->mTopLeft;
    field_28_tlv_data.mBottomRight = pTlv->mBottomRight;

    field_24_spawner_switch_id = pTlv->field_2C_spawner_switch_id;
    field_26_spawn_direction = pTlv->field_2E_spawn_direction;

    field_38_state = ScrabSpawnerStates::eInactive_0;

    field_3C_spawned_scrab_id = -1;
    field_40_bFindSpawnedScrab = 0;
}

s32 ScrabSpawner::CreateFromSaveState(const u8* pBuffer)
{
    const auto pState = reinterpret_cast<const ScrabSpawner_State*>(pBuffer);
    auto pTlv = static_cast<Path_ScrabSpawner*>(sPathInfo->TLV_From_Offset_Lvl_Cam(pState->field_4_tlvInfo));
    auto pScrabSpawner = relive_new ScrabSpawner(pTlv, pState->field_4_tlvInfo);
    if (pScrabSpawner)
    {
        pScrabSpawner->field_38_state = pState->field_8_state;
        pScrabSpawner->field_3C_spawned_scrab_id = pState->field_C_spawned_scrab_id;
        pScrabSpawner->field_40_bFindSpawnedScrab = 1;
    }

    return sizeof(ScrabSpawner_State);
}

ScrabSpawner::~ScrabSpawner()
{
    Path::TLV_Reset(field_20_tlvInfo, -1, 0, 0);
}

s32 ScrabSpawner::VGetSaveState(u8* pSaveBuffer)
{
    auto pSaveState = reinterpret_cast<ScrabSpawner_State*>(pSaveBuffer);

    pSaveState->field_0_type = AETypes::eScrabSpawner_113;
    pSaveState->field_4_tlvInfo = field_20_tlvInfo;
    pSaveState->field_8_state = field_38_state;
    pSaveState->field_C_spawned_scrab_id = -1;

    if (field_3C_spawned_scrab_id != -1)
    {
        BaseGameObject* pSpawnedScrab = sObjectIds.Find_Impl(field_3C_spawned_scrab_id);
        if (pSpawnedScrab)
        {
            pSaveState->field_C_spawned_scrab_id = pSpawnedScrab->mBaseGameObjectTlvInfo;
        }
    }
    return sizeof(ScrabSpawner_State);
}

void ScrabSpawner::VUpdate()
{
    if (field_40_bFindSpawnedScrab)
    {
        field_40_bFindSpawnedScrab = FALSE;
        if (field_3C_spawned_scrab_id != -1)
        {
            for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
            {
                BaseGameObject* pObj = gBaseGameObjects->ItemAt(i);
                if (!pObj)
                {
                    break;
                }

                if (pObj->Type() == ReliveTypes::eScrab && pObj->mBaseGameObjectTlvInfo == field_3C_spawned_scrab_id)
                {
                    // Redundant ??
                    field_3C_spawned_scrab_id = pObj->field_8_object_id;
                    break;
                }
            }
        }
    }

    auto pExistingSpawnedScrab = static_cast<Scrab*>(sObjectIds.Find_Impl(field_3C_spawned_scrab_id));
    if (!EventGet(kEventDeathReset) && !(mBaseGameObjectFlags.Get(BaseGameObject::eDead)))
    {
        if (field_38_state == ScrabSpawnerStates::eScrabSpawned_1)
        {
            if (!pExistingSpawnedScrab || pExistingSpawnedScrab->mBaseGameObjectFlags.Get(BaseGameObject::eDead))
            {
                SwitchStates_Do_Operation(field_24_spawner_switch_id, SwitchOp::eSetFalse_1);
                field_3C_spawned_scrab_id = -1;
                field_38_state = ScrabSpawnerStates::eInactive_0;
            }
        }
        else if (field_38_state == ScrabSpawnerStates::eInactive_0)
        {
            if (SwitchStates_Get(field_24_spawner_switch_id))
            {
                auto pTlv = static_cast<Path_ScrabSpawner*>(sPathInfo->TLV_Get_At_4DB4B0(
                    field_28_tlv_data.mTopLeft.x,
                    field_28_tlv_data.mTopLeft.y,
                    field_28_tlv_data.mTopLeft.x,
                    field_28_tlv_data.mTopLeft.y,
                    TlvTypes::ScrabSpawner_102));

                if (pTlv)
                {
                    auto pNewScrab = relive_new Scrab(pTlv, field_20_tlvInfo, field_26_spawn_direction);
                    if (pNewScrab)
                    {
                        SfxPlayMono(SoundEffect::ScrabSpawn_111, 0);

                        field_38_state = ScrabSpawnerStates::eScrabSpawned_1;
                        field_3C_spawned_scrab_id = pNewScrab->field_8_object_id;
                    }
                }
                else
                {
                    mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                }
            }
        }
    }
}
