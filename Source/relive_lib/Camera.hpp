#pragma once

#include "../relive_lib/Primitives.hpp"
#include "../relive_lib/MapWrapper.hpp"
#include "../relive_lib/DynamicArray.hpp"
#include "../relive_lib/ResourceManagerWrapper.hpp"

enum class LevelIds : s16;

class Camera final // TODO: May actually just be "ResourceList" ?
{
public:
    Camera();
    ~Camera();
    void CreateFG1();

public:
    static void On_Loaded(Camera* pCamera);

    CamResource mCamRes;
    s32 mCameraResourceId = 0;
    s16 mCamXOff = 0;
    s16 mCamYOff = 0;
    s16 mPath = 0;
    EReliveLevelIds mLevel;
    s16 mCamera = 0;
    char_type mCamName[16] = {};
    bool mCamResLoaded = false;
};
