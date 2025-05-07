#include "Hud.h"
#include <iostream>

Hud::Hud(int& s, int& l, int& m, float& lm, float& cp, float& db) :
	progressBarBackground(Vector2f(progressWidth, progressHeight)), progressBar(Vector2f(0, progressHeight)),
	abilityBarBackground(Vector2f(abilityBarWidth, abilityBarHeight)), abilityBar(Vector2f(0, abilityBarHeight)),
	scoreBackgroundPos(620, 70), livesBackgroundPos(870, 70), multipleBackgroundPos(1180, 70),
	score(s), lives(l), multiple(m), levelMax(lm), currentProgress(cp), dashBar(db)
{

}

void Hud::loadHUD(RenderWindow& window)
{
	if (!headerTexture.loadFromFile("./assets/header.png"))
		cerr << "Error loading header texture!" << endl;
	header.setTexture(headerTexture);
	header.setPosition(0, 0);
	//header.setScale(1.0f, 150.0f / headerTexture.getSize().y);
	Vector2u windowSize = window.getSize();
	float scaleX = windowSize.x / (float)headerTexture.getSize().x;
	header.setScale(scaleX, 150.0f / headerTexture.getSize().y);
	progressBarBackground.setFillColor(Color::Black);
	progressBarBackground.setPosition(100, 100);
	progressBar.setFillColor(Color::Yellow);
	progressBar.setPosition(100, 100);
	if (font.loadFromFile("./assets/fonts/BigSpace-rPKx.ttf"))
	{
		scoreTexture.loadFromFile("./assets/imagebg.png");
		multipleTexture.loadFromFile("./assets/imagebg.png");
		livesTexture.loadFromFile("./assets/imagebg.png");
		scoreSprite.setTexture(scoreTexture);
		livesSprite.setTexture(livesTexture);
		multipleSprite.setTexture(multipleTexture);
		FloatRect bounds = livesSprite.getLocalBounds();
		scoreSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
		livesSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
		multipleSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

		scoreSprite.setScale(0.75, 0.75);
		livesSprite.setScale(0.75, 0.75);
		multipleSprite.setScale(0.75, 0.75);


		menuText.setFont(font);
		menuText.setCharacterSize(20);
		menuText.setFillColor(Color::White);
		menuText.setPosition(20, 60);
		menuText.setString("MENU");

		growthText.setFont(font);
		growthText.setCharacterSize(20);
		growthText.setFillColor(Color::White);
		growthText.setPosition(20, 100);
		growthText.setString("GROWTH");

		scoreType.setFont(font);
		scoreType.setCharacterSize(20);
		scoreType.setFillColor(Color::Black);
		scoreType.setPosition(430, 50);
		scoreType.setString("score : ");
		scoreType.setScale(2.0f, 2.0f);

		multipleType = scoreType;
		livesType = scoreType;
		livesType.setPosition(700, 50);
		livesType.setString("lives : ");
		multipleType.setPosition(950, 50);
		multipleType.setString("multiple : ");

		scoreSprite.setPosition(scoreBackgroundPos);
		livesSprite.setPosition(livesBackgroundPos);
		multipleSprite.setPosition(multipleBackgroundPos);

		scoreText.setFont(font);
		scoreText.setCharacterSize(20);
		scoreText.setFillColor(Color::Black);
		livesText = scoreText;
		multipleText = scoreText;
		abilityText = scoreText;
		abilityText.setPosition(1300, 30);
		abilityText.setString("Ability");
		abilityText.setScale(2.0f, 2.0f);
		abilityBarBackground.setFillColor(Color(50, 50, 50));
		abilityBarBackground.setPosition(1250, 80);
		abilityBar.setFillColor(Color::Cyan);
		abilityBar.setPosition(1250, 80);


		if (!fish_frenzy_texture.loadFromFile("./assets/fish_ferenzy.png"))
			cerr << "Error loading fish_frenzy texture!" << endl;
		fish_frenzy_sprite.setTexture(fish_frenzy_texture);
		fish_frenzy_sprite.setPosition(1700, 80);
		FloatRect fish_frenzy_sprite_bounds = fish_frenzy_sprite.getLocalBounds();
		fish_frenzy_sprite.setOrigin(fish_frenzy_sprite_bounds.width / 2.f, fish_frenzy_sprite_bounds.height / 2.f);
	}
	setupMarkers();
	loadFishSprites();
}

