#include "stdafx.h"
#include "Alarm.hpp"
#include "Function.hpp"
#include "Path.hpp"
#include "Game.hpp"
#include "SwitchStates.hpp"
#include "stdlib.hpp"
#include "Events.hpp"
#include "Sfx.hpp"

ALIVE_VAR(1, 0x5c1bb4, s16, alarmInstanceCount_5C1BB4, 0);
ALIVE_VAR(1, 0x550d70, s32, sAlarmObjId_550D70, -1);

Alarm::Alarm(Path_Alarm* pTlv, s32 tlvInfo)
    : EffectBase(Layer::eLayer_Above_FG1_39, TPageAbr::eBlend_3)
{
    field_84_tlvOffsetLevelPathCamId = tlvInfo;

    SetType(AETypes::eAlarm_1);

    field_78_r_value = 0;
    field_90_state = States::eWaitForSwitchEnable_0;

    // This won't count as an alarm instance till this id is enabled
    field_88_switch_id = pTlv->field_10_switch_id;

    field_6E_r = 0;
    field_70_g = 0;
    field_72_b = 0;

    field_8A_duration = pTlv->field_12_duration;
}

Alarm::Alarm(s32 durationOffset, s32 switchId, s32 timerOffset, Layer layer)
    : EffectBase(layer, TPageAbr::eBlend_3)
{
    SetType(AETypes::eAlarm_1);

    field_78_r_value = 0;
    field_90_state = States::eAfterConstructed_1;
    field_84_tlvOffsetLevelPathCamId = 0xFFFF;
    field_7C_15_timer = sGnFrame_5C1B84 + timerOffset;
    field_80_duration_timer = field_7C_15_timer + durationOffset;
    field_88_switch_id = static_cast<s16>(switchId);

    alarmInstanceCount_5C1BB4++;

    if (alarmInstanceCount_5C1BB4 > 1)
    {
        // More than one instance, kill self
        mFlags.Set(BaseGameObject::eDead);
    }
    else
    {
        sAlarmObjId_550D70 = field_8_object_id;
    }

    field_6E_r = 0;
    field_70_g = 0;
    field_72_b = 0;
}

Alarm::~Alarm()
{
    if (field_90_state != States::eWaitForSwitchEnable_0)
    {
        alarmInstanceCount_5C1BB4--;
    }

    if (sAlarmObjId_550D70 == field_8_object_id)
    {
        sAlarmObjId_550D70 = -1;
    }

    if (field_84_tlvOffsetLevelPathCamId == 0xFFFF)
    {
        if (field_88_switch_id)
        {
            SwitchStates_Set_465FF0(field_88_switch_id, 0);
        }
    }
    else
    {
        Path::TLV_Reset_4DB8E0(field_84_tlvOffsetLevelPathCamId, -1, 0, 0);
    }
}

void Alarm::VRender(PrimHeader** ppOt)
{
    if (sNum_CamSwappers_5C1B66 == 0)
    {
        EffectBase::VRender(ppOt);
    }
}

void Alarm::VUpdate()
{
    if (field_90_state != States::eWaitForSwitchEnable_0)
    {
        Event_Broadcast_422BC0(kEventAlarm, this);
        if (static_cast<s32>(sGnFrame_5C1B84) > field_80_duration_timer)
        {
            mFlags.Set(BaseGameObject::eDead);
            return;
        }
    }

    switch (field_90_state)
    {
        case States::eWaitForSwitchEnable_0:
            if (Event_Get_422C00(kEventDeathReset))
            {
                mFlags.Set(BaseGameObject::eDead);
            }

            if (!SwitchStates_Get_466020(field_88_switch_id))
            {
                field_6E_r = field_78_r_value;
                return;
            }

            alarmInstanceCount_5C1BB4++;
            if (alarmInstanceCount_5C1BB4 > 1)
            {
                mFlags.Set(BaseGameObject::eDead);
            }
            else
            {
                sAlarmObjId_550D70 = field_8_object_id;
            }

            field_90_state = States::eEnabling_2;
            SFX_Play_46FA90(SoundEffect::SecurityDoorDeny_38, 0);
            field_80_duration_timer = sGnFrame_5C1B84 + field_8A_duration;
            field_6E_r = field_78_r_value;
            break;

        case States::eAfterConstructed_1: // When not created by a map TLV
            if (Event_Get_422C00(kEventHeroDying))
            {
                mFlags.Set(BaseGameObject::eDead);
            }
            else
            {
                if (static_cast<s32>(sGnFrame_5C1B84) <= field_7C_15_timer)
                {
                    field_6E_r = field_78_r_value;
                    return;
                }

                field_90_state = States::eEnabling_2;
                SFX_Play_46FA90(SoundEffect::SecurityDoorDeny_38, 0);

                if (!field_88_switch_id)
                {
                    field_6E_r = field_78_r_value;
                    return;
                }

                SwitchStates_Set_465FF0(field_88_switch_id, 1);
                field_6E_r = field_78_r_value;
            }
            break;

        case States::eEnabling_2:
            field_78_r_value += 25;

            if (field_78_r_value < 100)
            {
                field_6E_r = field_78_r_value;
                return;
            }

            field_78_r_value = 100;
            field_90_state = States::eOnFlash_3;
            field_7C_15_timer = sGnFrame_5C1B84 + 15;
            SFX_Play_46FA90(SoundEffect::SecurityDoorDeny_38, 0);
            field_6E_r = field_78_r_value;
            break;

        case States::eOnFlash_3:
            if (static_cast<s32>(sGnFrame_5C1B84) <= field_7C_15_timer)
            {
                field_6E_r = field_78_r_value;
                return;
            }

            field_90_state = States::eDisabling_4;
            field_6E_r = field_78_r_value;
            break;

        case States::eDisabling_4:
            field_78_r_value -= 25;

            if (field_78_r_value > 0)
            {
                field_6E_r = field_78_r_value;
                return;
            }

            field_78_r_value = 0;
            field_7C_15_timer = sGnFrame_5C1B84 + 15;
            field_90_state = States::eDisabled_5;
            field_6E_r = field_78_r_value;
            break;

        case States::eDisabled_5:
            if (Event_Get_422C00(kEventHeroDying))
            {
                mFlags.Set(BaseGameObject::eDead);
            }
            else
            {
                if (static_cast<s32>(sGnFrame_5C1B84) > field_7C_15_timer)
                {
                    field_90_state = States::eEnabling_2;
                    SFX_Play_46FA90(SoundEffect::SecurityDoorDeny_38, 0);
                }
                field_6E_r = field_78_r_value;
            }
            break;

        default:
            field_6E_r = field_78_r_value;
            break;
    }
}
