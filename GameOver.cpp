#include "GameOver.h"

using namespace std;
using namespace sf;

GameOver::GameOver(RenderWindow& window) {

	const int width = window.getSize().x;
	const int height = window.getSize().y;

	font.loadFromFile("./assets/fonts/BigSpace-rPKx.ttf");
	font2.loadFromFile("./assets/fonts/ArcadeClassic.ttf");
	font3.loadFromFile("./assets/fonts/AppleGaramond.ttf");

	Qite.setFont(font2);
	Qite.setString("Quit");
	Qite.setCharacterSize(50);
	Qite.setFillColor(Color(50, 80, 100));
	Qite.setOrigin(Qite.getGlobalBounds().width / 2, Qite.getLocalBounds().height / 2);
	Qite.setPosition(width / 3, height * 5 / 7.3);
	Qite.setScale(1.5, 1.5);


	contine.setFont(font2);
	contine.setString("Continue");
	contine.setCharacterSize(50);
	contine.setFillColor(Color(50, 80, 100));
	contine.setOrigin(contine.getGlobalBounds().width / 2, contine.getLocalBounds().height / 2);
	contine.setPosition(width * 2 / 3, height * 5 / 7.17);
	contine.setScale(1, 1);

	gameovertext.setFont(font);
	gameovertext.setString("GAME OVER");
	gameovertext.setCharacterSize(50);
	gameovertext.setFillColor(Color::White);
	gameovertext.setOrigin(gameovertext.getGlobalBounds().width / 2, gameovertext.getLocalBounds().height / 2);
	gameovertext.setPosition(width * 1.01 / 2, height * 2.3 / 7);
	gameovertext.setScale(1.8, 1.8);

	score.setFont(font3);
	score.setCharacterSize(50);
	score.setFillColor(Color::White);
	score.setOrigin(score.getGlobalBounds().width / 2, score.getLocalBounds().height / 2);
	score.setPosition(width / 2.2, height * 3 / 7);
	score.setScale(0.8, 0.8);

	quote.setFont(font3);
	quote.setString("Never stop trying");
	quote.setCharacterSize(50);
	quote.setFillColor(Color::White);
	quote.setOrigin(quote.getGlobalBounds().width / 2, quote.getLocalBounds().height / 2);
	quote.setPosition(width / 2, height * 3.5 / 7);
	quote.setScale(0.8, 0.8);

	quote2.setFont(font3);
	quote2.setString("Do you want to Continue and try again? ");
	quote2.setCharacterSize(50);
	quote2.setFillColor(Color::White);
	quote2.setOrigin(quote2.getGlobalBounds().width / 2, quote2.getLocalBounds().height / 2);
	quote2.setPosition(width / 2, height * 4 / 7);
	quote2.setScale(0.6, 0.6);

	gameoverbackg.loadFromFile("./assets/mainMenuBG.png");
	Gameoverbackg.setTexture(gameoverbackg);
	Gameoverbackg.setOrigin(Gameoverbackg.getGlobalBounds().width / 2, Gameoverbackg.getLocalBounds().height / 2);
	Gameoverbackg.setScale(3, 3);
	Gameoverbackg.setPosition(width / 2, height / 2);

	cersor_.loadFromFile("./assets/cersor_1.png");
	cersor.setTexture(cersor_);
	cersor.setOrigin(cersor.getGlobalBounds().width / 2, cersor.getGlobalBounds().height / 2);
	cersor.setPosition(width / 3, height * 5 / 7);
	cersor.setScale(2, 2);

	cersor2_.loadFromFile("./assets/cersor_1.png");
	cersor2.setTexture(cersor2_);
	cersor2.setOrigin(cersor2.getGlobalBounds().width / 2, cersor2.getGlobalBounds().height / 2);
	cersor2.setPosition(width * 2 / 3, height * 5 / 7);
	cersor2.setScale(2, 2);

	fishferenzy.loadFromFile("./assets/fish_ferenzy.png");
	fishFerenzy.setTexture(fishferenzy);
	fishFerenzy.setOrigin(fishFerenzy.getGlobalBounds().width / 2, fishFerenzy.getGlobalBounds().height / 2);
	fishFerenzy.setPosition(width / 2, height / 7);
	fishFerenzy.setScale(2, 2);

}

void GameOver::update(Vector2f mousePos, RenderWindow& window) {

	if (cersor.getGlobalBounds().contains(mousePos)) {
		cersor_.loadFromFile("./assets/cersor_2.png");
	}
	else
		cersor_.loadFromFile("./assets/cersor_1.png");
	if (cersor2.getGlobalBounds().contains(mousePos)) {
		cersor2_.loadFromFile("./assets/cersor_2.png");
	}
	else
		cersor2_.loadFromFile("./assets/cersor_1.png");

}

void GameOver::render(RenderWindow& window, int scorenum) {
	score.setString("Your Score : " + to_string(scorenum));
	window.draw(Gameoverbackg);
	window.draw(cersor);
	window.draw(fishFerenzy);
	window.draw(cersor2);
	window.draw(Qite);
	window.draw(contine);
	window.draw(gameovertext);
	window.draw(quote);
	window.draw(quote2);
	window.draw(score);


}

bool GameOver::continueClicked(Vector2f mousePos) {
	return cersor2.getGlobalBounds().contains(mousePos);
}

bool GameOver::QuitClicked(Vector2f mousePos) {
	return cersor.getGlobalBounds().contains(mousePos);
}