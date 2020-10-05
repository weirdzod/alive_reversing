#include "stdafx_ao.h"
#include "ResourceManager.hpp"
#include "Function.hpp"
#include "Particle.hpp"
#include "stdlib.hpp"
#include "PsxDisplay.hpp"
#include "Psx.hpp"
#include "PsxRender.hpp"

START_NS_AO

ALIVE_VAR(1, 0x9F0E48, DWORD, sManagedMemoryUsedSize_9F0E48, 0);

ALIVE_VAR(1, 0x5076A0, short, bHideLoadingIcon_5076A0, 0);
ALIVE_VAR(1, 0x5076A4, int, loading_ticks_5076A4, 0);
ALIVE_VAR(1, 0x9F0E38, short, sResources_Pending_Loading_9F0E38, 0);

ALIVE_VAR(1, 0x50EE2C, ResourceManager::ResourceHeapItem*, sFirstLinkedListItem_50EE2C, nullptr);

void CC Game_ShowLoadingIcon_445EB0()
{
    BYTE** ppLoadingAnimRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, 8010, 1, 0);
    if (ppLoadingAnimRes)
    {
        auto pParticle = ao_new<Particle>();
        if (pParticle)
        {
            pParticle->ctor_478880(FP_FromInteger(0), FP_FromInteger(0), 652, 50, 38, ppLoadingAnimRes);
        }

        pParticle->field_10_anim.field_4_flags.Clear(AnimFlags::eBit15_bSemiTrans);
        pParticle->field_10_anim.field_4_flags.Set(AnimFlags::eBit16_bBlending);

        pParticle->field_10_anim.field_C_layer = 0;

        int* local_ot[42] = {};
        PSX_DRAWENV drawEnv = {};

        PSX_SetDefDrawEnv_495EF0(&drawEnv, 0, 0, 640, 240);
        PSX_PutDrawEnv_495DD0(&drawEnv);
        PSX_DrawSync_496750(0);
        PSX_ClearOTag_496760(local_ot, 42);

        pParticle->field_10_anim.vRender(320, 220, local_ot, 0, 0);

        PSX_DrawOTag_4969F0(local_ot);
        PSX_DrawSync_496750(0);

        PSX_ClearOTag_496760(local_ot, 42);

        pParticle->field_10_anim.vRender(320, gPsxDisplay_504C78.field_2_height + 220, local_ot, 0, 0);

        PSX_DrawOTag_4969F0(local_ot);
        PSX_DrawSync_496750(0);

        PSX_DISPENV dispEnv = {};
        PSX_SetDefDispEnv_4959D0(&dispEnv, 0, 0, 640, 240);
        PSX_PutDispEnv_495CE0(&dispEnv);
        pParticle->field_6_flags.Set(BaseGameObject::eDead_Bit3);
        bHideLoadingIcon_5076A0 = TRUE;
    }
}

void CC ResourceManager::CancelPendingResourcesFor_41EA60(BaseGameObject* /*pObj*/)
{
    NOT_IMPLEMENTED();
}

void CC ResourceManager::LoadResource_446C90(const char* /*pFileName*/, int /*type*/, int /*resourceId*/, __int16 /*loadMode*/, __int16 /*bDontLoad*/)
{
    NOT_IMPLEMENTED();
}

void CC ResourceManager::LoadResourcesFromList_446E80(const char* /*pFileName*/, ResourcesToLoadList* /*list*/, __int16 /*loadMode*/, __int16)
{
    NOT_IMPLEMENTED();
}

EXPORT void CC ResourceManager::LoadingLoop_41EAD0(__int16 /*bShowLoadingIcon*/)
{
    NOT_IMPLEMENTED();
}

EXPORT void CC ResourceManager::Free_Resources_For_Camera_447170(Camera* /*pCamera*/)
{
    NOT_IMPLEMENTED();
}

void CC ResourceManager::Init_454DA0()
{
    NOT_IMPLEMENTED();
}


EXPORT ResourceManager::ResourceManager_FileRecord* CC ResourceManager::LoadResourceFile_4551E0(const char* /*pFileName*/, TLoaderFn /*fnOnLoad*/, Camera* /*pCamera1*/, Camera* /*pCamera2*/)
{
    NOT_IMPLEMENTED();
    return nullptr;
}

BYTE** ResourceManager::Alloc_New_Resource_Impl(DWORD type, DWORD id, DWORD size, bool locked, BlockAllocMethod allocType)
{
    BYTE** ppNewRes = Allocate_New_Block_454FE0(size + sizeof(Header), allocType);
    if (!ppNewRes)
    {
        // Failed, try to reclaim some memory and try again.
        Reclaim_Memory_455660(0);
        ppNewRes = Allocate_New_Block_454FE0(size + sizeof(Header), allocType);
    }

    if (ppNewRes)
    {
        Header* pHeader = Get_Header_455620(ppNewRes);
        pHeader->field_8_type = type;
        pHeader->field_C_id = id;
        pHeader->field_4_ref_count = 1;
        pHeader->field_6_flags = locked ? ResourceHeaderFlags::eLocked : 0;
    }
    return ppNewRes;
}

