#include <SFML/Graphics.hpp>
#include <string>
#include "GameLogic.h"
#include <filesystem>
int main()
{
    InitializePlayerData();
    InitializeBaseTile();
    GameData::LoadImages();
    GameData::MainLoop();

    return 0;
}