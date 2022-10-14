#include "stdafx_ao.h"
#include "Game.hpp"
#include "Function.hpp"
#include "../relive_lib/BaseGameObject.hpp"
#include "SwitchStates.hpp"
#include "DDCheat.hpp"
#include "Io.hpp"
#include "Psx.hpp"
#include "Sys.hpp"
#include "../relive_lib/DynamicArray.hpp"
#include "BaseAliveGameObject.hpp"
#include "../AliveLibAE/stdlib.hpp"
#include "ResourceManager.hpp"
#include "../relive_lib/PsxDisplay.hpp"
#include "Map.hpp"
#include "GameSpeak.hpp"
#include "CheatController.hpp"
#include "DDCheat.hpp"
#include "MusicController.hpp"
#include "VGA.hpp"
#include "Input.hpp"
#include "Midi.hpp"
#include "PauseMenu.hpp"
#include "Abe.hpp"
#include "../relive_lib/ShadowZone.hpp"
#include "CameraSwapper.hpp"
#include "AmbientSound.hpp"
#include "PsxRender.hpp"
#include "../relive_lib/ScreenManager.hpp"
#include "../relive_lib/Error.hpp"
#include "../relive_lib/Events.hpp"
#include "Sound.hpp"
#include "../AliveLibAE/Game.hpp"
#include "AddPointer.hpp"
#include "PathDataExtensions.hpp"
#include "GameAutoPlayer.hpp"
#include "../relive_lib/Error.hpp"

