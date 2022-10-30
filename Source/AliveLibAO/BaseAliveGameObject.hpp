#pragma once

#include "../relive_lib/BaseAnimatedWithPhysicsGameObject.hpp"
#include "../AliveLibCommon/Function.hpp"
#include "../AliveLibCommon/BitField.hpp"
#include "../AliveLibAE/Psx.hpp"
#include "../relive_lib/IBaseAliveGameObject.hpp"

class PathLine;

enum class CameraPos : s16;

namespace AO {

class PlatformBase;
class BirdPortal;

class BaseAliveGameObject : public IBaseAliveGameObject
{
public:
    BaseAliveGameObject();
    ~BaseAliveGameObject();

    virtual void VOnPathTransition(s32 camWorldX, s32 camWorldY, CameraPos direction) override;
    virtual void VCheckCollisionLineStillValid(s32 distance);

    virtual void VSetXSpawn(s16 camWorldX, s32 screenXPos) override;
    virtual void VSetYSpawn(s32 camWorldY, s16 bLeft) override;
    virtual BirdPortal* VIntoBirdPortal(s16 distance);

    s16 MapFollowMe(s16 snapToGrid);

    virtual s16 VOnPlatformIntersection(BaseAnimatedWithPhysicsGameObject* pPlatform) override;

protected:

    bool Check_IsOnEndOfLine(s16 direction, s16 distance);
    s16 WallHit(FP offY, FP offX);
    bool InAirCollision(PathLine** ppLine, FP* hitX, FP* hitY, FP vely);
    BaseGameObject* FindObjectOfType(ReliveTypes typeToFind, FP xpos, FP ypos);

    void UsePathTransScale_4020D0();
    static void OnResourceLoaded_4019A0(BaseAliveGameObject* ppRes);
};

} // namespace AO
