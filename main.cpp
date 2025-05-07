#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "Fish.h"
#include "Hud.h"
#include "PauseMenu.h"
#include "GameOver.h"
#include "options.h"
using namespace std;
using namespace sf;

// Create a window
// VideoMode(1500, 1000)
// VideoMode(1920, 1080)
bool isFullscreen = 1;
RenderWindow window(VideoMode::getDesktopMode(), "Fish Frenzy!", Style::Fullscreen);

View view(Vector2f(0, 0), Vector2f(window.getSize().x, window.getSize().y));
View camera;

// Main Variables
int score = 0, lives = 1, multiple = 1;
float levelMax = 50000.0f, currentProgress = 0.f; // 25000.0f;
int fisheaten = 0;
bool gameStarted = true;
int level = 4, currentLevel = 4;
bool arrOFlevels[4] = { 0 };

enum class GameState
{
	Playing,
	Paused,
	Gameover,
	options,
	levels
};
int readCurrentLevel(const string& filename)
{
	ifstream file(filename);
	int level = 0;
	if (file.is_open())
	{
		file >> level;
		file.close();
	}
	else
	{
	}
	return level;
}
void saveNewLevel(const string& filename, int newLevel)
{
	int currentLevel = readCurrentLevel(filename);
	if (newLevel + 1 > currentLevel)
	{
		ofstream file(filename);
		if (file.is_open())
		{
			file << newLevel;
			file.close();
		}
	}
}
// mona
struct MainFish
{
	Sprite sprite;
	float speed = 600.0f, fishSize = 1.f;
	int fishLevel = 2;
	float scaleFactor = 1.f;
	Clock immunityClock;
	bool isImmune = false;

	Vector2f imageSize;
	// Movement & Direction
	bool facingLeft = true, dashing = false;
	// State
	bool settled = false, swimming = false, eating = false, idle = true, turning = false, pendingFlip = false, eated = false;
	// Timers
	float dashTimer = 0.f, dashDuration = 0.3f, turnElapsed = 0.f, turnSwitch = 0.08f, idleFrameTimer = 0.0f, swimFrameTimer = 0.0f,
		eatFrameTimer = 0.0f, idleSwitch = 0.2f, swimSwitch = 0.07f, eatSwitch = 0.1f;

	// Animation
	int frame = 0;
	// Max frames per animation
	int maxIdleFrames = 6, maxSwimFrames = 15, maxEatFrames = 6, maxframes = 5;

	// Dash recharge system
	float dashBar = 1.0f, dashRechargeRate = 0.7f;
	bool canDash = true;
};

MainFish fish;

SoundBuffer menuBuffer, gameOverBuffer, bite1Buffer, bite2Buffer, bite3Buffer, mermaidBuffer, playerDieBuffer, playerSpawnBuffer, stageIntroBuffer, gameplayBuffer, mineExplosionBuffer;
Sound menuSound, gameOverSound, bite1Sound, bite2Sound, bite3Sound, mermaidSound, playerDieSound, playerSpawnSound, stageIntroSound, gameplaySound, mineExplosionSound;
Sound CurrentSound;
// 🛠️ Initialization Function
void initializeSounds()
{
	if (!menuBuffer.loadFromFile("./assets/sounds/Jingles/menuMusic.wav"))
		cout << "Failed to load menuMusic\n";
	if (!gameOverBuffer.loadFromFile("./assets/sounds/Jingles/gameOver.wav"))
		cout << "Failed to load gameOver\n";
	if (!bite1Buffer.loadFromFile("./assets/sounds/Other/bite1.wav"))
		cout << "Failed to load bite1\n";
	if (!bite2Buffer.loadFromFile("./assets/sounds/Other/bite2.wav"))
		cout << "Failed to load bite2\n";
	if (!bite3Buffer.loadFromFile("./assets/sounds/Other/bite3.wav"))
		cout << "Failed to load bite3\n";
	if (!mermaidBuffer.loadFromFile("./assets/sounds/Jingles/mermaid.wav"))
		cout << "Failed to load mermaid\n";
	if (!playerDieBuffer.loadFromFile("./assets/sounds/Jingles/playerDie.wav"))
		cout << "Failed to load playerDie\n";
	if (!playerSpawnBuffer.loadFromFile("./assets/sounds/Other/playerSpawn.wav"))
		cout << "Failed to load playerSpawn\n";
	if (!stageIntroBuffer.loadFromFile("./assets/sounds/Other/stageIntro.wav"))
		cout << "Failed to load stageIntro\n";
	if (!gameplayBuffer.loadFromFile("./assets/sounds/Jingles/TRACK1.wav"))
		cout << "Failed to load gameplay\n";
	if (!mineExplosionBuffer.loadFromFile("./assets/sounds/Other/mineExplode.wav"))
		cout << "Failed to load mineExplode\n";

	menuSound.setBuffer(menuBuffer);
	CurrentSound = menuSound;
	gameOverSound.setBuffer(gameOverBuffer);
	bite1Sound.setBuffer(bite1Buffer);
	bite2Sound.setBuffer(bite2Buffer);
	bite3Sound.setBuffer(bite3Buffer);
	mermaidSound.setBuffer(mermaidBuffer);
	playerDieSound.setBuffer(playerDieBuffer);
	playerSpawnSound.setBuffer(playerSpawnBuffer);
	stageIntroSound.setBuffer(stageIntroBuffer);
	gameplaySound.setBuffer(gameplayBuffer);
	mineExplosionSound.setBuffer(mineExplosionBuffer);
}

