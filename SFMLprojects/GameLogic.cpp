#include "GameLogic.h"

#include <fstream>
#include <sstream>
#include <filesystem>

void Enemy::TurnToPlayer(const BaseSprite* player)
{
	//If enemy could move only on tiles with shared edge it wouldnt be deterministic(or sensible)
	//Decreases distance as much as possible(manhattan metric)
	int diffX = player->GetX() - X;
	int diffY = player->GetY() - Y;
	//get sign of diff to determine direction
	//if wall are added check for walls and if there is one, resolve
	Direction_ = std::make_pair((diffX > 0) - (diffX < 0), (diffY > 0) - (diffY < 0));
}
#if 1 //region
sf::Sprite BaseTile::GroundSprite = sf::Sprite();

int GameData::currentLevel = 0;
std::string BaseTile::relativePath = "..\\Assets\\Blocks\\";
std::string BaseSprite::relativePath = "..\\Assets\\";
sf::Texture BaseTile::Texture_ = sf::Texture();
sf::Sprite Player::PlayerSprite = sf::Sprite();
sf::Texture Player::PlayerTexture = sf::Texture();
sf::Texture FreezingTile::Texture_ = sf::Texture();
sf::Sprite FreezingTile::Sprite_ = sf::Sprite();
sf::Sprite DestructibleTile::Sprite_ = sf::Sprite();
sf::Texture DestructibleTile::Texture_ = sf::Texture();
sf::Sprite Teleporter::Sprite_ = sf::Sprite();
sf::Texture Teleporter::Texture_ = sf::Texture();
sf::Sprite GoalTile::Sprite_ = sf::Sprite();
sf::Texture GoalTile::Texture_ = sf::Texture();
sf::Sprite Wall::Sprite_ = sf::Sprite();
sf::Texture Wall::Texture_ = sf::Texture();
sf::Sprite Enemy::EnemySprite = sf::Sprite();
sf::Texture Enemy::EnemyTexture = sf::Texture();
#endif
std::string GameData::UI_Img_Files[] = { std::string("TitleScreen.png"), std::string("Rules.png") ,
	std::string("BetweenLevels.png") ,std::string("VictoryScreen.png") ,std::string("Keyboard_Img.png"),
	std::string("Lost.png"), std::string("Credits.png") };
sf::Texture GameData::UI_Images_[] = {};

sf::RenderWindow GameData::window = sf::RenderWindow(sf::VideoMode(DefaultSizeX, DefaultSizeY), "Nimblest");


sf::Sprite GameData::ImageSprite = sf::Sprite();

Player* GameData::player = new Player(0, 0);
std::unique_ptr<LevelData> GameData::levelData = nullptr;

LevelData::LevelData(const std::string& mapFilename)
{
	{
		if(!LoadMap(mapFilename))
		{
			throw std::exception("Cant load map");
		}
	}
}
void GameData::LoadImages()
{
	for (int i = 0; i < (int)UI_Images::Count; i++ )
	{
		 if(!UI_Images_[i].loadFromFile(sf::String(AssetsPath + UI_Img_Files[i])))
		 {
			 throw std::exception("Failed to load images");
		 }
	}
}
void GameData::LoadNextLevel()
{
	currentLevel++;
	levelData = std::make_unique<LevelData>("..\\Assets\\level0" + std::to_string(currentLevel) + ".txt");
	PlayerTurn = true;
	//creating new level data resets player to right spot, preincrement makes 0th lvl read level1.txt as intended
}
void GameData::RestartLevel()
{
	levelData = std::make_unique<LevelData>("..\\Assets\\level0" + std::to_string(currentLevel) + ".txt");
	PlayerTurn = true;
}

/// <summary>
/// Resets player status and set location to passed arguments
/// </summary>
/// <param name="X">x coord</param>
/// <param name="Y">y coord</param>
void GameData::ResetPlayer(int X, int Y)
{
	player->Reset(X,Y);
}


