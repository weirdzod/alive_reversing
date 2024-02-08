#include "Model.hpp"
#include "ReliveApiWrapper.hpp"
#include <optional>
#include <fstream>
#include "../../relive_lib/data_conversion/relive_tlvs_serialization.hpp"

static std::optional<std::string> LoadFileToString(const std::string& fileName)
{
    EditorFileIO fileIo;
    auto file = fileIo.Open(fileName, ReliveAPI::IFileIO::Mode::ReadBinary);
    if (!file->IsOpen())
    {
        return {};
    }
    std::string s;
    file->ReadInto(s);
    return { s };
}

static nlohmann::json ReadArray(nlohmann::json& o, const std::string& key)
{
    if (!o.contains(key) || !o.at(key).is_array())
    {
        throw Model::JsonKeyNotFoundException(key);
    }
    return o.at(key);
}

static nlohmann::json ReadObject(nlohmann::json& o, const std::string& key)
{
    if (!o.contains(key) || !o.at(key).is_object())
    {
        throw Model::JsonKeyNotFoundException(key);
    }
    return o.at(key);
}

static int ReadNumber(nlohmann::json& o, const std::string& key)
{
    if (!o.contains(key) || !o.at(key).is_number())
    {
        throw Model::JsonKeyNotFoundException(key);
    }
    return o.at(key).get<s32>();
}

static std::string ReadString(nlohmann::json& o, const std::string& key)
{
    if (!o.contains(key) || !o.at(key).is_string())
    {
        throw Model::JsonKeyNotFoundException(key);
    }
    return o.at(key);   
}

static std::string ReadStringOptional(nlohmann::json& o, const std::string& key)
{
    if (!o.contains(key) || !o.at(key).is_string())
    {
        return "";
    }
    return o.at(key);
}

static bool ReadBool(nlohmann::json& o, const std::string& key)
{
    if (!o.contains(key) || !o.at(key).is_boolean())
    {
        throw Model::JsonKeyNotFoundException(key);
    }
    return o.at(key);
}

Model::Camera* Model::GetContainingCamera(MapObjectBase* pMapObject)
{
    Model::Camera* pContainingCamera = nullptr;
    for (auto& camera : mCameras)
    {
        for (auto& mapObj : camera->mMapObjects)
        {
            if (mapObj.get() == pMapObject)
            {
                pContainingCamera = camera.get();
                break;
            }
        }
    }
    return pContainingCamera;
}

UP_MapObjectBase Model::TakeFromContainingCamera(MapObjectBase* pMapObject)
{
    for (auto& camera : mCameras)
    {
        for (auto it = camera->mMapObjects.begin(); it != camera->mMapObjects.end(); )
        {
            if ((*it).get() == pMapObject)
            {
                UP_MapObjectBase takenObj((*it).release());
                camera->mMapObjects.erase(it);
                return takenObj;
            }
            it++;
        }
    }
    return nullptr;
}

Model::UP_Camera Model::RemoveCamera(Model::Camera* pCamera)
{
    for (auto it = mCameras.begin(); it != mCameras.end(); )
    {
        if ((*it).get() == pCamera)
        {
            auto ret = std::move(*it);
            mCameras.erase(it);
            return ret;
        }
        it++;
    }
    return nullptr;
}

void Model::AddCamera(UP_Camera pCamera)
{
    mCameras.push_back(std::move(pCamera));
}

void Model::SwapContainingCamera(MapObjectBase* pMapObject, Camera* pTargetCamera)
{
    pTargetCamera->mMapObjects.push_back(TakeFromContainingCamera(pMapObject));
}