Image icon;
Texture fishTexture;
void initializeMainFish(int currentLevel)
{

	if (currentLevel < 3)
		fishTexture.loadFromFile("./assets/fish_spritesheets/angel_fish.png");
	else
		fishTexture.loadFromFile("./assets/fish_spritesheets/Anglerfish.png");

	fish.imageSize = Vector2f(fishTexture.getSize().x / 15.f, fishTexture.getSize().y / 4.f);
	// Initialize fish
	fish.sprite.setTexture(fishTexture);
	fish.sprite.setOrigin(fish.imageSize.x / 2, fish.imageSize.y / 2);
	fish.sprite.setPosition(window.getSize().x / 2, -fish.imageSize.y); // start above screen
	fish.sprite.setTextureRect(IntRect(0 * fish.imageSize.x + 2, fish.imageSize.y + 1, fish.imageSize.x - 2, fish.imageSize.y - 1));
}
void moveFish(float deltaTime, bool bubblepressed[])
{
	Vector2i mousePos = Mouse::getPosition(window);
	Vector2f worldMouse = window.mapPixelToCoords(mousePos);
	Vector2f fishPos = fish.sprite.getPosition();
	Vector2f delta = worldMouse - fishPos;
	float distance = sqrt(delta.x * delta.x + delta.y * delta.y);
	// First drop animation
	if (!fish.settled)
	{
		if (fish.sprite.getPosition().y < window.getSize().y / 2.f)
		{
			fish.sprite.move(0.f, fish.speed * deltaTime);
		}
		else
		{
			/*Vector2i centerMousePos(window.getSize().x / 2, window.getSize().y / 2);
			Mouse::setPosition(centerMousePos, window);*/
			fish.settled = true;
		}
	}
	// Recharge dash bar over time
	if (fish.dashBar < 1.0f)
	{
		fish.dashBar += fish.dashRechargeRate * deltaTime;
		if (fish.dashBar >= 1.0f)
		{
			fish.dashBar = 1.0f;
			fish.canDash = true;
		}
	}
	// Dash logic
	if (Mouse::isButtonPressed(Mouse::Left) && fish.canDash && !fish.dashing && fish.settled && !fish.eated)
	{
		fish.dashing = true;
		fish.dashTimer = 0.f;
		fish.canDash = false;
		fish.dashBar = 0.f;
	}
	// Turning detection
	bool newFacingLeft = delta.x < 0;
	if (newFacingLeft != fish.facingLeft && fish.settled && !fish.eated)
	{
		fish.turning = true;
		fish.swimming = false;
		fish.eating = false;
		fish.idle = false;
		fish.frame = 0;
		fish.turnElapsed = 0.f;
		fish.facingLeft = newFacingLeft;
		fish.pendingFlip = true;
	}
	// Movement
	float factor = 1;
	for (int i = 4; i < 8; i++)
		if (bubblepressed[i])
			factor = i - 3;
	float moveFactor = fish.dashing ? 1.f : 0.5f; // min(0.2f + distance / 1000.f, 0.5f);
	if (distance > 10.f && fish.settled && !fish.eated)
	{
		Vector2f move = delta * factor * moveFactor * deltaTime;
		fish.sprite.move(move);
		fish.swimming = !fish.turning;
		fish.idle = false;
	}
	else
	{
		fish.swimming = false;
		fish.idle = true;
	}
	// Dash timer
	if (fish.dashing)
	{
		fish.dashTimer += deltaTime;
		if (fish.dashTimer >= fish.dashDuration)
		{
			fish.dashing = false;
		}
	}
}
void applyBoundaries()
{

	FloatRect bounds = fish.sprite.getGlobalBounds();
	FloatRect viewBounds(
		camera.getCenter().x - camera.getSize().x / 2.f,
		camera.getCenter().y - camera.getSize().y / 2.f,
		camera.getSize().x,
		camera.getSize().y);
	if (bounds.left < viewBounds.left)
		fish.sprite.setPosition(viewBounds.left + bounds.width / 2.f, fish.sprite.getPosition().y);
	if (bounds.left + bounds.width > viewBounds.left + viewBounds.width)
		fish.sprite.setPosition(viewBounds.left + viewBounds.width - bounds.width / 2.f, fish.sprite.getPosition().y);
	if ((bounds.top < viewBounds.top + (currentLevel < 3 ? 150.f : 100.f)) && fish.settled)
		fish.sprite.setPosition(fish.sprite.getPosition().x, viewBounds.top + (currentLevel < 3 ? 150.f : 100.f) + bounds.height / 2.f);
	if (bounds.top + bounds.height > viewBounds.top + viewBounds.height)
		fish.sprite.setPosition(fish.sprite.getPosition().x, viewBounds.top + viewBounds.height - bounds.height / 2.f);
}
void playEating(float deltaTime)
{
	fish.eatFrameTimer += deltaTime;
	if (fish.eatFrameTimer >= fish.eatSwitch)
	{
		fish.eatFrameTimer = 0.f;
		fish.frame = (fish.frame + 1) % fish.maxEatFrames;
		fish.sprite.setTextureRect(IntRect(fish.frame * fish.imageSize.x + 2, 0 * fish.imageSize.y + 1, fish.imageSize.x - 2, fish.imageSize.y - 1));
		if (fish.frame == 0)
		{
			fish.eating = false;
			fish.frame = 0;
			fish.idle = true;
			fish.swimming = false;
		}
	}
}
void updateAnimation(float deltaTime)
{
	// Animation logic
	if (fish.eated)
	{
		fish.eated = false;
		fish.sprite.setPosition(window.getSize().x / 2, -500);
		fish.settled = false;
	}
	if (fish.turning)
	{
		fish.turnElapsed += deltaTime;
		if ((fish.turnElapsed * 3) >= fish.turnSwitch)
		{
			fish.turnElapsed = 0.f;
			fish.sprite.setTextureRect(IntRect(
				fish.frame * fish.imageSize.x + 2,
				3 * fish.imageSize.y + 1,
				fish.imageSize.x - 2,
				fish.imageSize.y - 1));
			fish.frame++;
			if (fish.frame >= fish.maxframes)
			{
				fish.turning = false;
				fish.swimming = true;
				fish.frame = 0;
				fish.frame = 0;
				if (fish.pendingFlip)
				{
					fish.pendingFlip = false;
					fish.sprite.setScale(fish.fishSize * (fish.facingLeft ? 1.f : -1.f), fish.fishSize * 1);
				}
			}
		}
	}
	else if (fish.eating)
		playEating(deltaTime);
	else if (fish.swimming)
	{
		fish.swimFrameTimer += deltaTime;
		if (fish.swimFrameTimer >= fish.swimSwitch)
		{
			fish.swimFrameTimer = 0.f;
			fish.frame = (fish.frame + 1) % fish.maxSwimFrames;
			fish.sprite.setTextureRect(IntRect(fish.frame * fish.imageSize.x + 2, 2 * fish.imageSize.y + 1, fish.imageSize.x - 2, fish.imageSize.y - 1));
		}
	}
	else if (fish.idle)
	{
		fish.idleFrameTimer += deltaTime;
		if (fish.idleFrameTimer >= fish.idleSwitch)
		{
			fish.idleFrameTimer = 0.f;
			fish.frame = (fish.frame + 1) % fish.maxIdleFrames;
			fish.sprite.setTextureRect(IntRect(fish.frame * fish.imageSize.x + 2, 1 * fish.imageSize.y + 1, fish.imageSize.x - 2, fish.imageSize.y - 1));
		}
	}
}
void updateMainFishScale()
{
	fish.fishLevel = (int)(currentProgress / (levelMax / 3.0f)) + 2;
	int level = min(2, (int)(currentProgress / (levelMax / 3.0f)));
	fish.fishSize = 1 + (level * 0.2);
	if (!fish.pendingFlip && !fish.eated)
		fish.sprite.setScale(fish.fishSize * (fish.facingLeft ? 1.f : -1.f), fish.fishSize);
}
void updateMainFish(float deltaTime, bool bubblepressed[])
{
	if (fish.isImmune)
	{
		if (fish.immunityClock.getElapsedTime().asSeconds() >= 5.0f)
		{
			fish.isImmune = false;
		}
	}
	moveFish(deltaTime, bubblepressed);
	applyBoundaries();
	updateAnimation(deltaTime);
	updateMainFishScale();
}

Font font;
Texture pearltexture;
Sprite levelPearls[4];
Text levelTexts[4];
Vector2f levelsImageSize;

void initializeMapping()
{
	level = readCurrentLevel("./assets/level.txt");
	for (int i = 0; i < level + 1; i++)
		arrOFlevels[i] = 1;
	pearltexture.loadFromFile("./assets/pearl.png");
	levelsImageSize = Vector2f(pearltexture.getSize().x / 2.f, pearltexture.getSize().y / 2.f);
	if (!font.loadFromFile("./assets/fonts/BigSpace-rPKx.ttf"))
	{
		cout << "Failed to load font!\n";
	}
	for (int i = 0; i < 4; i++)
	{
		levelPearls[i].setTexture(pearltexture);
		levelPearls[i].setTextureRect(IntRect(0 * levelsImageSize.x, (arrOFlevels[i] ? 1 : 0) * levelsImageSize.y, levelsImageSize.x, levelsImageSize.y + 1));
		levelPearls[i].setPosition(300 * i + 300, 750);
		levelPearls[i].setScale(0.5, 0.5);

		levelTexts[i].setFont(font);
		levelTexts[i].setString(to_string(i + 1));
		levelTexts[i].setCharacterSize(30);
		levelTexts[i].setFillColor(Color::Black);
		Vector2f pearlPos = levelPearls[i].getPosition();
		Vector2f pearlSize(levelsImageSize.x * 0.5f, levelsImageSize.y * 0.5f);
		Vector2f centerPos(pearlPos.x + pearlSize.x / 2.7f, pearlPos.y + pearlSize.y / 3.f);
		levelTexts[i].setPosition(centerPos);
	}
}
Texture backgr;
void initializeBackground(Sprite& background, int currentLevel)
{
	if (currentLevel < 3)
		backgr.loadFromFile("./assets/ground1.png");
	else
		backgr.loadFromFile("./assets/lost_city.png");
	background.setTexture(backgr);
	background.setScale(3, 2);
	background.setPosition(0, 0);
	background.setOrigin(0, 0);
}

