#include <SFML/Graphics.hpp>
#include <string>
#include "GameLogic.h"
int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 800), "SFML project");
    sf::Texture testTexture;
    
    if (!testTexture.loadFromFile("C:\\Users\\Honza\\Desktop\\ukoly\\C++_husak\\nekardaj\\SFMLprojects\\Assets\\city_game_tileset\\ground_tile_porous2.png"))
    {
        //error
    }
    sf::Sprite sprite;
    std::string credits = "Block assets from: Ajay Karat | Devil's Work.shop\nhttp://devilswork.shop/";
    //to use isometry we count all as if it was in in xy grid and then rotate by matrix
    /*/
    //this does same thing as load texture from file but i can downscale the img(area takes subrectangle not downsc)
    //sprite set scale or sf::view
	sf::Image image;
    image.loadFromFile(filename);
    texture.loadFromImage(image, area);
    */
    sprite.setTexture(testTexture);
    //sprite.setTextureRect(sf::IntRect(0, 0, 64, 64));
    sprite.setScale(sf::Vector2f(0.5f,0.5f));
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        window.draw(sprite);
    	window.display();
    }

    return 0;
}