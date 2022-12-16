#include "../../relive_lib/stdafx.h"
#include "TlvObjectBase.hpp"

namespace ReliveAPI {
TlvObjectBase::TlvObjectBase(const std::string& typeName)
    : mStructTypeName(typeName)
{
}

void TlvObjectBase::AddTypes(TypesCollectionBase& /*types*/)
{
    // Default empty to prevent having to explicitly implement in every TLV wrapper
}

void TlvObjectBase::SetInstanceNumber(s32 instanceNumber)
{
    mInstanceNumber = instanceNumber;
}

[[nodiscard]] const std::string& TlvObjectBase::Name() const
{
    return mStructTypeName;
}

[[nodiscard]] jsonxx::Object TlvObjectBase::StructureToJson()
{
    jsonxx::Object ret;
    ret << "name" << Name();
    ret << "enum_and_basic_type_properties" << PropertiesToJson();
    return ret;
}

void TlvObjectBase::InstanceFromJson(TypesCollectionBase& types, const jsonxx::Object& obj, Context& context)
{
    const jsonxx::Object& properties = obj.get<jsonxx::Object>("properties");
    PropertiesFromJson(types, properties, context);
    InstanceFromJsonBase(obj);
}

[[nodiscard]] jsonxx::Object TlvObjectBase::InstanceToJson(TypesCollectionBase& types, Context& context)
{
    jsonxx::Object ret;
    InstanceToJsonBase(ret);

    jsonxx::Object properties;
    PropertiesToJson(types, properties, context);
    ret << "properties" << properties;

    return ret;
}

[[nodiscard]] s32 TlvObjectBase::InstanceNumber() const
{
    return mInstanceNumber;
}

void TlvObjectBase::AddResource(AnimId res, AddResourceTo type)
{
    mRequiredResources.push_back({res, type});
}

} // namespace ReliveAPI