bool gameWin = 0, winIn = 1, alreadywon = 0, esacpedFish = 0, gameLose = 0;
// nada
void cameraUpdate(const FloatRect& backgroundBounds, const FloatRect& deadZone)
{
	Vector2f targetPosition = fish.sprite.getPosition();
	float targetX = camera.getCenter().x;
	float targetY = camera.getCenter().y;

	// Horizontal dead zone check
	if (targetPosition.x < camera.getCenter().x - deadZone.width / 2)
		targetX = targetPosition.x + deadZone.width / 2;
	else if (targetPosition.x > camera.getCenter().x + deadZone.width / 2)
		targetX = targetPosition.x - deadZone.width / 2;
	// Vertical dead zone check
	if (targetPosition.y < camera.getCenter().y - deadZone.height / 2)
		targetY = targetPosition.y + deadZone.height / 2;
	else if (targetPosition.y > camera.getCenter().y + deadZone.height / 2)
		targetY = targetPosition.y - deadZone.height / 2;

	// Clamp to background bounds
	targetX = max(targetX, backgroundBounds.left + camera.getSize().x / 2);
	targetX = min(targetX, backgroundBounds.left + backgroundBounds.width - camera.getSize().x / 2);
	targetY = max(targetY, backgroundBounds.top + camera.getSize().y / 2);
	targetY = min(targetY, backgroundBounds.top + backgroundBounds.height - camera.getSize().y / 2);
	camera.setCenter(targetX, targetY);
}
void startImmunity()
{
	fish.isImmune = true;
	fish.immunityClock.restart();
}
bool checkCollision(const Sprite& a, const Sprite& b)
{
	return a.getGlobalBounds().intersects(b.getGlobalBounds());
}

void checkAndEatFishCollision(Fish& fish1, Fish& fish2)
{
	if (checkCollision(fish1.sprite, fish2.sprite) && fish1.isinBackground() && fish2.isinBackground() && !gameWin)
	{
		if (int(fish1.current_type) > int(fish2.current_type) //&& fish2.Eated_frame == 0
			)
		{
			if (fish1.Eating_frame == 0)
				fish1.isEating();
			if (fish2.Eated_frame == 0)
				fish2.isEated();
		}
		else
		{
			if (fish2.Eating_frame == 0)
				fish2.isEating();
			if (fish1.Eated_frame == 0)
				fish1.isEated();
		}
	}
}

void checkAndEatPlayerFish(Fish& otherFish, Hud& hud, GameState& state, bool bubblepressed[])
{
	if (checkCollision(fish.sprite, otherFish.sprite) && otherFish.Eated_frame == 0 && !fish.eated && fish.settled && !gameWin)
	{
		int npcLevel = (int)(otherFish.current_type);

		float ratio = currentProgress / levelMax;
		int effectiveLevel = 2 + (int)(ratio * 3);
		if (effectiveLevel >= npcLevel)
		{
			if (currentProgress < levelMax)
			{
				currentProgress += 1200 * (1 - (currentLevel * 0.2)) * ((int)(otherFish.current_type) - 1);
				score += ((int)(otherFish.current_type) - 1) * 10 * multiple;
			}
			if (!fish.eating)
			{
				fish.eating = true;
				fish.frame = 0;
				fish.eatFrameTimer = 0.f;
			}
			otherFish.isEated();
			if (npcLevel == 2)
			{
				if (bubblepressed[0])
					bite1Sound.play();
			}
			else if (npcLevel == 3)
			{
				if (bubblepressed[0])
					bite2Sound.play();
			}
			else
			{
				if (bubblepressed[0])
					bite3Sound.play();
			}

			fisheaten++;
		}
		else
		{
			if (!fish.isImmune && !fish.eated)
			{
				if (lives > 0)
					lives--;
				if (lives <= 0)
				{
					state = GameState::Gameover;
					if (bubblepressed[0])
						gameOverSound.play();
				}
				else
				{
					if (bubblepressed[0])
						playerDieSound.play();
					// reset player fish
					camera.setCenter(camera.getCenter());
					fish.sprite.setPosition(window.getSize().x / 2, -fish.imageSize.y);
					fish.settled = false;
					fish.eated = true;
					float stageSize = levelMax / 3.f;
					multiple = 1;
					currentProgress = floor(currentProgress / stageSize) * stageSize + 1;
					if (otherFish.Eating_frame == 0)
						otherFish.isEating();
					startImmunity();
				}
			}
		}
	}
}

// alaa mahmoud
void update(Event event, Vector2f mousePos, GameState& state, PauseMenu& pauseMenu,
	GameOver& gameover, options& option, bool& backToMenuTimerStarted);

// alaa ashraf
enum BubbleType
{
	Star,
	Multiple,
	Life
};

struct StarBubble
{
	Sprite sprite;
	bool isActive = false;
	float speed = 150.f;
	BubbleType type;

	void init(BubbleType bubbleType)
	{
		type = bubbleType;
	}

	void spawn(const Texture& texture, const Vector2u& windowSize)
	{
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(0, 0, type == BubbleType::Multiple ? 36 : 40, type == BubbleType::Multiple ? 36 : 40));
		sprite.setScale(2.f, 2.f);
		sprite.setPosition(rand() % (windowSize.x - 120), windowSize.y);
		isActive = true;
	}

	void update(float deltaTime)
	{
		if (!isActive)
			return;

		sprite.move(0, -speed * deltaTime);

		if (sprite.getPosition().y < -100)
		{
			isActive = false;
		}
	}

	void draw(RenderWindow& window)
	{
		if (isActive)
			window.draw(sprite);
	}

	void checkCollision(MainFish& fish, int& score, int& multiple, int& lives, bool bubblepressed[])
	{
		if (!isActive)
			return;
		if (sprite.getGlobalBounds().intersects(fish.sprite.getGlobalBounds()) && !gameWin)
		{
			isActive = false;
			if (type == Star)
				score += 100;
			else if (type == Multiple)
				multiple += 1;
			else if (type == Life)
				lives += 1;
			if (bubblepressed[0])
				bite1Sound.play();
			if (!fish.eating)
			{
				fish.eating = true;
				fish.frame = 0;
				fish.eatFrameTimer = 0.f;
			}
		}
	}
};

struct Mermaid
{
	Sprite sprite;
	Texture texture;
	Clock animationClock;
	int currentFrame = 0;
	float speed = 500.f;
	bool isActive = true;

	static const int columns = 10;
	static const int rows = 2;
};

void initMermaid(Mermaid& mermaid, const RenderWindow& window)
{
	if (!mermaid.texture.loadFromFile("./assets/mermaid.png"))
	{
		throw runtime_error("Failed to load mermaid texture!");
	}

	mermaid.sprite.setTexture(mermaid.texture);

	int frameWidth = mermaid.texture.getSize().x / Mermaid::columns;
	int frameHeight = mermaid.texture.getSize().y / Mermaid::rows;

	mermaid.sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
	mermaid.sprite.setScale(2.7f, 2.7f);
	mermaid.sprite.setPosition(
		window.getSize().x,
		window.getSize().y / 2 - (frameHeight * 2.5f) / 2);
}

