#include <SFML/Graphics.hpp>
#include <string>
#include "GameLogic.h"
#include <filesystem>
int main()
{
    const int width = 1280;
    const int height = 800;
    sf::RenderWindow window(sf::VideoMode(width, height), "SFML project");
    
    InitializePlayerData();
    InitializeBaseTile();
    std::string credits = "Block assets from: Ajay Karat | Devil's Work.shop\nhttp://devilswork.shop/";
    //to use isometry we count all as if it was in in xy grid and then rotate by matrix
    /*/
    //this does same thing as load texture from file but i can downscale the img(area takes subrectangle not downsc)
    //sprite set scale or sf::view
	sf::Image image;
    image.loadFromFile(filename);
    texture.loadFromImage(image, area);
    */

	//TODO use SetOrigin to rotate things easily(doesnt break centring), then use lib function
    
    //sprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
    GameData::ResetPlayer(3, 3);
    GameData::levelData->Sprites.push_back(std::make_unique<Enemy>(7,8));


    auto& map = GameData::levelData->map; //we need to acces map inside mainloop on every frame so we cache it
    sf::Font font;
    if(!font.loadFromFile(std::filesystem::absolute(std::filesystem::relative("..\\Assets\\")).string() + "\\corbel.ttf"))
    {
        throw std::exception("Font missing");
    }
    while (window.isOpen())
    {
        //take care of events such as player input, act if needed
    	sf::Event event;
        if(GameData::state == GameState::Lost || GameData::state == GameState::Won)
        {
            window.clear(sf::Color::Black);
            sf::Text text = sf::Text(sf::String((GameData::state == GameState::Lost ? "You lost\n" : "You won\n") + credits), font, 20);
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
	        continue;
        }
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
            if(event.type == sf::Event::KeyPressed)
            {
                const_cast<Player*>(GameData::GetPlayer())->ProcessInput();
                //We use const cast however all logic is done inside player - ensures validity
	            
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    window.close();
                }
            }
        }
        //If it is not players turn take care of enemy movement, if animations are implemented procces them instead if they are playing atm
        if(!GameData::PlayerTurn) //add delay/anim?
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
    }

    return 0;
}