void Model::LoadJsonFromString(const std::string& json)
{
    // TODO: Handle exception on bad data
    nlohmann::json root = nlohmann::json::parse(json);


   // mMapInfo.mGame = ReadString(root, "game");

    nlohmann::json map = ReadObject(root, "map");


    nlohmann::json cameras = ReadArray(map, "cameras");
    for (size_t i = 0; i < cameras.size(); i++)
    {
        nlohmann::json camera = cameras.at(static_cast<unsigned int>(i));

        auto tmpCamera = std::make_unique<Camera>();
        tmpCamera->mId = ReadNumber(camera, "id");
       // tmpCamera->mName = ReadString(camera, "name");
        tmpCamera->mX = ReadNumber(camera, "x");
        tmpCamera->mY = ReadNumber(camera, "y");

        /*
        tmpCamera->mCameraImageandLayers.mCameraImage = ReadStringOptional(camera, "image");
        tmpCamera->mCameraImageandLayers.mForegroundLayer = ReadStringOptional(camera, "foreground_layer");
        tmpCamera->mCameraImageandLayers.mBackgroundLayer = ReadStringOptional(camera, "background_layer");
        tmpCamera->mCameraImageandLayers.mForegroundWellLayer = ReadStringOptional(camera, "foreground_well_layer");
        tmpCamera->mCameraImageandLayers.mBackgroundWellLayer = ReadStringOptional(camera, "background_well_layer");
        */

        if (camera.contains("map_objects") && camera["map_objects"].is_array())
        {
            nlohmann::json mapObjects = ReadArray(camera, "map_objects");

            for (size_t j = 0; j < mapObjects.size(); j++)
            {
                nlohmann::json mapObject = mapObjects.at(static_cast<unsigned int>(j));
                auto tmpMapObject = std::make_unique<Path_TimedMine>(); // TODO: Create correct type based on the name
                //tmpMapObject->mName = ReadString(mapObject, "name");

                /*
                if (mapObject.has<jsonxx::Object>("properties"))
                {
                    jsonxx::Object properties = ReadObject(mapObject, "properties");
                    tmpMapObject->mProperties = ReadProperties(pObjStructure, properties);
                }
                */
                tmpCamera->mMapObjects.push_back(std::move(tmpMapObject));
                
            }
        }
        mCameras.push_back(std::move(tmpCamera));
    }

    nlohmann::json collisionsArray = ReadArray(map, "collisions");
    for (size_t i = 0; i < collisionsArray.size(); i++)
    {
        nlohmann::json collision = collisionsArray.at(static_cast<unsigned int>(i));
        auto tmpCollision = std::make_unique<CollisionObject>(static_cast<int>(i));
        collision.get_to(tmpCollision->mLine);
        mCollisions.push_back(std::move(tmpCollision));
    }

    CreateEmptyCameras();
}

void Model::LoadJsonFromFile(const std::string& jsonFile)
{
    std::optional<std::string> jsonString = LoadFileToString(jsonFile);
    if (!jsonString)
    {
        throw IOReadException(jsonFile);
    }

    LoadJsonFromString(*jsonString);
}

void Model::CreateAsNewPath(int newPathId)
{
    // Reset everything to a 1x1 empty map
    /*
    mMapInfo.mPathId = newPathId;
    mMapInfo.mXSize = 1;
    mMapInfo.mYSize = 1;
    */

    mCameras.clear();
    mCollisions.clear();

    auto cam = std::make_unique<Camera>();
    cam->mX = 0;
    cam->mY = 0;
    mCameras.emplace_back(std::move(cam));
}

