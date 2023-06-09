#ifndef HEADER_LOGICS
#define HEADER_LOGICS

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <filesystem>


enum class GameState { Menu, Playing, Lost, Victory, BetweenLevels, Rules, Credits  };
using Direction = std::pair<int_fast8_t, int_fast8_t>; //we only need +-1 - use the most efficient type

class DestructibleTile;
class FreezingTile;
class BaseTile;
class BaseSprite;
class Player;
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
	constexpr static int shiftX =  12 * 64;
	constexpr static int shiftY = 64;
	constexpr static int sizeX = MultiplierX * 64;
	constexpr static  int sizeY = MultiplierY * 32;
	static std::string relativePath;
	static sf::Sprite GroundSprite; //breakable tiles need to change, we can use one sprite for each type as changes happend rarely
	static sf::Texture Texture_;
	/// <summary>
	/// Takes care of displaying the sprite(special tiles need multiple draws)
	/// </summary>
	virtual void Render(sf::RenderWindow& window)
	{
		GroundSprite.setPosition(shiftX +(X-Y)*(sizeX/ 2.0f), (sizeY/2.0f) * (X+Y) + shiftY); //shift makes sure the sprite doesnt sit right on edge
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
	tilePtr map[sizeX][sizeY];
	/// <summary>
	/// Holds non-player sprites
	/// </summary>
	std::vector<spritePtr> Sprites;
	/// <summary>
	/// Loads layout from file
	/// </summary>
	/// <returns>returns true on success, false otherwise</returns>
	bool LoadMap(const std::string& filename); //Initialize grid drawing with size
};

enum class UI_Images{ Menu, Rules, BetweenLevels, Victory, Keyboard, Lost, Credits, Count  };
class GameData
{
public:
	static constexpr int LevelCount = 2;
	static int currentLevel;
	static std::unique_ptr<LevelData> levelData;
	inline static GameState state = GameState::Menu;
	inline static bool PlayerTurn = true;
	static sf::Sprite ImageSprite;
	static  constexpr int DefaultSizeX = 1600;
	static  constexpr int DefaultSizeY = 900;
	static void LoadImages();
	inline  static const std::string AssetsPath = "..\\Assets\\";
	//Images for UI
	static  sf::Texture UI_Images_[(int)UI_Images::Count];

	static std::string UI_Img_Files[(int)UI_Images::Count];
	//hack that makes sure size is always the same as Screen count
	static void SetVsync(bool value)
	{
		window.setVerticalSyncEnabled(value);
	}
	static void ShowImage()
	{
		window.clear(sf::Color::Black);
		ImageSprite.setTexture(UI_Images_[(int)image]);
		ImageSprite.setTextureRect(sf::IntRect(0,0,1600,900));
		ImageSprite.setPosition(0, 0);
		window.draw(ImageSprite);
		window.display();
	}
	//This allows everyone to read player data, modifying them should be done inside player class
	//Noncost usage requires explicit cast - consider effects
	static const Player* GetPlayer()
	{
		return player;
	}
	static void LoadNextLevel();
	static void RestartLevel();
	static void ResetPlayer(int X, int Y);
	static void SetImage(UI_Images image_)
	{
		image = image_;
		ShowImage();
	}

	//Game state processing

	static void MainLoop();
	static void Menu();
	static void Playing();
	static void BetweenLevels();
	static void Victory();
	static void Lost();
	static void CreditsRules();
private:
	//static tilePtr map[LevelData::sizeX][LevelData::sizeY]; //map = levelData->map; wont work
	static UI_Images image; //holds the enum value of image to be shown, used to process rules and credits in one function
	static Player* player;
	static sf::RenderWindow window;
};




