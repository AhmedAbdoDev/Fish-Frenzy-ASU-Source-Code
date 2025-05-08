#include "options.h"

using namespace std;
using namespace sf;

/*
	bubblepressed[0]-----> sound
	bubblepressed[1]-----> music
	bubblepressed[2]-----> fullscreen
	bubblepressed[3]-----> helpbubbles
	bubblepressed[4]-----> slow speed
	bubblepressed[5]-----> duble slow speed
	bubblepressed[6]-----> mid fast speed
	bubblepressed[7]-----> fast speed
*/

options::options(RenderWindow &window)
{

	const int width = window.getSize().x;
	const int height = window.getSize().y;

	font.loadFromFile("./assets/fonts/BigSpace-rPKx.ttf");
	font2.loadFromFile("./assets/fonts/AppleGaramond.ttf");
	font3.loadFromFile("./assets/fonts/Roboto-Italic.ttf");

	cersor_.loadFromFile("./assets/cersor_1.png");
	cersor.setTexture(cersor_);
	cersor.setOrigin(cersor.getGlobalBounds().width / 2, cersor.getGlobalBounds().height / 2);
	cersor.setPosition(width / 2, height * 5.4 / 7);
	cersor.setScale(2, 2);

	cersor3_.loadFromFile("./assets/cersor_2.png");
	cersor3.setTexture(cersor3_);
	cersor3.setOrigin(cersor3.getGlobalBounds().width / 2, cersor3.getGlobalBounds().height / 2);
	cersor3.setPosition(width / 2, height * 5.4 / 7);
	cersor3.setScale(2, 2);

	optionsbackg.loadFromFile("./assets/options.png");
	Optionsbackg.setTexture(optionsbackg);
	Optionsbackg.setOrigin(Optionsbackg.getGlobalBounds().width / 2, Optionsbackg.getLocalBounds().height / 2);
	Optionsbackg.setScale(1.5, 1.5);
	Optionsbackg.setPosition(width / 2, height / 2);

	fishferenzy.loadFromFile("./assets/fish_ferenzy.png");
	fishFerenzy.setTexture(fishferenzy);
	fishFerenzy.setOrigin(fishFerenzy.getGlobalBounds().width / 2, fishFerenzy.getGlobalBounds().height / 2);
	fishFerenzy.setPosition(width / 2, height * 1.5 / 7);
	fishFerenzy.setScale(2, 2);

	for (int i = 0; i < 8; i++)
	{
		bubb1[i].loadFromFile("./assets/babble_2.png");
		Bubb1[i].setTexture(bubb1[i]);
		Bubb1[i].setOrigin(Bubb1[i].getGlobalBounds().width / 2, Bubb1[i].getGlobalBounds().height / 2);
		Bubb1[i].setPosition(width / 1.8, height * 2.4 / 7 + i * 73);
		Bubb1[i].setScale(1.5, 1.5);
		if (i >= 4)
		{
			Bubb1[i].setPosition(width / 1.8 + 50 * (i - 4), height * 2.4 / 7 + 4 * 73);
			if (i == 4 || i == 5 || i == 7)
			{
				bubb1[i].loadFromFile("./assets/babble_1.png");
			}
		}
	}

	slow.setFont(font2);
	slow.setString("slow");
	slow.setCharacterSize(30);
	slow.setFillColor(Color(240, 250, 250));
	slow.setOrigin(slow.getGlobalBounds().width / 2, slow.getLocalBounds().height / 2);
	slow.setPosition(width / 1.8, height * 2.4 / 7 + 4 * 73 + 30);

	fast.setFont(font2);
	fast.setString("fast");
	fast.setCharacterSize(30);
	fast.setFillColor(Color(240, 250, 250));
	fast.setOrigin(fast.getGlobalBounds().width / 2, fast.getLocalBounds().height / 2);
	fast.setPosition(width / 1.8 + 150, height * 2.4 / 7 + 4 * 73 + 30);

	done.setFont(font);
	done.setString("Done");
	done.setCharacterSize(50);
	done.setFillColor(Color(50, 80, 100));
	done.setOrigin(done.getGlobalBounds().width / 2, done.getLocalBounds().height / 2);
	done.setPosition(width / 2, height * 5.25 / 7);
	done.setScale(1, 1);

	sound.setFont(font2);
	sound.setString("Sound");
	sound.setCharacterSize(50);
	sound.setFillColor(Color(240, 250, 250));
	sound.setOrigin(sound.getGlobalBounds().width / 2, sound.getLocalBounds().height / 2);
	sound.setPosition(width / 2.18, height * 2.3 / 7);
	sound.setScale(1, 1);

	music.setFont(font2);
	music.setString("Music");
	music.setCharacterSize(50);
	music.setFillColor(Color(240, 250, 250));
	music.setOrigin(music.getGlobalBounds().width / 2, music.getLocalBounds().height / 2);
	music.setPosition(width / 2.16, height * 2.3 / 7 + 73);
	music.setScale(1, 1);

	fullscreen.setFont(font2);
	fullscreen.setString("FullScreen");
	fullscreen.setCharacterSize(50);
	fullscreen.setFillColor(Color(240, 250, 250));
	fullscreen.setOrigin(fullscreen.getGlobalBounds().width / 2, fullscreen.getLocalBounds().height / 2);
	fullscreen.setPosition(width / 2.29, height * 2.3 / 7 + 2 * 73);
	fullscreen.setScale(1, 1);

	helpbubles.setFont(font2);
	helpbubles.setString("HelpBubles");
	helpbubles.setCharacterSize(50);
	helpbubles.setFillColor(Color(240, 250, 250));
	helpbubles.setOrigin(helpbubles.getGlobalBounds().width / 2, helpbubles.getLocalBounds().height / 2);
	helpbubles.setPosition(width / 2.317, height * 2.3 / 7 + 3 * 73);
	helpbubles.setScale(1, 1);

	mousespeed.setFont(font2);
	mousespeed.setString("MouseSpeed");
	mousespeed.setCharacterSize(50);
	mousespeed.setFillColor(Color(240, 250, 250));
	mousespeed.setOrigin(mousespeed.getGlobalBounds().width / 2, mousespeed.getLocalBounds().height / 2);
	mousespeed.setPosition(width / 2.333, height * 2.3 / 7 + 4 * 72);
	mousespeed.setScale(1, 1);
}