void updateMermaid(Mermaid& mermaid, float deltaTime, const RenderWindow& window)
{
	if (!mermaid.isActive)
		return;

	mermaid.sprite.move(-mermaid.speed * deltaTime, 0);

	if (mermaid.sprite.getPosition().x < -mermaid.sprite.getGlobalBounds().width)
	{
		mermaid.isActive = false;
	}

	if (mermaid.animationClock.getElapsedTime().asSeconds() > 0.2f && deltaTime > 0)
	{
		mermaid.currentFrame = (mermaid.currentFrame + 1) % (Mermaid::columns * Mermaid::rows);

		int frameWidth = mermaid.texture.getSize().x / Mermaid::columns;
		int frameHeight = mermaid.texture.getSize().y / Mermaid::rows;

		int row = mermaid.currentFrame / Mermaid::columns;
		int col = mermaid.currentFrame % Mermaid::columns;
		mermaid.sprite.setTextureRect(IntRect(col * frameWidth, row * frameHeight, frameWidth, frameHeight));
		mermaid.animationClock.restart();
	}
}

struct Oyster
{
	Sprite sprite;
	IntRect closedRect;
	IntRect openRect;
	bool isOpen;
	bool isClosing;
	float openTime;
	float closedTime;
	float timer;
	float closingDuration;
	float closingTimer;
	bool hasPowerUp;
	Sprite pearl;
	int blackOrWhite;
};
Texture oysterTexture;

void initOyster(Oyster& oyster, float x, float y)
{

	oyster.sprite.setTexture(oysterTexture);
	oyster.closedRect = IntRect(2, 2, 98.5, 98.5);
	oyster.openRect = IntRect(404 + 2, 2, 98.5, 98.5);

	oyster.sprite.setTextureRect(oyster.closedRect);
	oyster.sprite.setPosition(x, y);
	oyster.sprite.setScale(2.f, 2.f);

	oyster.isOpen = false;
	oyster.openTime = 2.0f + (rand() % 5);
	oyster.closedTime = 3.0f + (rand() % 3);
	oyster.timer = oyster.closedTime;

	oyster.hasPowerUp = (rand() % 2 == 0);
	oyster.blackOrWhite = rand() % 4;
	oyster.pearl.setTexture(oysterTexture);
	oyster.pearl.setTextureRect((oyster.blackOrWhite == 0) ? IntRect(2, 105, 45, 45) : IntRect(3, 155, 45, 45)); // white and black
	oyster.pearl.setOrigin(oyster.pearl.getLocalBounds().width / 2, oyster.pearl.getLocalBounds().height / 2 + 4);
	oyster.pearl.setPosition(
		x + oyster.sprite.getGlobalBounds().width / 2,
		y + oyster.sprite.getGlobalBounds().height * 0.7f);

	oyster.isClosing = false;
	oyster.closingDuration = 0.5f;
	oyster.closingTimer = 0.f;
}

bool checkOysterCollision(const Oyster& oyster, const Sprite& fishSprite)
{
	if (!oyster.isOpen && checkCollision(oyster.sprite, fishSprite))
	{
		return true;
	}
	return false;
}
void checkPearlCollision(Oyster& oyster, MainFish& fish, int& score, bool bubblepressed[])
{
	if (oyster.isOpen && oyster.hasPowerUp && checkCollision(oyster.pearl, fish.sprite))
	{
		score += 100;
		oyster.hasPowerUp = false;
		if (bubblepressed[0])
			bite1Sound.play();
		if (!fish.eating)
		{
			fish.eating = true;
			fish.frame = 0;
			fish.eatFrameTimer = 0.f;
		}
	}
}

const int oysterCount = 2;
Oyster oysters[oysterCount];

// Abdrahman Ezzat

struct Button
{
	Sprite sprite;
	Texture normalTexture;
	Texture hoverTexture;

	void updateTexture(const Vector2f& mousePos)
	{
		sprite.setTexture(sprite.getGlobalBounds().contains(mousePos) ? hoverTexture : normalTexture);
	}

	void setPosition(float x, float y)
	{
		sprite.setPosition(x, y);
	}

	void setScale(float scaleX, float scaleY)
	{
		sprite.setScale(scaleX, scaleY);
	}
};

Button newGameBtn, optionsBtn, highScoresBtn, exitBtn, doneButton;

// Textures
Texture mainMenuBGTexture, gameTitleTexture, optionsFrameTexture, creditsBarTexture;
// Sprites
Sprite mainMenuBGSprite, gameTitleSprite, optionsFrameSprite, creditsBarSprite;
Font gameFont;
long long highScores[11] = {};
int scoreCount = 0;
fstream highScoresFile;
string CreditsNames[7] = { "Abdrahman Ezzat", "Adham Elnomairy Ahmed", "Ahmed Abd El-Latif Mostafa", "Alaa Ashraf", "Alaa Mahmoud Ibrahem", "Mona Saber Abdullah", "Nada Mohamed Fekry" };
// Flags
bool gameStartClicked = false, gameOptionsClicked = false, highScoresClicked = false, mainMenuOpen = true, exitGame = false, donebuttonClicked = false, creditsClicked = false;
// Function Declarations
void loadMainMenuSprites();
void fitScale(Sprite&, const RenderWindow&);
void drawScores(RenderWindow&);
void saveScore(int);
void loadScores();
void drawDone(RenderWindow&);
void drawCreditsTitle(RenderWindow&);
void drawOurCredits(RenderWindow&);

Clock backToMenuClock;
bool backToMenuTimerStarted = true;

// Mona & Ahmed

struct Boom
{
	Texture boomTexture, boomAnimation;
	Sprite sprite;
	bool playing = false;
	bool falling = true;
	bool finished = false;
	bool visible = true;

	Clock clock;
	Clock hiddenClock;
	float frameDuration = 0.1f;
	int frame = 0;
	const int totalFrames = 5;

	Vector2f imageSize;
	float speed = 100.f;

	float respawnDelay = 0.f;
	float targetY;

	void initialize()
	{
		boomTexture.loadFromFile("./assets/Mine.png");
		boomAnimation.loadFromFile("./assets/Mine_expo.png");

		imageSize = Vector2f(boomTexture.getSize().x, boomTexture.getSize().y);
		sprite.setTexture(boomTexture);
		sprite.setTextureRect(IntRect(0, 0, imageSize.x, imageSize.y));
		sprite.setScale(1.5f, 1.5f);
		randomizePosition();
		falling = true;
		finished = false;
		playing = false;
	}

	void randomizePosition()
	{
		float randomX = (float)(rand() % 700 + 50);
		targetY = (float)(rand() % 200 + 300);
		sprite.setPosition(randomX, -imageSize.y);
	}

	void update(float deltaTime)
	{
		if (!visible)
		{
			if (hiddenClock.getElapsedTime().asSeconds() >= respawnDelay)
			{
				reset();
			}
			return;
		}

		if (falling)
		{
			sprite.move(0, speed * deltaTime);
			if (sprite.getPosition().y >= targetY)
			{
				falling = false;
			}
		}

		if (playing)
		{
			imageSize = Vector2f(boomAnimation.getSize().x / float(totalFrames), boomAnimation.getSize().y);
			sprite.setScale(1.f, 1.f);

			if (clock.getElapsedTime().asSeconds() >= frameDuration)
			{
				frame++;
				if (frame >= totalFrames)
				{
					playing = false;
					finished = true;
					visible = false;
					respawnDelay = (float)((rand() % 11) + 10);
					hiddenClock.restart();
					return;
				}
				sprite.setTextureRect(IntRect(frame * imageSize.x + 1, 0 * imageSize.y + 1, imageSize.x - 1, imageSize.y - 3));
				clock.restart();
			}
		}
	}