BYTE** CC ResourceManager::Alloc_New_Resource_454F20(DWORD type, DWORD id, DWORD size)
{
  return Alloc_New_Resource_Impl(type, id, size, false, BlockAllocMethod::eFirstMatching);
}

BYTE** CC ResourceManager::Allocate_New_Locked_Resource_454F80(DWORD type, DWORD id, DWORD size)
{
    return Alloc_New_Resource_Impl(type, id, size, true, BlockAllocMethod::eLastMatching);
}

EXPORT BYTE** CC ResourceManager::Allocate_New_Block_454FE0(DWORD /*sizeBytes*/, BlockAllocMethod /*allocMethod*/)
{
    NOT_IMPLEMENTED();
    return nullptr;
}

EXPORT __int16 CC ResourceManager::LoadResourceFile_455270(const char* /*filename*/, Camera* /*pCam*/, int /*allocMethod*/)
{
    NOT_IMPLEMENTED();
    return 0;
}

BYTE** CC ResourceManager::GetLoadedResource_4554F0(DWORD type, DWORD resourceId, __int16 addUseCount, __int16 bLock)
{
    // Iterate all list items
    ResourceHeapItem* pListIter = sFirstLinkedListItem_50EE2C;
    while (pListIter)
    {
        // Find something that matches the type and resource ID
        Header* pResHeader = Get_Header_455620(&pListIter->field_0_ptr);
        if (pResHeader->field_8_type == type && pResHeader->field_C_id == resourceId)
        {
            if (addUseCount)
            {
                pResHeader->field_4_ref_count++;
            }

            if (bLock)
            {
                pResHeader->field_6_flags |= ResourceHeaderFlags::eLocked;
            }

            return &pListIter->field_0_ptr;
        }

        pListIter = pListIter->field_4_pNext;
    }
    return nullptr;
}

__int16 CC ResourceManager::FreeResource_455550(BYTE** /*ppRes*/)
{
    NOT_IMPLEMENTED();
    return 0;
}


__int16 CC ResourceManager::FreeResource_Impl_4555B0(BYTE* handle)
{
    if (handle)
    {
        Header* pHeader = Get_Header_455620(&handle);
        if (pHeader->field_4_ref_count)
        {
            // Decrement ref count, if its not zero then we can't free it yet
            pHeader->field_4_ref_count--;
            if (pHeader->field_4_ref_count > 0)
            {
                return 0;
            }
            pHeader->field_8_type = Resource_Free;
            pHeader->field_6_flags = 0;
            sManagedMemoryUsedSize_9F0E48 -= pHeader->field_0_size;
        }
    }
    return 1;
}

ResourceManager::Header* CC ResourceManager::Get_Header_455620(BYTE** ppRes)
{
    return reinterpret_cast<Header*>((*ppRes - sizeof(Header)));
}

void CC ResourceManager::Reclaim_Memory_455660(DWORD /*sizeToReclaim*/)
{
    NOT_IMPLEMENTED();
}

void CC ResourceManager::Increment_Pending_Count_4557A0()
{
    sResources_Pending_Loading_9F0E38++;
}

void CC ResourceManager::Decrement_Pending_Count_4557B0()
{
    sResources_Pending_Loading_9F0E38--;
}

void CC ResourceManager::Set_Header_Flags_4557D0(BYTE** ppRes, __int16 flags)
{
    Get_Header_455620(ppRes)->field_6_flags |= flags;
}

int CC ResourceManager::Is_Resources_Pending_4557C0()
{
    return sResources_Pending_Loading_9F0E38 > 0 ? 1 : 0;
}

void CC ResourceManager::Free_Resource_Of_Type_455810(DWORD type)
{
    ResourceHeapItem* pListItem = sFirstLinkedListItem_50EE2C;
    while (pListItem)
    {
        // Free it if the type matches and its not flagged as never free
        Header* pHeader = Get_Header_455620(&pListItem->field_0_ptr);
        if (pHeader->field_8_type == type && !(pHeader->field_6_flags & ResourceHeaderFlags::eNeverFree))
        {
            pHeader->field_8_type = Resource_Free;
            pHeader->field_6_flags = 0;
            pHeader->field_4_ref_count = 0;

            sManagedMemoryUsedSize_9F0E48 -= pHeader->field_0_size;
        }
        pListItem = pListItem->field_4_pNext;
    }
}

END_NS_AO



