#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include "GameLogic.h"
#include <filesystem>
#include <filesystem>
int main()
{
    InitializePlayerData();
    InitializeBaseTile();
    GameData::LoadImages();
    sf::Music music;
    if(music.openFromFile(sf::String("..\\Assets\\BackgroundMusic.ogg")))
    {
        music.play();
        music.setLoop(true); //play bg music on loop
    }
    
    GameData::SetVsync(true); //makes sure rendering happens when needed
    GameData::MainLoop();

    return 0;
}