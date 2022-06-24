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
sf::Sprite BaseTile::GroundSprite = sf::Sprite();
std::unique_ptr<LevelData> GameData::levelData = std::make_unique<LevelData>("phony.lvl");
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

Player* GameData::player = new Player(0, 0);

LevelData::LevelData(const std::string& mapFilename)
{
	{
		/*/
		if(LoadMap(mapFilename))
		{
			throw std::exception("Cant load map");
		}
		*/
		for (int i = 0; i < sizeX; ++i)
		{
			for (int j = 0; j < sizeY; ++j)
			{
				map[i][j] = std::make_unique<BaseTile>(i, j);
			}
		}
		map[3][4] = std::make_unique<FreezingTile>(3,4);
		map[5][5] = std::make_unique<DestructibleTile>(5, 5);
		map[7][6] = std::make_unique<Teleporter>(8,9,7,6);
		map[8][9] = std::make_unique<Teleporter>(7, 6, 8, 9);
		map[10][5] = std::make_unique<GoalTile>(10,5);
	}
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
	const std::filesystem::path path{ filename };
	if(std::filesystem::exists(path))
	{
		return false; //file doesnt exist return false
	}
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
		if(coords[3] == -1)
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
					Sprites.insert(Sprites.begin(), std::make_unique<Player>(j, i));//we want player to be at start of list
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
