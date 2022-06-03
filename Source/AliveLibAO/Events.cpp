#include "stdafx_ao.h"
#include "Events.hpp"
#include "Function.hpp"
#include "BaseGameObject.hpp"
#include "BaseAliveGameObject.hpp"
#include "PsxDisplay.hpp"

namespace AO {

struct EventsArray final
{
    BaseGameObject* field_0_event_ptrs[20];
};

struct Events final
{
    // 2 copies for f64 buffering
    EventsArray field_0_events[2];
};

ALIVE_VAR(1, 0x4FFA48, s16, sEventsToUse_4FFA48, 0);
ALIVE_VAR(1, 0x4FF9A8, Events, sEventPtrs_4FF9A8, {});


BaseGameObject* Event_Get(s16 eventType)
{
    return sEventPtrs_4FF9A8.field_0_events[sEventsToUse_4FFA48].field_0_event_ptrs[eventType];
}

void Event_Broadcast(s32 eventType, BaseGameObject* pObject)
{
    sEventPtrs_4FF9A8.field_0_events[!sEventsToUse_4FFA48].field_0_event_ptrs[eventType] = pObject;
}

BaseAnimatedWithPhysicsGameObject* Event_Is_Event_In_Range(s16 eventType, FP xpos, FP ypos, FP scale)
{
    BaseGameObject* pObj = sEventPtrs_4FF9A8.field_0_events[sEventsToUse_4FFA48].field_0_event_ptrs[eventType];
    if (!pObj)
    {
        return nullptr;
    }

    if (!pObj->mGameObjectFlags.Get(BaseGameObject::eIsBaseAnimatedWithPhysicsObj_Bit5) || !pObj->mGameObjectFlags.Get(BaseGameObject::eDrawable_Bit4))
    {
        return nullptr;
    }

    // At this point we known the type must be BaseAnimatedWithPhysicsGameObject
    auto pDerived = static_cast<BaseAnimatedWithPhysicsGameObject*>(pObj);

    if (pDerived->mSpriteScale == scale && FP_Abs(xpos - pDerived->mXPos) < FP_FromInteger(gPsxDisplay_504C78.field_0_width) && FP_Abs(ypos - pDerived->mYPos) < FP_FromInteger(gPsxDisplay_504C78.field_2_height))
    {
        return pDerived;
    }

    return nullptr;
}

void Events_Reset_Active()
{
    for (auto& ptr : sEventPtrs_4FF9A8.field_0_events[sEventsToUse_4FFA48].field_0_event_ptrs)
    {
        ptr = nullptr;
    }

    sEventsToUse_4FFA48 = !sEventsToUse_4FFA48;
}

void Event_Cancel_For_Obj(BaseGameObject* pObj)
{
    for (s32 i = 0; i < 2; i++)
    {
        for (s32 j = 0; j < ALIVE_COUNTOF(EventsArray::field_0_event_ptrs); j++)
        {
            if (sEventPtrs_4FF9A8.field_0_events[i].field_0_event_ptrs[j] == pObj)
            {
                sEventPtrs_4FF9A8.field_0_events[i].field_0_event_ptrs[j] = nullptr;
            }
        }
    }
}

} // namespace AO