class BaseSprite
{
protected:
	int freeze = 0;
	int X;
	int Y;
	Direction Direction_ = std::make_pair(0, 0);

public:
	BaseSprite(int x, int y) : X(x), Y(y) { }
	static std::string relativePath;
	int GetX() const { return X; }
	int GetY() const { return Y; }
	virtual void Freeze(int duration) { freeze += duration; }
	virtual void Render(sf::RenderWindow& window) const = 0; //each sprite has to define this alone as it can be composed of more things or depend on state..
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
	Player(int x, int y) : BaseSprite(x,y){	}
	static sf::Sprite PlayerSprite;
	static sf::Texture PlayerTexture;
	void Die() override //end the game
	{
		GameData::state = GameState::Lost;
		GameData::SetImage(UI_Images::Lost);
		GameData::ShowImage();
	}
	void Reset(int x, int y)
	{
		freeze = 0;
		X = x;
		Y = y;
	}
	void ProcessInput()
	{
		//TODO if walls are added ignore if player tries to run into them
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			SetDirection(Direction(0, -1)); //because of isometry origin is at the top of screen
			if (GameData::PlayerTurn && MakeMove()) { GameData::PlayerTurn = false; }
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			SetDirection(Direction(0, 1));
			if (GameData::PlayerTurn && MakeMove()) { GameData::PlayerTurn = false; }
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			SetDirection(Direction(-1, 0));
			if (GameData::PlayerTurn && MakeMove()) { GameData::PlayerTurn = false; }
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			SetDirection(Direction(1, 0));
			if (GameData::PlayerTurn && MakeMove()) { GameData::PlayerTurn = false; }
			//we use lazy eval if it is not players turn move wont be evaluated
		}
	}
	void Render(sf::RenderWindow& window) const override
	{
		PlayerSprite.setPosition(BaseTile::shiftX + (X - Y) * (BaseTile::sizeX / 2.0f), (BaseTile::sizeY / 2.0f) * (X + Y - 2) + BaseTile::shiftY);
		if(freeze > 0)
		{
			PlayerSprite.setColor(sf::Color(192, 192, 255, 255)); //make frozen character blueish
		}
		window.draw(PlayerSprite);
		PlayerSprite.setColor(sf::Color(255, 255, 255, 255));
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
//Chases player who loses on contact with this, Unlike player can move to ALL neighbouring tiles(sharing vertex is enough)
{
protected:
	virtual void TurnToPlayer(const BaseSprite* player);
	
public:
	Enemy(int x, int y) : BaseSprite(x,y){	}
	static sf::Sprite EnemySprite;
	static sf::Texture EnemyTexture;
	void Die() override //end the game
	{
		Alive = false;
	}
	bool Alive = true;
	void Render(sf::RenderWindow& window) const override
	{
		if (!Alive) { return; }
		EnemySprite.setPosition(BaseTile::shiftX + (X - Y) * (BaseTile::sizeX / 2.0f), (BaseTile::sizeY / 2.0f) * (X + Y - 2) + BaseTile::shiftY);
		if (freeze > 0)
		{
			EnemySprite.setColor(sf::Color(192, 192, 255, 255)); //make frozen character blueish
		}
		window.draw(EnemySprite);
		EnemySprite.setColor(sf::Color(255, 255, 255, 255));
	}
	bool MakeMove() override
	{
		if (!Alive) { return true; } //not active anymore
		TurnToPlayer(GameData::GetPlayer()); //we need to change orientation and then do the same thing as defined in BaseSprite so we call base method
		//no need to check for bounds we always go towards player which is inside the level
		bool retval = BaseSprite::MakeMove();

		//TODO check for collisions with other enemies
		if (X == GameData::GetPlayer()->GetX() && Y == GameData::GetPlayer()->GetY())
		{
			GameData::state = GameState::Lost;
			GameData::SetImage(UI_Images::Lost);
			GameData::ShowImage();
		}
		return retval;
	}
};

static inline void InitializePlayerData()
{
	auto path = absolute(std::filesystem::relative(BaseSprite::relativePath));
	if (!Player::PlayerTexture.loadFromFile(path.string() + "\\PlayerSprite.png")
		|| !Enemy::EnemyTexture.loadFromFile(path.string() + "\\EvilRobot.png"))
	{
		throw std::exception("Cant load file");
	}
	sf::Sprite playerSprite;
	sf::Sprite enemySprite;
	playerSprite.setTexture(Player::PlayerTexture);
	enemySprite.setTexture(Enemy::EnemyTexture);
	playerSprite.setScale(BaseTile::MultiplierX / 2.0f, BaseTile::MultiplierY / 2.0f); //sprites should be scaled the same as tiles
	enemySprite.setScale(BaseTile::MultiplierX / 2.0f, BaseTile::MultiplierY / 2.0f); //robot is already scaled
	enemySprite.setOrigin(-37, 55 - BaseTile::sizeY);
	playerSprite.setOrigin(-37, -18);
	Player::PlayerSprite = playerSprite;
	Enemy::EnemySprite = enemySprite;
}

class Wall : public BaseTile
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
	Wall(int x, int y) : BaseTile(x, y)
	{

	}
	void SetTexture(const sf::Texture& texture) override
	{
		Texture_ = texture;
	}
	void SpriteEntered(BaseSprite* sprite) override //game should not allow this
	{
		throw std::exception("Sprite got inside a wall");
	}
};

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
			if (GameData::currentLevel == GameData::LevelCount)
			{
				GameData::state = GameState::Victory;
				GameData::SetImage(UI_Images::Victory);
				GameData::ShowImage(); //we need to make sure rendering scene doesnt overwrite this
			}
			else
			{
				GameData::state = GameState::BetweenLevels;
				GameData::SetImage(UI_Images::BetweenLevels);
				GameData::ShowImage();
			}
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

	BaseTile::GroundSprite.setScale(BaseTile::MultiplierX, BaseTile::MultiplierY);
	BaseTile::GroundSprite = BaseTile::GroundSprite;
	/**/

	if (!FreezingTile::Texture_.loadFromFile(path.string() + "\\blocks_69.png"))
	{
		throw std::exception("Cant load file");
	}
	FreezingTile::Sprite_.setTexture(FreezingTile::Texture_);
	FreezingTile::Sprite_.setScale(BaseTile::MultiplierX, BaseTile::MultiplierY);

	if (!DestructibleTile::Texture_.loadFromFile(path.string() + "\\blocks_28.png"))
	{
		throw std::exception("Cant load file");
	}
	DestructibleTile::Sprite_.setTexture(DestructibleTile::Texture_);
	DestructibleTile::Sprite_.setScale(BaseTile::MultiplierX, BaseTile::MultiplierY);

	if (!Teleporter::Texture_.loadFromFile(path.string() + "\\blocks_19.png"))
	{
		throw std::exception("Cant load file");
	}

	Teleporter::Sprite_.setTexture(Teleporter::Texture_);
	Teleporter::Sprite_.setScale(BaseTile::MultiplierX, BaseTile::MultiplierY);

	if (!GoalTile::Texture_.loadFromFile(path.string() + "\\blocks_100.png"))
	{
		throw std::exception("Cant load file");
	}
	GoalTile::Sprite_.setTexture(GoalTile::Texture_);
	GoalTile::Sprite_.setScale(BaseTile::MultiplierX, BaseTile::MultiplierY);
}

#endif