	void triggerExplosion(bool bubblepressed[])
	{
		if (bubblepressed[0])
			mineExplosionSound.play();
		playing = true;
		falling = false;
		finished = false;
		frame = 0;
		targetY = (float)(rand() % 200 + 300);

		clock.restart();
		sprite.setTexture(boomAnimation);
		sprite.setTextureRect(IntRect(1, 1, imageSize.x - 1, imageSize.y - 3));
	}

	bool checkCollision(const Sprite& target, GameState& state, bool bubblepressed[])
	{
		if (visible && !playing && !finished && sprite.getGlobalBounds().intersects(target.getGlobalBounds()) && !fish.isImmune && !gameWin)
		{
			triggerExplosion(bubblepressed);
			if (!fish.eated)
			{
				if (lives > 0)
					lives--;
				if (lives <= 0)
				{
					state = GameState::Gameover;
					// gameOverSound.play();
					CurrentSound = gameOverSound;
					if (bubblepressed[0])
						CurrentSound.play();
				}
				else
				{

					// playerDieSound.play();
					CurrentSound = playerDieSound;
					if (bubblepressed[0])
						CurrentSound.play(); // reset player fish
					camera.setCenter(camera.getCenter());
					fish.sprite.setPosition(window.getSize().x / 2, -fish.imageSize.y);
					fish.settled = false;
					fish.eated = true;
					float stageSize = levelMax / 3.f;
					multiple = 1;
					currentProgress = floor(currentProgress / stageSize) * stageSize + 1;
					startImmunity();
				}
			}
			return true;
		}
		return false;
	}
	bool checkfishes(Fish& target, GameState& state, bool bubblepressed[])
	{
		if (visible && !playing && !finished && sprite.getGlobalBounds().intersects(target.sprite.getGlobalBounds()) && target.Eated_frame == 0 && target.isinBackground())
		{
			triggerExplosion(bubblepressed);
			target.isEated();
			return true;
		}
		return false;
	}

	void draw(RenderWindow& window)
	{
		if (visible)
		{
			window.draw(sprite);
		}
	}

	void reset()
	{
		visible = true;
		playing = false;
		falling = true;
		finished = false;
		frame = 0;
		imageSize = Vector2f(boomTexture.getSize().x, boomTexture.getSize().y);
		sprite.setTexture(boomTexture);
		sprite.setScale(1.5f, 1.5f);
		sprite.setTextureRect(IntRect(0, 0, imageSize.x, imageSize.y));
		randomizePosition();
	}
};

