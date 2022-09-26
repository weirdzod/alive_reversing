#include "stdafx.h"
#include "Map.hpp"
#include "PathData.hpp"
#include "Function.hpp"
#include "../relive_lib/ScreenManager.hpp"
#include "ResourceManager.hpp"
#include "LvlArchive.hpp"
#include "Sound/Midi.hpp"
#include "SwitchStates.hpp"
#include "Game.hpp"
#include "Abe.hpp"
#include "MusicController.hpp"
#include "BackgroundMusic.hpp"
#include "stdlib.hpp"
#include "Path.hpp"
#include "QuikSave.hpp"
#include "Text.hpp"
#include "Sfx.hpp"
#include "FG1.hpp"
#include "CameraSwapper.hpp"
#include "MainMenu.hpp"
#include "../relive_lib/Events.hpp"
#include "Movie.hpp"
#include "../relive_lib/Particle.hpp"
#include "Door.hpp"
#include "Sound/PsxSpuApi.hpp"
#include "Sys.hpp"
#include <assert.h>

#include "../relive_lib/data_conversion/file_system.hpp"
#include "../relive_lib/data_conversion/data_conversion.hpp" // TODO: don't include this in the engine
#include "../relive_lib/data_conversion/relive_tlvs.hpp"
#include "../relive_lib/data_conversion/relive_tlvs_serialization.hpp"

ALIVE_VAR(1, 0x5c311c, s16, sMap_bDoPurpleLightEffect_5C311C, 0);
ALIVE_VAR(1, 0x5c3118, Camera*, sCameraBeingLoaded_5C3118, nullptr);
ALIVE_VAR(1, 0x5c3120, u32, sSoundChannelsMask_5C3120, 0);


// Map Path_ChangeTLV::field_18_wipe to CameraSwapEffects
const CameraSwapEffects kPathChangeEffectToInternalScreenChangeEffect_55D55C[10] = {
    CameraSwapEffects::ePlay1FMV_5,
    CameraSwapEffects::eRightToLeft_2,
    CameraSwapEffects::eLeftToRight_1,
    CameraSwapEffects::eBottomToTop_4,
    CameraSwapEffects::eTopToBottom_3,
    CameraSwapEffects::eBoxOut_8,
    CameraSwapEffects::eVerticalSplit_6,
    CameraSwapEffects::eHorizontalSplit_7,
    CameraSwapEffects::eUnknown_11,
    CameraSwapEffects::eInstantChange_0};

void Map::ScreenChange_Common()
{
    if (mCamState == CamChangeStates::eSliceCam_1)
    {
        ResourceManager::Reclaim_Memory_49C470(0);
        Handle_PathTransition();
    }
    else if (mCamState == CamChangeStates::eInstantChange_2)
    {
        ResourceManager::Reclaim_Memory_49C470(0);
        GoTo_Camera();
    }

    mCamState = CamChangeStates::eInactive_0;

    SND_Stop_Channels_Mask(sSoundChannelsMask_5C3120);
    sSoundChannelsMask_5C3120 = 0;
}

void Map::ScreenChange()
{
    if (mCamState == CamChangeStates::eInactive_0)
    {
        return;
    }

    if (sMap_bDoPurpleLightEffect_5C311C)
    {
        RemoveObjectsWithPurpleLight(1);
    }

    PSX_DrawSync_4F6280(0);

    for (s32 i = 0; i < 2; i++) // Not sure why this is done twice?
    {
        DynamicArrayIter iter = {};
        iter.field_4_idx = 0;
        iter.field_0_pDynamicArray = gBaseGameObjects;

        while (iter.field_4_idx < iter.field_0_pDynamicArray->field_4_used_size)
        {
            BaseGameObject* pItem = gBaseGameObjects->ItemAt(iter.field_4_idx);
            ++iter.field_4_idx;
            if (!pItem)
            {
                break;
            }

            pItem->VScreenChanged();

            // Did the screen change kill the object?
            if (pItem->mBaseGameObjectFlags.Get(BaseGameObject::eDead))
            {
                iter.Remove_At_Iter();
                delete pItem;
            }
        }
    }

    ResourceManager::NoEffect_49C700();

    //dword_5CA4A8 = 0; // TODO: Never used?

    // TODO: Refactor this logic
    if (!sMap_bDoPurpleLightEffect_5C311C && mNextLevel == mCurrentLevel)
    {
        ScreenChange_Common();
        return;
    }

    if (mNextLevel != mCurrentLevel)
    {
        SsUtAllKeyOff_4FDFE0(0);
    }

    if (mNextLevel != EReliveLevelIds::eNone)
    {
        if (mNextLevel == EReliveLevelIds::eCredits)
        {
            sSoundChannelsMask_5C3120 = 0;
            ScreenChange_Common();
            return;
        }
    }
    else if (mCurrentLevel == EReliveLevelIds::eMenu)
    {
        sSoundChannelsMask_5C3120 = 0;
        ScreenChange_Common();
        return;
    }
    sSoundChannelsMask_5C3120 = SND_MIDI(0, 0, 36, 70, 0, 0);
    ScreenChange_Common();
}

void Map::FreePathResourceBlocks()
{
    mLoadedPaths.clear();
}

BinaryPath* Map::GetPathResourceBlockPtr(u32 pathId)
{
    for (auto& loadedPath : mLoadedPaths)
    {
        if (loadedPath->GetPathId() == pathId)
        {
            return loadedPath.get();
        }
    }
    return nullptr;
}

void Map::ClearPathResourceBlocks()
{
    mLoadedPaths.clear();
}

