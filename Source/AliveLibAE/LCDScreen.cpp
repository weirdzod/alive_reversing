#include "stdafx.h"
#include "LCDScreen.hpp"
#include "Function.hpp"
#include "stdlib.hpp"
#include "SwitchStates.hpp"
#include "DebugHelpers.hpp"
#include "StringFormatters.hpp"
#include "Sound/Midi.hpp"
#include "Events.hpp"
#include "Sfx.hpp"
#include "VRam.hpp"
#include "Game.hpp"
#include "../AliveLibCommon/PathDataExtensionsTypes.hpp"
#include "Renderer/IRenderer.hpp"

u8 sLCDScreen_Palette[] = {
    0x00, 0x00, 0x01, 0x80, 0x01, 0x84, 0x20, 0x84, 0x21, 0x80,
    0x20, 0x84, 0x21, 0x84, 0x65, 0xCE, 0x65, 0x8C, 0x8C, 0xB1,
    0x60, 0x8E, 0x64, 0xCE, 0x65, 0xCE, 0xD7, 0x98, 0x14, 0xA1,
    0x18, 0xD8};

u8 sLCDScreen_Palette2[] = {
    0x00, 0x00, 0x01, 0x80, 0x01, 0x84, 0x20, 0x84, 0x21, 0x80,
    0x20, 0x84, 0x21, 0x84, 0x05, 0x84, 0x65, 0x8C, 0x8C, 0xB1,
    0x13, 0x94, 0x64, 0xCE, 0x65, 0xCE, 0xD7, 0x98, 0x14, 0xA1,
    0x18, 0xD8};