bool soundStopped = 0;
int main()
{
	srand(time(0));
	window.setFramerateLimit(60);
	initializeMapping();
	// Create PauseMenu object
	PauseMenu pauseMenu(window);
	options option(window);
	GameOver gameover(window);
	initializeSounds();
	srand(time(0));
	if (option.bubblepressed[1])
		CurrentSound.play();
	// menuSound.play();
	// menuSound.setLoop(true);
	//  Abdrahman Ezzat
	loadMainMenuSprites();
	loadScores();

	// alaa ashraf
	Texture bubbleTextures[3];
	if (!bubbleTextures[0].loadFromFile("assets/star_bubble.png") ||
		!bubbleTextures[1].loadFromFile("assets/multiple_bubble.png") ||
		!bubbleTextures[2].loadFromFile("assets/fish_bubble.png"))
	{
		cerr << "Failed to load bubble textures.\n";
		return 1;
	}
	Boom mine;
	mine.initialize();
	// Bubbles + Clock
	StarBubble bubbles[3];
	Clock spawnClock;
	float bubbleSpawnDelay = 5.f;
	for (int i = 0; i < 3; ++i)
		bubbles[i].init((BubbleType)(i));
	Clock GameWin;
	float GameWinTime = 0.f;

	Mermaid mermaid;
	try
	{
		initMermaid(mermaid, window);
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		return -1;
	}
	if (!oysterTexture.loadFromFile("./assets/oyester_project.png"))
	{
		// return EXIT_FAILURE;
	}
	oysterTexture.setSmooth(true);
	float totalWidth = window.getSize().x;
	float spacing = totalWidth / 4.0f;
	// float oysterY = window.getSize().y - 300;
	float oysterY = window.getSize().y - 150;

	for (int i = 0; i < oysterCount; ++i)
	{
		float x = spacing * (i * 3 + 0.5) - (115 * 1.3f / 2);
		initOyster(oysters[i], x, oysterY);
	}
	GameState state = GameState::Playing;

	if (icon.loadFromFile("./assets/icon.png"))
		window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	// Initialize HUD
	Hud hud(score, lives, multiple, levelMax, currentProgress, fish.dashBar);
	// Load HUD
	hud.loadHUD(window);
	initializeMainFish(1);

	Sprite background;
	initializeBackground(background, 1);
	// Define the background bounds
	FloatRect backgroundBounds = background.getGlobalBounds();
	// Define the dead zone (inner rectangle)
	FloatRect deadZone(
		window.getSize().x / 4,
		window.getSize().y / 4,
		window.getSize().x / 2,
		window.getSize().y / 2);

	// Create the camera
	camera = window.getDefaultView();
	// camera.zoom(1.2f);

	const int FISH_COUNT1 = 30, FISH_COUNT2 = 5, FISH_COUNT3 = 3;
	Fish fishes1[FISH_COUNT1], fishes2[FISH_COUNT2], fishes3[FISH_COUNT3];
	// Initialize AI fish at random positions
	for (int i = 0; i < FISH_COUNT1; i++)
		fishes1[i].init(Fish::type::MINNOW, backgr);
	for (int i = 0; i < FISH_COUNT2; i++)
		fishes2[i].init(Fish::type::SURGEON_FISH, backgr);
	for (int i = 0; i < FISH_COUNT3; i++)
		fishes3[i].init(Fish::type::LION_FISH, backgr);

	// make player/objects move at a fixed pace on all pcs types
	Clock clock;

	while (window.isOpen())
	{
		// Calculate delta time
		float deltaTime = clock.restart().asSeconds();
		if (deltaTime > 0.6f)
			deltaTime = 0.6f;
		// handle events
		Event event;
		Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

		newGameBtn.updateTexture(mousePos);
		optionsBtn.updateTexture(mousePos);
		highScoresBtn.updateTexture(mousePos);
		exitBtn.updateTexture(mousePos);
		doneButton.updateTexture(mousePos);
		if (!option.bubblepressed[1]) {
			CurrentSound.stop();
			soundStopped = 1;
		}
		else {
			if (soundStopped) {
				CurrentSound.play();
				soundStopped = 0;
			}
		}

		if (option.bubblepressed[2] != isFullscreen) {
			isFullscreen = option.bubblepressed[2];
			window.close();
			window.create(
				isFullscreen ? VideoMode::getDesktopMode() : VideoMode(1920, 1080),
				"Fish Frenzy!",
				isFullscreen ? Style::Fullscreen : Style::Default
			);
			window.setFramerateLimit(60);
		}
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			// Click Space to toggle eating
			update(event, mousePos, state, pauseMenu, gameover, option, backToMenuTimerStarted);

			if (mainMenuOpen && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left && state == GameState::Playing)
			{
				if (newGameBtn.sprite.getGlobalBounds().contains(mousePos))
				{
					cout << "Start Game clicked\n";
					// gameStartClicked = true;
					// state = GameState::Playing;
					state = GameState::levels;
				}
				else if (optionsBtn.sprite.getGlobalBounds().contains(mousePos))
				{
					cout << "Game Options clicked\n";
					backToMenuTimerStarted = false;
					state = GameState::options;
				}
				else if (highScoresBtn.sprite.getGlobalBounds().contains(mousePos))
				{
					backToMenuTimerStarted = false;
					cout << "High Scores clicked\n";
					highScoresClicked = true;
				}
				else if (creditsBarSprite.getGlobalBounds().contains(mousePos))
				{
					backToMenuTimerStarted = false;
					cout << "Credits clicked\n";
					creditsClicked = true;
				}
				else if (exitBtn.sprite.getGlobalBounds().contains(mousePos) && mainMenuOpen && state == GameState::Playing && backToMenuClock.getElapsedTime().asSeconds() >= 1.0f && backToMenuTimerStarted)
				{
					cout << "Game Closed, Bye Bye\n";
					window.close();
				}
				mainMenuOpen = !(gameStartClicked || gameOptionsClicked || highScoresClicked || creditsClicked || state == GameState::levels);
			}
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left && state == GameState::levels)
			{
				for (int i = 0; i < 4; i++)
				{
					if (levelPearls[i].getGlobalBounds().contains(mousePos) && arrOFlevels[i] && backToMenuClock.getElapsedTime().asSeconds() >= 1.0f && backToMenuTimerStarted)
					{
						CurrentSound = stageIntroSound;
						if (option.bubblepressed[0])
							CurrentSound.play();
						// stageIntroSound.play();
						currentLevel = i + 1;
						state = GameState::Playing;
						gameStartClicked = true;
						winIn = true;
						fish.sprite.setPosition(window.getSize().x / 2, -500);
						startImmunity();
						// camera.setCenter(0, 0);
						for (int i = 0; i < FISH_COUNT1; i++)
							fishes1[i].init(Fish::type::MINNOW, backgr);
						for (int i = 0; i < FISH_COUNT2; i++)
							fishes2[i].init(Fish::type::SURGEON_FISH, backgr);
						for (int i = 0; i < FISH_COUNT3; i++)
							fishes3[i].init(Fish::type::LION_FISH, backgr);
						fish.settled = false;
						fish.fishLevel = 2.f;
						gameWin = 0;
						fish.fishSize = 1.f;
						currentProgress = 25000.0f;
						lives = 3;
						score = 0;
						multiple = 1; // const in score .
						alreadywon = 0;
						esacpedFish = 0;
						mermaid.isActive = true;
						initializeMainFish(currentLevel);
						initializeBackground(background, currentLevel);
						if (option.bubblepressed[0])
							playerSpawnSound.play();
						CurrentSound = gameplaySound;
						if (option.bubblepressed[1])
						{
							CurrentSound.play();
							CurrentSound.setLoop(true);
						}
					}
				}
			}
			if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape && state == GameState::levels)
			{
				state = GameState::Playing;
				mainMenuOpen = 1;
				// menuSound.play();
				// menuSound.setLoop(true);
			}
			for (int i = 0; i < 4; i++)
			{
				if (levelPearls[i].getGlobalBounds().contains(mousePos))
					levelPearls[i].setTextureRect(IntRect(1 * levelsImageSize.x, (arrOFlevels[i] ? 1 : 0) * levelsImageSize.y, levelsImageSize.x, levelsImageSize.y + 1));

				else
					levelPearls[i].setTextureRect(IntRect(0 * levelsImageSize.x, (arrOFlevels[i] ? 1 : 0) * levelsImageSize.y, levelsImageSize.x, levelsImageSize.y + 1));
			}
		}
		if (currentProgress >= levelMax && !gameWin && gameStartClicked && winIn)
		{
			// state = GameState::Wining;
			gameWin = 1;
			GameWinTime += deltaTime;
			GameWin.restart();
			winIn = false;
			alreadywon = 1;
			for (int i = 0; i < FISH_COUNT1; i++)
				if (fishes1[i].Eated_frame == 0)
					fishes1[i].isEscaping();
			for (int i = 0; i < FISH_COUNT2; i++)
				if (fishes2[i].Eated_frame == 0)
					fishes2[i].isEscaping();
			for (int i = 0; i < FISH_COUNT3; i++)
				if (fishes3[i].Eated_frame == 0)
					fishes3[i].isEscaping();
			// mermaidSound.play();
			CurrentSound = mermaidSound;
			if (option.bubblepressed[0])
				CurrentSound.play();
		}
		if (GameWin.getElapsedTime().asSeconds() >= 1.5f && gameWin && gameStartClicked && !winIn)
			esacpedFish = 1;
		if (GameWin.getElapsedTime().asSeconds() >= 5.f && gameWin && gameStartClicked && !winIn)
		{
			saveScore(score);
			saveNewLevel("./assets/level.txt", currentLevel);
			gameWin = 0;

			state = GameState::Paused;
		}

		window.clear();
		if (mainMenuOpen)
		{
			window.setView(window.getDefaultView());
			window.draw(mainMenuBGSprite);
			if (state == GameState::Playing)
			{
				window.draw(gameTitleSprite);
				window.draw(newGameBtn.sprite);
				window.draw(highScoresBtn.sprite);
				window.draw(optionsBtn.sprite);
				window.draw(exitBtn.sprite);
				window.draw(creditsBarSprite);
				drawCreditsTitle(window);
			}
		}
		if (gameStartClicked && state == GameState::Playing)
		{
			window.draw(background);
			if (state != GameState::Playing)
				deltaTime = 0;
			updateMermaid(mermaid, deltaTime, window);
			if (spawnClock.getElapsedTime().asSeconds() >= bubbleSpawnDelay)
			{
				int index = rand() % 3;
				if (!bubbles[index].isActive)
				{
					bubbles[index].spawn(bubbleTextures[index], window.getSize());
					spawnClock.restart();
				}
			}
			if (currentLevel >= 2)
			{

				if (!alreadywon)
				{
					for (auto& oyster : oysters)
					{
						oyster.timer -= deltaTime;
						if (oyster.timer <= 0)
						{
							if (oyster.isOpen)
							{
								oyster.isClosing = true;
								oyster.closingTimer = oyster.closingDuration;
							}
							else
							{
								oyster.isOpen = true;
								oyster.sprite.setTextureRect(oyster.openRect);
							}
							oyster.hasPowerUp = (rand() % 2 == 0);
							oyster.blackOrWhite = rand() % 4;
							oyster.pearl.setTextureRect((oyster.blackOrWhite == 0) ? IntRect(2, 105, 45, 45) : IntRect(3, 155, 45, 45)); // white and black
							oyster.timer = oyster.isOpen ? oyster.openTime : oyster.closedTime;
						}
						if (oyster.isClosing)
						{
							oyster.closingTimer -= deltaTime;
							if (oyster.closingTimer <= 0)
							{
								oyster.isOpen = false;
								oyster.isClosing = false;
								oyster.sprite.setTextureRect(oyster.closedRect);
								if (checkOysterCollision(oyster, fish.sprite))
								{
									if (lives > 0)
										lives--;
									if (lives <= 0)
									{
										if (option.bubblepressed[0])
											gameOverSound.play();
										state = GameState::Gameover;
									}
									else
									{
										if (option.bubblepressed[0])
											playerDieSound.play();
										camera.setCenter(camera.getCenter());
										fish.sprite.setPosition(window.getSize().x / 2, -fish.imageSize.y);
										fish.settled = false;
										fish.eated = true;
										float stageSize = levelMax / 3.f;
										multiple = 1;
										currentProgress = floor(currentProgress / stageSize) * stageSize + 1;
										startImmunity();
									}
								}
							}
						}
						checkPearlCollision(oyster, fish, score, option.bubblepressed);
					}
				}
				for (auto& oyster : oysters)
				{
					window.draw(oyster.sprite);
					if (oyster.isOpen && oyster.hasPowerUp)
					{
						window.draw(oyster.pearl);
					}
				}
			}

			if (!alreadywon)
			{
				for (int i = 0; i < 3; ++i)
				{
					if (option.bubblepressed[3] && bubbles[i].isActive)
					{
						bubbles[i].update(deltaTime);
						bubbles[i].draw(window);
						bubbles[i].checkCollision(fish, score, multiple, lives, option.bubblepressed);
					}
				}
			}
			if (!esacpedFish)
			{
				for (int i = 0; i < FISH_COUNT1; i++)
				{
					fishes1[i].update(deltaTime);
					window.draw(fishes1[i].sprite);
					checkAndEatPlayerFish(fishes1[i], hud, state, option.bubblepressed);
					if (currentLevel >= 4)
						mine.checkfishes(fishes1[i], state, option.bubblepressed);
				}
				for (int i = 0; i < FISH_COUNT2; i++)
				{
					fishes2[i].update(deltaTime);
					window.draw(fishes2[i].sprite);
					for (int j = 0; j < FISH_COUNT1; j++)
						checkAndEatFishCollision(fishes2[i], fishes1[j]);
					checkAndEatPlayerFish(fishes2[i], hud, state, option.bubblepressed);
					if (currentLevel >= 4)
						mine.checkfishes(fishes2[i], state, option.bubblepressed);
				}
				for (int i = 0; i < FISH_COUNT3; i++)
				{
					fishes3[i].update(deltaTime);
					window.draw(fishes3[i].sprite);
					for (int j = 0; j < FISH_COUNT1; j++)
						checkAndEatFishCollision(fishes3[i], fishes1[j]);
					for (int j = 0; j < FISH_COUNT2; j++)
						checkAndEatFishCollision(fishes3[i], fishes2[j]);
					checkAndEatPlayerFish(fishes3[i], hud, state, option.bubblepressed);
					if (currentLevel >= 4)
						mine.checkfishes(fishes3[i], state, option.bubblepressed);
				}
			}
			if (currentLevel >= 4)
			{
				mine.update(deltaTime);
				mine.checkCollision(fish.sprite, state, option.bubblepressed);
				mine.draw(window);
			}

			updateMainFish(deltaTime, option.bubblepressed);
			view.setCenter(fish.sprite.getPosition());
			cameraUpdate(backgroundBounds, deadZone);
			hud.updateHUD(deltaTime);
			// Update HUD data
			window.setView(window.getDefaultView());
			// header
			hud.draw(window);
			window.setView(camera); // camera's view
			window.draw(fish.sprite);
			if (mermaid.isActive && gameWin)
			{
				window.draw(mermaid.sprite);
			}
			else
			{
				// mermaid.isActive = true;
				initMermaid(mermaid, window);
			}
			window.setView(window.getDefaultView());
			hud.draw(window);
			window.setView(camera); // camera's view
		}
		else if (highScoresClicked)
		{
			gameStartClicked = false;
			gameOptionsClicked = false;
			creditsClicked = false;

			window.draw(mainMenuBGSprite);
			window.draw(optionsFrameSprite);
			drawScores(window);

			// Done button
			window.draw(doneButton.sprite);
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
			{
				if (doneButton.sprite.getGlobalBounds().contains(mousePos))
				{
					mainMenuOpen = true;
					highScoresClicked = false;
					cout << "Done Button Clicked ---> Exitting High Sccores\n";
					backToMenuClock.restart();
					backToMenuTimerStarted = true;
				}
			}
			drawDone(window);
		}
		else if (creditsClicked)
		{
			highScoresClicked = false;
			gameOptionsClicked = false;
			gameStartClicked = false;

			window.draw(mainMenuBGSprite);
			window.draw(optionsFrameSprite);
			drawOurCredits(window);

			// Done button
			window.draw(doneButton.sprite);
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
			{
				if (doneButton.sprite.getGlobalBounds().contains(mousePos))
				{
					mainMenuOpen = true;
					creditsClicked = false;
					cout << "Done Button Clicked ---> Exitting Credits\n";
					backToMenuClock.restart();
					backToMenuTimerStarted = true;
				}
			}
			drawDone(window);
		}

		mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
		if (state == GameState::levels)
		{
			window.setView(window.getDefaultView()); // Reset to screen view
			window.draw(mainMenuBGSprite);
			for (int i = 0; i < 4; i++)
			{

				window.draw(levelPearls[i]);
				window.draw(levelTexts[i]);
			}
		}
		if (state == GameState::Paused)
		{
			window.setView(window.getDefaultView()); // Reset to screen view
			window.draw(mainMenuBGSprite);
			pauseMenu.render(window, score, highScores[0], fisheaten); // Draw pause menu
			pauseMenu.update(mousePos, window);
		}
		if (state == GameState::Gameover)
		{
			window.setView(window.getDefaultView()); // Reset to screen view
			gameover.render(window, score);					 // Draw pause menu
			gameover.update(mousePos, window);
		}
		if (state == GameState::options)
		{
			// gameStartClicked = false;
			creditsClicked = false;
			highScoresClicked = false;
			window.draw(mainMenuBGSprite);
			window.setView(window.getDefaultView()); // Reset to screen view
			option.render(window);
			option.update(mousePos, window, event);
		}
		window.display();
	}
	return 0;
}

