#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <algorithm>
#include <string>
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
float levelMax = 50000.0f, currentProgress = 0.f;
int fisheaten = 0;
int level = 4, currentLevel = 4;
bool arrOFlevels[4] = { 0 };
bool gameWin = 0, winIn = 1, alreadywon = 0, esacpedFish = 0;
Image icon;

string playerName = "";
float cursorBlinkTime = 0.f;
bool cursorVisible = true;


enum class GameState
{
	Playing,
	NameEntry,
	Paused,
	Gameover,
	options,
	levels,
	win
};
GameState state = GameState::NameEntry;

// mona
struct MainFish
{
	Texture fishTexture;
	Sprite sprite;
	float speed = 600.0f, fishSize = 1.f;
	int fishLevel = 2;
	float scaleFactor = 1.f;
	Clock immunityClock;
	bool isImmune = false;
	RectangleShape mouth;
	Vector2f imageSize;
	// Movement & Direction
	bool facingLeft = true, dashing = false;
	// State
	bool isGreen = false;
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

	FloatRect getGlobalBounds() {
		FloatRect globalBounds = sprite.getGlobalBounds();
		globalBounds.left += globalBounds.left / 12;
		globalBounds.top += globalBounds.top / 12;
		globalBounds.height -= globalBounds.height / 3;
		globalBounds.width -= globalBounds.width / 2;
		return globalBounds;
	}
};

MainFish fish;

void initializeMainFish(int currentLevel);
void moveFish(float deltaTimebool);
void applyBoundaries();
void playEating(float deltaTime);
void updateAnimation(float deltaTime);
void updateMainFishScale();
void updateMainFish(float deltaTime);
Font font;
Texture pearltexture;
Sprite levelPearls[4];
Text levelTexts[4];
Vector2f levelsImageSize;

// nada
void cameraUpdate(const FloatRect& backgroundBounds, const FloatRect& deadZone);
void startImmunity();
bool checkCollision(const Sprite& a, const Sprite& b);
void checkAndEatFishCollision(Fish& fish1, Fish& fish2);
void checkAndEatPlayerFish(Fish& otherFish);

// alaa mahmoud
void update(Event event, Vector2f mousePos, bool& backToMenuTimerStarted);
void win(Vector2f mousePos, Event event, int& scorenum, int& highscorenum, int& fisheat);
void winUpdate(Vector2f mousePos, Event event);
// Create PauseMenu object
PauseMenu pauseMenu(window);
PauseMenu wining(window);
options option(window);
GameOver gameover(window);
Texture normalTextureWin, hoverTextureWin;
string optionsSource = "main";

struct FishCounter
{
	Sprite sprite;
	Texture texture;
	Text counterText;
	int finalCount = 0;
	int currentCount = 0;
	Clock counterclock;
	float delay = 0.01f;

	void update()
	{
		if (finalCount == 0) counterText.setString("0");
		if (currentCount < finalCount && counterclock.getElapsedTime().asSeconds() >= delay)
		{
			currentCount++;
			counterText.setString(currentCount ? to_string(currentCount) : "0");
			counterclock.restart();
		}
		if (currentCount == finalCount)
		{
			counterText.setCharacterSize(50);
		}
		counterclock.restart();
	}
	void draw()
	{
		window.draw(sprite);
		window.draw(counterText);
	}
} fishscoresh[4];

// alaa ashraf
SoundBuffer menuBuffer, gameOverBuffer, bite1Buffer, bite2Buffer, bite3Buffer, mermaidBuffer, playerDieBuffer, playerSpawnBuffer, stageIntroBuffer, gameplayBuffer, mineExplosionBuffer;
Sound menuSound, gameOverSound, bite1Sound, bite2Sound, bite3Sound, mermaidSound, playerDieSound, playerSpawnSound, stageIntroSound, gameplaySound, mineExplosionSound;
Sound CurrentSound;
bool soundStopped = 0;
void initializeSounds();
enum BubbleType {
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
	bool fallDown = false;
	Vector2f velocity;
	float gravity = 500.f;

	void init(BubbleType bubbleType)
	{
		type = bubbleType;
	}

	void spawn(const Texture& texture, const Vector2u& windowSize, bool dropDown = false, Vector2f startPos = Vector2f(0, 0))
	{
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(0, 0, type == BubbleType::Multiple ? 36 : 40, type == BubbleType::Multiple ? 36 : 40));
		sprite.setScale(2.f, 2.f);
		sprite.setPosition(startPos);
		isActive = true;
		fallDown = dropDown;

		if (fallDown)
		{
			velocity.x = static_cast<float>((rand() % 60) - 30); 
			velocity.y = -300.f; 
		}
		else
		{
			velocity.x = 0.f;
			velocity.y = -speed;
		}
	}

	void update(float deltaTime)
	{
		if (!isActive) return;

		if (fallDown)
		{
			velocity.y += gravity * deltaTime;
			sprite.move(velocity * deltaTime);

			if (sprite.getPosition().y > window.getSize().y / 1.5f)
			{
				isActive = false;
				velocity = { 0.f, 0.f };
			}
		}
		else
		{
			sprite.move(velocity * deltaTime);
			if (sprite.getPosition().y < -100)
				isActive = false;
		}
	}

	void draw()
	{
		if (isActive)
			window.draw(sprite);
	}

	void checkCollision(MainFish& fish, int& score, int& multiple, int& lives)
	{
		if (!isActive) return;
		if (sprite.getGlobalBounds().intersects(fish.sprite.getGlobalBounds()) && (!gameWin || fallDown))
		{
			isActive = false;
			if (type == Star)
			{
				score += 100;
				fishscoresh[3].finalCount++;
			}
			else if (type == Multiple)
				multiple += 1;
			else if (type == Life)
				lives += 1;

			if (option.bubblepressed[0])
				bite1Sound.play();

			if (!fish.eating)
			{
				fish.eating = true;
				fish.frame = 0;
				fish.eatFrameTimer = 0.f;
			}
		}
	}
}bubbles[3], mermaidBubbles[10];
Texture bubbleTextures[3];