// TODO: Remove spaces and add them at runtime.
static const char_type* sLCDMessageTable_555768[101] = {
    "",
    "                               Mineria SoulStorm ofrece igualdad de oportunidades a sus empleados.",
    "                               A trabajar!",
    "                               El que rompe, paga.",
    "                               El Brebaje de SoulStorm, fresco, deprimente.",
    "                               Deprimido...? Pues trabaja mas!",
    "                               Necesitas vacaciones (permanentes)? Diselo al supervisor slig.",
    "                               Mangar huesos en los calzoncillos implica la eliminacion inmediata.",
    "                               MouthLube de Molluck --  Dilo, no lo escupas!",
    "                               Extractores de Comida FeeCo... Come el doble... Sientete bien... Por el mismo precio!",
    "                               Brebaje SoulStorm... Nuestro vomito es tu bebida!",
    "                               Pastel de scrab... MMMM. Te cuesta un ojo de la cara!",
    "                               Nuevo Brebaje SoulStorm...Doble sabor..doble de huesos...doble precio!",
    "                               Si estas mal, Brebaje!",
    "                               Brebaje SoulStorm: Bebetelo!",
    "                               Deposito Feeco: Donde iras hoy?",
    "                               Buscamos probadores de granadas y bombas. Presentarse al general Dripik. Solo personal cualificado.",
    "                               AzGasCo: Pedos de uno en uno.",
    "                               Encuentra el premio secreto dentro de cada paquete de carne de RuptureFarms!",
    "                               Novedad de RuptureFarms: Perritos calientes! Las salchichas nunca han sido tan guays!",
    "                               Se busca mudokon limpiador de fleech, sin experiencia.",
    "                               Vykkers Labs. Moldea el futuro a tu imagen.",
    "                               Pastahuesos phleg...buena hasta el final.",
    "                               Moolah Ticker, SolstrmBrw y FeeCo ARRIBA Bonwrkz ARRIBA... Granjas Hostiles Abajo! Abajo! Abajo!",
    "                               Sierrahuesos Blowco. El polvo perfecto - cada vez.",
    "                               Manten a tus empleados a oscuras con el Cegador 2000. La mejor maquina cegadora.",
    "                               Tinagrasa de FeeCo. Para grasas abundantes,tinas grandes.",
    "                               Vykkers Lab dice que el Brebaje es seguro! No pienses, Bebe!",
    "                               Legz Iz Uz: si encuentras mejor pata, comprala!",
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
    "                               Para entrar, ponte frente a la puerta y pulsa " kUp ".",
    "                               Para entrar al hueco, ponte delante y pulsa " kUp ".",
    "                               Para subir, ponte bajo los escombros que caen y pulsa " kUp ".",
    "                               Para brincar, pulsa " kJump ".",
    "                               Para despistar al slig dormido, pulsa " kSneak " al moverte.",
    "                               Para tirar de la palanca, acercate y  pulsa " kAction ".",
    "                               Para usar el anillo, ponte debajo y pulsa " kUp ", como para subir.",
    "                               Para subir a la plataforma, ponte en el centro y pulsa " kUp ".",
    "                               Para entrar al hueco, ponte delante y pulsa " kUp ".",
    "                               Para desactivar una bomba, agachate pulsando " kDown ". Luego pulsa " kAction " cuando la bomba este verde.  pulsa " kAction " de nuevo para reactivarla.",
    "                               Ve a las sombras pulsando " kSneak " mientras te mueves. Quedate quieto cuando veas aun slig, o te descubrira. Ponte detras del slig cuando se aleje de ti.",
    "                               Para sacar una roca del saco, ponte debajo y pulsa " kUp ". Para coger una roca, agachate y pulsa " kAction ". Para lanzarla, pulsa " kThrow ", y luego cualquier boton de direccion. Cada direccion la lanzara a distintos angulos.",
    "                               Practica tus lanzamientos. Ponte en la flecha y dale a las minas. Puedes lanzar agachandote.",
    "                               Para alertar a un mudokon, dile hola pulsando " kHello ". Luego, habla a los mudokons pulsando " kHello ", " kFollowMe ", " kWait ", " kWork ", " kAnger ", " kAllYa ", " kSorry " o " kStopIt ". Experimenta!",
    "                               Posee a los sligs cantando. Para ello, pulsa " kChant ".",
    "                               Ayuda a los mudokons a escapar de la esclavitud al ver un circulo de pajaros. Para ello, pulsa " kChant ".",
    "                               Para rodar, agachate pulsando " kDown ". Luego rueda a la derecha o izquierda pulsando " kLeft " o " kRight ".",
    "                               Para caminar, pulsa " kLeft " o " kRight ". Para correr, pulsa " kRun " al moverte.",
    "                               Para evitar una alarma, estate quieto cuando los detectores te toquen.",
    "                               Habla slig para pasar la puerta de seguridad.",
    "                               Sligs hablan con slogs.",
    "                               Usa la palanca con la sierra de huesos.",
    "                               Lleva a los mudokons a los portales de los pajaros, y canta para rescatarlos. Canta pulsando " kChant ".",
    "                               Para girar la llave, ponte delante y pulsa " kUp ".",
    "                               Tras decir hola al Mudokon, dile que lo sientes poniendote a su lado y pulsando " kSorry ".",
    "                               Rueda mientras corres pulsando " kFart ".",
    "                               Ponte sobre la flecha y pulsa " kDown ".",
    "                               Hay sligs... silencio.",
    "                               El gas de la risa aturde a los mudokons.",
    "                               El unico modo de calmar a un mudokon es abofeteandolo. Para ello, ponte cerca y pulsa " kAnger ".",
    "                               Ponte detras del slig cuando se aleje de ti.",
    "                               Necesitaras ayuda para girar todas las ruedas a la vez. Pon un Mudokon delante de una y dile que trabaje pulsando " kWork ". Abofetealo si tienes que calmarlo.",
    "                               Si te escabulles, los mudokons que te siguen te imitaran.",
    "                               Di hola a todos los mudokons a la vez pulsando " kAllYa ". Luego, diles que te sigan pulsando " kFollowMe ".",
    "                               Intenta rebotar rocas en la pared.",
    "                               Los mudokons ciegos andaran hasta que les digas que esperen. Para ello, pulsa " kWait ".",
    "                               Solo un asesino o un idiota tiraria de esta palanca.",
    "                               Haz que un slig poseido haga explotar algo en primer plano pulsando " kThrow " mientras pulsas " kDown ".",
    "                               Pulsa " kJump " para brincar.",
    "                               Para entrar al coche de la mina, ponte delante y pulsa " kUp ". Para salir, pulsa  " kAction ". Para hacer que caiga del techo, pulsa " kAction ".",
    "",
    "",
    "",
    "",
    "",
    "",
    "                               Para correr, pulsa " kRun " al moverte.",
    "                               Si estas callado en las sombras,no te pueden ver.",
    "",
    "",
    "",
    "",
};


static const StringTable* sPerLvlMessages[static_cast<u32>(LevelIds::eCredits_16) + 1][99] = {};

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

        if (msgId < ALIVE_COUNTOF(sLCDMessageTable_555768))
        {
            return sLCDMessageTable_555768[msgId];
        }
        else
        {
            LOG_WARNING("LCD message out of bounds using original message table id: " << msgId);
            return sLCDMessageTable_555768[0];
        }
    }
};
static LCDMessages gLCDMessages;

