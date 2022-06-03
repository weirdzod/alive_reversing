#include "stdafx_ao.h"
#include "Function.hpp"
#include "ParamiteWeb.hpp"
#include "Map.hpp"
#include "stdlib.hpp"
#include "PsxDisplay.hpp"
#include "ResourceManager.hpp"
#include "Rope.hpp"
#include "ScreenManager.hpp"
#include "ShadowZone.hpp"

namespace AO {

void ParamiteWeb::VScreenChanged()
{
    if (gMap.mOverlayId != gMap.GetOverlayId())
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

ParamiteWeb::~ParamiteWeb()
{
    ao_delete_free_447540(field_EC_pRes);
}

ParamiteWeb::ParamiteWeb(FP xpos, s32 bottom, s32 top, FP scale)
{
    mBaseGameObjectTypeId = Types::eRope_73;

    field_C8_yOffset = 0;

    if (scale == FP_FromInteger(1))
    {
        field_E6_segment_length = 15;
    }
    else
    {
        field_E6_segment_length = 7;
    }

    const AnimRecord& rec = AO::AnimRec(AnimId::ParamiteWeb);
    u8** ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);
    Animation_Init_417FD0(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1);

    field_10_anim.field_14_scale = scale;
    field_BC_sprite_scale = scale;

    if (scale == FP_FromInteger(1))
    {
        field_10_anim.field_C_layer = Layer::eLayer_RopeWebMeatSaw_24;
        field_C6_scale = 1;
    }
    else
    {
        field_10_anim.field_C_layer = Layer::eLayer_RopeWebMeatSaw_Half_5;
        field_C6_scale = 0;
    }


    field_10_anim.field_8_r = 128;
    field_10_anim.field_9_g = 128;
    field_10_anim.field_A_b = 128;

    field_A8_xpos = xpos;
    field_EA_ttl_remainder = static_cast<s16>(top);
    field_AC_ypos = FP_FromInteger(top);
    field_E8_ttl = static_cast<s16>(bottom);

    field_E4_number_of_segments = 240 / field_E6_segment_length;

    field_EC_pRes = reinterpret_cast<AnimationUnknown*>(ao_new_malloc_447520(sizeof(AnimationUnknown) * field_E4_number_of_segments));

    if (field_EC_pRes)
    {
        for (s32 i = 0; i < field_E4_number_of_segments; i++)
        {
            AnimationUnknown* pSegment = &field_EC_pRes[i];
            pSegment = new (pSegment) AnimationUnknown(); // We have memory but no constructor was called.. so use placement new to get a constructed instance
            pSegment->field_4_flags.Set(AnimFlags::eBit3_Render);
            pSegment->field_68_anim_ptr = &field_10_anim;
            pSegment->field_C_layer = field_10_anim.field_C_layer;
            pSegment->field_6C_scale = field_BC_sprite_scale;
            pSegment->field_4_flags.Clear(AnimFlags::eBit15_bSemiTrans);
            pSegment->field_4_flags.Clear(AnimFlags::eBit16_bBlending);
        }
    }

    field_F0_bEnabled = 0;
}

void ParamiteWeb::VUpdate()
{
    if (field_F0_bEnabled == 1)
    {
        field_EA_ttl_remainder -= 8;
        if (field_EA_ttl_remainder <= field_E8_ttl)
        {
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        }
    }
}

//TODO @ 100% - merge with Rope::vRender()
void ParamiteWeb::VRender(PrimHeader** ppOt)
{
    PSX_Point camCoords = {};
    gMap.GetCurrentCamCoords(&camCoords);
    if (field_B2_lvl_number == gMap.mCurrentLevel && field_B0_path_number == gMap.mCurrentPath)
    {
        if (field_A8_xpos >= FP_FromInteger(camCoords.field_0_x) && field_A8_xpos <= FP_FromInteger(camCoords.field_0_x + 1024))
        {
            const FP cam_y = pScreenManager_4FF7C8->field_10_pCamPos->field_4_y;
            const FP cam_x = pScreenManager_4FF7C8->field_10_pCamPos->field_0_x;

            s16 minY = FP_GetExponent(FP_FromInteger(pScreenManager_4FF7C8->field_16_ypos + field_E8_ttl) - cam_y);
            s16 maxY = FP_GetExponent(FP_FromInteger(pScreenManager_4FF7C8->field_16_ypos + field_EA_ttl_remainder) - cam_y);

            s16 ypos_int = FP_GetExponent(field_AC_ypos);
            if (ypos_int > field_EA_ttl_remainder)
            {
                ypos_int = field_EA_ttl_remainder + (ypos_int - field_EA_ttl_remainder) % field_E6_segment_length;
            }

            const s16 x_start = PsxToPCX<s16>(FP_GetExponent(field_A8_xpos + FP_FromInteger(pScreenManager_4FF7C8->field_14_xpos) - cam_x));

            s16 y_start = FP_GetExponent(FP_FromInteger(pScreenManager_4FF7C8->field_16_ypos + ypos_int) - cam_y);
            if (field_C8_yOffset + y_start > 240)
            {
                y_start = y_start % field_E6_segment_length + 240;
                ypos_int = FP_GetExponent(cam_y + FP_FromInteger(y_start - pScreenManager_4FF7C8->field_16_ypos));
            }

            if (minY < 0)
            {
                minY = 0;
            }

            if (maxY > 240)
            {
                maxY = 240;
            }

            field_10_anim.VRender(640, 240, ppOt, 0, 0);

            if (y_start >= minY)
            {
                for (s32 idx = 0; idx < field_E4_number_of_segments; ++idx)
                {
                    s16 r = 128;
                    s16 g = 128;
                    s16 b = 128;
                    ShadowZone::ShadowZones_Calculate_Colour(FP_GetExponent(field_A8_xpos), ypos_int - (idx * field_E6_segment_length), field_C6_scale, &r, &g, &b);
                    field_EC_pRes[idx].field_8_r = static_cast<u8>(r);
                    field_EC_pRes[idx].field_9_g = static_cast<u8>(g);
                    field_EC_pRes[idx].field_A_b = static_cast<u8>(b);
                    field_EC_pRes[idx].VRender2(x_start, y_start + field_C8_yOffset, ppOt);
                    PSX_RECT rect = {};
                    field_EC_pRes[idx].GetRenderedSize(&rect);
                    pScreenManager_4FF7C8->InvalidateRect(rect.x, rect.y, rect.w, rect.h, pScreenManager_4FF7C8->field_2E_idx);
                    ClipPoly_Vertically_4584B0(&field_EC_pRes[idx].field_10_polys[gPsxDisplay_504C78.field_A_buffer_index], field_C8_yOffset + minY, field_C8_yOffset + maxY);
                    y_start -= field_E6_segment_length;
                    if (y_start < minY)
                    {
                        break;
                    }
                }
            }
        }
    }
}

} // namespace AO
