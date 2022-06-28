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
	//TODO if wall are added check for walls and if there is one, resolve
	Direction_ = std::make_pair((diffX > 0) - (diffX < 0), (diffY > 0) - (diffY < 0));
}
#if 1 //region
sf::Sprite BaseTile::GroundSprite = sf::Sprite();

int GameData::currentLevel = 0;
std::string BaseTile::relativePath = "..\\Assets\\Blocks\\";
std::string BaseSprite::relativePath = "..\\Assets\\Blocks\\";
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
					GameData::ResetPlayer(j, i);//we want player to be at start of list
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
