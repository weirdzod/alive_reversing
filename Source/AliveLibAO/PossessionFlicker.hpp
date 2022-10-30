#pragma once

#include "../AliveLibCommon/Function.hpp"
#include "../relive_lib/BaseGameObject.hpp"

class IBaseAliveGameObject;

namespace AO {

class PossessionFlicker final : public ::BaseGameObject
{
public:
    PossessionFlicker(IBaseAliveGameObject* pToApplyFlicker, s32 duration, s32 r, s32 g, s32 b);
    ~PossessionFlicker();

    virtual void VScreenChanged() override;
    virtual void VUpdate() override;

    IBaseAliveGameObject* field_10_pObj = nullptr;
    s32 field_14_time_to_flicker = 0;

    s16 field_18_r = 0;
    s16 field_1A_g = 0;
    s16 field_1C_b = 0;

    s16 field_1E_old_r = 0;
    s16 field_20_old_g = 0;
    s16 field_22_old_b = 0;
};

} // namespace AO
