#include "stdafx.h"
#include "VRam.hpp"
#include "Function.hpp"
#include "PsxDisplay.hpp"
#include <gmock/gmock.h>
#include "Renderer/IRenderer.hpp"

const s32 kMaxAllocs = 512;

ALIVE_ARY(1, 0x5cb888, PSX_RECT, kMaxAllocs, sVramAllocations_5CB888, {});
ALIVE_VAR(1, 0x5cc888, s32, sVramNumberOfAllocations_5CC888, 0);
ALIVE_VAR(1, 0x5CC88C, u16, unused_5CC88C, 0);

s8 Vram_calc_width_4955A0(s32 width, s32 depth)
{
    switch (depth)
    {
        case 0:
            return ((width + 7) >> 2) & 0xFE;
        case 1:
            return ((width + 3) >> 1) & 0xFE;
        case 2:
            return (width + 1) & 0xFE;
    }
    return 0;
}

s32 Vram_Is_Area_Free_4958F0(PSX_RECT* pRect, s32 depth)
{
    pRect->x = 1024 - pRect->w;
    if (pRect->x < 0)
    {
        return 0;
    }

    s16 newX = 0;
    const s32 depthShift = 2 - depth;
    while (true)
    {
        if ((pRect->w << depthShift) + ((pRect->x & 63) << depthShift) > 256)
        {
            newX = pRect->x + 64 - 1;
            newX = (pRect->x + (64 - 1)) & ~(64 - 1);
            if (newX < pRect->x)
            {
                pRect->x = newX;
            }
        }
        else
        {
            if (sVramNumberOfAllocations_5CC888 <= 0)
            {
                return 1;
            }

            s32 i = 0;
            while (!Vram_rects_overlap_4959E0(pRect, &sVramAllocations_5CB888[i]))
            {
                i++;
                if (i >= sVramNumberOfAllocations_5CC888)
                {
                    return 1;
                }
            }

            newX = sVramAllocations_5CB888[i].x - pRect->w + 1;

            if (newX < pRect->x)
            {
                pRect->x = newX;
            }
        }

        if (--pRect->x < 0)
        {
            return 0;
        }
    }
}

s32 Vram_alloc_block_4957B0(PSX_RECT* pRect, s32 depth)
{
    if (pRect->w > 1024 || pRect->h > 512)
    {
        return 0;
    }

    if (pRect->h * pRect->w >= 1024)
    {
        pRect->y = 512 - pRect->h;
        while (pRect->y >= 0)
        {
            // Old Code: if (static_cast<u8>(pRect->y) + pRect->h <= 256)
            // Instead of casting to u8 to wrap around the integer, we're
            // going to do it manually in case other platforms don't auto wrap integers on
            // cast.
            if ((pRect->y % 256) + pRect->h <= 256)
            {
                if (Vram_Is_Area_Free_4958F0(pRect, depth))
                {
                    return 1;
                }
            }
            else
            {
                // v7 &= 0xFFFFFF00; // Todo: check this. was LOBYTE(v7) = 0; Doesn't seem needed
                // to pass tests.
                const s16 ypos = (pRect->y + 255) - pRect->h + 1;
                if (ypos < pRect->y)
                {
                    pRect->y = ypos;
                }
            }
            --pRect->y;
        }
        return 0;
    }

    pRect->y = 0;
    if (512 - pRect->h <= 0)
    {
        return 0;
    }

    // Search Loop
    while (true)
    {
        const s16 yPos = pRect->y;
        if (pRect->h + yPos <= 255 || yPos >= 256)
        {
            if (!Vram_Is_Area_Free_4958F0(pRect, depth))
            {
                pRect->y++;
                if (pRect->y >= 512 - pRect->h)
                {
                    return 0;
                }

                continue;
            }
            else
            {
                return 1;
            }
        }
        pRect->y = 255;

        ++pRect->y;
        if (pRect->y >= 512 - pRect->h)
        {
            return 0;
        }
    }

    return 1;
}

