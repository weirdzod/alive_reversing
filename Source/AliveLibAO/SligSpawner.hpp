#pragma once

#include "../relive_lib/BaseGameObject.hpp"
#include "../relive_lib/data_conversion/relive_tlvs.hpp"

namespace AO {

struct Path_Slig;

class SligSpawner final : public ::BaseGameObject
{
public:
    SligSpawner(relive::Path_TLV* pTlv, relive::Path_Slig_Data* pTlvData, const Guid& tlvId);
    ~SligSpawner();

    virtual void VUpdate() override;
    virtual void VScreenChanged() override;

private:
    Guid mTlvInfo;
    u16 mSligSpawnerSwitchId = 0;
    s16 mSpawnerFlags = 0;
    relive::Path_TLV mPathTlv = {};
};

} // namespace AO