void Map::RemoveObjectsWithPurpleLight(s16 bMakeInvisible)
{
    auto pObjectsWithLightsArray = relive_new DynamicArrayT<BaseAnimatedWithPhysicsGameObject>(16);

    auto pPurpleLightArray = relive_new DynamicArrayT<Particle>(16);

    bool bAddedALight = false;
    for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
    {
        BaseGameObject* pObj = gBaseGameObjects->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj->mBaseGameObjectFlags.Get(BaseGameObject::eIsBaseAnimatedWithPhysicsObj_Bit5))
        {
            if (pObj->mBaseGameObjectFlags.Get(BaseGameObject::eDrawable_Bit4))
            {
                auto pBaseObj = static_cast<BaseAnimatedWithPhysicsGameObject*>(pObj);

                const PSX_RECT objRect = pBaseObj->VGetBoundingRect();

                if (pBaseObj->mVisualFlags.Get(BaseAnimatedWithPhysicsGameObject::VisualFlags::eDoPurpleLightEffect))
                {
                    if (pBaseObj->mAnim.mFlags.Get(AnimFlags::eBit3_Render))
                    {
                        if (!pBaseObj->mBaseGameObjectFlags.Get(BaseGameObject::eDead) && pBaseObj != sControlledCharacter && gMap.Rect_Location_Relative_To_Active_Camera(&objRect) == CameraPos::eCamCurrent_0)
                        {
                            pObjectsWithLightsArray->Push_Back(pBaseObj);
                            const FP k60Scaled = (pBaseObj->mSpriteScale * FP_FromInteger(60));
                            Particle* pPurpleLight = New_DestroyOrCreateObject_Particle(
                                FP_FromInteger((objRect.x + objRect.w) / 2),
                                FP_FromInteger(((objRect.y + objRect.h) / 2)) + k60Scaled,
                                pBaseObj->mSpriteScale);

                            if (pPurpleLight)
                            {
                                pPurpleLightArray->Push_Back(pPurpleLight);
                                bAddedALight = true;
                            }
                        }
                    }
                }
            }
        }
    }

    if (bAddedALight)
    {
        SFX_Play_Pitch(relive::SoundEffects::PossessEffect, 40, 2400);

        for (s32 counter = 0; counter < 12; counter++)
        {
            if (bMakeInvisible && counter == 4)
            {
                // Make all the objects that have lights invisible now that the lights have been rendered for a few frames
                for (s32 i = 0; i < pObjectsWithLightsArray->Size(); i++)
                {
                    BaseAnimatedWithPhysicsGameObject* pObj = pObjectsWithLightsArray->ItemAt(i);
                    if (!pObj)
                    {
                        break;
                    }
                    pObj->mAnim.mFlags.Clear(AnimFlags::eBit3_Render);
                }
            }

            for (s32 i = 0; i < pPurpleLightArray->Size(); i++)
            {
                Particle* pLight = pPurpleLightArray->ItemAt(i);
                if (!pLight)
                {
                    break;
                }

                if (!pLight->mBaseGameObjectFlags.Get(BaseGameObject::eDead))
                {
                    pLight->VUpdate();
                }
            }

            // TODO/HACK what is the point of the f64 loop? Why not do both in 1 iteration ??
            for (s32 i = 0; i < pPurpleLightArray->Size(); i++)
            {
                Particle* pLight = pPurpleLightArray->ItemAt(i);
                if (!pLight)
                {
                    break;
                }

                if (!pLight->mBaseGameObjectFlags.Get(BaseGameObject::eDead))
                {
                    pLight->mAnim.VDecode();
                }
            }

            for (s32 i = 0; i < gObjListDrawables->Size(); i++)
            {
                BaseGameObject* pDrawable = gObjListDrawables->ItemAt(i);
                if (!pDrawable)
                {
                    break;
                }

                if (!pDrawable->mBaseGameObjectFlags.Get(BaseGameObject::eDead))
                {
                    // TODO: Seems strange to check this flag, how did it get in the drawable list if its not a drawable ??
                    if (pDrawable->mBaseGameObjectFlags.Get(BaseGameObject::eDrawable_Bit4))
                    {
                        pDrawable->VRender(gPsxDisplay.mDrawEnvs[gPsxDisplay.mBufferIndex].mOrderingTable);
                    }
                }
            }

            PSX_DrawSync_4F6280(0);
            pScreenManager->VRender(gPsxDisplay.mDrawEnvs[gPsxDisplay.mBufferIndex].mOrderingTable);
            SYS_EventsPump_494580();
            gPsxDisplay.RenderOrderingTable();
        }

        if (bMakeInvisible)
        {
            // Make all the objects that had lights visible again
            for (s32 i = 0; i < pObjectsWithLightsArray->Size(); i++)
            {
                BaseAnimatedWithPhysicsGameObject* pObj = pObjectsWithLightsArray->ItemAt(i);
                if (!pObj)
                {
                    break;
                }
                pObj->mAnim.mFlags.Set(AnimFlags::eBit3_Render);
            }
        }
    }

    pObjectsWithLightsArray->field_4_used_size = 0;
    pPurpleLightArray->field_4_used_size = 0;

    relive_delete pObjectsWithLightsArray;
    relive_delete pPurpleLightArray;
}

