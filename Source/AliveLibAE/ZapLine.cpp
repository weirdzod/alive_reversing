#include "stdafx.h"
#include "ZapLine.hpp"
#include "Function.hpp"
#include "stdlib.hpp"
#include "../relive_lib/ScreenManager.hpp"
#include "../relive_lib/PsxDisplay.hpp"
#include "Game.hpp"
#include "ResourceManager.hpp"
#include "Map.hpp"

ZapLine::~ZapLine()
{
    relive_delete[] field_134_pSprites;
    relive_delete[] field_138_sprite_positions;
    relive_delete[] field_13C_zap_points;
    relive_delete[] field_140_sprite_segment_positions;
}

ZapLine::ZapLine(FP xPosSource, FP yPosSource, FP xPosDest, FP yPosDest, s32 aliveTime, ZapLineType type, Layer layer)
    : BaseAnimatedWithPhysicsGameObject(0)
{
    SetType(ReliveTypes::eZapLine);
    field_12A_type = type;

    if (type == ZapLineType::eThin_1)
    {
        // Creates thin blue zap lines.
        field_130_number_of_pieces_per_segment = 20;
        field_12E_number_of_segments = 12;
        mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::Zap_Line_Blue));
        Animation_Init(GetAnimRes(AnimId::Zap_Line_Blue));
        field_12C_tPageAbr = TPageAbr::eBlend_3;
    }
    else if (type == ZapLineType::eThick_0)
    {
        // Creates thick red zap lines.
        field_130_number_of_pieces_per_segment = 10;
        field_12E_number_of_segments = 28;
        mLoadedAnims.push_back(ResourceManagerWrapper::LoadAnimation(AnimId::Zap_Line_Red));
        Animation_Init(GetAnimRes(AnimId::Zap_Line_Red));
        field_12C_tPageAbr = TPageAbr::eBlend_1;
    }

    //mAnim.mFlags.Set(AnimFlags::eBit25_bDecompressDone); // HIBYTE |= 1
    GetAnimation().mFlags.Clear(AnimFlags::eSemiTrans);
    GetAnimation().SetRenderLayer(layer);
    field_132_number_of_sprites = field_12E_number_of_segments * field_130_number_of_pieces_per_segment;


    field_134_pSprites = relive_new ZapLineSprites[field_132_number_of_sprites];
    field_138_sprite_positions = relive_new PSX_Point[field_132_number_of_sprites];
    field_13C_zap_points = relive_new ZapPoint[field_130_number_of_pieces_per_segment];
    field_140_sprite_segment_positions = relive_new FP_Point[field_12E_number_of_segments];

    field_128_max_alive_time = static_cast<s16>(aliveTime);

    mXPos = xPosSource;
    mYPos = yPosSource;

    field_F4_state = ZapLineState::eInit_0;
    field_126_alive_timer = 0;

    auto pFrameHeader = GetAnimation().Get_FrameHeader(-1);

    const auto frameW = pFrameHeader->mWidth;
    const auto frameH = pFrameHeader->mHeight;

    for (s32 i = 0; i < 2; i++)
    {
        for (s32 j = 0; j < field_12E_number_of_segments; j++)
        {
            for (s32 k = 0; k < field_130_number_of_pieces_per_segment; k++)
            {
                Prim_Sprt* pSprt = &field_134_pSprites[(j * field_130_number_of_pieces_per_segment) + k].field_0_sprts[i];
                Sprt_Init(pSprt);

                Poly_Set_SemiTrans(&pSprt->mBase.header, 1);
                Poly_Set_Blending(&pSprt->mBase.header, 1);

                pSprt->mAnim = &GetAnimation();

                SetUV0(pSprt, 0, 0);
                pSprt->field_14_w = static_cast<s16>(frameW - 1);
                pSprt->field_16_h = static_cast<s16>(frameH - 1);
            }
        }
    }

    CalculateSourceAndDestinationPositions(xPosSource, yPosSource, xPosDest, yPosDest);
}

