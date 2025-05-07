#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;
using namespace sf;

class GameOver {
public:

	GameOver(RenderWindow& window);

	void update(Vector2f mousePos, RenderWindow& window);

	void render(RenderWindow& window, int scorenum);

	bool continueClicked(Vector2f mousePos);
	bool QuitClicked(Vector2f mousePos);

private:
	Font font;
	Font font2;
	Font font3;

	Text Qite;
	Text contine;
	Text quote;
	Text gameovertext;
	Text quote2;
	Text score;

	Texture gameoverbackg;
	Texture cersor_;
	Texture cersor2_;
	Texture fishferenzy;

	Sprite Gameoverbackg;
	Sprite cersor;
	Sprite cersor2;
	Sprite fishFerenzy;

};

#endif