void Map::Handle_PathTransition()
{
    relive::Path_PathTransition* pPathChangeTLV = nullptr;
    if (mAliveObj)
    {
        pPathChangeTLV = static_cast<relive::Path_PathTransition*>(sPathInfo->TLV_Get_At_4DB4B0(
            FP_GetExponent(mAliveObj->mXPos),
            FP_GetExponent(mAliveObj->mYPos),
            FP_GetExponent(mAliveObj->mXPos),
            FP_GetExponent(mAliveObj->mYPos),
            ReliveTypes::ePathTransition));
    }

    if (mAliveObj && pPathChangeTLV)
    {
        mNextLevel = pPathChangeTLV->mNextLevel;
        mNextPath = pPathChangeTLV->mNextPath;
        mNextCamera = pPathChangeTLV->mNextCamera;
        mFmvBaseId = pPathChangeTLV->mMovieId;

        mCameraSwapEffect = kPathChangeEffectToInternalScreenChangeEffect_55D55C[pPathChangeTLV->mWipeEffect];

        mAliveObj->mCurrentLevel = mNextLevel;
        mAliveObj->mCurrentPath = mNextPath;

        // Save value before pPathChangeTLV is deleted
        const auto nextScale = pPathChangeTLV->mNextPathScale;

        GoTo_Camera();

        switch (nextScale)
        {
            case relive::reliveScale::eFull:
                sActiveHero->mSpriteScale = FP_FromDouble(1.0);
                sActiveHero->mAnim.mRenderLayer = Layer::eLayer_AbeMenu_32;
                break;

            case relive::reliveScale::eHalf:
                sActiveHero->mSpriteScale = FP_FromDouble(0.5);
                sActiveHero->mAnim.mRenderLayer = Layer::eLayer_AbeMenu_Half_13;
                break;

            default:
                LOG_ERROR("Invalid scale " << (s32) nextScale);
                break;
        }

        CameraPos remapped = CameraPos::eCamInvalid_m1;
        switch (mMapDirection)
        {
            case MapDirections::eMapLeft_0:
                remapped = CameraPos::eCamLeft_3;
                break;
            case MapDirections::eMapRight_1:
                remapped = CameraPos::eCamRight_4;
                break;
            case MapDirections::eMapTop_2:
                remapped = CameraPos::eCamTop_1;
                break;
            case MapDirections::eMapBottom_3:
                remapped = CameraPos::eCamBottom_2;
                break;
        }

        mAliveObj->VOnPathTransition(
            mCamIdxOnX * mPathData->field_A_grid_width,
            mCamIdxOnY * mPathData->field_C_grid_height,
            remapped);
    }
    else
    {
        switch (mMapDirection)
        {
            case MapDirections::eMapLeft_0:
                mCamIdxOnX--;
                mCameraSwapEffect = CameraSwapEffects::eRightToLeft_2;
                break;
            case MapDirections::eMapRight_1:
                mCamIdxOnX++;
                mCameraSwapEffect = CameraSwapEffects::eLeftToRight_1;
                break;
            case MapDirections::eMapTop_2:
                mCamIdxOnY--;
                mCameraSwapEffect = CameraSwapEffects::eBottomToTop_4;
                break;
            case MapDirections::eMapBottom_3:
                mCamIdxOnY++;
                mCameraSwapEffect = CameraSwapEffects::eTopToBottom_3;
                break;
            default:
                break;
        }

        const BinaryPath* pPathRes = GetPathResourceBlockPtr(mCurrentPath);
        auto pCameraName = pPathRes->CameraName(mCamIdxOnX, mCamIdxOnY);

        // Convert the 2 digit camera number string to an integer
        mNextCamera = 1 * (pCameraName[7] - '0') + 10 * (pCameraName[6] - '0');

        GoTo_Camera();
    }
}

CameraPos Map::GetDirection(EReliveLevelIds level, s32 path, FP xpos, FP ypos)
{
    if (level != mCurrentLevel)
    {
        return CameraPos::eCamInvalid_m1;
    }

    if (path != mCurrentPath)
    {
        return CameraPos::eCamInvalid_m1;
    }

    PSX_RECT rect = {};
    rect.x = FP_GetExponent(xpos);
    rect.w = FP_GetExponent(xpos);
    rect.y = FP_GetExponent(ypos);
    rect.h = FP_GetExponent(ypos);

    CameraPos ret = Rect_Location_Relative_To_Active_Camera(&rect);

    PSX_RECT camWorldRect = {};
    if (!Get_Camera_World_Rect(ret, &camWorldRect))
    {
        return CameraPos::eCamInvalid_m1;
    }

    const FP x = FP_FromInteger(camWorldRect.x);
    const FP y = FP_FromInteger(camWorldRect.y);
    const FP w = FP_FromInteger(camWorldRect.w);
    const FP h = FP_FromInteger(camWorldRect.h);

    switch (ret)
    {
        case CameraPos::eCamCurrent_0:
            return ret;

        case CameraPos::eCamTop_1:
            if (ypos < y || xpos < x || xpos > w)
            {
                return CameraPos::eCamInvalid_m1;
            }
            return ypos > h ? CameraPos::eCamCurrent_0 : ret;

        case CameraPos::eCamBottom_2:
            if (ypos > h || xpos < x || xpos > w)
            {
                return CameraPos::eCamInvalid_m1;
            }
            return ypos < y ? CameraPos::eCamCurrent_0 : ret;

        case CameraPos::eCamLeft_3:
            if (xpos < x || ypos < y || ypos > h)
            {
                return CameraPos::eCamInvalid_m1;
            }
            return xpos > w ? CameraPos::eCamCurrent_0 : ret;

        case CameraPos::eCamRight_4:
            if (xpos > w || ypos < y || ypos > h)
            {
                return CameraPos::eCamInvalid_m1;
            }
            return xpos < x ? CameraPos::eCamCurrent_0 : ret;

        default:
            return CameraPos::eCamInvalid_m1;
    }
}

void Map::Init(EReliveLevelIds level, s16 path, s16 camera, CameraSwapEffects screenChangeEffect, s16 fmvBaseId, s16 forceChange)
{
    sPathInfo = relive_new Path();

    field_2C_camera_array[0] = nullptr;
    field_2C_camera_array[1] = nullptr;
    field_2C_camera_array[2] = nullptr;
    field_2C_camera_array[3] = nullptr;
    field_2C_camera_array[4] = nullptr;

    mOverlayId = -1;

    mCurrentCamera = static_cast<s16>(-1);
    mCurrentPath = static_cast<s16>(-1);
    mCurrentLevel = EReliveLevelIds::eNone;

    mForceLoad = 0;

    SetActiveCam(level, path, camera, screenChangeEffect, fmvBaseId, forceChange);
    GoTo_Camera();

    mCamState = CamChangeStates::eInactive_0;
}

void Map::Shutdown()
{
    sLvlArchive_5BC520.Free_433130();
    stru_5C3110.Free_433130();

    // Free Path resources
    FreePathResourceBlocks();

    // Free cameras
    for (s32 i = 0; i < ALIVE_COUNTOF(field_2C_camera_array); i++)
    {
        if (field_2C_camera_array[i])
        {
            relive_delete field_2C_camera_array[i];
            field_2C_camera_array[i] = nullptr;
        }
    }

    pScreenManager = nullptr;

    // Free 
    relive_delete sPathInfo;
    sPathInfo = nullptr;

    ResourceManager::Reclaim_Memory_49C470(0);
    Reset();
}

Map::Map()
{
    Reset();
}

Map::~Map()
{
    Shutdown();
}

void Map::Reset()
{
    for (s32 i = 0; i < ALIVE_COUNTOF(field_2C_camera_array); i++)
    {
        field_2C_camera_array[i] = nullptr;
    }

    ClearPathResourceBlocks();

    field_CC_unused = 1;
    mFreeAllAnimAndPalts = 0;
    mRestoreQuickSaveData = 0;
}