std::string Model::ToJson() const
{
    nlohmann::json root;

   // root << "path_version" << mMapInfo.mPathVersion;
   // root << "game" << mMapInfo.mGame;

    nlohmann::json map;
    /*
    map << "path_bnd" << mMapInfo.mPathBnd;
    map << "path_id" << mMapInfo.mPathId;
    map << "x_grid_size" << mMapInfo.mXGridSize;
    map << "x_size" << mMapInfo.mXSize;
    map << "y_grid_size" << mMapInfo.mYGridSize;
    map << "y_size" << mMapInfo.mYSize;

    map << "abe_start_xpos" << mMapInfo.mAbeStartXPos;
    map << "abe_start_ypos" << mMapInfo.mAbeStartYPos;

    map << "num_muds_in_path" << mMapInfo.mNumMudsInPath;
    map << "total_muds" << mMapInfo.mTotalMuds;
    map << "num_muds_for_bad_ending" << mMapInfo.mBadEndingMuds;
    map << "num_muds_for_good_ending" << mMapInfo.mGoodEndingMuds;

    jsonxx::Array LCDScreenMessages;
    for (const auto& msg : mMapInfo.mLCDScreenMessages)
    {
        LCDScreenMessages << msg;
    }
    map << "lcdscreen_messages" << LCDScreenMessages;

    jsonxx::Array hintFlyMessages;
    for (const auto& msg : mMapInfo.mHintFlyMessages)
    {
        hintFlyMessages << msg;
    }
    map << "hintfly_messages" << hintFlyMessages;
    */

    nlohmann::json cameras;
    for (auto& camera : mCameras)
    {
       // if (!camera->mMapObjects.empty() || !camera->mCameraImageandLayers.mCameraImage.empty())
        {

            /*
            nlohmann::json{
            {"continue_point_zone_number", p.mContinuePoint_ZoneNumber},
            */

           
           // camObj << "name" << camera->mName;


            /*/
            if (!camera->mCameraImageandLayers.mCameraImage.empty())
            {
                camObj << "image" << camera->mCameraImageandLayers.mCameraImage;
            }

            if (!camera->mCameraImageandLayers.mForegroundLayer.empty())
            {
                camObj << "foreground_layer" << camera->mCameraImageandLayers.mForegroundLayer;
            }

            if (!camera->mCameraImageandLayers.mBackgroundLayer.empty())
            {
                camObj << "background_layer" << camera->mCameraImageandLayers.mBackgroundLayer;
            }

            if (!camera->mCameraImageandLayers.mForegroundWellLayer.empty())
            {
                camObj << "foreground_well_layer" << camera->mCameraImageandLayers.mForegroundWellLayer;
            }

            if (!camera->mCameraImageandLayers.mBackgroundWellLayer.empty())
            {
                camObj << "background_well_layer" << camera->mCameraImageandLayers.mBackgroundWellLayer;
            }
            */

            nlohmann::json mapObjects;
            for (auto& mapObject : camera->mMapObjects)
            {
                //nlohmann::json mapObj{
                //    {mapObjects, mapObj};
               // mapObj << "name" << mapObject->mName;
               // mapObj << "properties" << propertiesObject;
            }

            nlohmann::json camObj{
                {"id", camera->mId},
                {"x", camera->mX},
                {"y", camera->mY},
                {"map_objects", mapObjects}
            };

           // cameras << camObj;
        }
    }

    nlohmann::json collisionsObject;
    nlohmann::json collisionItems;
    for (auto& collision : mCollisions)
    {
        nlohmann::json collisionObj;
    //    collisionItems << collisionObj;
    }

    /*
    collisionsObject << "items" << collisionItems;
    map << "collisions" << collisionsObject;

    map << "cameras" << cameras;
    root << "map" << map;
    */

    return root;
}

Model::UP_CollisionObject Model::RemoveCollisionItem(Model::CollisionObject* pItem)
{
    for (auto it = mCollisions.begin(); it != mCollisions.end(); )
    {
        if ((*it).get() == pItem)
        {
            auto ret = std::move(*it);
            mCollisions.erase(it);
            return ret;
        }
        it++;
    }
    return nullptr;
}

void Model::CreateEmptyCameras()
{
    /*
    // Make sure every cell in the "map" has a camera object
    for (int x = 0; x < mMapInfo.mXSize; x++)
    {
        for (int y = 0; y < mMapInfo.mYSize; y++)
        {
            if (!CameraAt(x, y))
            {
                auto cam = std::make_unique<Camera>();
                cam->mX = x;
                cam->mY = y;
                mCameras.emplace_back(std::move(cam));
            }
        }
    }
    */
}


Model::CollisionObject::CollisionObject(int id, const Model::CollisionObject& rhs)
    : mId(id)
{

}
