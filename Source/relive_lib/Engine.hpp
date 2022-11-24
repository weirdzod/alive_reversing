#pragma once

#include "GameType.hpp"

class Engine final
{
public:
    Engine(GameType gameType, const char_type* pCommandLine);
    void Run();

private:
    GameType mGameType = GameType::eAe;
    const char_type* mCommandLine = nullptr;
};