void Map::GoTo_Camera()
{
    s16 bShowLoadingIcon = FALSE;
    if (mCurrentLevel != EReliveLevelIds::eMenu && mCurrentLevel != EReliveLevelIds::eCredits && mCurrentLevel != EReliveLevelIds::eNone)
    {
        bShowLoadingIcon = TRUE;
    }

    if (mCameraSwapEffect == CameraSwapEffects::eUnknown_11)
    {
        CamResource nullRes;
        BaseGameObject* pFmvRet = FMV_Camera_Change(nullRes, this, mCurrentLevel);
        do
        {
            SYS_EventsPump_494580();

            for (s32 i = 0; i < gBaseGameObjects->Size(); i++)
            {
                BaseGameObject* pBaseGameObj = gBaseGameObjects->ItemAt(i);
                if (!pBaseGameObj)
                {
                    break;
                }

                if (pBaseGameObj->mBaseGameObjectFlags.Get(BaseGameObject::eUpdatable_Bit2))
                {
                    if (!(pBaseGameObj->mBaseGameObjectFlags.Get(BaseGameObject::eDead)) && (!sNum_CamSwappers_5C1B66 || pBaseGameObj->mBaseGameObjectFlags.Get(BaseGameObject::eUpdateDuringCamSwap_Bit10)))
                    {
                        const s32 updateDelay = pBaseGameObj->UpdateDelay();
                        if (updateDelay > 0)
                        {
                            pBaseGameObj->SetUpdateDelay(updateDelay - 1);
                        }
                        else
                        {
                            pBaseGameObj->VUpdate();
                        }
                    }
                }
            }
        }
        while (!pFmvRet->mBaseGameObjectFlags.Get(BaseGameObject::eDead));

        if (sSoundChannelsMask_5C3120)
        {
            SND_Stop_Channels_Mask(sSoundChannelsMask_5C3120);
        }
        sSoundChannelsMask_5C3120 = SND_MIDI(0, 0, 36, 70, 0, 0);
    }

    if (mCurrentLevel != EReliveLevelIds::eMenu && mCurrentLevel != EReliveLevelIds::eNone)
    {
        if (mNextLevel != mCurrentLevel
            || mForceLoad
            || (mNextPath != mCurrentPath && mCameraSwapEffect == CameraSwapEffects::ePlay1FMV_5))
        {
            Game_ShowLoadingIcon_482D80();
        }
    }

    if (mNextLevel != mCurrentLevel
        || mNextPath != mCurrentPath
        || mForceLoad)
    {
        mOverlayId = GetOverlayId();
    }




    if (mNextLevel != mCurrentLevel || mForceLoad)
    {
        pResourceManager_5C1BB0->LoadingLoop_465590(bShowLoadingIcon);

        // Free all cameras
        for (s32 i = 0; i < ALIVE_COUNTOF(field_2C_camera_array); i++)
        {
            if (field_2C_camera_array[i])
            {
                relive_delete field_2C_camera_array[i];
                field_2C_camera_array[i] = nullptr;
            }
        }

        if (mCurrentLevel != EReliveLevelIds::eNone)
        {
            // Close LVL archives
            sLvlArchive_5BC520.Free_433130();
            stru_5C3110.Free_433130();

            // Free all but the first ?
            FreePathResourceBlocks();

            sPathInfo->Free();

            if (mNextLevel != mCurrentLevel)
            {
                SND_Reset();
            }

            ResourceManager::Reclaim_Memory_49C470(0);
        }

        pResourceManager_5C1BB0->LoadingLoop_465590(bShowLoadingIcon);

        // Open LVL
        while (!sLvlArchive_5BC520.Open_Archive_432E80(CdLvlName(mNextLevel)))
        {
            if (gAttract_5C1BA0)
            {
                // NOTE: Dead branch? Given no attract directory exists
                char_type fileName[256] = {};
                strcpy(fileName, "ATTRACT");
                strcat(fileName, CdLvlName(mNextLevel));
                if (sLvlArchive_5BC520.Open_Archive_432E80(fileName))
                {
                    break;
                }
            }
            Display_Full_Screen_Message_Blocking(Path_Get_Unknown(mNextLevel), MessageType::eLongTitle_0);
        }


        // TODO: Load level_info.json so we know which path jsons to load for this level
        FileSystem::Path pathDir;
        pathDir.Append("relive_data").Append("ae").Append(ToString(MapWrapper::ToAE(mNextLevel))).Append("paths");

        FileSystem::Path levelInfo = pathDir;
        levelInfo.Append("level_info.json");

        FileSystem fs;
        const std::string jsonStr = fs.LoadToString(levelInfo);
        nlohmann::json j = nlohmann::json::parse(jsonStr);
        const auto& paths = j["paths"];
        for (const auto& path : paths)
        {
            FileSystem::Path pathJsonFile = pathDir;
            pathJsonFile.Append(path);
            const std::string pathJsonStr = fs.LoadToString(pathJsonFile);

            // TODO: set the res ptrs to the parsed json data
            nlohmann::json pathJson = nlohmann::json::parse(pathJsonStr);
            LOG_INFO("Cam count " << pathJson["cameras"].size());

            auto pathBuffer = std::make_unique<BinaryPath>(pathJson["id"]);
            pathBuffer->CreateFromJson(pathJson);
            mLoadedPaths.emplace_back(std::move(pathBuffer));
        }


        if (mNextLevel == mCurrentLevel)
        {
            MusicController::static_PlayMusic(MusicController::MusicTypes::eNone_0, sActiveHero, 0, 0);
        }
        else
        {
            SND_Load_VABS(Path_Get_MusicInfo(mNextLevel), Path_Get_Reverb(mNextLevel));
            SND_Load_Seqs(sSeqData_558D50.mSeqs, Path_Get_BsqFileName(mNextLevel));

            relive_new BackgroundMusic(Path_Get_BackGroundMusicId(mNextLevel));
        }

        if (!mForceLoad)
        {
            SwitchStates_SetRange(2, 255);
        }

        if (mFreeAllAnimAndPalts)
        {
            ResourceManager::Free_Resource_Of_Type_49C6B0(ResourceManager::Resource_Animation);
            ResourceManager::Free_Resource_Of_Type_49C6B0(ResourceManager::Resource_Palt);
            mFreeAllAnimAndPalts = FALSE;
        }
    }

    if (!mNextPath)
    {
        mNextPath = 1;
    }

    const s16 prevPathId = mCurrentPath;
    const EReliveLevelIds prevLevelId = mCurrentLevel;

    mCurrentPath = mNextPath;
    mCurrentLevel = mNextLevel;
    mCurrentCamera = mNextCamera;

    const PathBlyRec* pPathRec_1 = Path_Get_Bly_Record(mNextLevel, mNextPath);
    mPathData = pPathRec_1->field_4_pPathData;

    sPathInfo->Init(
        mPathData,
        mNextLevel,
        mNextPath,
        mNextCamera,
        GetPathResourceBlockPtr(mNextPath));

    if (sQuickSave_saved_switchResetters_count_BB234C > 0)
    {
        Quicksave_RestoreSwitchResetterStates();
    }

    char_type pStrBuffer[13] = {};
    Path_Format_CameraName(pStrBuffer, mNextLevel, mNextPath, mNextCamera);

    BinaryPath* pNextPath = GetPathResourceBlockPtr(mNextPath);
    for (auto& cam : pNextPath->GetCameras())
    {
        if (!strncmp(cam->mName.c_str(), pStrBuffer, sizeof(CameraName)))
        {
            mCamIdxOnX = static_cast<s16>(cam->mX);
            mCamIdxOnY = static_cast<s16>(cam->mY);
            break;
        }
    }

    field_24_camera_offset.x = FP_FromInteger(mCamIdxOnX * mPathData->field_A_grid_width);
    field_24_camera_offset.y = FP_FromInteger(mCamIdxOnY * mPathData->field_C_grid_height);

    // If map has changed then load new collision info
    if (prevPathId != mCurrentPath || prevLevelId != mCurrentLevel)
    {
        relive_delete sCollisions;
        sCollisions = relive_new Collisions(GetPathResourceBlockPtr(mCurrentPath)->GetCollisions());
    }

    if (mRestoreQuickSaveData)
    {
        QuikSave_RestoreBlyData(mRestoreQuickSaveData);
        mRestoreQuickSaveData = nullptr;
    }

    // Copy camera array and blank out the source
    for (s32 i = 0; i < ALIVE_COUNTOF(field_40_stru_5); i++)
    {
        field_40_stru_5[i] = field_2C_camera_array[i];
        field_2C_camera_array[i] = nullptr;
    }

    field_2C_camera_array[0] = Create_Camera(mCamIdxOnX, mCamIdxOnY, 1);
    field_2C_camera_array[3] = Create_Camera(mCamIdxOnX - 1, mCamIdxOnY, 0);
    field_2C_camera_array[4] = Create_Camera(mCamIdxOnX + 1, mCamIdxOnY, 0);
    field_2C_camera_array[1] = Create_Camera(mCamIdxOnX, mCamIdxOnY - 1, 0);
    field_2C_camera_array[2] = Create_Camera(mCamIdxOnX, mCamIdxOnY + 1, 0);

    // Free resources for each camera
    for (s32 i = 0; i < ALIVE_COUNTOF(field_40_stru_5); i++)
    {
        if (field_40_stru_5[i])
        {
            pResourceManager_5C1BB0->Free_Resources_For_Camera_4656F0(field_40_stru_5[i]);
        }
    }

    pResourceManager_5C1BB0->LoadingLoop_465590(bShowLoadingIcon);

    // Free each camera itself
    for (s32 i = 0; i < ALIVE_COUNTOF(field_40_stru_5); i++)
    {
        if (field_40_stru_5[i])
        {
            relive_delete field_40_stru_5[i];
            field_40_stru_5[i] = nullptr;
        }
    }

    Map::Load_Path_Items(field_2C_camera_array[0], LoadMode::ConstructObject_0);
    pResourceManager_5C1BB0->LoadingLoop_465590(bShowLoadingIcon);
    Map::Load_Path_Items(field_2C_camera_array[3], LoadMode::ConstructObject_0);
    Map::Load_Path_Items(field_2C_camera_array[4], LoadMode::ConstructObject_0);
    Map::Load_Path_Items(field_2C_camera_array[1], LoadMode::ConstructObject_0);
    Map::Load_Path_Items(field_2C_camera_array[2], LoadMode::ConstructObject_0);

    // Create the screen manager if it hasn't already been done (probably should have always been done by this point though?)
    if (!pScreenManager)
    {
        pScreenManager = relive_new ScreenManager(field_2C_camera_array[0]->field_C_pCamRes, &field_24_camera_offset);
    }

    sPathInfo->Loader_4DB800(mCamIdxOnX, mCamIdxOnY, LoadMode::ConstructObject_0, ReliveTypes::eNone); // none = load all
    if (prevPathId != mCurrentPath || prevLevelId != mCurrentLevel)
    {
        if (sActiveHero)
        {
            if (mCurrentPath == sActiveHero->mCurrentPath)
            {
                sActiveHero->VCheckCollisionLineStillValid(10);
            }
        }
    }

    Create_FG1s();

    if (mCameraSwapEffect == CameraSwapEffects::ePlay1FMV_5)
    {
        Map::FMV_Camera_Change(field_2C_camera_array[0]->field_C_pCamRes, this, mNextLevel);
    }

    if (mCameraSwapEffect == CameraSwapEffects::eUnknown_11)
    {
        pScreenManager->DecompressCameraToVRam(field_2C_camera_array[0]->field_C_pCamRes);
        pScreenManager->EnableRendering();
    }

    if (prevLevelId != mCurrentLevel)
    {
        pResourceManager_5C1BB0->LoadingLoop_465590(FALSE);
    }

    if (mCameraSwapEffect != CameraSwapEffects::ePlay1FMV_5 && mCameraSwapEffect != CameraSwapEffects::eUnknown_11)
    {
        if (mDoorTransition)
        {
            // TODO: Add template helpers

            // Door transition
            relive::Path_Door* pDoorTlv = static_cast<relive::Path_Door*>(sPathInfo->TLV_First_Of_Type_In_Camera(ReliveTypes::eDoor, 0));
            while (pDoorTlv->mDoorId != sActiveHero->field_1A0_door_id)
            {
                pDoorTlv = static_cast<relive::Path_Door*>(Path::TLV_Next_Of_Type(pDoorTlv, ReliveTypes::eDoor));
            }

            CreateScreenTransistionForTLV(pDoorTlv);
        }
        else
        {
            if (!mTeleporterTransition)
            {
                relive_new CameraSwapper(field_2C_camera_array[0]->field_C_pCamRes, mCameraSwapEffect, 368 / 2, 240 / 2);
            }
            else
            {
                // TODO: Add template helpers

                // Teleporter transition
                relive::Path_Teleporter* pTeleporterTlv = static_cast<relive::Path_Teleporter*>(sPathInfo->TLV_First_Of_Type_In_Camera(ReliveTypes::eTeleporter, 0));
                while (pTeleporterTlv->mTeleporterId != sActiveHero->field_1A0_door_id)
                {
                    pTeleporterTlv = static_cast<relive::Path_Teleporter*>(Path::TLV_Next_Of_Type(pTeleporterTlv, ReliveTypes::eTeleporter));
                }

                CreateScreenTransistionForTLV(pTeleporterTlv);
            }
        }
    }

    bHideLoadingIcon_5C1BAA = 0;
    loading_ticks_5C1BAC = 0;

    mForceLoad = 0;

    if (sSoundChannelsMask_5C3120)
    {
        SND_Stop_Channels_Mask(sSoundChannelsMask_5C3120);
        sSoundChannelsMask_5C3120 = 0;
    }
}

