#pragma once

#include "JsonModelTypes.hpp"
#include "PropertyCollection.hpp"
#include "../../relive_lib/Collisions.hpp"
#include "TlvObjectBaseMacros.hpp"

namespace ReliveAPI {
class AOLine final : public PropertyCollection
{
public:
    AOLine(TypesCollectionBase& globalTypes, const PathLineAO* line = nullptr)
    {
        if (line)
        {
            mLine = *line;
        }

        ADD("x1", mLine.mRect.x);
        ADD("y1", mLine.mRect.y);

        ADD("x2", mLine.mRect.w);
        ADD("y2", mLine.mRect.h);

        ADD("Type", mLine.mLineType);

        ADD("Next", mLine.mNext);
        ADD("Previous", mLine.mPrevious);
    }

    PathLineAO mLine = {};
};

class AELine final : public PropertyCollection
{
public:
    AELine(TypesCollectionBase& globalTypes, const PathLineAE* line = nullptr)
    {
        if (line)
        {
            mLine = *line;
        }

        ADD("x1", mLine.mRect.x);
        ADD("y1", mLine.mRect.y);

        ADD("x2", mLine.mRect.w);
        ADD("y2", mLine.mRect.h);

        ADD("Type", mLine.mLineType);

        ADD("Next", mLine.mNext);
        ADD("Previous", mLine.mPrevious);

        ADD("Next 2", mLine.mNext2);
        ADD("Previous 2", mLine.mPrevious2);

        ADD("Length", mLine.mLineLength);
    }

    PathLineAE mLine = {};
};

class IFileIO;
class Context;


struct LoadedJsonBase final
{
    std::vector<CameraNameAndTlvBlob> mPerCamData;
    std::set<AnimId> mResourcesRequiredInLvl;
    jsonxx::Object mMapJson;
};

class JsonReaderBase
{
public:
    MapInfo mRootInfo;

protected:
    LoadedJsonBase Load(TypesCollectionBase& types, IFileIO& fileIO, const std::string& fileName, Context& context);

    std::vector<PathLineAO> ReadAOLines(TypesCollectionBase& types, const jsonxx::Array& collisionsArray, Context& context);
    std::vector<::PathLineAE> ReadAELines(TypesCollectionBase& types, const jsonxx::Array& collisionsArray, Context& context);
};
} // namespace ReliveAPI
