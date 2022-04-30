#ifndef HEADER_LOGICS
#define HEADER_LOGICS

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <filesystem>

enum class Directions { Up, Down, Left, Right, None };

enum class GameState { Menu, Playing, Lost, Won  };
using Direction = std::pair<int_fast8_t, int_fast8_t>; //we only need +-1 - use the most efficient type

class DestructibleTile;
class FreezingTile;
class BaseTile;
class BaseSprite;
using spritePtr = std::unique_ptr<BaseSprite>;
using tilePtr = std::unique_ptr<BaseTile>;
class BaseTile
{
protected:
	const int X;
	const int Y;
public:
	constexpr static int MultiplierX = 2;
	constexpr static int MultiplierY = 2;
	constexpr static int shiftX = 256 + 128;
	constexpr static int shiftY = 64;
	constexpr static int sizeX = MultiplierX * 64;
	constexpr static  int sizeY = MultiplierY * 32;//TODO

	constexpr static int midTileX = 191;
	constexpr static int midTileY = 728;
	static std::string relativePath;
	static sf::Sprite GroundSprite; //breakable tiles need to change, we can use one sprite for each type as changes happend rarely
	static sf::Texture Texture_;
	/// <summary>
	/// Takes care of displaying the sprite(special tiles need multiple draws)
	/// </summary>
	virtual void Render(sf::RenderWindow& window)
	{
		//GroundSprite.setPosition(sizeX * (X/2) + shift - sizeX * 0.5 * (X % 2), sizeY * (Y / 2) + shift + sizeY * 0.5 * (Y % 2)); //shift makes sure the sprite doesnt sit right on edge
		GroundSprite.setPosition(shiftX +(X-Y)*(sizeX/ 2.0f), (sizeY/2.0f) * (X+Y) + shiftY); //shift makes sure the sprite doesnt sit right on edge
		//int division y/2 is intended - half move happens in odd number Y/2 stays same
		window.draw(GroundSprite);
	}
	virtual void SetTexture(const sf::Texture& texture)
	{
		GroundSprite.setTexture(texture);
	}
	BaseTile(int x, int y) : X(x), Y(y)
	{
		
	}
	virtual void SpriteEntered(BaseSprite* sprite){} //needs to call nonconst methods, should be called by sprite passing itself
	virtual ~BaseTile() = default;
	static void DrawGrid()
	{
		
	}
};


/// <summary>
/// Instantiated for each level, gamedata hold the data that dont always change with level
/// </summary>
class LevelData
{
public:
	LevelData(const std::string& mapFilename);
	static constexpr  int sizeX = 12;
	static constexpr  int sizeY = 12;
	static constexpr int maxTeleporters = 6;
	tilePtr map[sizeX][sizeY];
	/// <summary>
	/// Players should always be at pos 0 he will always die last so we dont move whole vector
	/// </summary>
	std::vector<spritePtr> Sprites;
	/// <summary>
	/// Loads layout from file
	/// </summary>
	/// <returns>returns true on success, false otherwise</returns>
	bool LoadMap(const std::string& filename); //Initialize grid drawing with size
	bool End = false;
};

class GameData
{
public:
	static constexpr int LevelCount = 1;
	static int currentLevel;
	static std::string Levels[];
	static std::unique_ptr<LevelData> levelData;
	inline static GameState state = GameState::Playing;
};
/*/
std::string GameData::Levels[] =
{
		"level01.txt"
};
*/
//int GameData::currentLevel = 1;
//these values need to be initialized




class BaseSprite
{
protected:
	int freeze = 0;
	int X;
	int Y;
	Direction Direction_ = std::make_pair(0, 0);

public:
	static std::string relativePath;
	int GetX() const { return X; }
	int GetY() const { return Y; }
	virtual void Freeze(int duration) { freeze += duration; }
	virtual void Render(sf::RenderWindow& window) = 0; //each sprite has to define this alone as it can be composed of more things or depend on state..
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
	static sf::Texture Texture_;
	static sf::Sprite Sprite_;
	void Render(sf::RenderWindow& window) override
	{
		Sprite_.setPosition(shiftX + (X - Y) * (sizeX / 2.0f), (sizeY / 2.0f) * (X + Y) + shiftY); //shift makes sure the sprite doesnt sit right on edge
		//int division y/2 is intended - half move happens in odd number Y/2 stays same
		window.draw(Sprite_);
	}
	void SetTexture(const sf::Texture& texture) override
	{
		Texture_ = texture;
	}
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
	static sf::Texture Texture_;
	static sf::Sprite Sprite_;
	void Render(sf::RenderWindow& window) override
	{
		Sprite_.setPosition(shiftX + (X - Y) * (sizeX / 2.0f), (sizeY / 2.0f) * (X + Y) + shiftY); //shift makes sure the sprite doesnt sit right on edge
		//int division y/2 is intended - half move happens in odd number Y/2 stays same
		window.draw(Sprite_);
	}
	void SetTexture(const sf::Texture& texture) override
	{
		Texture_ = texture;
	}
	FreezingTile(int x, int y) : BaseTile(x,y)
	{
		
	}
	static constexpr int FreezeDuration = 1;
	void SpriteEntered(BaseSprite* sprite) override
	{
		sprite->Freeze(FreezeDuration);
	}
};