void Map::TLV_Reset(const Guid& tlvId, s16 hiFlags, s8 bSetCreated, s8 bSetDestroyed)
{
    Path::TLV_Reset(tlvId, hiFlags, bSetCreated, bSetDestroyed);
}

Camera* Map::GetCamera(CameraPos pos)
{
    return field_2C_camera_array[static_cast<s32>(pos)];
}

void Map::CreateScreenTransistionForTLV(relive::Path_TLV* pTlv)
{
    // TODO: Refactor
    const s16 doorYDiff = static_cast<s16>(pTlv->mTopLeftY - FP_GetExponent(pScreenManager->CamYPos()));
    const s16 midX = (pTlv->mTopLeftX + pTlv->mBottomRightX) / 2;
    const s16 rightPos = static_cast<s16>(midX - FP_GetExponent(pScreenManager->CamXPos()));
    relive_new CameraSwapper(field_2C_camera_array[0]->field_C_pCamRes, mCameraSwapEffect, rightPos, doorYDiff);
}

void Map::Get_map_size(PSX_Point* pPoint)
{
    pPoint->x = mPathData->field_4_bTop;
    pPoint->y = mPathData->field_6_bBottom;
}

void Map::GetCurrentCamCoords(PSX_Point* pPoint)
{
    pPoint->x = mCamIdxOnX * mPathData->field_A_grid_width;
    pPoint->y = mCamIdxOnY * mPathData->field_C_grid_height;
}

