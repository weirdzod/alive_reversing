#pragma once

#include "../AliveLibCommon/Function.hpp"
#include "BaseAnimatedWithPhysicsGameObject.hpp"
#include "Map.hpp"
#include "../AliveLibAE/Path.hpp"

namespace AO {

union OffLevelOrDx
{
    LevelIds level;
    s16 dx;
};

struct Path_WellBase : public Path_TLV
{
    Scale_short mScale;
    s16 mSwitchId;
    s16 mOtherWellId;
    s16 mAnimId;
    s16 mExitX;
    s16 mExitY;
    OffLevelOrDx mOffLevelOrDestX;
    s16 mOffPathOrDestY;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_WellBase, 0x28);

struct Path_WellExpress final : public Path_WellBase
{
    s16 mOffDestCamera;
    s16 mOffOtherWellId;
    LevelIds mOnDestLevel;
    s16 mOnDestPath;
    s16 mOnDestCamera;
    s16 mOnOtherWellId;
    Choice_short mEmitLeaves;
    s16 mLeafX;
    s16 mLeafY;
    s16 mMovieId;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_WellExpress, 0x3C);

struct Path_WellLocal final : public Path_WellBase
{
    s16 mOnDestX;
    s16 mOnDestY;
    Choice_short mEmitLeaves;
    s16 mLeafX;
    s16 mLeafY;
    s16 field_32_pad;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_WellLocal, 0x34);

class Well final : public BaseAnimatedWithPhysicsGameObject
{
public:
    Well(Path_WellBase* pTlv, FP xpos, FP ypos, s32 tlvInfo);
    ~Well();

    void WellLocal_Init(Path_WellLocal* pTlv, FP xpos, FP ypos);
    void WellExpress_Init(Path_WellExpress* pTlv, FP xpos, FP ypos);

    virtual void VUpdate() override;
    virtual void VRender(PrimHeader** ppOt) override;
    virtual void VScreenChanged() override;

    s32 field_D4[4] = {};
    s32 field_E4_tlvInfo = 0;
    s16 field_E8_switch_id = 0;
    FP field_EC_scale = {};
    FP field_F0_exit_x = {};
    FP field_F4_exit_y = {};
    FP field_F8_leaf_xpos = {};
    FP field_FC_leaf_ypos = {};
    Choice_short field_100_emit_leaves = Choice_short::eNo_0;
};
ALIVE_ASSERT_SIZEOF(Well, 0x104);

} // namespace AO
