#pragma once
#pragma once
#include <SFML/Graphics.hpp> 
#include<SFML/Audio.hpp>
#include<iostream>
using namespace sf;
using namespace std;

// hud_ellipse
class Hud {
private:
	// Start Header 
	Font font;
	// Start Variables
	static const int markerCount = 2;
	int progressWidth = 300, progressHeight = 20, maxLevels = 3, abilityBarWidth = 200, abilityBarHeight = 20;
	int& score;
	int& lives;
	int& multiple;
	float& levelMax;
	float& currentProgress;
	float& dashBar;
	// End Variables
	// Start Textures & Sprites
	Texture headerTexture, fishTexture, fishTexture1, fishTexture2, scoreTexture, livesTexture, multipleTexture, hud_ellipse_texture, fish_frenzy_texture;
	Sprite header, fishSprite, fish1Sprite, fish2Sprite, scoreSprite, livesSprite, multipleSprite, hud_ellipse_sprites[2], fish_frenzy_sprite;
	// End Textures & Sprites
	// Start Shapes
	RectangleShape progressBarBackground, progressBar, markers[markerCount], abilityBarBackground, abilityBar;
	// End Shapes
	// Start Texts
	Text scoreText, scoreType, menuText, growthText, livesText, livesType, multipleText, multipleType, abilityText;
	Vector2f scoreBackgroundPos, livesBackgroundPos, multipleBackgroundPos;
	// End Texts
	void setupMarkers();
	void loadFishSprites();
	void centerText(Text&, Vector2f);
	// End Header 
public:
	Hud(int& score, int& lives, int& multiple, float& levelMax, float& currentProgress, float& dashBar);
	void loadHUD(RenderWindow& window);
	void updateHUD(float dt);
	void eatFish();
	void draw(RenderWindow& window);
};