void options::update(Vector2f mousePos, RenderWindow &window, Event event)
{

	const int width = window.getSize().x;
	const int height = window.getSize().y;

	bool hover = Bubb1[1].getGlobalBounds().contains(mousePos);
	if (cersor.getGlobalBounds().contains(mousePos))
	{
		window.draw(cersor3);
		window.draw(done);
	}

	if (event.type == Event::MouseButtonPressed)
	{
		for (int i = 0; i < 8; i++)
		{
			if (Bubb1[i].getGlobalBounds().contains(mousePos))
			{
				if (bubblepressed[i] == 0)
				{
					bubb1[i].loadFromFile("./assets/babble_2.png");
					bubblepressed[i] = 1;
					if (i == 4)
					{
						bubb1[5].loadFromFile("./assets/babble_1.png");
						bubb1[6].loadFromFile("./assets/babble_1.png");
						bubb1[7].loadFromFile("./assets/babble_1.png");
						bubblepressed[5] = 0;
						bubblepressed[6] = 0;
						bubblepressed[7] = 0;
					}
					if (i == 5)
					{
						bubb1[4].loadFromFile("./assets/babble_1.png");
						bubb1[6].loadFromFile("./assets/babble_1.png");
						bubb1[7].loadFromFile("./assets/babble_1.png");
						bubblepressed[4] = 0;
						bubblepressed[6] = 0;
						bubblepressed[7] = 0;
					}
					if (i == 6)
					{
						bubb1[4].loadFromFile("./assets/babble_1.png");
						bubb1[5].loadFromFile("./assets/babble_1.png");
						bubb1[7].loadFromFile("./assets/babble_1.png");
						bubblepressed[4] = 0;
						bubblepressed[5] = 0;
						bubblepressed[7] = 0;
					}
					if (i == 7)
					{
						bubb1[4].loadFromFile("./assets/babble_1.png");
						bubb1[5].loadFromFile("./assets/babble_1.png");
						bubb1[6].loadFromFile("./assets/babble_1.png");
						bubblepressed[4] = 0;
						bubblepressed[5] = 0;
						bubblepressed[6] = 0;
					}
				}
				else if (bubblepressed[i] == 1)
				{
					bubb1[i].loadFromFile("./assets/babble_1.png");
					bubblepressed[i] = 0;
				}
			}
		}
	}
}

void options::render(RenderWindow &window)
{
	window.draw(Optionsbackg);
	window.draw(cersor);
	window.draw(done);
	window.draw(fishFerenzy);
	for (int i = 0; i < 8; i++)
	{
		window.draw(Bubb1[i]);
	}
	window.draw(slow);
	window.draw(fast);
	window.draw(sound);
	window.draw(music);
	window.draw(fullscreen);
	window.draw(helpbubles);
	window.draw(mousespeed);
}

bool options::doneClicked(Vector2f mousePos)
{
	return cersor.getGlobalBounds().contains(mousePos);
}
