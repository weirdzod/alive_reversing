#include "stdafx.h"
#include "ObjectIds.hpp"
#include "GameObjects/BaseGameObject.hpp"
#include "FatalError.hpp"

ObjectIds sObjectIds;

ObjectIds::~ObjectIds()
{

}

ObjectIds::ObjectIds()
{

}

void ObjectIds::Insert(TObjectId_KeyType nextId, BaseGameObject* pGameObj)
{
    mMap[nextId] = pGameObj;
}

s16 ObjectIds::Remove(TObjectId_KeyType idToRemove)
{
    auto it = mMap.find(idToRemove);
    if (it != std::end(mMap))
    {
        mMap.erase(it);
        return 1;
    }
    return 0;
}

BaseGameObject* ObjectIds::Find_Impl(TObjectId_KeyType idToFind)
{
    auto it = mMap.find(idToFind);
    if (it != std::end(mMap))
    {
        return it->second;
    }
    return nullptr;
}

BaseGameObject* ObjectIds::Find(TObjectId_KeyType idToFind, ReliveTypes type)
{
    BaseGameObject* pItem = Find_Impl(idToFind);
    if (pItem && pItem->Type() != type)
    {
        ALIVE_FATAL("Expected type %d for object with id %s but got %d", static_cast<s32>(type), idToFind.ToString().c_str(), static_cast<s32>(pItem->Type()));
    }
    return pItem;
}