void update(Event event, Vector2f mousePos, GameState& state, PauseMenu& pauseMenu,
	GameOver& gameover, options& option, bool& backToMenuTimerStarted)
{
	// Toggle pause menu with Escape key
	if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape && !alreadywon && gameStartClicked)
	{
		if (state == GameState::Playing)
		{
			camera.setCenter(0.f + camera.getSize().x / 2.f, 0.f + camera.getSize().y / 2.f);

			state = GameState::Paused;
		}
		else
			state = GameState::Playing;
	}

	// Handle mouse clicks when paused
	if (state == GameState::Paused && event.type == Event::MouseButtonPressed)
	{

		if (pauseMenu.option(mousePos))
			state = GameState::options;

		if (pauseMenu.isResumeClicked(mousePos))
		{
			state = GameState::Playing;

			if (currentProgress >= levelMax)
			{
				mainMenuOpen = 0;
				gameStartClicked = 0;
				state = GameState::levels;
				CurrentSound = menuSound;
				if (option.bubblepressed[1])
				{
					CurrentSound.play();
					CurrentSound.setLoop(true);
				}
				initializeMapping();
				backToMenuClock.restart();
				backToMenuTimerStarted = true;
				gameWin = 0;
			}
		}
		if (pauseMenu.isQuitClicked(mousePos))
		{
			window.close();
		}
		mainMenuOpen = 0;
	}
	if (state == GameState::options && event.type == Event::MouseButtonPressed)
	{
		if (option.doneClicked(mousePos))
		{
			if (!mainMenuOpen)
				state = GameState::Paused;
			else
			{
				backToMenuClock.restart();
				backToMenuTimerStarted = true;
				state = GameState::Playing;
			}
		}

		option.update(mousePos, window, event);
	}

	if (event.type == Event::KeyPressed && event.key.code == Keyboard::P)
	{
		if (state == GameState::Playing)
			state = GameState::Gameover;
		else
			state = GameState::Playing;
	}

	// Handle mouse clicks when gameover
	if (state == GameState::Gameover && event.type == Event::MouseButtonPressed)
	{

		if (gameover.continueClicked(mousePos))
		{
			backToMenuClock.restart();
			backToMenuTimerStarted = true;
			mainMenuOpen = 0;
			gameStartClicked = 0;
			if (option.bubblepressed[1])
			{
				CurrentSound = menuSound;
				CurrentSound.play();
			}
			// menuSound.play();
			CurrentSound.setLoop(true);
			gameWin = 0;
			state = GameState::levels;
		}
		if (gameover.QuitClicked(mousePos))
			window.close();
	}
}
// Abdrahman Ezzat
void fitScale(Sprite& sprite, const RenderWindow& window)
{
	const Vector2u textureSize = sprite.getTexture()->getSize();
	const Vector2u windowSize = window.getSize();

	float scaleX = float(windowSize.x) / textureSize.x;
	float scaleY = float(windowSize.y) / textureSize.y;

	sprite.setScale(scaleX, scaleY);
}