struct Mermaid
{
	Sprite sprite;
	Texture texture;
	Clock animationClock;
	Clock bubbleClock;
	int spawnedBubbleCount = 0;
	int currentFrame = 0;
	float speed = 500.f;
	bool isActive = true;

	static const int columns = 10;
	static const int rows = 2;
};
void initMermaid(Mermaid& mermaid);
void updateMermaid(Mermaid& mermaid, float deltaTime);
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
	RectangleShape boundaries;

};
Texture oysterTexture;
void initOyster(Oyster& oyster, float x, float y);
bool checkOysterCollision(const Oyster& oyster, const Sprite& fishSprite);
void checkPearlCollision(Oyster& oyster, MainFish& fish, int& score);
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

Button newGameBtn, optionsBtn, highScoresBtn, exitBtn, doneButton, exitName;

// Textures
Texture mainMenuBGTexture, gameTitleTexture, optionsFrameTexture, creditsBarTexture;
// Sprites
Sprite mainMenuBGSprite, gameTitleSprite, optionsFrameSprite, creditsBarSprite;
Font gameFont;
struct PlayerScore {
	string name;
	int score;
};
PlayerScore highScores[11] = {};
int scoreCount = 0;
fstream highScoresFile;
string CreditsNames[7] = { "Abdrahman Ezzat", "Adham Elnomairy Ahmed", "Ahmed Abd El-Latif Mostafa", "Alaa Ashraf", "Alaa Mahmoud Ibrahem", "Mona Saber Abdullah", "Nada Mohamed Fekry" };
// Flags
bool gameStartClicked = false, highScoresClicked = false, mainMenuOpen = true, creditsClicked = false;
// Function Declarations
void loadMainMenuSprites();
void fitScale(Sprite&);
void drawScores();
void saveScore(int);
void loadScores();
void drawDone();
void drawCreditsTitle();
void drawOurCredits();
Clock backToMenuClock;
bool backToMenuTimerStarted = true;

