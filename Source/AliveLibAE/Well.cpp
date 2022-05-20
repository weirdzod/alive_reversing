#include "stdafx.h"
#include "Well.hpp"
#include "Function.hpp"
#include "Leaf.hpp"
#include "PsxDisplay.hpp"
#include "Path.hpp"
#include "Events.hpp"
#include "stdlib.hpp"
#include "SwitchStates.hpp"

ALIVE_VAR(1, 0x563aa0, u32, sWellRndSeed_563AA0, 4);

Well* Well::ctor_4E2BE0(Path_WellBase* pTlv, FP xpos, FP ypos, s32 tlvInfo)
{
    BaseGameObject_ctor_4DBFA0(TRUE, 0);
    SetVTable(this, 0x547FE8); // vTbl_ExpressWell_547FE8

    field_20_tlvInfo = tlvInfo;
    SetType(AETypes::eWell_147);

    if (pTlv->field_4_type == TlvTypes::LocalWell_8)
    {
        WellLocal_Init_4E2CD0(static_cast<Path_WellLocal*>(pTlv), xpos, ypos);
    }
    else
    {
        WellExpress_Init_4E2E00(static_cast<Path_WellExpress*>(pTlv), xpos, ypos);
    }

    return this;
}

BaseGameObject* Well::VDestructor(s32 flags)
{
    return vdtor_4E2CA0(flags);
}

void Well::VUpdate()
{
    vUpdate_4E2F60();
}

void Well::VScreenChanged()
{
    vScreenChanged_4E3070();
}

void Well::WellExpress_Init_4E2E00(Path_WellExpress* pTlv, FP /*xpos*/, FP ypos)
{
    if (pTlv->field_0_scale != Scale_short::eFull_0)
    {
        field_28_scale = FP_FromDouble(0.5);
    }
    else
    {
        field_28_scale = FP_FromInteger(1);
    }

    field_24_switch_id = pTlv->field_2_switch_id;

    field_2C_exit_x = FP_FromInteger(pTlv->field_18_exit_x) / FP_FromInteger(100);
    field_30_exit_y = FP_FromInteger(pTlv->field_1A_exit_y) / FP_FromInteger(100);

    field_3C_bEmitLeaves = pTlv->field_2C_bEmit_leaves;
    if (field_3C_bEmitLeaves == Choice_short::eYes_1)
    {
        PSX_Point abeSpawnPos = {};
        gMap.Get_Abe_Spawn_Pos_4806D0(&abeSpawnPos);

        field_34_leaf_xpos = FP_FromInteger(pTlv->field_2E_leaf_x);
        if (pTlv->field_2E_leaf_x > 0)
        {
            field_34_leaf_xpos += FP_FromInteger(abeSpawnPos.field_0_x);
        }
        else
        {
            const s32 pos = (PsxToPCX(pTlv->field_C_bottom_right.field_0_x - pTlv->field_8_top_left.field_0_x) / 2) + pTlv->field_8_top_left.field_0_x;
            field_34_leaf_xpos = FP_FromInteger(pos);
        }

        field_38_leaf_ypos = FP_FromInteger(pTlv->field_30_leaf_y);
        if (pTlv->field_30_leaf_y > 0)
        {
            field_38_leaf_ypos += FP_FromInteger(abeSpawnPos.field_2_y);
        }
        else
        {
            field_38_leaf_ypos = ypos;
        }
    }
}

void Well::WellLocal_Init_4E2CD0(Path_WellLocal* pTlv, FP /*xpos*/, FP ypos)
{
    if (pTlv->field_0_scale != Scale_short::eFull_0)
    {
        field_28_scale = FP_FromDouble(0.5);
    }
    else
    {
        field_28_scale = FP_FromInteger(1);
    }

    field_24_switch_id = pTlv->field_2_switch_id;

    field_3C_bEmitLeaves = pTlv->field_20_bEmit_leaves;
    if (field_3C_bEmitLeaves == Choice_short::eYes_1)
    {
        PSX_Point abeSpawnPos = {};
        gMap.Get_Abe_Spawn_Pos_4806D0(&abeSpawnPos);
        field_34_leaf_xpos = FP_FromInteger(pTlv->field_22_leaf_x);

        if (pTlv->field_22_leaf_x > 0)
        {
            field_34_leaf_xpos += FP_FromInteger(abeSpawnPos.field_0_x);
        }
        else
        {
            const s32 pos = (PsxToPCX(pTlv->field_C_bottom_right.field_0_x - pTlv->field_8_top_left.field_0_x) / 2) + pTlv->field_8_top_left.field_0_x;
            field_34_leaf_xpos = FP_FromInteger(pos);
        }

        field_38_leaf_ypos = FP_FromInteger(pTlv->field_24_leaf_y);
        if (pTlv->field_24_leaf_y > 0)
        {
            field_38_leaf_ypos += FP_FromInteger(abeSpawnPos.field_2_y);
        }
        else
        {
            field_38_leaf_ypos = ypos;
        }
    }
}

Well* Well::vdtor_4E2CA0(s32 flags)
{
    dtor_4E3090();
    if (flags & 1)
    {
        ae_delete_free_495540(this);
    }
    return this;
}

void Well::dtor_4E3090()
{
    SetVTable(this, 0x547FE8); // vTbl_ExpressWell_547FE8

    if (field_20_tlvInfo != -1)
    {
        Path::TLV_Reset_4DB8E0(field_20_tlvInfo, -1, 0, 0);
    }

    BaseGameObject_dtor_4DBEC0();
}

void Well::vScreenChanged_4E3070()
{
    mFlags.Set(BaseGameObject::eDead);
}

static s16 Well_NextRandom()
{
    const auto curRand = sRandomBytes_546744[sWellRndSeed_563AA0];
    sWellRndSeed_563AA0++;

    if (sWellRndSeed_563AA0 > 255)
    {
        sWellRndSeed_563AA0 = 0;
    }

    return curRand;
}

void Well::vUpdate_4E2F60()
{
    if (Event_Get_422C00(kEventDeathReset))
    {
        mFlags.Set(BaseGameObject::eDead);
        // Reset well state when Abe dies.
        Path::TLV_Reset_4DB8E0(field_20_tlvInfo, -1, 0, 0);
    }

    if (field_3C_bEmitLeaves == Choice_short::eYes_1)
    {
        // Always on or has been enabled?
        if (field_24_switch_id == 0 || SwitchStates_Get_466020(field_24_switch_id))
        {
            // Random chance of leaves emitting.
            if (Well_NextRandom() < 10)
            {
                auto pLeaf = ae_new<Leaf>();
                if (pLeaf)
                {
                    pLeaf->ctor_4E3120(
                        field_34_leaf_xpos,
                        field_38_leaf_ypos,
                        FP_FromInteger(2),
                        FP_FromInteger(-20),
                        field_28_scale);
                }
            }
        }
    }
}
