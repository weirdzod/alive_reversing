#pragma once

#include "BaseAliveGameObject.hpp"

namespace relive
{
    struct Path_SlamDoor;
}

struct SlamDoorSaveState final
{
    ReliveTypes mType;
    Guid mTlvInfo;
};

class SlamDoor final : public BaseAliveGameObject
{
public:
    SlamDoor(relive::Path_SlamDoor* tlv_params, const Guid& tlvId);
    ~SlamDoor();

    void LoadAnimations();

    virtual s32 VGetSaveState(u8* pSaveBuffer) override;
    virtual void VUpdate() override;
    virtual void VOnThrowableHit(BaseGameObject*) override
    {
        // Empty
    }

    static s32 CreateFromSaveState(const u8*);

private:
    void ClearInsideSlamDoor(IBaseAliveGameObject* pObj, s16 xPosition, s16 width);

private:
    enum SlamDoorFlags
    {
        eClosed = 0x1,
        eOpen = 0x2,
        eLastFrame = 0x4,
        eSlamDoorFlipY = 0x8,
        eDelete = 0x10,
    };
    BitField16<SlamDoorFlags> mSlamDoorFlags = {};
    PathLine* mCollisionLine1 = nullptr;
    PathLine* mCollisionLine2 = nullptr;
    s16 mCollisionX = 0;
    s16 mCollisionY = 0;
    s16 mSwitchId = 0;
    Guid mTlvInfo;
};