// Ahmed
int readCurrentLevel();
void saveNewLevel(int newLevel);
void initializeMapping();
Texture backgr;
void initializeBackground(Sprite& background, int currentLevel);
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

	void triggerExplosion()
	{
		if (option.bubblepressed[0])
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

	bool checkCollision(const Sprite& target)
	{
		if (visible && !playing && !finished && sprite.getGlobalBounds().intersects(target.getGlobalBounds()) && !fish.isImmune && !gameWin)
		{
			triggerExplosion();
			if (!fish.eated)
			{
				if (lives > 0)
					lives--;
				if (lives <= 0)
				{
					state = GameState::Gameover;
					CurrentSound = gameOverSound;
					if (option.bubblepressed[0])
						CurrentSound.play();
				}
				else
				{

					CurrentSound = playerDieSound;
					if (option.bubblepressed[0])
						CurrentSound.play();
					camera.setCenter(camera.getCenter());
					fish.sprite.setPosition(camera.getCenter().x, -fish.imageSize.y);
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
	bool checkfishes(Fish& target)
	{
		if (visible && !playing && !finished && sprite.getGlobalBounds().intersects(target.sprite.getGlobalBounds()) && target.Eated_frame == 0 && target.isinBackground())
		{
			triggerExplosion();
			target.isEated();
			return true;
		}
		return false;
	}

	void draw()
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

int main()
{
	srand(time(0));
	window.setFramerateLimit(60);
	initializeMapping();

	fishscoresh[0].texture.loadFromFile("./assets/fish_spritesheets/Minow.png");
	fishscoresh[1].texture.loadFromFile("./assets/fish_spritesheets/surgeon_fish.png");
	fishscoresh[2].texture.loadFromFile("./assets/fish_spritesheets/lionfish.png");
	fishscoresh[3].texture.loadFromFile("./assets/star_bubble.png");
	font.loadFromFile("./assets/fonts/BigSpace-rPKx.ttf");
	if (!normalTextureWin.loadFromFile("./assets/cersor_1.png") ||
		!hoverTextureWin.loadFromFile("./assets/cersor_2.png"))
		cerr << "Error loading options button textures!\n"
		<< endl;
	initializeSounds();
	srand(time(0));
	if (option.bubblepressed[1])
		CurrentSound.play();

	//  Abdrahman Ezzat
	loadMainMenuSprites();
	loadScores();

	// alaa ashraf
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
	Clock spawnClock;
	float bubbleSpawnDelay = 5.f;
	for (int i = 0; i < 3; ++i)
		bubbles[i].init((BubbleType)(i));
	Clock GameWin;
	float GameWinTime = 0.f;

	Mermaid mermaid;
	try
	{
		initMermaid(mermaid);
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		return -1;
	}
	if (!oysterTexture.loadFromFile("./assets/oyester_project.png"))
	{
	}
	oysterTexture.setSmooth(true);
	float totalWidth = window.getSize().x;
	float spacing = totalWidth / 4.0f;
	float oysterY = window.getSize().y - 150;

	for (int i = 0; i < oysterCount; ++i)
	{
		float x = spacing * (i * 3 + 0.5) - (115 * 1.3f / 2);
		initOyster(oysters[i], x, oysterY);
	}

	if (icon.loadFromFile("./assets/icon.png"))
		window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	Hud hud(score, lives, multiple, levelMax, currentProgress, fish.dashBar);
	hud.loadHUD(window);
	initializeMainFish(1);

	Sprite background;
	initializeBackground(background, 1);
	FloatRect backgroundBounds = background.getGlobalBounds();
	FloatRect deadZone(
		window.getSize().x / 4,
		window.getSize().y / 4,
		window.getSize().x / 2,
		window.getSize().y / 2);

	camera = window.getDefaultView();

	const int FISH_COUNT1 = 30, FISH_COUNT2 = 5, FISH_COUNT3 = 3;
	Fish fishes1[FISH_COUNT1], fishes2[FISH_COUNT2], fishes3[FISH_COUNT3];
	for (int i = 0; i < FISH_COUNT1; i++)
		fishes1[i].init(Fish::type::MINNOW, backgr, background);
	for (int i = 0; i < FISH_COUNT2; i++)
		fishes2[i].init(Fish::type::SURGEON_FISH, backgr, background);
	for (int i = 0; i < FISH_COUNT3; i++)
		fishes3[i].init(Fish::type::LION_FISH, backgr, background);

	Clock clock;


	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();
		if (deltaTime > 0.6f)
			deltaTime = 0.6f;
		Event event;
		Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
		exitName = exitBtn;
		FloatRect exitBtnBounds = exitName.sprite.getLocalBounds();
		exitName.sprite.setOrigin(exitBtnBounds.left + exitBtnBounds.width / 2.f, exitBtnBounds.top + exitBtnBounds.height / 2.f);
		exitName.setPosition(window.getSize().x / 2.f, exitName.sprite.getPosition().y);
		newGameBtn.updateTexture(mousePos);
		optionsBtn.updateTexture(mousePos);
		highScoresBtn.updateTexture(mousePos);
		exitBtn.updateTexture(mousePos);
		exitName.updateTexture(mousePos);
		doneButton.updateTexture(mousePos);
		if (!option.bubblepressed[1])
		{
			CurrentSound.stop();
			soundStopped = 1;
		}
		else
		{
			if (soundStopped)
			{
				CurrentSound.play();
				soundStopped = 0;
			}
		}

		if (option.bubblepressed[2] != isFullscreen)
		{
			isFullscreen = option.bubblepressed[2];
			window.close();
			window.create(
				isFullscreen ? VideoMode::getDesktopMode() : VideoMode(1920, 1080),
				"Fish Frenzy!",
				isFullscreen ? Style::Fullscreen : Style::Default);
			window.setFramerateLimit(60);
		}
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			update(event, mousePos, backToMenuTimerStarted);
			if (state == GameState::win) winUpdate(mousePos, event);
			if (mainMenuOpen && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left && (state == GameState::Playing))
			{
				if (newGameBtn.sprite.getGlobalBounds().contains(mousePos))
				{
					cout << "Start Game clicked\n";
					state = GameState::levels;
				}
				else if (optionsBtn.sprite.getGlobalBounds().contains(mousePos))
				{
					cout << "Game Options clicked\n";
					backToMenuTimerStarted = false;
					state = GameState::options;
					optionsSource = "main";
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
				else if (
					exitBtn.sprite.getGlobalBounds().contains(mousePos) && mainMenuOpen && state == GameState::Playing && backToMenuClock.getElapsedTime().asSeconds() >= 1.0f && backToMenuTimerStarted
					)
				{
					cout << "Game Closed, Bye Bye\n";
					window.close();
				}


				mainMenuOpen = !(gameStartClicked || highScoresClicked || creditsClicked || state == GameState::levels);
			}
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left && exitName.sprite.getGlobalBounds().contains(mousePos) && mainMenuOpen && state == GameState::NameEntry) {
				cout << "Game Closed, Bye Bye\n";
				window.close();
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
						currentLevel = i + 1;
						state = GameState::Playing;
						gameStartClicked = true;
						winIn = true;

						fish.sprite.setPosition(camera.getCenter().x, -500);
						startImmunity();
						for (int i = 0; i < FISH_COUNT1; i++)
							fishes1[i].init(Fish::type::MINNOW, backgr, background);
						for (int i = 0; i < FISH_COUNT2; i++)
							fishes2[i].init(Fish::type::SURGEON_FISH, backgr, background);
						for (int i = 0; i < FISH_COUNT3; i++)
							fishes3[i].init(Fish::type::LION_FISH, backgr, background);
						for (int i = 0; i < 3; ++i)
						{
							bubbles[i].spawn(bubbleTextures[i], window.getSize());
							bubbles[i].isActive = 0;
						}
						fish.settled = false;
						fish.fishLevel = 2.f;
						gameWin = 0;
						fish.fishSize = 1.f;
						currentProgress = 48000.f;
						lives = 3;
						score = 0;
						multiple = 1;
						alreadywon = 0;
						esacpedFish = 0;
						mermaid.isActive = true;
						mermaid.spawnedBubbleCount = 0;
						for (int i = 0; i < 4; i++) fishscoresh[i].finalCount = 0;
						for (int i = 0; i < 10; i++) mermaidBubbles[i].spawn(bubbleTextures[0], window.getSize(), true);

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
			}
			if (state == GameState::options)
				option.update(mousePos, window, event);
			for (int i = 0; i < 4; i++)
			{
				if (levelPearls[i].getGlobalBounds().contains(mousePos))
					levelPearls[i].setTextureRect(IntRect(1 * levelsImageSize.x, (arrOFlevels[i] ? 1 : 0) * levelsImageSize.y, levelsImageSize.x, levelsImageSize.y + 1));

				else
					levelPearls[i].setTextureRect(IntRect(0 * levelsImageSize.x, (arrOFlevels[i] ? 1 : 0) * levelsImageSize.y, levelsImageSize.x, levelsImageSize.y + 1));
			}

			if (event.type == Event::TextEntered) {
				if (event.text.unicode == '\b' && !playerName.empty()) {
					playerName.pop_back();
				}
				else if (event.text.unicode < 128 && isprint(event.text.unicode) && playerName.size() < 12) {
					playerName += char(event.text.unicode);
				}
			}
			if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
				if (playerName.empty()) playerName = "Anonymous";
				state = GameState::Playing;
			}
		}
		if (currentProgress >= levelMax && !gameWin && gameStartClicked && winIn)
		{
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
			CurrentSound = mermaidSound;
			if (option.bubblepressed[0])
				CurrentSound.play();
		}
		if (GameWin.getElapsedTime().asSeconds() >= 1.5f && gameWin && gameStartClicked && !winIn)
			esacpedFish = 1;
		if (GameWin.getElapsedTime().asSeconds() >= 5.f && gameWin && gameStartClicked && !winIn)
		{
			saveScore(score);
			saveNewLevel(currentLevel);
			gameWin = 0;
			state = GameState::win;
			mermaid.spawnedBubbleCount = 0;
			//mermaid.bubbleClock.restart();
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
				drawCreditsTitle();
			}
		}
		if (gameStartClicked && state == GameState::Playing)
		{
			window.setMouseCursorVisible(false);
			window.draw(background);
			if (state != GameState::Playing)
				deltaTime = 0;
			updateMermaid(mermaid, deltaTime);
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
								oyster.hasPowerUp = (rand() % 2 == 0);
								oyster.blackOrWhite = rand() % 4;
								oyster.pearl.setTextureRect((oyster.blackOrWhite == 0) ? IntRect(2, 105, 45, 45) : IntRect(3, 155, 45, 45));
							}

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
										fish.sprite.setPosition(camera.getCenter().x, -fish.imageSize.y);
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
						checkPearlCollision(oyster, fish, score);
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
					if (option.bubblepressed[3] && bubbles[i].isActive && !gameWin)
					{
						bubbles[i].update(deltaTime);
						bubbles[i].draw();
						bubbles[i].checkCollision(fish, score, multiple, lives);
					}
				}
			}
			else {
				for (int i = 0; i < 10; ++i)
				{
					if (mermaidBubbles[i].isActive)
					{
						mermaidBubbles[i].update(deltaTime);
						mermaidBubbles[i].draw();
						mermaidBubbles[i].checkCollision(fish, score, multiple, lives);
					}
				}
			}
			if (!esacpedFish)
			{
				for (int i = 0; i < FISH_COUNT1; i++)
				{
					fishes1[i].update(deltaTime);
					window.draw(fishes1[i].sprite);
					checkAndEatPlayerFish(fishes1[i]);
					if (currentLevel >= 4)
						mine.checkfishes(fishes1[i]);
				}
				for (int i = 0; i < FISH_COUNT2; i++)
				{
					fishes2[i].update(deltaTime);
					window.draw(fishes2[i].sprite);
					for (int j = 0; j < FISH_COUNT1; j++)
						checkAndEatFishCollision(fishes2[i], fishes1[j]);
					checkAndEatPlayerFish(fishes2[i]);
					if (currentLevel >= 4)
						mine.checkfishes(fishes2[i]);
				}
				for (int i = 0; i < FISH_COUNT3; i++)
				{
					fishes3[i].update(deltaTime);
					window.draw(fishes3[i].sprite);
					for (int j = 0; j < FISH_COUNT1; j++)
						checkAndEatFishCollision(fishes3[i], fishes1[j]);
					for (int j = 0; j < FISH_COUNT2; j++)
						checkAndEatFishCollision(fishes3[i], fishes2[j]);
					checkAndEatPlayerFish(fishes3[i]);
					if (currentLevel >= 4)
						mine.checkfishes(fishes3[i]);
				}
			}
			if (currentLevel >= 4)
			{
				mine.update(deltaTime);
				mine.checkCollision(fish.sprite);
				mine.draw();
			}

			updateMainFish(deltaTime);
			view.setCenter(fish.sprite.getPosition());
			cameraUpdate(backgroundBounds, deadZone);
			hud.updateHUD(deltaTime);
			window.setView(camera);
			window.draw(fish.sprite);
			if (mermaid.isActive && gameWin)
			{
				window.draw(mermaid.sprite);
			}
			else
			{
				initMermaid(mermaid);
			}
			window.setView(window.getDefaultView());
			hud.draw(window);
			window.setView(camera);
		}
		else if (highScoresClicked)
		{
			window.setMouseCursorVisible(true);
			gameStartClicked = false;
			creditsClicked = false;

			window.draw(mainMenuBGSprite);
			window.draw(optionsFrameSprite);
			drawScores();

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
			drawDone();
		}
		else if (creditsClicked)
		{
			window.setMouseCursorVisible(true);
			highScoresClicked = false;
			gameStartClicked = false;

			window.draw(mainMenuBGSprite);
			window.draw(optionsFrameSprite);
			drawOurCredits();

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
			drawDone();
		}
		else {
			window.setMouseCursorVisible(true);
		}

		mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
		if (state == GameState::NameEntry) {
			window.setView(window.getDefaultView());
			window.setMouseCursorVisible(true);
			Text nameText;
			nameText.setFont(font);
			if (playerName.size())
			{
				cursorBlinkTime += deltaTime;
				if (cursorBlinkTime >= 0.5f)
				{
					cursorVisible = !cursorVisible;
					cursorBlinkTime = 0.f;
				}
			}
			else
			{
				cursorVisible = false;
				cursorBlinkTime = 0.f;
			}
			nameText.setString("Player: " + (playerName.size() ? playerName + (cursorVisible ? "|" : "") : "Type Your Name"));
			nameText.setCharacterSize(50);
			nameText.setFillColor(Color::White);
			FloatRect textBounds = nameText.getLocalBounds();
			Vector2u windowSize = window.getSize();
			nameText.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
			nameText.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
			window.draw(nameText);
			window.draw(exitName.sprite);
		}
		if (state == GameState::levels)
		{
			window.setMouseCursorVisible(true);
			window.setView(window.getDefaultView());
			window.draw(mainMenuBGSprite);
			for (int i = 0; i < 4; i++)
			{

				window.draw(levelPearls[i]);
				window.draw(levelTexts[i]);
			}
		}
		if (state == GameState::Paused)
		{
			window.setMouseCursorVisible(true);
			window.setView(window.getDefaultView());
			window.draw(mainMenuBGSprite);
			pauseMenu.render(window, score, highScores[0].score, fisheaten);
			pauseMenu.update(mousePos, window);
		}
		if (state == GameState::Gameover)
		{
			window.setMouseCursorVisible(true);
			window.setView(window.getDefaultView());
			gameover.render(window, score);
			gameover.update(mousePos, window);
		}
		if (state == GameState::options)
		{
			window.setMouseCursorVisible(true);
			creditsClicked = false;
			highScoresClicked = false;
			window.draw(mainMenuBGSprite);
			window.setView(window.getDefaultView());
			option.render(window);
			option.hover(mousePos, window);
		}
		if (state == GameState::win)
		{
			window.setMouseCursorVisible(true);
			window.setView(window.getDefaultView());
			window.draw(mainMenuBGSprite);
			win(mousePos, event, score, highScores[0].score, fisheaten);
		}

		window.display();
	}
	return 0;
}

