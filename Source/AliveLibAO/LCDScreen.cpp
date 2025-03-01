#include "stdafx_ao.h"
#include "CameraSwapper.hpp"
#include "Events.hpp"
#include "PsxDisplay.hpp"
#include "ScreenManager.hpp"
#include "Math.hpp"
#include "Sfx.hpp"
#include "Function.hpp"
#include "LCDScreen.hpp"
#include "VRam.hpp"
#include "stdlib.hpp"
#include "Game.hpp"
#include "StringFormatters.hpp"
#include "Primitives_common.hpp"
#include "Input.hpp"
#include "Renderer/IRenderer.hpp"
#include "../AliveLibCommon/PathDataExtensionsTypes.hpp"

namespace AO {

const u8 sLCDScreen_Palette_4C75A8[32] = {
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
    57u,
    231u,
    140u,
    177u,
    96u,
    142u,
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

const u8 sLCDScreen_Palette2_4C7588[32] = {
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
    5u,
    132u,
    57u,
    231u,
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

static const char_type* sLCDMessageTable_4C7420[90] = {
    "",
    "                               Las ganancias justifican los medios.",
    "                               Eres lo que comes.",
    "                               Este mes solo 1.236 accidentes laborales. Buen trabajo, seguid asi!",
    "                               No juegues con la comida a menos que ella juegue contigo antes.",
    "                               Venera a la autoridad.",
    "                               Lleva a los Mudokons a los portales de pajaros.",
    "                               No eres el unico que puede esconderse entre las sombras.",
    "                               Cuando un slig grita 'cuidado!', el mudokon inteligente se tira al suelo.",
    "                               Cuota de decapitacion reducida a la mitad!",
    "                               Trabaja duro, muere joven, gana valiosos premios!",
    "                               Un poco de trabajo duro nunca ha matado a nadie importante.",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "                               Para impulsarte hacia arriba, ponte bajo la cornisa y pulsa " kAO_Up ".",
    "                               Para saltar, aprieta " kAO_Jump_Or_Hello ". Para saltar con carrerilla, aprieta " kAO_Jump_Or_Hello " mientras corres apretando " kAO_Run ".",
    "                               Para escabullirte junto al slig dormido, manten apretado " kAO_Sneak " mientras te mueves.",
    "                               Para tirar de la palanca, ponte junto a ella y aprieta " kAO_Action ".",
    "                               Para usar el anillo, ponte debajo y pulsa " kAO_Up ".",
    "                               Para viajar en la plataforma, ponte en el centro y aprieta  " kAO_Down ".",
    "                               Para cruzar la puerta, ponte frente a ella y aprieta " kAO_Up ".",
    "                               Para desactivar una bomba, ponte en cuclillas junto a ella apretando " kAO_Down ". Luego, con cuidado, aprieta " kAO_Action " cuando la bomba tenga un parpadeo verde. Aprieta " kAO_Action " otra vez para reactivarla.",
    "                               Escabullete hacia las sombras manteniendo apretado " kAO_Sneak " mientras te mueves. Quedate quieto en la sombra mientras el slig mira en tu direccion, o te vera. Escabullete tras el slig cuando se aleje de ti.",
    "                               Para sacar una granada de la Maquina Explosiva, situate frente a ella y aprieta " kAO_Up ". Para coger una granada, ponte en cuclillas frente a ella y aprieta " kAO_Action ". Para lanzar la granada, manten apretado " kAO_Throw ", y luego aprieta cualquier boton de direccion. Cada boton la enviara a diferentes angulos.",
    "                               Es posible hacer el lanzamiento estando en cuclillas.",
    "                               Para alertar a un mudokon, di hola manteniendo apretado " kAO_Speak1 " y apretando " kAO_Jump_Or_Hello ". Luego, habla con los mudokons manteniendo apretado " kAO_Speak1 " o " kAO_Speak2 " y apretando " kAO_Jump_Or_Hello ", " kAO_Action ", " kAO_Throw ", o " kAO_Crouch ". No temas experimentar!",
    "                               Puedes poseer a los sligs cantando. Para cantar, manten apretado " kAO_Speak1 " y " kAO_Speak2 " al mismo tiempo o (0) en el teclado.",
    "                               Para ayudar a los mudokons a escapar de la esclavitud en las Granjas Hostiles, canta cuando veas un circulo de pajaros. Para cantar, manten apretados " kAO_Speak1 " y " kAO_Speak2 " al mismo tiempo o (0) en el teclado.",
    "                               Para rodar por el suelo, primero ponte en cuclillas apretando " kAO_Down ". Luego rueda hacia la izquierda o la derecha apretando " kAO_Left " o " kAO_Right ".",
    "                               Para correr, manten apretado " kAO_Run " mientras aprietas " kAO_Left " o " kAO_Right ".",
    "                               Para evitar la alarma, quedate quieto cuando te toquen los Detectores de Movimiento.",
    "                               Usa al slig para cruzar la puerta de seguridad a base de labia.",
    "                               Los sligs pueden hablar a los slogs.",
    "                               Usa la palanca para activar la sierra de carne.",
    "                               Conduce a los Mudokons a los portales de pajaros, despues canta para rescatarlos. Canta pulsando " kAO_Speak1 " y " kAO_Speak2 " al mismo tiempo o (0) en el teclado.",
    "",
    "",
    "",
    "",
    "                               Casi eres libre, Abe! Tienes que cruzar los corrales. Buena suerte... y cuidado con esos scrabs! Hace tiempo que no los alimentan.",
    "                               Los mudokons que crucen esta puerta seran sacrificados y empaquetados como productos carnicos repugnantes pero novedosos.",
    "                               Hazlo, Abe...  hazlo!!!",
    "                               Tu primera tarea es escapar de las Granjas Hostiles. Cuidado con los sligs!",
    "                               Corre, Abe! Los sligs tienen orden de disparar en cuanto te vean!",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    ""};

static const StringTable* sPerLvlMessages[static_cast<u32>(LevelIds::eDesertEscape_15) + 1][99] = {};

void SetLcdMessagesForLvl(const StringTable& msgs, LevelIds lvl, u32 pathId)
{
    sPerLvlMessages[static_cast<u32>(lvl)][pathId] = &msgs;
}

class LCDMessages final
{
public:
    const char_type* GetMessage(LevelIds lvlId, u32 pathId, u32 msgId) const
    {
        const StringTable* pTable = sPerLvlMessages[static_cast<u32>(lvlId)][pathId];
        if (pTable && pTable->mStringCount > 0)
        {
            if (msgId < pTable->mStringCount)
            {
                return pTable->mStrings[msgId].string_ptr;
            }
            else
            {
                LOG_WARNING("LCD message out of bounds, using first message for id: " << msgId);
                return pTable->mStrings[0].string_ptr;
            }
        }

        if (msgId < ALIVE_COUNTOF(sLCDMessageTable_4C7420))
        {
            return sLCDMessageTable_4C7420[msgId];
        }
        else
        {
            LOG_WARNING("LCD message out of bounds using original message table id: " << msgId);
            return sLCDMessageTable_4C7420[0];
        }
    }
};
static LCDMessages gLCDMessages;

LCDScreen* LCDScreen::ctor_433F60(Path_LCDScreen* pTlv, s32 tlvInfo)
{
    ctor_487E10(1);
    SetVTable(this, 0x4BB468);

    field_2BC_tlv = *pTlv;

    field_2B8_tlv_item_info = tlvInfo;

    field_2AC_message_1_id = pTlv->field_18_message_1_id;

    field_50_font_context.LoadFontType_41C040(2);
    field_60_font.ctor_41C170(60, sLCDScreen_Palette_4C75A8, &field_50_font_context);

    IRenderer::PalRecord rec;
    rec.depth = 16;
    if (!IRenderer::GetRenderer()->PalAlloc(rec))
    {
        LOG_ERROR("PalAlloc failure");
    }

    field_98_pal_rect.x = rec.x;
    field_98_pal_rect.y = rec.y;
    field_98_pal_rect.w = rec.depth;
    field_98_pal_rect.h = 1;

    IRenderer::GetRenderer()->PalSetData(rec, sLCDScreen_Palette2_4C7588);

    if (Input_JoyStickEnabled() || field_2AC_message_1_id != 62)
    {
        String_FormatString_450DC0(gLCDMessages.GetMessage(gMap_507BA8.field_0_current_level, gMap_507BA8.field_2_current_path, field_2AC_message_1_id), field_AC_message_buffer);
    }
    else
    {
        strcpy(field_AC_message_buffer,
               "                               Para alertar a un mudokon de tu presencia, saludale manteniendo pulsada la tecla (1) del teclado. Luego puedes"
               " hablar con los Mudokon utilizando los numeros (1) al (8) del teclado. Experimenta!");
    }

    field_A0_message = field_AC_message_buffer;
    field_A4_message_cutoff_ptr = 0;
    field_2B0_x_offset = 0;

    field_6_flags.Set(Options::eDrawable_Bit4);

    field_2D8_message_rand_min = pTlv->field_1A_message_rand_min;
    pad_2DA = pTlv->field_1C_message_rand_max;

    field_2DC_message_rand_max = pTlv->field_1C_message_rand_max;

    sFontDrawScreenSpace_508BF4 = 1;
    field_2B4_character_width = field_60_font.MeasureWidth_41C200(field_AC_message_buffer[0]);
    sFontDrawScreenSpace_508BF4 = 0;

    field_2D4 = 0;
    gObjList_drawables_504618->Push_Back(this);

    return this;
}

BaseGameObject* LCDScreen::dtor_434100()
{
    SetVTable(this, 0x4BB468);
    IRenderer::GetRenderer()->PalFree(IRenderer::PalRecord{field_98_pal_rect.x, field_98_pal_rect.y, field_98_pal_rect.w});

    gObjList_drawables_504618->Remove_Item(this);
    gMap_507BA8.TLV_Reset_446870(field_2B8_tlv_item_info, -1, 0, 0);
    field_60_font.dtor_41C130();
    field_50_font_context.dtor_41C110();
    return dtor_487DF0();
}

BaseGameObject* LCDScreen::VDestructor(s32 flags)
{
    return Vdtor_434630(flags);
}

BaseGameObject* LCDScreen::Vdtor_434630(s32 flags)
{
    dtor_434100();
    if (flags & 1)
    {
        ao_delete_free_447540(this);
    }
    return this;
}

void LCDScreen::VScreenChanged()
{
    VScreenChanged_434620();
}

void LCDScreen::VScreenChanged_434620()
{
    field_6_flags.Set(BaseGameObject::eDead_Bit3);
}


void LCDScreen::VUpdate()
{
    VUpdate_4341B0();
}

void LCDScreen::VUpdate_4341B0()
{
    if (Event_Get_417250(kEventDeathReset_4))
    {
        field_6_flags.Set(BaseGameObject::eDead_Bit3);
    }

#if LCD_PS1_SPEED
    field_2B0_x_offset += 6;
#else
    field_2B0_x_offset += 3;
#endif

    if (field_2B0_x_offset > field_2B4_character_width)
    {
        field_2B0_x_offset -= field_2B4_character_width;
        char_type* pMsg = field_A0_message;
        field_A0_message++;
        if (!*pMsg)
        {
            field_A0_message = field_AC_message_buffer;
            field_2D4++;
            if (field_2D4 == 1)
            {
                const auto rangedRandom = Math_RandomRange_450F20(
                    field_2D8_message_rand_min,
                    field_2DC_message_rand_max);

                if (Input_JoyStickEnabled() || rangedRandom != 62)
                {
                    String_FormatString_450DC0(gLCDMessages.GetMessage(gMap_507BA8.field_0_current_level, gMap_507BA8.field_2_current_path, rangedRandom), field_AC_message_buffer);
                }
                else
                {
                    strcpy(
                        field_AC_message_buffer,
                        "                               Para alertar a un mudokon de tu presencia, saludale manteniendo pulsada la tecla (1) del teclado. Luego puedes hablar con los Mudokon utilizando los numeros (1) al (8) del teclado. Experimenta!");
                }
            }
            else
            {
                field_2D4 = 0;

                if (Input_JoyStickEnabled() || field_2AC_message_1_id != 62)
                {
                    String_FormatString_450DC0(
                        gLCDMessages.GetMessage(gMap_507BA8.field_0_current_level, gMap_507BA8.field_2_current_path, field_2AC_message_1_id),
                        field_AC_message_buffer);
                }
                else
                {
                    strcpy(
                        field_AC_message_buffer,
                        "                               Para alertar a un mudokon de tu presencia, saludale manteniendo pulsada la tecla (1) del teclado. Luego puedes hablar con los Mudokon utilizando los numeros (1) al (8) del teclado. Experimenta!");
                }
            }
            auto palSwap = field_98_pal_rect;
            field_98_pal_rect = field_60_font.field_28_palette_rect;
            field_60_font.field_28_palette_rect = palSwap;
        }
        sFontDrawScreenSpace_508BF4 = 1;
        field_2B4_character_width = field_60_font.MeasureWidth_41C200(*field_A0_message);
    }
    sFontDrawScreenSpace_508BF4 = 1;

    auto screenLeft = field_2BC_tlv.field_10_top_left.field_0_x - FP_GetExponent(pScreenManager_4FF7C8->field_10_pCamPos->field_0_x);
    auto screenRight = field_2BC_tlv.field_14_bottom_right.field_0_x - FP_GetExponent(pScreenManager_4FF7C8->field_10_pCamPos->field_0_x);

    const char_type* slicedText = field_60_font.SliceText_41C6C0(
        field_A0_message,
        PsxToPCX(screenLeft - pScreenManager_4FF7C8->field_14_xpos, 11) - field_2B0_x_offset,
        FP_FromInteger(1),
        screenRight - pScreenManager_4FF7C8->field_14_xpos);
    sFontDrawScreenSpace_508BF4 = 0;
    if (slicedText != field_A4_message_cutoff_ptr)
    {
        field_A4_message_cutoff_ptr = slicedText;
        if (*slicedText != ' ')
        {
            SFX_Play_43AD70(SoundEffect::LCDScreen_87, 0, 0);
        }
    }
}

void LCDScreen::VRender(PrimHeader** ppOt)
{
    VRender_434400(ppOt);
}

void LCDScreen::VRender_434400(PrimHeader** ppOt)
{
    if (sNumCamSwappers_507668 == 0)
    {
        const FP_Point* camPos = pScreenManager_4FF7C8->field_10_pCamPos;

        auto endY = field_2BC_tlv.field_10_top_left.field_2_y + field_2BC_tlv.field_14_bottom_right.field_2_y;
        auto endX = pScreenManager_4FF7C8->field_14_xpos + field_2BC_tlv.field_14_bottom_right.field_0_x;

        const s32 screenX = field_2BC_tlv.field_10_top_left.field_0_x - FP_GetExponent(camPos->field_0_x - FP_FromInteger(pScreenManager_4FF7C8->field_14_xpos));
        const s32 screenY = endY / 2 - FP_GetExponent(camPos->field_4_y - FP_FromInteger(pScreenManager_4FF7C8->field_16_ypos)) - 7;
        const s32 maxWidth = FP_GetExponent(FP_FromInteger(endX) - camPos->field_0_x);

        PSX_RECT clipRect = {
            0,
            0,
            640,
            static_cast<s16>(gPsxDisplay_504C78.field_2_height)};

        auto* pClippers = &field_10_prim_clippers[0][gPsxDisplay_504C78.field_A_buffer_index];
        Init_PrimClipper_495FD0(
            pClippers,
            &clipRect);
        OrderingTable_Add_498A80(OtLayer(ppOt, Layer::eLayer_BeforeWell_22), &pClippers->mBase);

        auto fontFlickerAmount = 50;
        if (sDisableFontFlicker_5080E4)
        {
            fontFlickerAmount = 0;
        }
        if (fontFlickerAmount)
        {
            fontFlickerAmount = 40;
        }

        sFontDrawScreenSpace_508BF4 = 1;
        field_60_font.DrawString_41C360(
            ppOt,
            field_A0_message,
            static_cast<s16>(PsxToPCX(screenX, 11) - field_2B0_x_offset),
            static_cast<s16>(screenY),
            TPageAbr::eBlend_1,
            1,
            0,
            Layer::eLayer_BeforeWell_22,
            127,
            127,
            127,
            0,
            FP_FromInteger(1),
            maxWidth,
            fontFlickerAmount);
        sFontDrawScreenSpace_508BF4 = 0;

        PSX_RECT clipRect2 = {};

        clipRect2.x = static_cast<s16>(PsxToPCX(screenX, 11));
        clipRect2.y = static_cast<s16>(screenY - 12);
        clipRect2.w = static_cast<s16>(PsxToPCX(maxWidth - screenX, 51));
        clipRect2.h = 48;

        auto* clipper = &field_10_prim_clippers[1][gPsxDisplay_504C78.field_A_buffer_index];
        Init_PrimClipper_495FD0(clipper, &clipRect2);
        OrderingTable_Add_498A80(OtLayer(ppOt, Layer::eLayer_BeforeWell_22), &clipper->mBase);

        pScreenManager_4FF7C8->InvalidateRect_406E40(
            clipRect2.x,
            clipRect2.y,
            clipRect2.w,
            24,
            pScreenManager_4FF7C8->field_2E_idx);
    }
}

} // namespace AO
