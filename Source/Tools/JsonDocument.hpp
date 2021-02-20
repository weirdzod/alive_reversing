#pragma once

#include <string>
#include <vector>
#include <jsonxx/jsonxx.h>

struct CollisionObject
{
    int mX1 = 0;
    int mY1 = 0;
    int mX2 = 0;
    int mY2 = 0;
    // TODO: Other properties

    jsonxx::Object ToJsonObject() const
    {
        jsonxx::Object obj;

        obj << "x1" << mX1;
        obj << "y1" << mY1;

        obj << "x2" << mX2;
        obj << "y2" << mY2;

        return obj;
    }
};

struct CameraObject
{
    std::string mName;
    int mId = 0;
    int mX = 0;
    int mY = 0;
    //std::vector<MapObject> mMapObjects;

    jsonxx::Object ToJsonObject() const
    {
        jsonxx::Object obj;

        obj << "name" << mName;
        obj << "x" << mX;
        obj << "y" << mY;
        obj << "id" << mId;
        /*
        jsonxx::Array mapObjectsArray;
        for (const auto& mapObject : mMapObjects)
        {
            mapObjectsArray << mapObject.ToJsonObject();
        }

        obj << "map_objects" << mapObjectsArray;
        */
        return obj;
    }
};


struct PathInfo
{
    int mGridWidth = 0;
    int mWidth = 0;
    int mGridHeight = 0;
    int mHeight = 0;
    unsigned int mObjectOffset = 0;
    unsigned int mIndexTableOffset = 0;
};

class JsonDocument
{
public:
    int mVersion = 0;
    std::string mGame;

    std::string mPathBnd;
    int mPathId = 0;

    int mXGridSize = 0;
    int mXSize = 0;

    int mYGridSize = 0;
    int mYSize = 0;

    std::vector<CollisionObject> mCollisions;
    std::vector<CameraObject> mCameras;

    void Load(const std::string& fileName)
    {

    }

    void SaveAE(int pathId, const std::vector<BYTE>& pathResource, const PathInfo& info, const std::string& fileName) const
    {
        jsonxx::Object rootObject;

        rootObject << "api_version" << mVersion;

        rootObject << "game" << mGame;

        jsonxx::Object rootMapObject;
        rootMapObject << "path_bnd" <<  mPathBnd;
        rootMapObject << "path_id" << mPathId;

        rootMapObject << "x_grid_size" << mXGridSize;
        rootMapObject << "x_size" << mXSize;

        rootMapObject << "y_grid_size" << mYGridSize;
        rootMapObject << "y_size" << mYSize;

        jsonxx::Array collisionsArray;
        for (const auto& item : mCollisions)
        {
            collisionsArray << item.ToJsonObject();
        }
        rootMapObject << "collisions" << collisionsArray;

        jsonxx::Array cameraArray;
        for (const auto& camera : mCameras)
        {
            cameraArray << camera.ToJsonObject();
        }
        rootMapObject << "cameras" << cameraArray;

        /*
        jsonxx::Object objectStructurePropertiesObject;
        for (const auto& propertyEnum : mStructureAndTypes.mEnums)
        {
            propertyEnum.AddJsonArray(objectStructurePropertiesObject);
        }
        rootMapObject << "object_structure_property_enums" << objectStructurePropertiesObject; 

        jsonxx::Array objectStructureBasicTypes;
        for (const auto& basicType : mStructureAndTypes.mBasicTypes)
        {
            basicType.AddToJsonArray(objectStructureBasicTypes);
        }
        rootMapObject << "object_structure_property_basic_types" << objectStructureBasicTypes;

        jsonxx::Array objectStructures;
        for (const auto& objectStructure : mStructureAndTypes.mObjectStructures)
        {
            objectStructure.AddToJsonArray(objectStructures);
        }
        rootMapObject << "object_structures" << objectStructures;
        */

        rootObject << "map" << rootMapObject;

        std::ofstream s(fileName.c_str());
        if (s)
        {
            s << rootObject.json();
        }

    }
};