void Map::Get_Abe_Spawn_Pos(PSX_Point* pPoint)
{
    pPoint->x = mPathData->field_1A_abe_start_xpos;
    pPoint->y = mPathData->field_1C_abe_start_ypos;
}

s16 Map::GetOverlayId()
{
    // TODO: Probably need to redo field_C data as 1 bytes instead of a word
    return Path_Get_Bly_Record(mNextLevel, mNextPath)->field_C_overlay_id & 0xFF;
}

void Map::Create_FG1s()
{
    Camera* pCamera = field_2C_camera_array[0];
    pCamera->LoadFG1();
    /*
    for (s32 i = 0; i < pCamera->field_0.Size(); i++)
    {
        u8** ppRes = pCamera->field_0.ItemAt(i);
        if (!ppRes)
        {
            break;
        }

        if (*ppRes)
        {
            ResourceManager::Header* pHeader = ResourceManager::Get_Header_49C410(ppRes);
            if (pHeader->mResourceType == ResourceManager::Resource_FG1)
            {
                relive_new FG1(ppRes);
            }
        }
    }*/
}

s16 Map::Get_Camera_World_Rect(CameraPos camIdx, PSX_RECT* pRect)
{
    if (camIdx < CameraPos::eCamCurrent_0 || camIdx > CameraPos::eCamRight_4)
    {
        return 0;
    }

    Camera* pCamera = field_2C_camera_array[static_cast<s32>(camIdx)];
    if (!pCamera)
    {
        return 0;
    }

    if (!pRect)
    {
        return 1;
    }

    const s16 xpos = pCamera->mCamXOff * mPathData->field_A_grid_width;
    const s16 ypos = pCamera->mCamYOff * mPathData->field_C_grid_height;

    pRect->x = xpos;
    pRect->y = ypos;
    pRect->w = xpos + 368;
    pRect->h = ypos + 240;
    return 1;
}

s16 Map::Is_Point_In_Current_Camera(EReliveLevelIds level, s32 path, FP xpos, FP ypos, s16 width)
{
    const FP calculated_width = (width != 0) ? FP_FromInteger(6) : FP_FromInteger(0);
    if (level != mCurrentLevel || path != mCurrentPath) // TODO: Remove when 100%
    {
        return FALSE;
    }

    PSX_RECT rect = {};
    rect.x = FP_GetExponent(xpos - calculated_width);
    rect.w = FP_GetExponent(calculated_width + xpos);
    rect.y = FP_GetExponent(ypos);
    rect.h = FP_GetExponent(ypos);
    return Rect_Location_Relative_To_Active_Camera(&rect) == CameraPos::eCamCurrent_0;
}


CameraPos Map::Rect_Location_Relative_To_Active_Camera(const PSX_RECT* pRect, s16 /*width*/)
{
    if (EventGet(kEventDeathReset))
    {
        return CameraPos::eCamNone_5;
    }

    const s32 camX = FP_GetExponent(field_24_camera_offset.x);
    const s32 camY = FP_GetExponent(field_24_camera_offset.y);

    if (pRect->x > (camX + 368))
    {
        return CameraPos::eCamRight_4;
    }

    if (pRect->y > (camY + 240))
    {
        return CameraPos::eCamBottom_2;
    }

    if (pRect->w >= camX)
    {
        if (pRect->h < camY)
        {
            return CameraPos::eCamTop_1;
        }
        else
        {
            return CameraPos::eCamCurrent_0;
        }
    }

    return CameraPos::eCamLeft_3;
}