s16 Vram_alloc_4956C0(u16 width, s16 height, u16 colourDepth, PSX_RECT* pRect)
{
    PSX_RECT rect = {};

    const s32 depth = colourDepth / 8;

    rect.w = Vram_calc_width_4955A0(width, depth);
    rect.h = height;

    if (sVramNumberOfAllocations_5CC888 >= kMaxAllocs || !Vram_alloc_block_4957B0(&rect, depth))
    {
        return 0;
    }

    sVramAllocations_5CB888[sVramNumberOfAllocations_5CC888++] = rect;
    *pRect = rect;

    return 1;
}

void Vram_init_495660()
{
    for (s32 i = 0; i < kMaxAllocs; i++)
    {
        sVramAllocations_5CB888[i] = {};
    }
    sbDebugFontLoaded = 0;
    sVramNumberOfAllocations_5CC888 = 0;
}

void Vram_alloc_explicit(s16 x, s16 y, s16 w, s16 h)
{
    if (sVramNumberOfAllocations_5CC888 < kMaxAllocs)
    {
        sVramAllocations_5CB888[sVramNumberOfAllocations_5CC888].x = x;
        sVramAllocations_5CB888[sVramNumberOfAllocations_5CC888].y = y;
        sVramAllocations_5CB888[sVramNumberOfAllocations_5CC888].w = w - x + 1;
        sVramAllocations_5CB888[sVramNumberOfAllocations_5CC888].h = h - y + 1;
        sVramNumberOfAllocations_5CC888++;
    }
}

void Vram_free_495A60(PSX_Point xy, PSX_Point wh)
{
#if RENDERER_OPENGL
    auto pRend = IRenderer::GetRenderer();
    if (pRend)
    {
        pRend->PalFree(IRenderer::PalRecord{xy.x, xy.y, 0});
    }
#endif
    // Find the allocation
    for (s32 i = 0; i < sVramNumberOfAllocations_5CC888; i++)
    {
        if (sVramAllocations_5CB888[i].x == xy.x && sVramAllocations_5CB888[i].y == xy.y && sVramAllocations_5CB888[i].w == wh.x && sVramAllocations_5CB888[i].h == wh.y)
        {
            // Copy the last element to this one
            sVramAllocations_5CB888[i] = sVramAllocations_5CB888[sVramNumberOfAllocations_5CC888 - 1];

            // Decrement the used count
            sVramNumberOfAllocations_5CC888--;
            return;
        }
    }
}

Bool32 Vram_rects_overlap_4959E0(const PSX_RECT* pRect1, const PSX_RECT* pRect2)
{
    const s32 x1 = pRect1->x;
    const s32 x2 = pRect2->x;
    if (x1 >= x2 + pRect2->w)
    {
        return 0;
    }

    const s32 y2 = pRect2->y;
    const s32 y1 = pRect1->y;
    if (y1 >= y2 + pRect2->h)
    {
        return 0;
    }

    if (x2 < x1 + pRect1->w)
    {
        return y2 < y1 + pRect1->h;
    }

    return 0;
}


ALIVE_VAR(1, 0x5c9162, s16, pal_xpos_5C9162, 0);
ALIVE_VAR(1, 0x5c9160, s16, pal_ypos_5C9160, 0);

ALIVE_VAR(1, 0x5c915c, s16, pal_width_5C915C, 0);
ALIVE_VAR(1, 0x5c915e, s16, pal_free_count_5C915E, 0);

ALIVE_ARY(1, 0x5c9164, s32, 77, sPal_table_5C9164, {}); // TODO: Actually 32 in size ?

static bool Pal_Allocate_Helper(s32& i, s32& palX_idx, s32 maskValue, s32 numBits)
{
    for (i = 0; i < pal_free_count_5C915E; i++)
    {
        if (sPal_table_5C9164[i] != (1 << (pal_width_5C915C + 1)) - 1)
        {
            if (pal_width_5C915C != numBits)
            {
                palX_idx = 0;
                bool foundMatch = true;
                while ((maskValue << palX_idx) & sPal_table_5C9164[i])
                {
                    if (++palX_idx >= pal_width_5C915C - numBits)
                    {
                        foundMatch = false;
                        break;
                    }
                }

                if (foundMatch)
                {
                    return true;
                }
            }
        }
    }

    // Failed, out of pals
    return false;
}