LCDScreen* LCDScreen::ctor_460680(Path_LCDScreen* params, TlvItemInfoUnion itemInfo)
{
    BaseGameObject_ctor_4DBFA0(1, 0);
    SetVTable(this, 0x545AAC);

    field_C_objectId = itemInfo.all;

    field_2C0_tlv = *params;

    field_2AA_message_1_id = params->field_10_message_1_id;
    field_2B0_message_2_id = params->field_16_message_2_id;
    field_2B2_toggle_message_switch_id = static_cast<u16>(params->field_18_toggle_message_switch_id);
    field_2BC_tlv_item_info = itemInfo;

    if (!sFontType2LoadCount_5BC5E8)
    {
        sFont2Context_5BC5D8.LoadFontType_433400(2);
    }
    sFontType2LoadCount_5BC5E8++;

    field_60_font.ctor_433590(60, sLCDScreen_Palette, &sFont2Context_5BC5D8);

    IRenderer::PalRecord rec;
    rec.depth = 16;
    if (!IRenderer::GetRenderer()->PalAlloc(rec))
    {
        LOG_ERROR("PalAlloc failed");
    }

    field_98_pal_rect.x = rec.x;
    field_98_pal_rect.y = rec.y;
    field_98_pal_rect.w = rec.depth;
    field_98_pal_rect.h = 1;

    IRenderer::GetRenderer()->PalSetData(rec, sLCDScreen_Palette2);

    if (SwitchStates_Get_466020(field_2B2_toggle_message_switch_id))
    {
        field_A0_message = gLCDMessages.GetMessage(gMap_5C3030.field_0_current_level, gMap_5C3030.field_2_current_path, field_2B0_message_2_id);
    }
    else
    {
        field_A0_message = gLCDMessages.GetMessage(gMap_5C3030.field_0_current_level, gMap_5C3030.field_2_current_path, field_2AA_message_1_id);
    }

    //field_A0_message = "                               THIS IS A TEST";

    String_FormatString_4969D0(field_A0_message, field_A8_message_buffer, 512, 1);
    field_A0_message = field_A8_message_buffer;
    field_A4_message_cutoff_ptr = nullptr;
    field_2AC_x_offset = 0;
    sFontDrawScreenSpace_5CA4B4 = 1;
    field_2AE_character_width = static_cast<u16>(field_60_font.MeasureWidth_433630(*field_A0_message) + 2);
    sFontDrawScreenSpace_5CA4B4 = 0;
    field_2B4_show_random_message = 1;
    field_2B6_message_rand_min_id = params->field_12_message_rand_min_id;
    field_6_flags.Set(BaseGameObject::eDrawable_Bit4);
    field_2B8_message_rand_max_id = params->field_14_message_rand_max_id;
    field_2A8_play_sound_toggle = 0;
    gObjList_drawables_5C1124->Push_Back_40CAF0(this);

    return this;
}

void LCDScreen::Update_460A00()
{
    if (Event_Get_422C00(kEventDeathReset))
    {
        field_6_flags.Set(BaseGameObject::eDead_Bit3);
    }

#if LCD_PS1_SPEED
    field_2AC_x_offset += 6;
#else
    field_2AC_x_offset += 3;
#endif

    if (field_2AC_x_offset > field_2AE_character_width)
    {
        field_2AC_x_offset -= field_2AE_character_width;
        s8 lastChar = *field_A0_message;
        field_A0_message++; // Offset s8 index

        if (lastChar == 0)
        {
            if (field_2B4_show_random_message == 1)
            {
                field_2B4_show_random_message = 0;
                field_A0_message = gLCDMessages.GetMessage(gMap_5C3030.field_0_current_level, gMap_5C3030.field_2_current_path, Math_RandomRange_496AB0(field_2B6_message_rand_min_id, field_2B8_message_rand_max_id));
            }
            else
            {
                field_2B4_show_random_message = 1;
                if (SwitchStates_Get_466020(field_2B2_toggle_message_switch_id))
                {
                    field_A0_message = gLCDMessages.GetMessage(gMap_5C3030.field_0_current_level, gMap_5C3030.field_2_current_path, field_2B0_message_2_id);
                }
                else
                {
                    field_A0_message = gLCDMessages.GetMessage(gMap_5C3030.field_0_current_level, gMap_5C3030.field_2_current_path, field_2AA_message_1_id);
                }
            }

            String_FormatString_4969D0(field_A0_message, field_A8_message_buffer, 512, 1);
            field_A0_message = field_A8_message_buffer;

            auto palSwap = field_98_pal_rect;
            field_98_pal_rect = field_60_font.field_28_palette_rect;
            field_60_font.field_28_palette_rect = palSwap;
        }

        sFontDrawScreenSpace_5CA4B4 = 1;
        field_2AE_character_width = static_cast<u16>(field_60_font.MeasureWidth_433630(*field_A0_message) + 2);
        sFontDrawScreenSpace_5CA4B4 = 0;
    }

    auto screenLeft = field_2C0_tlv.field_8_top_left.field_0_x - FP_GetExponent(pScreenManager_5BB5F4->field_20_pCamPos->field_0_x);
    auto screenRight = field_2C0_tlv.field_C_bottom_right.field_0_x - FP_GetExponent(pScreenManager_5BB5F4->field_20_pCamPos->field_0_x);

    sFontDrawScreenSpace_5CA4B4 = 1;
    auto slicedText = field_60_font.SliceText_433BD0(
        field_A0_message,
        PCToPsxX(screenLeft) - field_2AC_x_offset,
        FP_FromInteger(1),
        screenRight);
    sFontDrawScreenSpace_5CA4B4 = 0;
    if (slicedText != field_A4_message_cutoff_ptr)
    {
        field_2A8_play_sound_toggle = !field_2A8_play_sound_toggle;
        field_A4_message_cutoff_ptr = slicedText;
        if (*slicedText != ' ')
        {
            if (field_2A8_play_sound_toggle)
            {
                SFX_Play_46FA90(SoundEffect::LCDScreen_73, 0);
            }
        }
    }
}