void ZapLine::CalculateSourceAndDestinationPositions(FP xPosSource, FP yPosSource, FP xPosDest, FP yPosDest)
{
    field_11C_x_position_source = FP_GetExponent(xPosSource - pScreenManager->CamXPos());
    field_11E_y_position_source = FP_GetExponent(yPosSource - pScreenManager->CamYPos());
    field_120_x_position_destination = FP_GetExponent(xPosDest - pScreenManager->CamXPos());
    field_122_y_position_destination = FP_GetExponent(yPosDest - pScreenManager->CamYPos());

    field_11C_x_position_source = PsxToPCX(field_11C_x_position_source, 11);
    field_120_x_position_destination = PsxToPCX(field_120_x_position_destination, 11);

    s16 xOff = 0;
    s16 yOff = 0;
    GetAnimation().Get_Frame_Offset(&xOff, &yOff);

    field_11C_x_position_source = FP_GetExponent(FP_FromInteger(xOff) + FP_FromInteger(field_11C_x_position_source));
    field_11E_y_position_source = FP_GetExponent(FP_FromInteger(yOff) + FP_FromInteger(field_11E_y_position_source));
    field_120_x_position_destination = FP_GetExponent(FP_FromInteger(xOff) + FP_FromInteger(field_120_x_position_destination));
    field_122_y_position_destination = FP_GetExponent(FP_FromInteger(yOff) + FP_FromInteger(field_122_y_position_destination));
}