s16 Map::SetActiveCam(EReliveLevelIds level, s16 path, s16 cam, CameraSwapEffects screenChangeEffect, s16 fmvBaseId, s16 forceChange)
{
    if (!forceChange && cam == mCurrentCamera && level == mCurrentLevel && path == mCurrentPath)
    {
        return 0;
    }

    mNextCamera = cam;
    mFmvBaseId = fmvBaseId;
    mNextPath = path;
    mNextLevel = level;
    mCameraSwapEffect = screenChangeEffect;

    mCamState = CamChangeStates::eInstantChange_2;

    if (screenChangeEffect == CameraSwapEffects::ePlay1FMV_5 || screenChangeEffect == CameraSwapEffects::eUnknown_11)
    {
        sMap_bDoPurpleLightEffect_5C311C = 1;
    }
    else
    {
        sMap_bDoPurpleLightEffect_5C311C = 0;
    }

    return 1;
}

BaseGameObject* Map::FMV_Camera_Change(CamResource& ppBits, Map* pMap, EReliveLevelIds lvlId)
{
    if (pMap->mFmvBaseId > 10000u)
    {
        // Trippe FMV
        FmvInfo* pFmvRec1 = Path_Get_FMV_Record(lvlId, pMap->mFmvBaseId / 10000);
        FmvInfo* pFmvRec2 = Path_Get_FMV_Record(lvlId, pMap->mFmvBaseId / 100 % 100);
        FmvInfo* pFmvRec3 = Path_Get_FMV_Record(lvlId, pMap->mFmvBaseId % 100);
        sLevelId_dword_5CA408 = static_cast<s32>(MapWrapper::ToAE(lvlId)); // TODO: Strongly type this, but it hasn't got the same underlaying type as the enum grr..
        u32 pos1 = 0;
        u32 pos2 = 0;
        u32 pos3 = 0;
        Get_fmvs_sectors(
            pFmvRec1->field_0_pName,
            pFmvRec2->field_0_pName,
            pFmvRec3->field_0_pName,
            &pos1,
            &pos2,
            &pos3);

        return relive_new CameraSwapper(ppBits,
                                              pos1,
                                              pFmvRec1->field_4_id,
                                              pos2,
                                              pFmvRec2->field_4_id,
                                              pos3,
                                              pFmvRec3->field_4_id,

                                              pFmvRec1->field_6_flags & 1,
                                              pFmvRec1->field_8_flags,
                                              pFmvRec1->field_A_volume,

                                              pFmvRec2->field_6_flags & 1,
                                              pFmvRec2->field_8_flags,
                                              pFmvRec2->field_A_volume,

                                              pFmvRec3->field_6_flags & 1,
                                              pFmvRec3->field_8_flags,
                                              pFmvRec3->field_A_volume);
    }
    else if (pMap->mFmvBaseId >= 100u)
    {
        // Double FMV
        FmvInfo* pFmvRec1 = Path_Get_FMV_Record(lvlId, pMap->mFmvBaseId / 100);
        FmvInfo* pFmvRec2 = Path_Get_FMV_Record(lvlId, pMap->mFmvBaseId % 100);
        u32 cdPos1 = 0;
        u32 cdPos2 = 0;
        Get_fmvs_sectors(pFmvRec1->field_0_pName, pFmvRec2->field_0_pName, 0, &cdPos1, &cdPos2, 0);
        sLevelId_dword_5CA408 = static_cast<s32>(MapWrapper::ToAE(lvlId)); // HACK
        return relive_new CameraSwapper(ppBits,
                                              cdPos1,
                                              pFmvRec1->field_4_id,
                                              cdPos2,
                                              pFmvRec2->field_4_id,

                                              pFmvRec1->field_6_flags & 1,
                                              pFmvRec1->field_8_flags,
                                              pFmvRec1->field_A_volume,

                                              pFmvRec2->field_6_flags & 1,
                                              pFmvRec2->field_8_flags,
                                              pFmvRec2->field_A_volume);
    }
    else // < 100
    {
        // Single FMV
        FmvInfo* pFmvRec1 = Path_Get_FMV_Record(lvlId, pMap->mFmvBaseId);
        u32 cdPos = 0;
        Get_fmvs_sectors(pFmvRec1->field_0_pName, 0, 0, &cdPos, 0, 0);
        sLevelId_dword_5CA408 = static_cast<s32>(MapWrapper::ToAE(lvlId)); // HACK
        return relive_new CameraSwapper(ppBits,
                                              cdPos,
                                              pFmvRec1->field_4_id,
                                              pFmvRec1->field_6_flags & 1,
                                              pFmvRec1->field_8_flags,
                                              pFmvRec1->field_A_volume);
    }
}

Camera* Map::Create_Camera(s16 xpos, s16 ypos, s32 /*a4*/)
{
    // Check min bound
    if (xpos < 0 || ypos < 0)
    {
        return nullptr;
    }

    // Check max bounds
    if (xpos >= sPathInfo->mCamsOnX || ypos >= sPathInfo->mCamsOnY)
    {
        return nullptr;
    }

    // Return existing camera if we already have one
    for (s32 i = 0; i < ALIVE_COUNTOF(field_40_stru_5); i++)
    {
        if (field_40_stru_5[i]
            && field_40_stru_5[i]->field_1A_level == mCurrentLevel
            && field_40_stru_5[i]->field_18_path == mCurrentPath
            && field_40_stru_5[i]->mCamXOff == xpos
            && field_40_stru_5[i]->mCamYOff == ypos)
        {
            Camera* pTemp = field_40_stru_5[i];
            field_40_stru_5[i] = nullptr;
            return pTemp;
        }
    }

    // Get a pointer to the camera name from the Path resource
    const BinaryPath* pPathData = GetPathResourceBlockPtr(mCurrentPath);
    auto pCamName = pPathData->CameraName(xpos, ypos);

    // Empty/blank camera in the map array
    if (!pCamName || !pCamName[0])
    {
        return nullptr;
    }

    Camera* newCamera = relive_new Camera();

    // Copy in the camera name from the Path resource and append .CAM
    memset(newCamera->field_1E_cam_name, 0, sizeof(newCamera->field_1E_cam_name));
    strncpy(newCamera->field_1E_cam_name, pCamName, ALIVE_COUNTOF(CameraName::name));
    strcat(newCamera->field_1E_cam_name, ".CAM");

    newCamera->mCamXOff = xpos;
    newCamera->mCamYOff = ypos;

    newCamera->field_30_flags &= -1;

    newCamera->field_1A_level = mCurrentLevel;
    newCamera->field_18_path = mCurrentPath;

    // Calculate hash/resource ID of the camera
    newCamera->field_10_camera_resource_id = 1 * (pCamName[7] - '0') + 10 * (pCamName[6] - '0') + 100 * (pCamName[4] - '0') + 1000 * (pCamName[3] - '0');

    // Convert the 2 digit camera number string to an integer
    newCamera->field_1C_camera_number = 1 * (pCamName[7] - '0') + 10 * (pCamName[6] - '0');

    return newCamera;
}

