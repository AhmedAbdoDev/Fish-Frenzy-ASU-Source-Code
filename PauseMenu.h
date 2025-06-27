#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;
using namespace sf;

class PauseMenu
{
public:
	PauseMenu(RenderWindow& window);

	void update(Vector2f mousePos, RenderWindow& window);

	void render(RenderWindow& window, int scorenum, int highscorenum, int fisheat);

	bool isResumeClicked(Vector2f mousePos);
	bool isQuitClicked(Vector2f mousePos);
	bool option(Vector2f mousePos);

	Text continu;
	Text end;
	Text options;

	Texture cersor_;

	Sprite Puase;
	Sprite cersor;
	Sprite cersor2;
	Sprite cersor4;
	Sprite fishFerenzy;

	Text score;
	Text highscore;
	Text fisheaten;
private:
	Font font;
	Font font2;

	Text standard;


	Texture puase;
	Texture cersor2_;
	Texture cersor4_;
	Texture fishferenzy;
};

#endif