void ZapLine::VScreenChanged()
{
    if (gMap.LevelChanged() || gMap.PathChanged())
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

void ZapLine::CalculateThickSpriteSegmentPositions()
{
    // TODO: Convert bit operations to something more readable.
    s32 v1 = 0;
    if (field_126_alive_timer >= 8)
    {
        const s32 remainingAliveTime = field_128_max_alive_time - field_126_alive_timer;
        if (remainingAliveTime >= 8)
        {
            v1 = 4;
        }
        else
        {
            v1 = (remainingAliveTime / 4) + 3;
        }
    }
    else
    {
        v1 = field_126_alive_timer / 4 + 3;
    }

    s32 v5 = 1 << v1;
    s32 v6 = 1 << (v1 - 1);

    field_140_sprite_segment_positions[0].x = FP_FromInteger(field_11C_x_position_source);
    field_140_sprite_segment_positions[0].y = FP_FromInteger(field_11E_y_position_source);

    field_140_sprite_segment_positions[field_12E_number_of_segments - 1].x = FP_FromInteger(field_120_x_position_destination);
    field_140_sprite_segment_positions[field_12E_number_of_segments - 1].y = FP_FromInteger(field_122_y_position_destination);

    s32 angExtra = 0;
    if ((sGnFrame / 8) & 1)
    {
        angExtra = 0;
    }
    else
    {
        angExtra = 128;
    }

    const FP xDiff = FP_FromInteger(field_120_x_position_destination - field_11C_x_position_source) / FP_FromInteger(field_12E_number_of_segments);
    const FP xDiffDiv = -xDiff * FP_FromDouble(1.5);

    const FP yDiff = FP_FromInteger(field_122_y_position_destination - field_11E_y_position_source) / FP_FromInteger(field_12E_number_of_segments);
    const FP yDiffDiv = yDiff * FP_FromDouble(1.5);


    // First and last done above.
    for (s32 i = 1; i < field_12E_number_of_segments - 1; i++)
    {
        const u8 ang = static_cast<u8>(angExtra + 18 * i);
        field_140_sprite_segment_positions[i].x = FP_FromInteger(Math_NextRandom() % v5) + (Math_Cosine_496CD0(ang) * xDiffDiv) + FP_FromInteger(field_11C_x_position_source) + (FP_FromInteger(i) * xDiff) - FP_FromInteger(v6);

        field_140_sprite_segment_positions[i].y = FP_FromInteger(Math_NextRandom() % v5) + (Math_Cosine_496CD0(ang) * yDiffDiv) + FP_FromInteger(field_11E_y_position_source) + (FP_FromInteger(i) * yDiff) - FP_FromInteger(v6);
    }

    field_144_rects[0].x = 0;
    field_144_rects[0].y = 0;
    field_144_rects[0].w = gPsxDisplay.mWidth;
    field_144_rects[0].h = gPsxDisplay.mHeight;

    field_144_rects[1].x = 0;
    field_144_rects[1].y = 0;
    field_144_rects[1].w = gPsxDisplay.mWidth;
    field_144_rects[1].h = gPsxDisplay.mHeight;
}

void ZapLine::CalculateThinSpriteSegmentPositions()
{
    field_140_sprite_segment_positions[0].x = FP_FromInteger(field_11C_x_position_source);
    field_140_sprite_segment_positions[0].y = FP_FromInteger(field_11E_y_position_source);
    field_140_sprite_segment_positions[field_12E_number_of_segments - 1].x = FP_FromInteger(field_120_x_position_destination);
    field_140_sprite_segment_positions[field_12E_number_of_segments - 1].y = FP_FromInteger(field_122_y_position_destination);

    const FP x2Diff = FP_FromInteger(field_120_x_position_destination - field_11C_x_position_source) / FP_FromInteger(field_12E_number_of_segments);
    const FP y2Diff = FP_FromInteger(field_122_y_position_destination - field_11E_y_position_source) / FP_FromInteger(field_12E_number_of_segments);

    const FP y2DiffDiv = -y2Diff * FP_FromDouble(0.1);
    const FP x2DiffDiv = x2Diff * FP_FromDouble(0.1);

    for (s32 i = 1; i < field_12E_number_of_segments - 1; i++)
    {
        const FP rnd = FP_FromInteger(Math_NextRandom() % 32 - 16);
        field_140_sprite_segment_positions[i].x = (y2DiffDiv * rnd) + FP_FromInteger(field_11C_x_position_source) + (FP_FromInteger(i) * x2Diff);
        field_140_sprite_segment_positions[i].y = (x2DiffDiv * rnd) + FP_FromInteger(field_11E_y_position_source) + (FP_FromInteger(i) * y2Diff);
    }
}

void ZapLine::CalculateZapPoints()
{
    FP acc = FP_FromInteger(0);
    const FP delta = FP_FromInteger(1) / FP_FromInteger(field_130_number_of_pieces_per_segment);
    for (s32 i = 0; i < field_130_number_of_pieces_per_segment; i++)
    {
        const FP accSqrd = (acc * acc);
        field_13C_zap_points[i].field_0_part_1 = accSqrd - FP_FromRaw(2 * acc.fpValue) + FP_FromInteger(1);
        field_13C_zap_points[i].field_4_part_2 = -FP_FromRaw(2 * accSqrd.fpValue) + FP_FromRaw(2 * acc.fpValue) + FP_FromInteger(1);
        field_13C_zap_points[i].field_8_part_3 = accSqrd;
        acc += delta;
    }
}

void ZapLine::CalculateSpritePositionsInner(s32 idx1, s32 idx2, s32 idx3, s16 idx4)
{
    const FP x1 = field_140_sprite_segment_positions[idx1].x;
    const FP y1 = field_140_sprite_segment_positions[idx1].y;

    const FP x2 = field_140_sprite_segment_positions[idx2].x;
    const FP y2 = field_140_sprite_segment_positions[idx2].y;

    const FP x3 = field_140_sprite_segment_positions[idx3].x;
    const FP y3 = field_140_sprite_segment_positions[idx3].y;

    for (s32 i = 0; i < field_130_number_of_pieces_per_segment; i++)
    {
        auto pItem = &field_138_sprite_positions[i + (idx4 * field_130_number_of_pieces_per_segment)];

        pItem->x = FP_GetExponent(
            FP_FromRaw((
                           (field_13C_zap_points[i].field_8_part_3 * x3) + (field_13C_zap_points[i].field_4_part_2 * x2) + (field_13C_zap_points[i].field_0_part_1 * x1))
                           .fpValue
                       >> 1));

        pItem->y = FP_GetExponent(
            FP_FromRaw((
                           (field_13C_zap_points[i].field_8_part_3 * y3) + (field_13C_zap_points[i].field_4_part_2 * y2) + (field_13C_zap_points[i].field_0_part_1 * y1))
                           .fpValue
                       >> 1));
    }
}

void ZapLine::UpdateSpriteVertexPositions()
{
    for (s32 i = 0; i < field_12E_number_of_segments; i++)
    {
        for (s32 j = 0; j < field_130_number_of_pieces_per_segment; j++)
        {
            const auto pPoint = &field_138_sprite_positions[j + (i * field_130_number_of_pieces_per_segment)];
            Prim_Sprt* pSprt = &field_134_pSprites->field_0_sprts[j + (i * field_130_number_of_pieces_per_segment)];
            SetXY0(&pSprt[0], pPoint->x, pPoint->y);
            SetXY0(&pSprt[1], pPoint->x, pPoint->y);
        }
    }
}

void ZapLine::CalculateSpritePositionsOuter()
{
    for (s16 i = 0; i < field_12E_number_of_segments; i++)
    {
        if (i == 0)
        {
            // First item.
            CalculateSpritePositionsInner(0, 0, 1, 0);
        }
        else
        {
            const s16 lastIdx = field_12E_number_of_segments - 1;
            if (i == lastIdx)
            {
                // Last item.
                CalculateSpritePositionsInner(field_12E_number_of_segments - 2, lastIdx, lastIdx, field_12E_number_of_segments - 1);
            }
            else
            {
                // Other items.
                CalculateSpritePositionsInner(i - 1, i, i + 1, i);
            }
        }
    }
}

void ZapLine::VUpdate()
{
    field_126_alive_timer++;

    // TODO: States 3 and 4 might not actually be needed, since states 1 and 2 do the same thing; though the class only seems to render in states 3 and 4.
    switch (field_F4_state)
    {
        case ZapLineState::eInit_0:
            CalculateZapPoints();

            if (field_12A_type == ZapLineType::eThin_1)
            {
                CalculateThinSpriteSegmentPositions();
            }
            else if (field_12A_type == ZapLineType::eThick_0)
            {
                CalculateThickSpriteSegmentPositions();
            }

            field_F4_state = ZapLineState::eInitSpritePositions_1;
            break;

        case ZapLineState::eInitSpritePositions_1:
            CalculateSpritePositionsOuter();
            field_F4_state = ZapLineState::eInitSpriteVertices_2;
            break;

        case ZapLineState::eInitSpriteVertices_2:
        case ZapLineState::eUpdateSpriteVertices_4:
            UpdateSpriteVertexPositions();

            if (field_126_alive_timer >= field_128_max_alive_time && field_12A_type != ZapLineType::eThin_1)
            {
                mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                return;
            }

            if (field_12A_type == ZapLineType::eThin_1)
            {
                CalculateThinSpriteSegmentPositions();
            }
            else if (field_12A_type == ZapLineType::eThick_0)
            {
                CalculateThickSpriteSegmentPositions();
            }
            field_F4_state = ZapLineState::eUpdateSpritePositions_3;
            break;

        case ZapLineState::eUpdateSpritePositions_3:
            CalculateSpritePositionsOuter();
            field_F4_state = ZapLineState::eUpdateSpriteVertices_4;
            break;
    }
}

void ZapLine::VRender(PrimHeader** ppOt)
{
    if (gMap.Is_Point_In_Current_Camera(
            mCurrentLevel,
            mCurrentPath,
            mXPos,
            mYPos,
            0)
        && field_F4_state > ZapLineState::eInitSpriteVertices_2)
    {
        const auto bufferIdx = gPsxDisplay.mBufferIndex;

        for (s32 i = 0; i < field_12E_number_of_segments; i++)
        {
            for (s32 j = 0; j < field_130_number_of_pieces_per_segment; j++)
            {
                Prim_Sprt* pSprt = &field_134_pSprites->field_0_sprts[j + (i * field_130_number_of_pieces_per_segment)];
                OrderingTable_Add(OtLayer(ppOt, GetAnimation().GetRenderLayer()), &pSprt[bufferIdx].mBase.header);
            }
        }

        const s32 calcTPage = PSX_getTPage(
            field_124_tPageMode,
            field_12C_tPageAbr,
            0,
            0);

        Prim_SetTPage* pTPage = &field_FC_tPage_p8[bufferIdx];
        Init_SetTPage(pTPage, 0, 0, calcTPage);
        OrderingTable_Add(OtLayer(ppOt, GetAnimation().GetRenderLayer()), &pTPage->mBase);

        PSX_RECT* pRect = &field_144_rects[bufferIdx];
        pRect->x = 32767;
        pRect->w = -32767;
        pRect->y = 32767;
        pRect->h = -32767;

        for (s32 i = 0; i < field_12E_number_of_segments; i++)
        {
            const PSX_Point* pPoint = &field_138_sprite_positions[i * field_130_number_of_pieces_per_segment];
            for (s32 j = 0; j < field_130_number_of_pieces_per_segment; j++)
            {
                if (pPoint->x < pRect->x)
                {
                    pRect->x = pPoint->x;
                }

                if (pPoint->x > pRect->w)
                {
                    pRect->w = pPoint->x;
                }

                if (pPoint->y < pRect->y)
                {
                    pRect->y = pPoint->y;
                }

                if (pPoint->y > pRect->h)
                {
                    pRect->h = pPoint->y;
                }
            }
        }

        pRect->x -= 25;
        pRect->w += 25;
        pRect->y -= 25;
        pRect->h += 25;
    }
}