bool LevelData::LoadMap(const std::string& filename)
{
	//const std::filesystem::path path = std::filesystem::absolute(std::filesystem::relative(filename));
	/*/
	if(std::filesystem::exists(path))
	{
		return false; //file doesnt exist return false
	}
	/**/
	std::ifstream file(filename);
	std::string line;
	std::getline(file, line);

	
	while (line != "#") //end of teleporters(special) section
	{
		std::stringstream s(line);
		std::string str;
		s >> str;
		if (str != "tel_pair") { return false; } //check for token(currently just teleporter is special so we dont need branching)
		if(s.bad())
		{
			return false;
		}
		int coords[4];
		coords[3] = -1;
		for (int i = 0; i < 4 && !s.bad();i++)
		{
			s >> coords[i];
		}
		if(coords[3] == -1) //we didnt read enough coords, failed to load
		{
			return false;
		}
		map[coords[0]][coords[1]] = std::make_unique<Teleporter>(coords[2], coords[3], coords[0], coords[1]); //target xy and location xy
		map[coords[2]][coords[3]] = std::make_unique<Teleporter>(coords[0], coords[1], coords[2], coords[3]);
		std::getline(file, line);
		if (file.bad())
		{
			return false;
		}
	}

	for(int i = sizeY - 1;i>=0;--i) //correct y coord
	{
		std::getline(file,line);
		if(file.bad())
		{
			break;
		}
		for (int j = 0; j < line.length() && j < sizeX;++j)
		{
			switch (line[j])
			{
				case 'F': //freezing tile
				{
					map[j][i] = std::make_unique<FreezingTile>(j, i);
					break;
				}
				case 'B': //breakable tile
				{
					map[j][i] = std::make_unique<DestructibleTile>(j, i);
					break;
				}
				case 'P': //player on a (normal) tile
				{
					map[j][i] = std::make_unique<BaseTile>(j, i);
					GameData::ResetPlayer(j, i); //set new pos and remove effects
					break;
				}
				case 'E': //enemy on a (normal) tile
				{
					map[j][i] = std::make_unique<BaseTile>(j, i);
					Sprites.push_back(std::make_unique<Enemy>(j,i));
					break;
				}
				case 'G': //target
				{
					map[j][i] = std::make_unique<GoalTile>(j, i);
					break;
				}
				case 'W': //wall
				{
					map[j][i] = std::make_unique<Wall>(j, i);
				}
				case 'T': //teleporter, it was already created is specials section
				{
					break;
				}
				default: //there is a letter and it is no special tile - normal tile
				{
					map[j][i] = std::make_unique<BaseTile>(j, i);
				}
			}
			//Teleporter is handled separately as we need to pair them(or at least know location and I suppose this leaves more freedom)
		}
	}

	return true;
}
UI_Images GameData::image = UI_Images::Menu; //holds the enum  value of image to be shown
void GameData::MainLoop()
{
    std::string credits = R"(Block assets from: Ajay Karat | Devil's Work.shop\nhttp://devilswork.shop/
        Keyboard assets were created by Caz Wolf, and are available itch.io: https://cazwolf.itch.io/ 
        )";
    auto map = levelData->map; //we need to acces map inside mainloop on every frame so we cache it
	LoadImages();
    image = UI_Images::Menu;
    ShowImage();
    // main loop
    while (window.isOpen())
    {
        switch (state)
        {
        case GameState::Victory:
	    {
            Victory();
			break;
	    }
        case GameState::Credits: //we want same behaviour as in rules just different image - set image and fall to case with same behaviour
        {
            image = UI_Images::Credits;
            CreditsRules();
            break;
        }
        case GameState::Rules:
        {
            image = UI_Images::Rules;
            CreditsRules();
            break;
        }
        case GameState::Menu:
        {
            Menu();
            break;
        }
        case GameState::BetweenLevels:
        {
            BetweenLevels();
            break;
        }
        case GameState::Lost:
        {
            Lost();
            break;
        }
        case GameState::Playing:
        {
            Playing();
            break;
        }
        }

    }
}

void GameData::Victory()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        {
            state = GameState::Menu;
            image = UI_Images::Menu;
        	currentLevel = 0; //pressing play would otherwise crash the game(load level out of range)
            ShowImage();
        }
        if (event.type == sf::Event::Resized)
        {
            // update the view to the new size of the window
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));
        }
    }
}

void GameData::CreditsRules()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        {
            state = GameState::Menu;
            image = UI_Images::Menu;
        	ShowImage();
        }
        if (event.type == sf::Event::Resized)
        {
            // update the view to the new size of the window
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));
        }
    }
}

void GameData::Menu()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && GameData::state == GameState::Menu)
        {
            state = GameState::Playing;
            LoadNextLevel();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
        {
            state = GameState::Rules;
            image = UI_Images::Rules;
            ShowImage();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
        {
            state = GameState::Credits;
            image = UI_Images::Credits;
            ShowImage();
        }
        if (event.type == sf::Event::Resized)
        {
            // update the view to the new size of the window
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));
        }
    }
}

void GameData::BetweenLevels()
{
    sf::Event event;
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && state == GameState::BetweenLevels)
        {
            LoadNextLevel();
            state = GameState::Playing;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        {
            state = GameState::Menu;
            image = UI_Images::Menu;
            ShowImage();
        }
    }
}

void GameData::Lost()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && state == GameState::Lost)
            //there can be events left this makes sure we restart once
        {
            RestartLevel(); //we load the level again bcs all sprites moved and tiles could break
            state = GameState::Playing;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::M) && state == GameState::Lost)
        {
            state = GameState::Menu;
            image = UI_Images::Menu;
            currentLevel -= 1;//so that load next level loads this one
            ShowImage();
        }
        if (event.type == sf::Event::Resized)
        {
            // update the view to the new size of the window
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));
        }
    }
}

void GameData::Playing()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::Resized) //resize does not squeeze stuff but show more/less
        {
            // update the view to the new size of the window
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));
        }
        if (event.type == sf::Event::KeyPressed)
        {
            if (PlayerTurn)
            {
                player->ProcessInput();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                window.close();
            }
        }
    }

    //If it is not players turn take care of enemy movement, if animations are implemented procces them instead if they are playing atm
    if (!PlayerTurn) //add delay/anim?
    {
        for (auto&& item : levelData->Sprites)
        {
            item->MakeMove();
        }
        PlayerTurn = true;
    }
    //Draw the scene
    if(state != GameState::Playing) { return;} //we dont overwrite static win / loss img
    window.clear(sf::Color::Transparent);
    /**/
    for (int i = 0; i <levelData->sizeY; i++)
    {
        for (int j = 0; j < levelData->sizeX; j++)
        {
            levelData->map[j][i]->Render(window);
            //window.display(); //Debug only
        }
    }
    ImageSprite.setTexture(UI_Images_[(int)UI_Images::Keyboard]);
    ImageSprite.setPosition(50, 750);
    window.draw(ImageSprite);
    ImageSprite.setPosition(0, 0);
    for (auto&& item : levelData->Sprites)
    {
        item->Render(window);
    }
    GetPlayer()->Render(window);
    BaseTile::DrawGrid(); //takes care if the gridlines
    window.display();

}