void Hud::updateHUD(float dt)
{
	scoreText.setString(to_string(score));
	livesText.setString(to_string(lives));
	multipleText.setString(to_string(multiple) + "x");
	centerText(scoreText, scoreBackgroundPos);
	centerText(livesText, livesBackgroundPos);
	centerText(multipleText, multipleBackgroundPos);
	float progress = currentProgress / levelMax;
	progressBar.setSize(Vector2f(progressWidth * progress, progressHeight));
	abilityBar.setSize(Vector2f(abilityBarWidth * dashBar, abilityBarHeight));
	if (currentProgress >= levelMax / 3.0f)
		fish1Sprite.setColor(Color::White);
	else
		fish1Sprite.setColor(Color(255, 255, 255, 100));
	if (currentProgress >= (2 * levelMax / 3.0f))
		fish2Sprite.setColor(Color::White);
	else
		fish2Sprite.setColor(Color(255, 255, 255, 100));

}
void Hud::setupMarkers()
{
	for (int i = 0; i < markerCount; i++)
	{
		markers[i].setSize(Vector2f(2, 20));
		markers[i].setFillColor(Color::Red);
		markers[i].setPosition(100 + (i + 1) * (progressWidth / maxLevels), 100);
	}
}

void Hud::eatFish() {
	/*if (currentProgress < levelMax)
	{
		currentProgress += 150.f;
		score += 1;
	}*/
}

void Hud::centerText(Text& text, Vector2f center) {
	FloatRect bounds = text.getLocalBounds();
	text.setOrigin(bounds.left + bounds.width / 2.0f,
		bounds.top + bounds.height / 2.0f);
	text.setPosition(center);
}

void Hud::loadFishSprites()
{
	if (!fishTexture.loadFromFile("./assets/fish_spritesheets/Minow.png"))
		cerr << "Error loading minnow_fish texture!" << endl;
	if (!fishTexture1.loadFromFile("./assets/fish_spritesheets/surgeon_fish.png"))
		cerr << "Error loading surgeon_fish texture!" << endl;
	if (!fishTexture2.loadFromFile("./assets/fish_spritesheets/lionfish.png"))
		cerr << "Error loading lion_fish texture!" << endl;
	if (!hud_ellipse_texture.loadFromFile("./assets/hud_ellipse.png"))
		cerr << "Error loading hud_ellipse texture!" << endl;
	fishSprite.setTexture(fishTexture);
	fish1Sprite.setTexture(fishTexture1);
	fish2Sprite.setTexture(fishTexture2);
	for (int i = 0; i < 2; i++) hud_ellipse_sprites[i].setTexture(hud_ellipse_texture);
	int row = 0, column = 0;
	float widthSize = 991 / 15.0f, heightSize = 99 / 2.0f;
	fishSprite.setTextureRect(IntRect(column * widthSize + 1, row * heightSize + 1, widthSize - 1, heightSize - 1));
	float widthSize1 = (2395) / 14.0f, heightSize1 = 319 / 3.0f;
	fish1Sprite.setTextureRect(IntRect(column * widthSize1 + 1, row * heightSize1 + 1, widthSize1 - 2, heightSize1 - 1));
	float widthSize2 = (2535) / 14.0f, heightSize2 = 645 / 4.0f;
	fish2Sprite.setTextureRect(IntRect(column * widthSize2 + 1, row * heightSize2 + 1, widthSize2 - 1, heightSize2 - 1));
	Vector2f markerPos = markers[0].getPosition();
	fishSprite.setPosition(100 + 10, markerPos.y - 55);
	fish1Sprite.setPosition(100 + 115, markerPos.y - 65);
	fish1Sprite.setScale(0.5f, 0.5f);
	fish2Sprite.setScale(0.5f, 0.5f);
	fish2Sprite.setPosition(100 + 220, markerPos.y - 80);
}
void Hud::draw(RenderWindow& window)
{
	window.draw(header);
	window.draw(progressBarBackground);
	window.draw(progressBar);
	for (int i = 0; i < markerCount; i++)
		window.draw(markers[i]);
	window.draw(fishSprite);
	window.draw(fish1Sprite);
	window.draw(fish2Sprite);
	for (int i = 0; i < 2; i++) {
		hud_ellipse_sprites[i].setPosition(200 + (i * 100), 1);
		hud_ellipse_sprites[i].setScale(0.5, 0.9);
		hud_ellipse_sprites[i].setColor(Color(255, 255, 255, 200));
		if (currentProgress < ((i + 1) * levelMax / 3.0f)) window.draw(hud_ellipse_sprites[i]);
	}
	window.draw(scoreSprite);
	window.draw(scoreType);
	window.draw(livesType);
	window.draw(multipleType);
	window.draw(scoreText);
	window.draw(menuText);
	window.draw(growthText);
	window.draw(livesSprite);
	window.draw(livesText);
	window.draw(multipleSprite);
	window.draw(multipleText);
	window.draw(abilityText);
	window.draw(abilityBarBackground);
	window.draw(abilityBar);
	window.draw(fish_frenzy_sprite);
}
