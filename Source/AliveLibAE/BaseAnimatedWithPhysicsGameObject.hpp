#pragma once

#include "BaseGameObject.hpp"
#include "FixedPoint.hpp"
#include "../AliveLibCommon/Function.hpp"
#include "Animation.hpp"
#include "AnimationUnknown.hpp"
#include "Map.hpp"
#include "PathData.hpp"

class Shadow;

struct TintEntry final
{
    LevelIds_s8 field_0_level;
    u8 field_1_r;
    u8 field_2_g;
    u8 field_3_b;
};
ALIVE_ASSERT_SIZEOF(TintEntry, 0x4);

#ifdef _MSC_VER
// NOTE: __single_inheritance required to workaround MSVC code gen bug
// https://stackoverflow.com/questions/8676879/member-function-pointer-runtime-error-the-value-of-esp-was-not-properly-saved
class __single_inheritance BaseAnimatedWithPhysicsGameObject;
#else
class BaseAnimatedWithPhysicsGameObject;
#endif

using TCollisionCallBack = s16 (BaseGameObject::*)(BaseGameObject*); // Typically points to something in the derived type.. pretty strange, probably also why its a function pointer

class BaseAnimatedWithPhysicsGameObject : public BaseGameObject
{
public:
    explicit BaseAnimatedWithPhysicsGameObject(s16 resourceArraySize);
    ~BaseAnimatedWithPhysicsGameObject();

    virtual void VUpdate() override;
    virtual void VRender(PrimHeader** ppOt) override;

    void Animation_Init(s32 frameTableOffset, s32 maxW, s32 maxH, u8** ppAnimData, s16 bAddToDrawableList, u8 bOwnsPalData);

    virtual void VOnCollisionWith(PSX_Point xy, PSX_Point wh, DynamicArrayT<BaseGameObject>* pObjList, s32 startingPointIdx, TCollisionCallBack pFn);
    // TODO: Just return by value - this is odd optimization the compiler does
    virtual PSX_RECT* VGetBoundingRect(PSX_RECT* pRect, s32 pointIdx);
    virtual s16 VIsObjNearby(FP radius, BaseAnimatedWithPhysicsGameObject* pObj);
    virtual s16 VIsObj_GettingNear(BaseAnimatedWithPhysicsGameObject* pObj);
    virtual s16 VIsFacingMe(BaseAnimatedWithPhysicsGameObject* pOther);
    virtual s16 VOnSameYLevel(BaseAnimatedWithPhysicsGameObject* pOther);
    virtual void VStackOnObjectsOfType(AETypes typeToFind);
    virtual void VOnPickUpOrSlapped();
    virtual void VOnThrowableHit(BaseGameObject* pFrom);

    CameraPos Is_In_Current_Camera();
    void DealDamageRect(const PSX_RECT* pRect);
    void SetTint(const TintEntry* pTintArray, LevelIds level_id);

protected:
    void SetRGB(s16 r, s16 g, s16 b);
    void DeathSmokeEffect(bool bPlaySound);

public:
    Animation mAnim;
    FP mXPos;
    FP mYPos;
    s16 mPathNumber;
    LevelIds mLvlNumber;
    FP mVelX;
    FP mVelY;
    FP mSpriteScale;
    s16 mRed;
    s16 mGreen;
    s16 mBlue;
    s16 mScale;
    s16 mYOffset;
    s16 mXOffset;
    s16 mApplyShadows;
    s16 field_DE_pad;
    Shadow* mShadow;
    FP_RECT mCollectionRect;
};
ALIVE_ASSERT_SIZEOF(BaseAnimatedWithPhysicsGameObject, 0xF4);
