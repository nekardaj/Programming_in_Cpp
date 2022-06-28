#include <SFML/Graphics.hpp>
#include <string>
#include "GameLogic.h"
#include <filesystem>
int main()
{
    const int width = 1600;
    const int height = 900;
    sf::RenderWindow window(sf::VideoMode(width, height), "SFML project");
    
    InitializePlayerData();
    InitializeBaseTile();
    std::string credits = "Block assets from: Ajay Karat | Devil's Work.shop\nhttp://devilswork.shop/";

    GameData::LoadNextLevel();
    auto map = GameData::levelData->map; //we need to acces map inside mainloop on every frame so we cache it
    sf::Font font;
    if(!font.loadFromFile(std::filesystem::absolute(std::filesystem::relative("..\\Assets\\")).string() + "\\corbel.ttf"))
    {
        throw std::exception("Font missing");
    }

    // main loop
    while (window.isOpen())
    {
	    switch (GameData::state)
	    {
        case GameState::Finished:
	    {
            sf::Event event;
            window.clear(sf::Color::Black);
            sf::Text text = sf::Text(sf::String("You won\n" + credits + "\nPress ESC to quit"), font, 20);
            text.setPosition(20, 20);
            window.draw(text);
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    window.close();
                }
                if (event.type == sf::Event::Resized)
                {
                    // update the view to the new size of the window
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                }
            }
            window.display();
            
	    }
        case GameState::Rules:
        {
            break;
        }
        case GameState::Menu:
        {
            break;
        }
        case GameState::BetweenLevels:
        {
            sf::Event event;
            window.clear(sf::Color::Black);
            sf::Text text = sf::Text(sf::String("You win. Press space to proceed to next level. ESC to quit."), font, 20);
            text.setPosition(20, 20);
            window.draw(text);
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    window.close();
                }
                if (event.type == sf::Event::Resized)
                {
                    // update the view to the new size of the window
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && GameData::state == GameState::BetweenLevels)
                {
                    GameData::LoadNextLevel();
                    GameData::state = GameState::Playing;
                    map = GameData::levelData->map;
                }
            }
            window.display();
            break;
        }
        case GameState::Lost:
        {
            sf::Event event;

            window.clear(sf::Color::Black);
            sf::Text text = sf::Text(sf::String("You lost\n" + credits + "\nPress R to restart level or ESC to quit."), font, 20);
            text.setPosition(20, 20);
            window.draw(text);
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    window.close();
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && GameData::state == GameState::Lost)
                {
                    GameData::RestartLevel(); //we load the level again bcs all sprites moved and tiles could break
                    GameData::state = GameState::Playing;
                    map = GameData::levelData->map;
                }
                if (event.type == sf::Event::Resized)
                {
                    // update the view to the new size of the window
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                }
            }
            window.display();
            break;
        }
        case GameState::Playing:
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::Resized) //makes resize not squeeze stuff but show more/less
                {
                    // update the view to the new size of the window
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                }
                if (event.type == sf::Event::KeyPressed)
                {
                    if (GameData::PlayerTurn)
                    {
                        const_cast<Player*>(GameData::GetPlayer())->ProcessInput();
                    }
                    //We use const cast however all logic is done inside player - ensures validity

                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                    {
                        window.close();
                    }
                }
            }

            //If it is not players turn take care of enemy movement, if animations are implemented procces them instead if they are playing atm
            if (!GameData::PlayerTurn) //add delay/anim?
            {
                for (auto&& item : GameData::levelData->Sprites)
                {
                    item->MakeMove();
                }
                GameData::PlayerTurn = true;
            }
            //Draw the scene
            window.clear(sf::Color::Transparent);
            /**/
            for (int i = 0; i < GameData::levelData->sizeY; i++)
            {
                for (int j = 0; j < GameData::levelData->sizeX; j++)
                {
                    map[j][i]->Render(window);
                    //window.display(); //Debug only
                }
            }
            for (auto&& item : GameData::levelData->Sprites)
            {
                item->Render(window);
            }
            //GameData::GetPlayer()->Render(window);
            GameData::GetPlayer()->Render(window);
            BaseTile::DrawGrid(); //takes care if the gridlines
            //GameData::GetPlayer()->MakeMove();
            /**/
            //map[2][2]->Render(window);

            //window.draw(BaseTile::GroundSprite);

            //window.draw(BaseTile::GroundSprite);
            //render spites

            window.display();

            break;
        }
	    }


        //take care of events such as player input, act if needed
    	
    }

    return 0;
}