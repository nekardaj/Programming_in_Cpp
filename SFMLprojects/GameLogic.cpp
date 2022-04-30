#include "GameLogic.h"

#include <fstream>
#include <filesystem>

void Enemy::TurnToPlayer(const BaseSprite* player)
{
	{
		int difX = player->GetX() - X;
		int difY = player->GetY() - Y;
		if (difX > 0 && difY > 0)
		{
			if (difX > difY)
			{
				Direction_ = std::make_pair(1, 0);
			}
			else
			{
				Direction_ = std::make_pair(0, 1);
			}
			return;
		}
		if (difX < 0 && difY < 0)
		{
			if (difX < difY)
			{
				Direction_ = std::make_pair(1, 0);
			}
			else
			{
				Direction_ = std::make_pair(0, 1);
			}
			return;
		}
		//one dif is positive and one negative
		int flip = difY > 0 ? -1 : 1;
		if(difX > difY)
		{
			
		}
	}
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

bool LevelData::LoadMap(const std::string& filename)
{
	const std::filesystem::path path{ filename };
	if(std::filesystem::exists(path))
	{
		return false; //file doesnt exist return false
	}
	std::ifstream file(filename);
	std::string line;
	int teleporterX = -1;
	int teleporterY = -1;
	for(int i = sizeY - 1;i>=0;--i)
	{
		std::getline(file,line);
		if(file.bad())
		{
			break;
		}
		for (int j = 0; j < line.length() && j < sizeX;++j)
		{
			std::vector<std::pair<int, int>> teleporters(maxTeleporters, std::make_pair(-1,-1));
			if(islower( line[j])) //lowercase will be teleporter
			{
				if(teleporters[line[j] - 't'].first == -1)
				{
					teleporters[line[j] - 't'] = std::make_pair(j, i);
				}
				
			}
			switch (line[j])
			{
			case 'N': //normal tile
				{
					break;
				}
			case 'F': //freezing tile
			{
				break;
			}
			case 't':
			case 'u':
			case 'v':
			case 'w':
			{
				break;
			}
			case 'B': //breakable tile
			{
				break;
			}
			case 'f':
			{
				break;
			}
			}
		}
	}

	return true;
}
