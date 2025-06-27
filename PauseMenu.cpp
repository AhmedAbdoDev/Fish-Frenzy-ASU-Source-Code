#include "PauseMenu.h"

using namespace std;
using namespace sf;

PauseMenu::PauseMenu(RenderWindow &window)
{

	/*const int width = window.getSize().x;
	const int height = window.getSize().y;*/
	const int width = window.getSize().x;
	const int height = window.getSize().y;
	font.loadFromFile("./assets/fonts/BigSpace-rPKx.ttf");
	font2.loadFromFile("./assets/fonts/Roboto-Bold.ttf");

	puase.loadFromFile("./assets/optionsFrame.png");
	Puase.setTexture(puase);
	Puase.setOrigin(Puase.getGlobalBounds().width / 2, Puase.getGlobalBounds().height / 2);
	Puase.setScale(1.5, 1.5);
	Puase.setPosition(width / 2, height / 2);

	standard.setFont(font);
	standard.setString("Standard  ");
	standard.setCharacterSize(50);
	standard.setFillColor(Color(240, 250, 250));
	standard.setOrigin(standard.getGlobalBounds().width / 2, standard.getGlobalBounds().height / 2);
	standard.setPosition(width / 2, height * 2.3 / 7);
	standard.setScale(1.3, 1.3);

	score.setFont(font2);
	score.setCharacterSize(50);
	score.setFillColor(Color(240, 250, 250));
	score.setOrigin(score.getGlobalBounds().width / 2, score.getGlobalBounds().height / 2);
	score.setPosition(width / 2.6, height * 3 / 7);
	score.setScale(0.9, 0.9);

	highscore.setFont(font2);
	highscore.setCharacterSize(50);
	highscore.setFillColor(Color(240, 250, 250));
	highscore.setOrigin(highscore.getGlobalBounds().width / 2, highscore.getGlobalBounds().height / 2);
	highscore.setPosition(width / 2.6, height * 3.65 / 7);
	highscore.setScale(0.9, 0.9);

	fisheaten.setFont(font2);
	fisheaten.setCharacterSize(50);
	fisheaten.setFillColor(Color(240, 250, 250));
	fisheaten.setOrigin(fisheaten.getGlobalBounds().width / 2, fisheaten.getGlobalBounds().height / 2);
	fisheaten.setPosition(width / 2.6, height * 4.3 / 7);
	fisheaten.setScale(0.9, 0.9);

	cersor_.loadFromFile("./assets/cersor_1.png");
	cersor.setTexture(cersor_);
	cersor.setOrigin(cersor.getGlobalBounds().width / 2, cersor.getGlobalBounds().height / 2);
	cersor.setPosition(width / 2, height * 5.4 / 7);
	cersor.setScale(2, 2);

	cersor2_.loadFromFile("./assets/cersor_4.png");
	cersor2.setTexture(cersor2_);
	cersor2.setOrigin(cersor2.getGlobalBounds().width / 2, cersor2.getGlobalBounds().height / 2);
	cersor2.setPosition(width * 2.03 / 3.15, height * 5.55 / 7);
	cersor2.setScale(1.7, 1.7);

	cersor4_.loadFromFile("./assets/cersor_3.png");
	cersor4.setTexture(cersor4_);
	cersor4.setOrigin(cersor4.getGlobalBounds().width / 2, cersor4.getGlobalBounds().height / 2);
	cersor4.setPosition(width / 2.8, height * 5.55 / 7);
	cersor4.setScale(1.7, 1.7);

	fishferenzy.loadFromFile("./assets/fish_ferenzy.png");
	fishFerenzy.setTexture(fishferenzy);
	fishFerenzy.setOrigin(fishFerenzy.getGlobalBounds().width / 2, fishFerenzy.getGlobalBounds().height / 2);
	fishFerenzy.setPosition(width / 2, height * 1.5 / 7);
	fishFerenzy.setScale(2, 2);

	continu.setFont(font);
	continu.setString("Continue");
	continu.setCharacterSize(50);
	continu.setFillColor(Color(50, 80, 100));
	continu.setOrigin(continu.getGlobalBounds().width / 2, continu.getGlobalBounds().height / 2);
	continu.setPosition(width / 2, height * 5.25 / 7);
	continu.setScale(1, 1);

	end.setFont(font);
	end.setString("Main Menu");
	end.setCharacterSize(45);
	end.setFillColor(Color(240, 250, 250));
	end.setOrigin(continu.getGlobalBounds().width / 2, continu.getGlobalBounds().height / 2);
	end.setPosition(width * 2.03 / 3.16, height * 5.45 / 7);
	end.setScale(.7, .7);

	options.setFont(font);
	options.setString("Options");
	options.setCharacterSize(50);
	options.setFillColor(Color(240, 250, 250));
	options.setOrigin(continu.getGlobalBounds().width / 2, continu.getGlobalBounds().height / 2);
	options.setPosition(width / 2.8, height * 5.45 / 7);
	options.setScale(.7, .7);
}

void PauseMenu::update(Vector2f mousePos, RenderWindow &window)
{

	if (cersor.getGlobalBounds().contains(mousePos))
	{
		cersor_.loadFromFile("./assets/cersor_2.png");
	}
	else
		cersor_.loadFromFile("./assets/cersor_1.png");

	if (cersor2.getGlobalBounds().contains(mousePos))
	{
		end.setFillColor(Color(200, 200, 250));
	}
	else
		end.setFillColor(Color(240, 250, 250));

	if (cersor4.getGlobalBounds().contains(mousePos))
	{
		options.setFillColor(Color(200, 200, 250));
	}
	else
		options.setFillColor(Color(240, 250, 250));
}

void PauseMenu::render(RenderWindow &window, int scorenum, int highscorenum, int fisheat)
{
	score.setString("Your score : " + to_string(scorenum));
	highscore.setString("High Score : " + to_string(highscorenum));
	fisheaten.setString("Fish Eaten : " + to_string(fisheat));
	window.draw(Puase);
	window.draw(cersor);
	window.draw(cersor2);
	window.draw(cersor4);
	window.draw(end);
	window.draw(options);
	window.draw(continu);
	window.draw(fishFerenzy);
	window.draw(score);
	window.draw(fisheaten);
	window.draw(highscore);
	window.draw(standard);
}
bool PauseMenu::isResumeClicked(Vector2f mousePos)
{
	return cersor.getGlobalBounds().contains(mousePos);
}

bool PauseMenu::isQuitClicked(Vector2f mousePos)
{
	return cersor2.getGlobalBounds().contains(mousePos);
}

bool PauseMenu::option(Vector2f mousePos)
{
	return cersor4.getGlobalBounds().contains(mousePos);
}