void loadMainMenuSprites()
{
	if (!gameFont.loadFromFile("./assets/fonts/BigSpace-rPKx.ttf"))
		cerr << "Error loading main menu font!\n"
		<< endl;

	if (!mainMenuBGTexture.loadFromFile("./assets/mainMenuBG.png"))
		cerr << "Error loading main menu background!\n"
		<< endl;

	if (!optionsFrameTexture.loadFromFile("./assets/optionsFrame.png"))
		cerr << "Error loading options frame!\n"
		<< endl;

	if (!gameTitleTexture.loadFromFile("./assets/fish_ferenzy.png"))
		cerr << "Error loading game title!\n"
		<< endl;

	if (!creditsBarTexture.loadFromFile("./assets/cersor_3.png"))
		cerr << "Error loading credits bar!\n"
		<< endl;

	if (!newGameBtn.normalTexture.loadFromFile("./assets/newGame_normal.png") ||
		!newGameBtn.hoverTexture.loadFromFile("./assets/newGame_hover.png"))
		cerr << "Error loading new game button textures!\n"
		<< endl;

	if (!optionsBtn.normalTexture.loadFromFile("./assets/options_normal.png") ||
		!optionsBtn.hoverTexture.loadFromFile("./assets/options_hover.png"))
		cerr << "Error loading options button textures!\n"
		<< endl;

	if (!highScoresBtn.normalTexture.loadFromFile("./assets/highScores_normal.png") ||
		!highScoresBtn.hoverTexture.loadFromFile("./assets/highScores_hover.png"))
		cerr << "Error loading high scores button textures!\n"
		<< endl;

	if (!exitBtn.normalTexture.loadFromFile("./assets/exit_normal.png") ||
		!exitBtn.hoverTexture.loadFromFile("./assets/exit_hover.png"))
		cerr << "Error loading exit button textures!\n"
		<< endl;

	if (!doneButton.normalTexture.loadFromFile("./assets/cersor_1.png") ||
		!doneButton.hoverTexture.loadFromFile("./assets/cersor_2.png"))
		cerr << "Error loading done button textures!\n"
		<< endl;

	// Background
	mainMenuBGSprite.setTexture(mainMenuBGTexture);
	fitScale(mainMenuBGSprite, window);

	// Title
	gameTitleSprite.setTexture(gameTitleTexture);
	gameTitleSprite.setScale(2.f, 2.f);
	gameTitleSprite.setPosition(750.f, 50.f);

	// Options Frame
	optionsFrameSprite.setTexture(optionsFrameTexture);
	optionsFrameSprite.setScale(1.85f, 1.85f);
	optionsFrameSprite.setPosition(450.f, 100.f);

	// Buttons
	newGameBtn.sprite.setTexture(newGameBtn.normalTexture);
	newGameBtn.setPosition(625.f, 225.f);
	newGameBtn.setScale(2.25f, 2.25f);

	optionsBtn.sprite.setTexture(optionsBtn.normalTexture);
	optionsBtn.setPosition(1050.f, 400.f);
	optionsBtn.setScale(2.25f, 2.25f);

	highScoresBtn.sprite.setTexture(highScoresBtn.normalTexture);
	highScoresBtn.setPosition(650.f, 575.f);
	highScoresBtn.setScale(2.25f, 2.25f);

	exitBtn.sprite.setTexture(exitBtn.normalTexture);
	exitBtn.setPosition(1050.f, 750.f);
	exitBtn.setScale(2.25f, 2.25f);

	doneButton.sprite.setTexture(doneButton.normalTexture);
	doneButton.setPosition(800.f, 800.f);
	doneButton.setScale(2.f, 2.f);

	creditsBarSprite.setTexture(creditsBarTexture);
	creditsBarSprite.setScale(2.f, 2.f);
	creditsBarSprite.setPosition(1675.f, 935.f);
}

void loadScores()
{
	scoreCount = 0;
	highScoresFile.open("./assets/HighestScoresFile.txt", ios::in);

	if (highScoresFile.is_open())
	{
		vector<long long> tempScores;
		long long temp;
		while (highScoresFile >> temp)
		{
			if (temp > 0)
				tempScores.push_back(temp);
		}
		highScoresFile.close();

		// Sort descending
		sort(tempScores.begin(), tempScores.end(), greater<long long>());

		// Copy top 10
		scoreCount = min(10, int(tempScores.size()));
		for (int i = 0; i < scoreCount; ++i)
		{
			highScores[i] = tempScores[i];
		}
	}
}
void saveScore(int newScore)
{
	loadScores();

	highScores[scoreCount] = newScore;
	scoreCount++;

	sort(highScores, highScores + scoreCount, greater<long long>());
	scoreCount = min(scoreCount, 10);

	highScoresFile.open("./assets/HighestScoresFile.txt", ios::out | ios::trunc);
	if (highScoresFile.is_open())
	{
		for (int i = 0; i < scoreCount; ++i)
		{
			highScoresFile << highScores[i] << '\n';
		}
		highScoresFile.close();
	}
	else
		cerr << "Error opening the Highest Scores File!!" << endl;
}

void drawScores(RenderWindow& window)
{
	Text HighScoresTitle("[ Highest Scores ]", gameFont, 90);
	HighScoresTitle.setFillColor(Color::White);
	HighScoresTitle.setPosition(620.f, 225.f);
	window.draw(HighScoresTitle);

	int displayCount = min(scoreCount, 10);
	for (int i = 0; i < displayCount; ++i)
	{
		Text scoreText("Frenzy.\t\t" + to_string(highScores[i]), gameFont, 45);
		scoreText.setFillColor(Color::White);
		scoreText.setPosition(790.f, (float)i * 40.f + 350.f);

		window.draw(scoreText);
	}
}

void drawDone(RenderWindow& window)
{
	Text DoneTitle("Done", gameFont, 60);
	DoneTitle.setFillColor(Color::Black);
	DoneTitle.setPosition(880.f, 815.f);
	window.draw(DoneTitle);
}

void drawCreditsTitle(RenderWindow& window)
{
	Text CreditsTitle("Credits", gameFont, 40);
	CreditsTitle.setFillColor(Color::White);
	CreditsTitle.setPosition(1718.f, 950.f);
	window.draw(CreditsTitle);
}

void drawOurCredits(RenderWindow& window)
{
	Text OurCredits("[ Credits ]", gameFont, 90);
	OurCredits.setFillColor(Color::White);
	OurCredits.setPosition(785.f, 225.f);
	window.draw(OurCredits);

	for (int i = 0; i < 7; ++i)
	{
		Text OurNames(CreditsNames[i], gameFont, 45);
		OurNames.setFillColor(Color::White);
		OurNames.setPosition(790.f, (float)i * 50.f + 380.f);

		window.draw(OurNames);
	}
}