class DestructibleTile : public BaseTile
{
protected:
	static constexpr int tileDurability = 1;
	int durability;

public:
	static sf::Texture Texture_;
	static sf::Sprite Sprite_;
	void Render(sf::RenderWindow& window) override
	{
		if (durability <= 0)
		{
			return; //if the tile was destroyed draw differently
		}
		Sprite_.setPosition(shiftX + (X - Y) * (sizeX / 2.0f), (sizeY / 2.0f) * (X + Y) + shiftY); //shift makes sure the sprite doesnt sit right on edge
		//int division y/2 is intended - half move happens in odd number Y/2 stays same
		window.draw(Sprite_);
	}
	void SetTexture(const sf::Texture& texture) override
	{
		Texture_ = texture;
	}
	DestructibleTile(int x, int y) : BaseTile(x, y)
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
	static sf::Sprite PlayerSprite;
	static sf::Texture PlayerTexture;
	void Die() override //end the game
	{
		GameData::state = GameState::Lost;
	}
	void Render(sf::RenderWindow& window) override
	{
		PlayerSprite.setPosition(BaseTile::shiftX + (X - Y) * (BaseTile::sizeX / 2.0f), (BaseTile::sizeY / 2.0f) * (X + Y - 2) + BaseTile::shiftY);
		

		window.draw(PlayerSprite);
	}
	//make move doesnt need override
	bool MakeMove() override
	{
		if(X + Direction_.first < 0 || X + Direction_.first >= GameData::levelData->sizeX || Y + Direction_.second < 0 || Y + Direction_.second >= GameData::levelData->sizeY)
		{
			return false;
		}
		return BaseSprite::MakeMove();
	}
	
};

class Enemy : public BaseSprite
{
protected:
	virtual void TurnToPlayer(const BaseSprite* player); //TODO solve how to fetch player pos(GameData::levelData->Sprites[0]->GetX() seems to complicated)
	
public:
	bool MakeMove() override
	{
		//TurnToPlayer(); //we need to change orientation and then do the same thing as defined in BaseSprite so we call base method
		BaseSprite::MakeMove();
	}
	void Render(sf::RenderWindow& window) override
	{
		
	}
};

static inline void InitializePlayerData()
{
	auto path = absolute(std::filesystem::relative(BaseSprite::relativePath));
	if (!Player::PlayerTexture.loadFromFile(path.string() + "\\blocks_11.png"))
	{
		throw std::exception("Cant load file");
	}
	sf::Sprite sprite;
	sprite.setTexture(Player::PlayerTexture);
	//sprite.setOrigin(BaseTile::midTileX, BaseTile::midTileY);
	//sf::IntRect rect(110, 640, 164 , 164); //gets 164x164 square
	//sprite.setTextureRect(rect); //cut only the part we want
	//sprite.setScale(0.3902439, 0.3902439); //ration of 64/164
	sprite.setScale(2, 2);
	Player::PlayerSprite = sprite;
}

class GoalTile : public BaseTile
{
public:
	void Render(sf::RenderWindow& window) override
	{
		Sprite_.setPosition(shiftX + (X - Y) * (sizeX / 2.0f), (sizeY / 2.0f) * (X + Y) + shiftY); //shift makes sure the sprite doesnt sit right on edge
		//int division y/2 is intended - half move happens in odd number Y/2 stays same
		window.draw(Sprite_);
	}
	static sf::Texture Texture_;
	static sf::Sprite Sprite_;
	GoalTile(int x, int y) : BaseTile(x,y)
	{
		
	}
	void SetTexture(const sf::Texture& texture) override
	{
		Texture_ = texture;
	}
	void SpriteEntered(BaseSprite* sprite) override
	{
		//this check will be performed only few times per whole game overhead is unsignificant
		if (dynamic_cast<Player*>(sprite) != nullptr) //child of player entered - win
		{
			GameData::levelData->End = true;
			GameData::state = GameState::Won;
		}
	}
};

static inline void InitializeBaseTile()
{
	/**/
	auto path =  absolute(std::filesystem::relative(BaseTile::relativePath));
	if (!BaseTile::Texture_.loadFromFile(path.string() + "\\blocks_1_line.png"))
	{
		throw std::exception("Cant load file");
	}
	BaseTile::GroundSprite.setTexture(BaseTile::Texture_);
	//sprite.setOrigin(BaseTile::midTileX, BaseTile::midTileY);
	//sf::IntRect rect(110, 640, 164 , 164); //gets 164x164 square
	//sprite.setTextureRect(rect); //cut only the part we want
	//sprite.setScale(0.3902439, 0.3902439); //ration of 64/164
	BaseTile::GroundSprite.setScale(2, 2);
	BaseTile::GroundSprite = BaseTile::GroundSprite;
	/**/

	if (!FreezingTile::Texture_.loadFromFile(path.string() + "\\blocks_69.png"))
	{
		throw std::exception("Cant load file");
	}
	FreezingTile::Sprite_.setTexture(FreezingTile::Texture_);
	FreezingTile::Sprite_.setScale(2, 2);

	if (!DestructibleTile::Texture_.loadFromFile(path.string() + "\\blocks_28.png"))
	{
		throw std::exception("Cant load file");
	}
	DestructibleTile::Sprite_.setTexture(DestructibleTile::Texture_);
	DestructibleTile::Sprite_.setScale(2, 2);

	if (!Teleporter::Texture_.loadFromFile(path.string() + "\\blocks_19.png"))
	{
		throw std::exception("Cant load file");
	}

	Teleporter::Sprite_.setTexture(Teleporter::Texture_);
	Teleporter::Sprite_.setScale(2, 2);

	if (!GoalTile::Texture_.loadFromFile(path.string() + "\\blocks_100.png"))
	{
		throw std::exception("Cant load file");
	}
	GoalTile::Sprite_.setTexture(GoalTile::Texture_);
	GoalTile::Sprite_.setScale(2, 2);
}

#endif