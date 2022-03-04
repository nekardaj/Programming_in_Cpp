#ifndef HEADER_LOGICS
#define HEADER_LOGICS

#include <iostream>
#include <vector>

enum class Directions { Up, Down, Left, Right, None };

class BaseTile;
class BaseSprite;
using spritePtr = std::unique_ptr<BaseSprite>;
static class GameData
{
public:
	static std::unique_ptr<BaseTile> Map[4][4];
	static std::vector<spritePtr> Sprites;
	/// <summary>
	/// Loads layout from file
	/// </summary>
	/// <returns>returns true on success, false otherwise</returns>
	static bool LoadMap();
};

class BaseSprite
{
protected:
	int freeze = 0;
	int X;
	int Y;
	virtual bool CanMove()
	{
		return freeze == 0;
	}

public:
	virtual ~BaseSprite() = default;
	/// <summary>
	/// Makes the sprite move(player gets called with dir in params, enemies ignore it, i find it convinient to have common API)
	///	Player can be affected by tile only after calling this method - freeze can trigger >1 per move
	/// </summary>
	/// <param name="dir">Direction to move in</param>
	/// <returns></returns>
	virtual bool TakeTurn(Directions dir = Directions::None)
	{

		return true;
	}
	virtual void Die() = 0; //player and enemy need to have different imp
};


class BaseTile
{
protected:
	constexpr static int sizeX = 64;
	constexpr static  int sizeY = 64;
	const int X;
	const int Y;
public:
	virtual void Render()
	{
		
	}
	virtual void SpriteEntered(BaseSprite& sprite){} //needs to call nonconst methods
};


class Player : public BaseSprite
{
public:
	void Die() override //end the game
	{
		
	}
	bool TakeTurn(Directions dir) override;
	
};

#endif