s16 Pal_Allocate_483110(PSX_RECT* pRect, u32 paletteColorCount)
{
    if (!pal_free_count_5C915E)
    {
        return 0;
    }

    if (paletteColorCount != 256 && paletteColorCount != 64 && paletteColorCount != 16)
    {
        return 0;
    }

    s32 pal_rect_y = 0;
    s32 palX_idx = 0;
    s32 palBitMask = 0;

    if (paletteColorCount == 16)
    {
        palBitMask = 1;
        if (!Pal_Allocate_Helper(pal_rect_y, palX_idx, palBitMask, 0))
        {
            return 0;
        }
    }
    else if (paletteColorCount == 64)
    {
        palBitMask = 0xF;
        if (!Pal_Allocate_Helper(pal_rect_y, palX_idx, palBitMask, paletteColorCount / 16)) // 64/16 = 4
        {
            return 0;
        }
    }
    else if (paletteColorCount == 256)
    {
        palBitMask = 0xFFFF;
        if (!Pal_Allocate_Helper(pal_rect_y, palX_idx, palBitMask, paletteColorCount / 16)) // 256/16 = 16
        {
            return 0;
        }
    }

    pRect->w = static_cast<s16>(paletteColorCount);

    palBitMask = palBitMask << palX_idx;
    sPal_table_5C9164[pal_rect_y] |= palBitMask;
    pRect->x = static_cast<s16>(pal_xpos_5C9162 + (16 * palX_idx));
    pRect->y = static_cast<s16>(pal_rect_y + pal_ypos_5C9160);
    return 1;
}

void Pal_free_483390(PSX_Point xy, s16 palDepth)
{
    const s32 palIdx = xy.y - pal_ypos_5C9160;
    const s32 palWidthBits = xy.x - pal_xpos_5C9162;

    switch (palDepth)
    {
        case 16:                                                     // 1 bit
            sPal_table_5C9164[palIdx] ^= 1 << ((palWidthBits) / 16); // div 16 to get num bits
            break;
        case 64: // 4 bits
            sPal_table_5C9164[palIdx] ^= 0xF << ((palWidthBits) / 16);
            break;
        case 256: // 16 bits
            sPal_table_5C9164[palIdx] ^= 0xFFFF << ((palWidthBits) / 16);
            break;
    }
}

void Pal_Area_Init_483080(s16 xpos, s16 ypos, u16 width, u16 height)
{
    pal_xpos_5C9162 = xpos;
    pal_ypos_5C9160 = ypos;

    pal_width_5C915C = width / 4;
    pal_free_count_5C915E = height;

    Vram_alloc_explicit(xpos, ypos, xpos + width - 1, ypos + height - 1);

    for (s32 i = 0; i < height; i++)
    {
        sPal_table_5C9164[i] = 0;
    }
}

void Pal_Copy_483560(PSX_Point pPoint, s16 w, u16* pPalData, PSX_RECT* rect)
{
    rect->x = pPoint.x;
    rect->y = pPoint.y;
    rect->w = w;
    rect->h = 1;
    PSX_StoreImage_4F5E90(rect, pPalData);
}

u32 Pal_Make_Colour_4834C0(u8 r, u8 g, u8 b, s16 bOpaque)
{
    return (bOpaque != 0 ? 0x8000 : 0) + ((u32) r >> 3) + 4 * ((g & 0xF8) + 32 * (b & 0xF8));
}

void Pal_Set_483510(PSX_Point xy, s16 w, const u8* palData, PSX_RECT* rect)
{
    rect->x = xy.x;
    rect->y = xy.y;
    rect->w = w;
    rect->h = 1;
    IRenderer::GetRenderer()->PalSetData(IRenderer::PalRecord{xy.x, xy.y, w}, palData);
}