void LCDScreen::Render_460CB0(PrimHeader** ppOt)
{
    if (sNum_CamSwappers_5C1B66 == 0)
    {
        const FP_Point* camPos = pScreenManager_5BB5F4->field_20_pCamPos;
        const s32 screenX = field_2C0_tlv.field_8_top_left.field_0_x - FP_GetExponent(camPos->field_0_x);
        const s32 screenY = ((field_2C0_tlv.field_8_top_left.field_2_y + field_2C0_tlv.field_C_bottom_right.field_2_y) / 2 - FP_GetExponent(camPos->field_4_y)) - 7;
        const s32 screenXWorld = PsxToPCX(screenX);
        const s32 maxWidth = field_2C0_tlv.field_C_bottom_right.field_0_x - FP_GetExponent(camPos->field_0_x);

        PSX_RECT clipRect = {
            0,
            0,
            640,
            240};

        Init_PrimClipper_4F5B80(&field_20_prim_clippers[0][gPsxDisplay_5C1130.field_C_buffer_index], &clipRect);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_RopeWebDrill_24), &field_20_prim_clippers[0][gPsxDisplay_5C1130.field_C_buffer_index].mBase);

        sFontDrawScreenSpace_5CA4B4 = 1;
        field_60_font.DrawString_4337D0(
            ppOt,
            field_A0_message,
            static_cast<s16>(screenXWorld - field_2AC_x_offset),
            static_cast<s16>(screenY),
            TPageAbr::eBlend_1,
            1,
            0,
            Layer::eLayer_RopeWebDrill_24,
            127,
            127,
            127,
            0,
            FP_FromInteger(1),
            maxWidth,
            sDisableFontFlicker_5C9304 != 0 ? 0 : 40);
        sFontDrawScreenSpace_5CA4B4 = 0;

        clipRect = {
            static_cast<s16>(screenXWorld),
            static_cast<s16>(screenY - 12),
            static_cast<s16>(PsxToPCX(maxWidth - screenX)),
            48};

        auto* clipper = &field_20_prim_clippers[1][gPsxDisplay_5C1130.field_C_buffer_index];
        Init_PrimClipper_4F5B80(clipper, &clipRect);
        OrderingTable_Add_4F8AA0(OtLayer(ppOt, Layer::eLayer_RopeWebDrill_24), &clipper->mBase);

        pScreenManager_5BB5F4->InvalidateRect_40EC90(
            screenXWorld,
            screenY,
            clipRect.w,
            24,
            pScreenManager_5BB5F4->field_3A_idx);
    }
}

void LCDScreen::vSetDead_460F10()
{
    field_6_flags.Set(BaseGameObject::eDead_Bit3);
}

void LCDScreen::dtor_460920()
{
    SetVTable(this, 0x545AAC); // vTbl_LCDScreen_545AAC

    IRenderer::GetRenderer()->PalFree(IRenderer::PalRecord{field_98_pal_rect.x, field_98_pal_rect.y, field_98_pal_rect.w});

    gObjList_drawables_5C1124->Remove_Item(this);
    Path::TLV_Reset_4DB8E0(field_2BC_tlv_item_info.all, -1, 0, 0);

    if (!--sFontType2LoadCount_5BC5E8)
    {
        sFont2Context_5BC5D8.dtor_433510();
    }
    field_60_font.dtor_433540();
    BaseGameObject_dtor_4DBEC0();
}

BaseGameObject* LCDScreen::vdtor_4608F0(s32 flags)
{
    dtor_460920();
    if (flags & 1)
    {
        ae_delete_free_495540(this);
    }
    return this;
}

void LCDScreen::VUpdate()
{
    Update_460A00();
}

void LCDScreen::VRender(PrimHeader** ppOt)
{
    Render_460CB0(ppOt);
}

BaseGameObject* LCDScreen::VDestructor(s32 flags)
{
    return vdtor_4608F0(flags);
}

void LCDScreen::VScreenChanged()
{
    vSetDead_460F10();
}