namespace AO {

DynamicArrayT<BaseGameObject>* gLoadingFiles = nullptr;

DynamicArrayT<BaseGameObject>* gPlatformsArray = nullptr;

void Game_ForceLink()
{
}

s16 gbKillUnsavedMudsDone_5076CC = 0;

// TODO: Move to game ender controller for AO sync
s16 gRestartRuptureFarmsKilledMuds_5076C4 = 0;
s16 gRestartRuptureFarmsSavedMuds_5076C8 = 0;

s16 gOldKilledMuds_5076D0 = 0;
s16 gOldSavedMuds_5076D4 = 0;

s16 sBreakGameLoop_507B78 = 0;
s16 gAttract_507698 = 0;
s32 gTimeOut_NotUsed_507B0C = 0;
s32 gFileOffset_NotUsed_507B10 = 0;

s8 gDDCheatMode_508BF8 = 0;
s8 byte_508BFC = 0;

s32 Game_End_Frame_4505D0(u32 bSkip)
{
    return Game_End_Frame_4950F0(bSkip);
}

static void Main_ParseCommandLineArguments()
{
    IO_Init_48E1A0(0);

    std::string windowTitle = WindowTitleAO();

    if (GetGameAutoPlayer().IsRecording())
    {
        windowTitle += " [Recording]";
    }
    else if (GetGameAutoPlayer().IsPlaying())
    {
        windowTitle += " [AutoPlay]";
    }

    Sys_WindowClass_Register("ABE_WINCLASS", windowTitle.c_str(), 32, 64, 640, 480);

    Sys_Set_Hwnd(Sys_GetWindowHandle());

    const LPSTR pCmdLine = Sys_GetCommandLine();
    if (pCmdLine)
    {
        if (_strcmpi(pCmdLine, "-it_is_me_your_father") == 0)
        {
            Input_GetCurrentKeyStates();
            if (Input_IsVKPressed(VK_SHIFT))
            {
                gDDCheatMode_508BF8 = 1;
                PSX_DispEnv_Set_48D900(2);
                PSX_EMU_Set_screen_mode_499910(2);
            }
        }
        // Force DDCheat
#if FORCE_DDCHEAT
        gDDCheatMode_508BF8 = 1;
#endif
    }

    if (!pCmdLine)
    {
        PSX_DispEnv_Set_48D900(2);
        PSX_EMU_Set_screen_mode_499910(2);
    }
    else
    {
        if (_strcmpi(pCmdLine, "-interline") == 0)
        {
            PSX_DispEnv_Set_48D900(1);
            PSX_EMU_Set_screen_mode_499910(1);
            byte_508BFC = 0;
        }
        else if (_strcmpi(pCmdLine, "-vstretch") == 0)
        {
            PSX_DispEnv_Set_48D900(0);
            PSX_EMU_Set_screen_mode_499910(0);
            byte_508BFC = 0;
        }
        else if (_strcmpi(pCmdLine, "-vdouble") == 0)
        {
            PSX_DispEnv_Set_48D900(0);
            PSX_EMU_Set_screen_mode_499910(0);
            byte_508BFC = 1;
        }
        else
        {
            PSX_DispEnv_Set_48D900(2);
            PSX_EMU_Set_screen_mode_499910(2);
        }
    }

    Init_VGA_AndPsxVram();

    PSX_EMU_Init_49A1D0();
    PSX_EMU_VideoAlloc_49A2B0();
    PSX_EMU_SetCallBack_499920(1, Game_End_Frame_4505D0);
}

void Init_GameStates()
{
    sKilledMudokons_5076BC = gRestartRuptureFarmsKilledMuds_5076C4;
    sRescuedMudokons_5076C0 = gRestartRuptureFarmsSavedMuds_5076C8;
    gSwitchStates = {};
    gOldKilledMuds_5076D0 = 0;
    gOldSavedMuds_5076D4 = 0;
    gbKillUnsavedMudsDone_5076CC = 0;
}


void Init_Sound_DynamicArrays_And_Others_41CD20()
{
    DebugFont_Init();

    pPauseMenu_5080E0 = nullptr;
    sActiveHero = nullptr;
    sControlledCharacter = nullptr;
    gNumCamSwappers = 0;
    sGnFrame = 0;

    gFilesPending_507714 = 0;
    bLoadingAFile_50768C = 0;

    gPlatformsArray = relive_new DynamicArrayT<BaseGameObject>(20);

    ObjList_5009E0 = relive_new DynamicArrayT<ResourceManager::ResourceManager_FileRecord>(10); // not used in AE

    ShadowZone::MakeArray();

    gBaseAliveGameObjects = relive_new DynamicArrayT<BaseAliveGameObject>(20);

    gLoadingFiles = relive_new DynamicArrayT<BaseGameObject>(20); // TODO: Leaked on purpose for now

    SND_Init_476E40();
    SND_Init_Ambiance();
    MusicController::Create();

    Init_GameStates(); // Note: inlined

    // TODO: The switch state clearing is done in Init_GameStates in AE
    // check this is not an AO bug
    SwitchStates_ClearAll();
}

void Game_Init_LoadingIcon_445E30()
{
    /*
    u8** ppRes = ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kLoadingAOResID, 1, 0);
    if (!ppRes)
    {
        ResourceManager::LoadResourceFile_455270("LOADING.BAN", nullptr);
        ppRes = ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kLoadingAOResID, 1, 0);
    }
    ResourceManager::Set_Header_Flags_4557D0(ppRes, ResourceManager::ResourceHeaderFlags::eNeverFree);
    */
}

void Game_Free_LoadingIcon_445E80()
{
    /*
    u8** ppRes = ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kLoadingAOResID, 0, 0);
    if (ppRes)
    {
        ResourceManager::FreeResource_455550(ppRes);
    }
    */
}


using TExitGameCB = AddPointer_t<void CC()>;

TExitGameCB sGame_OnExitCallback_9F664C = nullptr;

void Game_SetExitCallBack_48E040(TExitGameCB)
{
    
}

void Game_ExitGame_450730()
{
    PSX_EMU_VideoDeAlloc_49A550();
}


s32 CreateTimer_48F030(s32, void*)
{
    
    return 0;
}

void Game_Shutdown_48E050()
{
    if (sGame_OnExitCallback_9F664C)
    {
        sGame_OnExitCallback_9F664C();
        sGame_OnExitCallback_9F664C = nullptr;
    }

    CreateTimer_48F030(0, nullptr); // Creates a timer that calls a call back which is always null, therefore seems like dead code?
    Input_DisableInput();
    //SND_MCI_Close_493C30(); // TODO: Seems like more dead code because the mci is never set?
    SND_SsQuit();
    IO_Stop_ASync_IO_Thread_491A80();
    VGA_Shutdown_4900E0();
    Error_ShowErrorStackToUser(true);
}


void Game_Loop_437630()
{
    sBreakGameLoop_507B78 = 0;

    while (!sBreakGameLoop_507B78 && !gBaseGameObjects->Empty())
    {
        EventsResetActive();

        // Update objects
        GetGameAutoPlayer().SyncPoint(SyncPoints::StartGameObjectUpdate);
        for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
        {
            BaseGameObject* pObjIter = gBaseGameObjects->ItemAt(i);
            if (!pObjIter)
            {
                break;
            }

            if (pObjIter->mBaseGameObjectFlags.Get(BaseGameObject::eUpdatable_Bit2) && !pObjIter->mBaseGameObjectFlags.Get(BaseGameObject::eDead) && (gNumCamSwappers == 0 || pObjIter->mBaseGameObjectFlags.Get(BaseGameObject::eUpdateDuringCamSwap_Bit10)))
            {
                const s32 updateDelay = pObjIter->UpdateDelay();
                if (pObjIter->UpdateDelay() > 0)
                {
                    pObjIter->SetUpdateDelay(updateDelay - 1);
                }
                else
                {
                    pObjIter->VUpdate();
                }
            }
        }

        for (s32 i = 0; i < gLoadingFiles->Size(); i++)
        {
            BaseGameObject* pObjIter = gLoadingFiles->ItemAt(i);
            if (pObjIter->mBaseGameObjectFlags.Get(BaseGameObject::eUpdatable_Bit2) && !pObjIter->mBaseGameObjectFlags.Get(BaseGameObject::eDead) && (gNumCamSwappers == 0 || pObjIter->mBaseGameObjectFlags.Get(BaseGameObject::eUpdateDuringCamSwap_Bit10)))
            {
                const s32 updateDelay = pObjIter->UpdateDelay();
                if (pObjIter->UpdateDelay() > 0)
                {
                    pObjIter->SetUpdateDelay(updateDelay - 1);
                }
                else
                {
                    pObjIter->VUpdate();
                }
            }

            if (pObjIter->mBaseGameObjectFlags.Get(BaseGameObject::eDead) && pObjIter->mBaseGameObjectRefCount == 0)
            {
                i = gLoadingFiles->RemoveAt(i);
                relive_delete pObjIter;
            }
        }

        GetGameAutoPlayer().SyncPoint(SyncPoints::EndGameObjectUpdate);

        // Animate everything
        if (gNumCamSwappers <= 0)
        {
            AnimationBase::AnimateAll(AnimationBase::gAnimations);
        }

        // Render objects
        PrimHeader** ppOt = gPsxDisplay.mDrawEnvs[gPsxDisplay.mBufferIndex].mOrderingTable;

        for (s32 i = 0; i < gObjListDrawables->Size(); i++)
        {
            BaseGameObject* pDrawable = gObjListDrawables->ItemAt(i);
            if (!pDrawable)
            {
                break;
            }

            if (!pDrawable->mBaseGameObjectFlags.Get(BaseGameObject::eDead) && pDrawable->mBaseGameObjectFlags.Get(BaseGameObject::eDrawable_Bit4))
            {
                pDrawable->VRender(ppOt);
            }
        }

        DebugFont_Flush();
        PSX_DrawSync_496750(0);
        pScreenManager->VRender(ppOt);
        SYS_EventsPump();

        gPsxDisplay.RenderOrderingTable();

        // Destroy objects with certain flags
        for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
        {
            BaseGameObject* pObj = gBaseGameObjects->ItemAt(i);
            if (!pObj)
            {
                break;
            }

            if (pObj->mBaseGameObjectFlags.Get(BaseGameObject::eDead) && pObj->mBaseGameObjectRefCount == 0)
            {
                i = gBaseGameObjects->RemoveAt(i);
                delete pObj;
            }
        }

        gMap.ScreenChange();
        Input().Update(GetGameAutoPlayer());

        if (gNumCamSwappers == 0)
        {
            sGnFrame++;
        }

        GetGameAutoPlayer().ValidateObjectStates();

    } // Main loop end

    //const PSX_RECT rect = {0, 0, 368, 480};
    //PSX_ClearImage_496020(&rect, 0, 0, 0);
    PSX_DrawSync_496750(0);
    PSX_VSync_496620(0);

    for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
    {
        BaseGameObject* pObjToKill = gBaseGameObjects->ItemAt(i);
        if (!pObjToKill)
        {
            break;
        }

        if (pObjToKill->mBaseGameObjectRefCount == 0)
        {
            i = gBaseGameObjects->RemoveAt(i);
            delete pObjToKill;
        }
    }
}

void DDCheat_Allocate_409560()
{
    relive_new DDCheat();
}

void Game_Run_4373D0()
{
    SYS_EventsPump();

    gAttract_507698 = 0;
    gTimeOut_NotUsed_507B0C = 6000;
    gFileOffset_NotUsed_507B10 = 34;

    DDCheat::DebugStr("Abe's Oddysee Attract=%d Timeout=%d FileOffset=%d DA Track=NA\n", 0, 200, 34);
    SYS_EventsPump();
    PSX_ResetCallBack_49AFB0();

    //Nop_49BAF0();
    //Nop_49BB50();

    gPsxDisplay.Init();
    Input().InitPad(1);

    gBaseGameObjects = relive_new DynamicArrayT<BaseGameObject>(90);

    BaseAnimatedWithPhysicsGameObject::MakeArray();

    AnimationBase::CreateAnimationArray();

    Init_Sound_DynamicArrays_And_Others_41CD20();
    Input_Init();

#if DEVELOPER_MODE
    // Boot directly to the "abe hello" screen
    gMap.Init(EReliveLevelIds::eMenu, 1, 1, CameraSwapEffects::eInstantChange_0, 0, 0);
#else
    // Normal copy right screen boot
    gMap.Init(EReliveLevelIds::eMenu, 1, 10, CameraSwapEffects::eInstantChange_0, 0, 0);
#endif

    DDCheat_Allocate_409560();

    gEventSystem = relive_new GameSpeak();

    gCheatController = relive_new CheatController();

    Game_Init_LoadingIcon_445E30();
    Game_Loop_437630();
    Game_Free_LoadingIcon_445E80();

    DDCheat::ClearProperties();

    gMap.Shutdown();

    AnimationBase::FreeAnimationArray();
    BaseAnimatedWithPhysicsGameObject::FreeArray();
    relive_delete gBaseGameObjects;

    MusicController::Shutdown();
    SND_Reset_Ambiance();
    SND_Shutdown_476EC0();
    PSX_CdControlB_49BB40(8, 0, 0);
    PSX_ResetCallBack_49AFB0();
    PSX_StopCallBack_49AFC0();
    InputObject::Shutdown();
    PSX_ResetGraph_4987E0(3);

    DDCheat::DebugStr("Abe's Oddysee Demo Done\n");
}


void Game_Main()
{
    GetGameAutoPlayer().ParseCommandLine(Sys_GetCommandLine());

    Main_ParseCommandLineArguments();
    Game_SetExitCallBack_48E040(Game_ExitGame_450730);

    Game_Run_4373D0();

    // TODO: AE inlined calls here (pull AE's code into another func)
    Game_Shutdown_48E050();
}

} // namespace AO
