#ifndef HEADER_LOGICS
#define HEADER_LOGICS

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

enum class Directions { Up, Down, Left, Right, None };
using Direction = std::pair<int_fast8_t, int_fast8_t>; //we only need +-1 - use the most efficient type

class BaseTile;
class BaseSprite;
using spritePtr = std::unique_ptr<BaseSprite>;
/// <summary>
/// Instantiated for each level, gamedata hold the data that dont always change with level
/// </summary>
class LevelData
{
public:
	static constexpr  int sizeX = 12;
	static constexpr  int sizeY = 12;
	std::unique_ptr<BaseTile> map[sizeX][sizeY];
	/// <summary>
	/// Players should always be at pos 0 he will always die last so we dont move whole vector
	/// </summary>
	std::vector<spritePtr> Sprites;
	/// <summary>
	/// Loads layout from file
	/// </summary>
	/// <returns>returns true on success, false otherwise</returns>
	static bool LoadMap();
	bool End = false;
};

class GameData
{
public:
	static constexpr int LevelCount = 1;
	static int currentLevel;
	static std::string Levels[];
	static std::unique_ptr<LevelData> levelData;
};
std::string GameData::Levels[] =
{
		"level01.txt"
};
int GameData::currentLevel = 1;
//these values need to be initialized
class BaseTile
{
protected:
	constexpr static int sizeX = 64;
	constexpr static  int sizeY = 64;
	const int X;
	const int Y;
public:
	/// <summary>
	/// Takes care of displaying the sprite(special tiles need multiple draws)
	/// </summary>
	virtual void Render()
	{
		
	}
	BaseTile(int x, int y) : X(x), Y(y)
	{
		
	}
	virtual void SpriteEntered(BaseSprite* sprite){} //needs to call nonconst methods, should be called by sprite passing itself
	virtual ~BaseTile() = default;
};

class BaseSprite
{
protected:
	int freeze = 0;
	int X;
	int Y;
	Direction Direction_ = std::make_pair(0, 0);

public:
	int GetX() { return X; };
	int GetY() { return Y; }
	virtual void SetLocation(int x, int y) //if sprite entered needs to be called it is callers responibility
	{
		X = x;
		Y = y;
		//we cant call sprite entered as teleport tile would cause unbounded reccursion
	}
	virtual void SetDirection(Direction dir)
	{
		Direction_ = dir;
	}
	virtual ~BaseSprite() = default;
	/// <summary>
	/// Makes the sprite move. 
	///	Player can be affected by tile only after calling this method - freeze cant trigger >1 per move (exception are tiles that move player)
	/// </summary>
	/// <returns></returns>
	virtual bool MakeMove()
	{
		if (freeze > 0)
		{
			--freeze;
			return true;
		}
		X += Direction_.first;
		Y += Direction_.second;
		GameData::levelData->map[X][Y]->SpriteEntered(this);
		//map
		return true;
	}
	virtual void Die() = 0; //player and enemy need to have different imp
};

class Teleporter : public BaseTile
{
protected:
	const int DestinationX;
	const int DestinationY;
	//portal should not change its 
public:
	Teleporter(int destinationX, int destinationY, int x,int y) : BaseTile(x,y), DestinationX(destinationX), DestinationY(destinationY)
	{
		
	}
	~Teleporter() override = default;
	/// <summary>
	/// assumes that portals are paired and symmetric(A<->B), if not check whether SpriteEntered needs to be called(paired TPs would cause crash)
	/// </summary>
	/// <param name="sprite"></param>
	void SpriteEntered(BaseSprite* sprite) override
	{
		sprite->SetLocation(DestinationX, DestinationY);
	}
};

class FreezingTile : public BaseTile
{
public:
	FreezingTile(int x, int y) : BaseTile(x,y)
	{
		
	}
	static constexpr int FreezeDuration = 1;
	void SpriteEntered(BaseSprite* sprite) override
	{
		
	}
};

class DesturctibleTile : public BaseTile
{
protected:
	static constexpr int tileDurability = 1;
	int durability;

public:
	DesturctibleTile(int x, int y) : BaseTile(x, y)
	{
		durability = tileDurability;
	}
	void SpriteEntered(BaseSprite* sprite) override
	{
		if(durability-- < 1) //everytime this is true one sprite dies - cant underflow in any reasonable map
		{
			sprite->Die();
		}
	}
	
};


class Player : public BaseSprite
{
public:
	void Die() override //end the game
	{
		
	}
	bool MakeMove() override;
	
};

class Enemy : public BaseSprite
{
protected:
	virtual void TurnToPlayer() //TODO solve how to fetch player pos(GameData::levelData->Sprites[0]->GetX() seems to complicated)
	{
		
	}
public:
	bool MakeMove() override
	{
		TurnToPlayer(); //we need to change orientation and then do the same thing as defined in BaseSprite so we call base method
		BaseSprite::MakeMove();
	}
};

class GoalTile : public BaseTile
{
public:
	void SpriteEntered(BaseSprite* sprite) override
	{
		//this check will be performed only few times per whole game overhead is unsignificant
		if (dynamic_cast<Player*>(sprite) != nullptr) //child of player entered - win
		{
			GameData::levelData->End = true;
		}
	}
};

#endif