// Ahmed
int readCurrentLevel()
{
	ifstream file("./assets/level.txt");
	int level = 0;
	if (file.is_open())
	{
		file >> level;
		file.close();
	}
	return level;
}
void saveNewLevel(int newLevel)
{
	int currentLevel = readCurrentLevel();
	if (newLevel + 1 > currentLevel)
	{
		ofstream file("./assets/level.txt");
		if (file.is_open())
		{
			file << newLevel;
			file.close();
		}
	}
}

void initializeMapping()
{
	level = readCurrentLevel();
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

// Abdrahman Ezzat
void fitScale(Sprite& sprite)
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
	fitScale(mainMenuBGSprite);

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
		vector<PlayerScore> tempScores;
		long long temp;
		string name;
		int score;
		while (highScoresFile >> name >> score)
		{
			if (score > 0)
				tempScores.push_back({ name, score });

		}
		highScoresFile.close();

		sort(tempScores.begin(), tempScores.end(), [](const PlayerScore& a, const PlayerScore& b) {
			return a.score > b.score;
			});
		scoreCount = min(10, (int)tempScores.size());
		for (int i = 0; i < scoreCount; ++i) {
			highScores[i] = tempScores[i];
		}
	}
}
void saveScore(int newScore)
{
	loadScores();

	highScores[scoreCount] = { playerName, newScore };
	scoreCount++;

	sort(highScores, highScores + scoreCount, [](const PlayerScore& a, const PlayerScore& b) {
		return a.score > b.score;
		});
	scoreCount = min(scoreCount, 10);

	highScoresFile.open("./assets/HighestScoresFile.txt", ios::out | ios::trunc);
	if (highScoresFile.is_open())
	{
		for (int i = 0; i < scoreCount; ++i)
		{
			highScoresFile << highScores[i].name << " " << highScores[i].score << '\n';
		}
		highScoresFile.close();
	}
	else
		cerr << "Error opening the Highest Scores File!!" << endl;
}
void drawScores()
{
	Text HighScoresTitle("[ Highest Scores ]", gameFont, 90);
	HighScoresTitle.setFillColor(Color::White);
	HighScoresTitle.setPosition(620.f, 225.f);
	window.draw(HighScoresTitle);

	int displayCount = min(scoreCount, 10);
	const int maxNameLength = 10;
	const float nameX = 720.f;
	const float scoreX = 1050.f;
	const float baseY = 350.f;
	const float rowHeight = 40.f;

	for (int i = 0; i < displayCount; ++i)
	{
		string displayName = highScores[i].name;
		if ((int)displayName.size() > maxNameLength)
			displayName = displayName.substr(0, maxNameLength - 3) + "...";

		Text nameText(to_string(i + 1) + ". " + displayName, gameFont, 45);
		nameText.setFillColor(Color::White);
		nameText.setPosition(nameX, baseY + i * rowHeight);
		window.draw(nameText);

		Text scoreText(to_string(highScores[i].score), gameFont, 45);
		scoreText.setFillColor(Color::White);
		scoreText.setPosition(scoreX, baseY + i * rowHeight);
		window.draw(scoreText);
	}
}
void drawDone()
{
	Text DoneTitle("Done", gameFont, 60);
	DoneTitle.setFillColor(Color::Black);
	DoneTitle.setPosition(880.f, 815.f);
	window.draw(DoneTitle);
}
void drawCreditsTitle()
{
	Text CreditsTitle("Credits", gameFont, 40);
	CreditsTitle.setFillColor(Color::White);
	CreditsTitle.setPosition(1718.f, 950.f);
	window.draw(CreditsTitle);
}
void drawOurCredits()
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

