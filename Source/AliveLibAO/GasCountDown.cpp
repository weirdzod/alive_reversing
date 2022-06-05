#include "stdafx_ao.h"
#include "GasCountDown.hpp"
#include "Function.hpp"
#include "Map.hpp"
#include "Game.hpp"
#include "stdlib.hpp"
#include "ScreenManager.hpp"
#include "Events.hpp"
#include "SwitchStates.hpp"
#include "Abe.hpp"
#include "Alarm.hpp"
#include "Sfx.hpp"
#include "DeathGas.hpp"

namespace AO {

const u8 byte_4C5080[32] = {
    0u,
    0u,
    1u,
    128u,
    1u,
    132u,
    32u,
    132u,
    33u,
    128u,
    32u,
    132u,
    33u,
    132u,
    101u,
    206u,
    101u,
    140u,
    140u,
    177u,
    19u,
    148u,
    100u,
    206u,
    101u,
    206u,
    215u,
    152u,
    20u,
    161u,
    24u,
    216u};

ALIVE_VAR(1, 0x507700, s32, sGasTimer_507700, 0);
ALIVE_VAR(1, 0x4FF888, s16, gGasOn_4FF888, 0);

GasCountDown::GasCountDown(Path_GasCountDown* pTlv, s32 tlvInfo)
    : BaseGameObject(TRUE, 0)
{
    mBaseGameObjectTypeId = ReliveTypes::eGasClock;
    field_58_tlvInfo = tlvInfo;
    field_10_font_context.LoadFontType(2);
    field_20_font.Load(5, byte_4C5080, &field_10_font_context);
    mBaseGameObjectFlags.Set(Options::eDrawable_Bit4);
    gObjListDrawables->Push_Back(this);

    gGasOn_4FF888 = 0;

    field_62_time_left = 120;

    field_5C_xpos = FP_GetExponent((FP_FromInteger(pScreenManager_4FF7C8->field_14_xpos + pTlv->field_10_top_left.field_0_x) - pScreenManager_4FF7C8->field_10_pCamPos->field_0_x));

    field_5E_ypos = FP_GetExponent((FP_FromInteger(pScreenManager_4FF7C8->field_16_ypos + pTlv->field_10_top_left.field_2_y)) - pScreenManager_4FF7C8->field_10_pCamPos->field_4_y);

    field_60_start_switch_id = pTlv->field_18_start_switch_id;
}

GasCountDown::~GasCountDown()
{
    gObjListDrawables->Remove_Item(this);
    gMap.TLV_Reset(field_58_tlvInfo, -1, 0, 0);
}

void GasCountDown::VScreenChanged()
{
    mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    if (gMap.mCurrentLevel != gMap.mLevel || gMap.mCurrentPath != gMap.mPath)
    {
        sGasTimer_507700 = 0;
    }
}

void GasCountDown::VUpdate()
{
    if (Event_Get(kEventDeathReset))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    if (Event_Get(kEventDeathResetEnd))
    {
        sGasTimer_507700 = 0;
        gGasOn_4FF888 = 0;
    }

    // Enable
    if (!sGasTimer_507700 && SwitchStates_Get(field_60_start_switch_id) && !SwitchStates_Get(70))
    {
        sGasTimer_507700 = sGnFrame;
        ao_new<Alarm>(3600, 0, 0, Layer::eLayer_Above_FG1_39);
    }

    if (!sGasTimer_507700)
    {
        // Off/idle
        field_62_time_left = 120;
    }
    else
    {
        // Running
        if (SwitchStates_Get(70))
        {
            sGasTimer_507700 = 0;
            return;
        }

        if (Event_Get(kEventResetting))
        {
            sGasTimer_507700++;
        }

        const s32 oldTimer = field_62_time_left;
        const s32 newTimer = 120 - (static_cast<s32>(sGnFrame) - sGasTimer_507700) / 30;
        field_62_time_left = static_cast<s16>(newTimer);
        if (oldTimer != field_62_time_left && field_62_time_left > 0)
        {
            SFX_Play_Pitch(SoundEffect::RedTick_4, 55, -1000, 0);
        }
    }

    DealDamage();
}

void GasCountDown::DealDamage()
{
    if (field_62_time_left < 0)
    {
        if (-field_62_time_left > 2)
        {
            sActiveHero_507678->VTakeDamage(this);
            for (s32 i = 0; i < gBaseAliveGameObjects_4FC8A0->Size(); i++)
            {
                BaseAliveGameObject* pObj = gBaseAliveGameObjects_4FC8A0->ItemAt(i);
                if (!pObj)
                {
                    break;
                }

                if (pObj->mBaseGameObjectTypeId == ReliveTypes::eMudokon)
                {
                    pObj->VTakeDamage(this);
                }
            }
        }
        field_62_time_left = 0;
    }

    if (!gGasOn_4FF888 && field_62_time_left <= 0)
    {
        gGasOn_4FF888 = TRUE;
        ao_new<DeathGas>(Layer::eLayer_Above_FG1_39, 2);
    }
}

void GasCountDown::VRender(PrimHeader** ppOt)
{
    char_type text[128] = {};
    sprintf(text, "%02d:%02d", field_62_time_left / 60, field_62_time_left % 60);
    const auto textWidth = field_20_font.MeasureTextWidth(text);

    field_20_font.DrawString(
        ppOt,
        text,
        field_5C_xpos,
        field_5E_ypos,
        TPageAbr::eBlend_1,
        1,
        0,
        Layer::eLayer_BeforeWell_22,
        127,
        127,
        127,
        0,
        FP_FromInteger(1),
        textWidth + field_5C_xpos,
        sDisableFontFlicker_5080E4 ? 0 : 50);

    pScreenManager_4FF7C8->InvalidateRect(
        field_5C_xpos,
        field_5E_ypos,
        field_5C_xpos + textWidth,
        field_5E_ypos + 16,
        pScreenManager_4FF7C8->field_2E_idx);
}

} // namespace AO