void Map::Load_Path_Items(Camera* pCamera, LoadMode loadMode)
{
    if (!pCamera)
    {
        return;
    }

    // Is camera resource loaded check
    if (!(pCamera->field_30_flags & 1))
    {
        if (loadMode == LoadMode::ConstructObject_0)
        {
            // Async camera load
            // ResourceManager::LoadResourceFile_49C130(pCamera->field_1E_cam_name, Camera::On_Loaded, pCamera, pCamera);
            pCamera->field_C_pCamRes = ResourceManagerWrapper::LoadCam(pCamera->field_1A_level, pCamera->field_18_path, pCamera->field_1C_camera_number);

            sCameraBeingLoaded_5C3118 = pCamera;
            sPathInfo->Loader_4DB800(pCamera->mCamXOff, pCamera->mCamYOff, LoadMode::LoadResourceFromList_1, ReliveTypes::eNone); // none = load all
        }
        else
        {
            // Blocking camera load
            // ResourceManager::LoadResourceFile_49C170(pCamera->field_1E_cam_name, pCamera);
            pCamera->field_30_flags |= 1;
            // pCamera->field_C_pCamRes = ResourceManagerWrapper::LoadCam(pCamera->field_1A_level, pCamera->field_18_path, pCamera->field_1C_camera_number);

            sCameraBeingLoaded_5C3118 = pCamera;
            sPathInfo->Loader_4DB800(pCamera->mCamXOff, pCamera->mCamYOff, LoadMode::LoadResource_2, ReliveTypes::eNone); // none = load all
        }
        sCameraBeingLoaded_5C3118 = nullptr;
    }
}

void Map::LoadResource(const char_type* pFileName, s32 type, s32 resourceId, LoadMode loadMode, s16 bDontLoad)
{
    if (!bDontLoad)
    {
        pResourceManager_5C1BB0->LoadResource_464EE0(pFileName, type, resourceId, sCameraBeingLoaded_5C3118, sCameraBeingLoaded_5C3118, 0, 1);
        if (loadMode == LoadMode::LoadResource_2)
        {
            pResourceManager_5C1BB0->LoadingLoop_465590(0);
        }
    }
}

void Map::LoadResourcesFromList(const char_type* pFileName, ResourceManager::ResourcesToLoadList* pList, LoadMode loadMode, s16 bDontLoad)
{
    if (!bDontLoad)
    {
        pResourceManager_5C1BB0->LoadResourcesFromList_465150(pFileName, pList, sCameraBeingLoaded_5C3118, sCameraBeingLoaded_5C3118, 0, 1);
        if (loadMode == LoadMode::LoadResource_2)
        {
            pResourceManager_5C1BB0->LoadingLoop_465590(0);
        }
    }
}

s16 Map::SetActiveCameraDelayed(MapDirections direction, BaseAliveGameObject* pObj, s16 swapEffect)
{
    relive::Path_PathTransition* pPathChangeTLV = nullptr;
    CameraSwapEffects convertedSwapEffect = CameraSwapEffects::eInstantChange_0;
    if (pObj)
    {
        pPathChangeTLV = reinterpret_cast<relive::Path_PathTransition*>(sPathInfo->TLV_Get_At_4DB4B0(
            FP_GetExponent(pObj->mXPos),
            FP_GetExponent(pObj->mYPos),
            FP_GetExponent(pObj->mXPos),
            FP_GetExponent(pObj->mYPos),
            ReliveTypes::ePathTransition));
    }

    if (pObj && pPathChangeTLV)
    {
        mNextLevel = pPathChangeTLV->mNextLevel;
        mNextPath = pPathChangeTLV->mNextPath;
        mNextCamera = pPathChangeTLV->mNextCamera;
        if (swapEffect < 0)
        {
            // Map the TLV/editor value of screen change to the internal screen change
            convertedSwapEffect = kPathChangeEffectToInternalScreenChangeEffect_55D55C[pPathChangeTLV->mWipeEffect];
        }
        else
        {
            // If not negative then its an actual swap effect
            convertedSwapEffect = static_cast<CameraSwapEffects>(swapEffect);
        }
    }
    else
    {
        switch (direction)
        {
            case MapDirections::eMapLeft_0:
                if (!GetCamera(CameraPos::eCamLeft_3))
                {
                    return 0;
                }
                break;
            case MapDirections::eMapRight_1:
                if (!GetCamera(CameraPos::eCamRight_4))
                {
                    return 0;
                }
                break;
            case MapDirections::eMapBottom_3:
                if (!GetCamera(CameraPos::eCamBottom_2))
                {
                    return 0;
                }
                break;
            case MapDirections::eMapTop_2:
                if (!GetCamera(CameraPos::eCamTop_1))
                {
                    return 0;
                }
                break;
        }

        mNextLevel = mCurrentLevel;
        mNextPath = mCurrentPath;
        convertedSwapEffect = static_cast<CameraSwapEffects>(swapEffect); // TODO: Correct ??
    }

    mMapDirection = direction;
    mAliveObj = pObj;
    mConvertedCameraSwapEffect_NeverRead = convertedSwapEffect;
    mCamState = CamChangeStates::eSliceCam_1;
    sMap_bDoPurpleLightEffect_5C311C = 0;

    if (convertedSwapEffect == CameraSwapEffects::ePlay1FMV_5 || convertedSwapEffect == CameraSwapEffects::eUnknown_11)
    {
        sMap_bDoPurpleLightEffect_5C311C = 1;
    }

    return 1;
}

ALIVE_VAR(1, 0x5C3030, Map, gMap, {});