// mona
void initializeMainFish(int currentLevel)
{

	if (currentLevel < 3) {
		fish.fishTexture.loadFromFile("./assets/fish_spritesheets/angel_fish.png");
		fish.mouth.setSize({ 25,25 });
		fish.isGreen = false;
	}
	else {
		fish.fishTexture.loadFromFile("./assets/fish_spritesheets/Anglerfish.png");
		fish.mouth.setSize({ 50,50 });
		fish.isGreen = true;

	}
	fish.imageSize = Vector2f(fish.fishTexture.getSize().x / 15.f, fish.fishTexture.getSize().y / 4.f);
	fish.sprite.setTexture(fish.fishTexture);
	fish.sprite.setOrigin(fish.imageSize.x / 2, fish.imageSize.y / 2);

	fish.sprite.setPosition(camera.getCenter().x, -fish.imageSize.y); // start above screen
	fish.sprite.setTextureRect(IntRect(0 * fish.imageSize.x + 2, fish.imageSize.y + 1, fish.imageSize.x - 2, fish.imageSize.y - 1));
}
void moveFish(float deltaTime)
{
	Vector2i mousePos = Mouse::getPosition(window);
	Vector2f worldMouse = window.mapPixelToCoords(mousePos);
	Vector2f fishPos = fish.sprite.getPosition();
	Vector2f delta = worldMouse - fishPos;
	float distance = sqrt(delta.x * delta.x + delta.y * delta.y);
	if (!fish.settled)
	{
		if (fish.sprite.getPosition().y < window.getSize().y / 2.f)
			fish.sprite.move(0.f, fish.speed * deltaTime);
		else
			fish.settled = true;
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
		if (option.bubblepressed[i])
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
	if (fish.facingLeft) {
		if (fish.isGreen) {
			fish.mouth.setPosition(fish.sprite.getGlobalBounds().left, fish.sprite.getGlobalBounds().top + fish.sprite.getGlobalBounds().height / 2);
		}
		else {
			fish.mouth.setPosition(fish.sprite.getGlobalBounds().left, fish.sprite.getGlobalBounds().top + fish.sprite.getGlobalBounds().height / 2 - 10);
		}
	}
	if (!fish.facingLeft) {
		if (fish.isGreen) {
			fish.mouth.setPosition(fish.sprite.getGlobalBounds().left + fish.sprite.getGlobalBounds().width - 60, fish.sprite.getGlobalBounds().top + fish.sprite.getGlobalBounds().height / 2);

		}
		else
			fish.mouth.setPosition(fish.sprite.getGlobalBounds().left + fish.sprite.getGlobalBounds().width - 25, fish.sprite.getGlobalBounds().top + fish.sprite.getGlobalBounds().height / 2 - 10);

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
}
void updateMainFish(float deltaTime)
{
	if (fish.isImmune)
	{
		if (fish.immunityClock.getElapsedTime().asSeconds() >= 5.0f)
		{
			fish.isImmune = false;
		}
	}
	moveFish(deltaTime);
	applyBoundaries();
	updateAnimation(deltaTime);
	updateMainFishScale();
}

// nada
void cameraUpdate(const FloatRect& backgroundBounds, const FloatRect& deadZone)
{
	Vector2f targetPosition = fish.sprite.getPosition();
	float targetX = camera.getCenter().x;
	float targetY = camera.getCenter().y;

	if (targetPosition.x < camera.getCenter().x - deadZone.width / 2)
		targetX = targetPosition.x + deadZone.width / 2;
	else if (targetPosition.x > camera.getCenter().x + deadZone.width / 2)
		targetX = targetPosition.x - deadZone.width / 2;
	if (targetPosition.y < camera.getCenter().y - deadZone.height / 2)
		targetY = targetPosition.y + deadZone.height / 2;
	else if (targetPosition.y > camera.getCenter().y + deadZone.height / 2)
		targetY = targetPosition.y - deadZone.height / 2;

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
bool checkFishCollision(Fish& a, Fish& b) {
	if (int(a.current_type) > int(b.current_type))
		return a.mouth.getGlobalBounds().intersects(b.getGlobalBounds());
	if (int(b.current_type) > int(a.current_type))
		return b.mouth.getGlobalBounds().intersects(a.getGlobalBounds());

}
bool checkMainFishCollision(MainFish& a, Fish& b) {
	int npcLevel = (int)(b.current_type);

	float ratio = currentProgress / levelMax;
	int effectiveLevel = 2 + (int)(ratio * 3);
	if (effectiveLevel >= npcLevel)
		return a.mouth.getGlobalBounds().intersects(b.getGlobalBounds());
	return b.mouth.getGlobalBounds().intersects(a.getGlobalBounds());

}
void checkAndEatFishCollision(Fish& fish1, Fish& fish2)
{
	if (checkFishCollision(fish1, fish2) && fish1.isinBackground() && fish2.isinBackground() && !gameWin)
	{
		if (int(fish1.current_type) > int(fish2.current_type)
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
void checkAndEatPlayerFish(Fish& otherFish)
{
	if (checkMainFishCollision(fish, otherFish) && otherFish.Eated_frame == 0 && !fish.eated && fish.settled && !gameWin)
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
				fishscoresh[0].finalCount++;
				if (option.bubblepressed[0])
					bite1Sound.play();
			}
			else if (npcLevel == 3)
			{
				fishscoresh[1].finalCount++;
				if (option.bubblepressed[0])
					bite2Sound.play();
			}
			else
			{
				fishscoresh[2].finalCount++;
				if (option.bubblepressed[0])
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
					if (option.bubblepressed[0])
						gameOverSound.play();
				}
				else
				{
					if (option.bubblepressed[0])
						playerDieSound.play();
					camera.setCenter(camera.getCenter());
					fish.sprite.setPosition(camera.getCenter().x, -fish.imageSize.y);

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
void update(Event event, Vector2f mousePos, bool& backToMenuTimerStarted)
{
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

	if (state == GameState::Paused && event.type == Event::MouseButtonPressed)
	{

		if (pauseMenu.option(mousePos))
		{
			state = GameState::options;
			optionsSource = "pause";
			mainMenuOpen = 0;
		}

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
			mainMenuOpen = 0;
		}
		if (pauseMenu.isQuitClicked(mousePos))
		{
			if (option.bubblepressed[1])
			{
				CurrentSound = menuSound;
				CurrentSound.play();
				CurrentSound.setLoop(true);
			}
			state = GameState::Playing;
			mainMenuOpen = 1;
			gameStartClicked = 0;
			initializeMapping();
			backToMenuClock.restart();
			backToMenuTimerStarted = true;
		}
	}
	if (state == GameState::options && event.type == Event::MouseButtonPressed)
	{
		if (option.doneClicked(mousePos))
		{

			if (!mainMenuOpen) {
				state = GameState::Paused;
				if (optionsSource == "win")
				{
					state = GameState::win;
				}
			}
			else
			{
				backToMenuClock.restart();
				backToMenuTimerStarted = true;
				state = GameState::Playing;
				if (optionsSource == "win")
				{
					state = GameState::win;
				}
			}
		}
	}

	if (event.type == Event::KeyPressed && event.key.code == Keyboard::P)
	{
		if (state == GameState::Playing)
			state = GameState::Gameover;
		else
			state = GameState::Playing;
	}

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
			CurrentSound.setLoop(true);
			gameWin = 0;
			state = GameState::levels;
		}
		if (gameover.QuitClicked(mousePos))
			window.close();
	}
}

// alaa ashraf
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

void initMermaid(Mermaid& mermaid)
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
void updateMermaid(Mermaid& mermaid, float deltaTime)
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
	if (gameWin && mermaid.spawnedBubbleCount < 10 && mermaid.bubbleClock.getElapsedTime().asSeconds() > 0.5f)
	{
		int i = mermaid.spawnedBubbleCount;
		Vector2f startPos = mermaid.sprite.getPosition() + Vector2f(300.f, 20.f);

		mermaidBubbles[i].init(BubbleType::Star);
		mermaidBubbles[i].spawn(bubbleTextures[0], window.getSize(), true, startPos);
		mermaid.spawnedBubbleCount++;
		mermaid.bubbleClock.restart();
	}
}
void initOyster(Oyster& oyster, float x, float y)
{

	oyster.sprite.setTexture(oysterTexture);
	oyster.closedRect = IntRect(2, 2, 98.5, 98.5);
	oyster.openRect = IntRect(404 + 2, 2, 98.5, 98.5);
	oyster.boundaries.setFillColor(sf::Color::Blue);
	oyster.boundaries.setSize({ 100,75 });
	oyster.sprite.setTextureRect(oyster.closedRect);
	oyster.sprite.setPosition(x, y);
	oyster.boundaries.setPosition(oyster.sprite.getGlobalBounds().left + oyster.sprite.getGlobalBounds().width - 45,
		oyster.sprite.getGlobalBounds().top + oyster.sprite.getGlobalBounds().height / 2 + 20);
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
	;
	if (!oyster.isOpen && oyster.boundaries.getGlobalBounds().intersects(fishSprite.getGlobalBounds()))
	{
		return true;
	}
	return false;
}
void checkPearlCollision(Oyster& oyster, MainFish& fish, int& score)
{
	if (oyster.isOpen && oyster.hasPowerUp && checkCollision(oyster.pearl, fish.sprite))
	{
		score += 100;
		oyster.hasPowerUp = false;
		if (option.bubblepressed[0])
			bite1Sound.play();
		if (!fish.eating)
		{
			fish.eating = true;
			fish.frame = 0;
			fish.eatFrameTimer = 0.f;
		}
	}
}




void win(Vector2f mousePos, Event event, int& scorenum, int& highscorenum, int& fisheat)
{

	const int width = window.getSize().x;
	const int height = window.getSize().y;

	wining.Puase.setScale(2.6, 2.3);
	wining.cersor.setPosition(width / 2, height * 6.3 / 7);
	wining.cersor.setScale(2.5, 2.5);
	wining.cersor2.setPosition(wining.Puase.getGlobalBounds().width, wining.Puase.getGlobalBounds().height - 10);
	wining.cersor2.setScale(2.5, 2.5);
	wining.cersor4.setPosition(width - wining.Puase.getGlobalBounds().width, wining.Puase.getGlobalBounds().height - 10);
	wining.cersor4.setScale(2.5, 2.5);
	wining.fishFerenzy.setPosition(width / 2, height * 0.7 / 7);
	wining.fishFerenzy.setScale(2.5, 2.5);
	wining.continu.setPosition(width / 2, height * 6.15 / 7);
	wining.continu.setScale(1.7, 1.7);
	wining.end.setPosition(wining.Puase.getGlobalBounds().width, wining.Puase.getGlobalBounds().height - 25);
	wining.end.setScale(1, 1);
	wining.options.setPosition(width - wining.Puase.getGlobalBounds().width, wining.Puase.getGlobalBounds().height - 25);
	wining.options.setScale(1, 1);
	wining.score.setPosition(width * 3 / 7, wining.Puase.getGlobalBounds().height - 25);
	wining.score.setScale(1, 1);
	wining.score.setPosition(width * 2 / 7, height * 4 / 7);
	wining.score.setScale(1, 1);
	wining.highscore.setPosition(width * 2 / 7, height * 4.5 / 7);
	wining.highscore.setScale(1, 1);
	wining.fisheaten.setPosition(width * 2 / 7, height * 5 / 7);
	wining.fisheaten.setScale(1, 1);



	int fish_row = 0, fish_column = 0;
	int fish_widthSize = 991 / 15.0f;
	int fish_heightSize = 99 / 2.0f;
	fishscoresh[0].sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
		fish_row * fish_heightSize + 1, fish_widthSize - 3, fish_heightSize - 1));

	fish_widthSize = 2395 / 14.0f;
	fish_heightSize = 319 / 3.0f;
	fishscoresh[1].sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
		fish_row * fish_heightSize + 1, fish_widthSize - 3, fish_heightSize - 1));


	fish_widthSize = 2535 / 14.0f;
	fish_heightSize = 645 / 4.0f;
	fishscoresh[2].sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
		fish_row * fish_heightSize + 1, fish_widthSize - 2, fish_heightSize - 1));


	for (int i = 0; i < 4; i++) {
		fishscoresh[i].sprite.setTexture(fishscoresh[i].texture);
		fishscoresh[i].sprite.setOrigin(fishscoresh[i].sprite.getGlobalBounds().width / 2, fishscoresh[i].sprite.getGlobalBounds().height / 2);
		fishscoresh[i].sprite.setPosition(wining.Puase.getGlobalBounds().width * (2.5 + i + (i >= 3 ? 1 : 0)) / 7, height - wining.Puase.getGlobalBounds().height + (i >= 3 ? 290 : 250));
		fishscoresh[i].counterText.setFont(font);
		fishscoresh[i].counterText.setFillColor(Color(240, 250, 250));
		fishscoresh[i].counterText.setOrigin(fishscoresh[i].counterText.getGlobalBounds().width / 2, fishscoresh[i].counterText.getLocalBounds().height / 2);
		fishscoresh[i].counterText.setPosition(wining.Puase.getGlobalBounds().width * (2.5 + i + (i >= 3 ? 1 : 0)) / 7, height - wining.Puase.getGlobalBounds().height + 350);
		fishscoresh[i].update();

	}
	//    fishscoresh[0].counterText.setCharacterSize(30);
	fishscoresh[3].sprite.setScale(1.5, 1.5);
	wining.cersor_ = wining.cersor.getGlobalBounds().contains(mousePos) ? hoverTextureWin : normalTextureWin;
	if (wining.cersor2.getGlobalBounds().contains(mousePos))
	{
		wining.end.setFillColor(Color(200, 200, 250));
	}
	else
		wining.end.setFillColor(Color(240, 250, 250));

	if (wining.cersor4.getGlobalBounds().contains(mousePos))
	{
		wining.options.setFillColor(Color(200, 200, 250));
	}
	else
		wining.options.setFillColor(Color(240, 250, 250));
	wining.score.setString("Your score : " + to_string(scorenum));
	wining.highscore.setString("High Score : " + to_string(highscorenum));
	wining.fisheaten.setString("Fish Eaten : " + to_string(fisheat));

	window.draw(wining.Puase);
	window.draw(wining.cersor);
	window.draw(wining.cersor2);
	window.draw(wining.cersor4);
	window.draw(wining.end);
	window.draw(wining.options);
	window.draw(wining.continu);
	window.draw(wining.fishFerenzy);
	window.draw(wining.score);
	window.draw(wining.highscore);
	window.draw(wining.fisheaten);
	for (int i = 0; i < 4; i++)
		fishscoresh[i].draw();
}


void winUpdate(Vector2f mousePos, Event event) {

	if (event.type == Event::MouseButtonPressed)
	{

		if (wining.option(mousePos))
		{
			state = GameState::options;
			optionsSource = "win";
			mainMenuOpen = 1;
		}
		if (wining.isResumeClicked(mousePos))
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
			mainMenuOpen = 0;
			//Wining = 0;
		}

		if (wining.isQuitClicked(mousePos))
		{
			if (option.bubblepressed[1])
			{
				CurrentSound = menuSound;
				CurrentSound.play();
				CurrentSound.setLoop(true);
			}
			state = GameState::Playing;
			mainMenuOpen = 1;
			gameStartClicked = 0;
			initializeMapping();
			backToMenuClock.restart();
			backToMenuTimerStarted = true;
			//Wining = 0;
		}
	}

}
