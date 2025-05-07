#pragma once

#ifndef OPTIONS_H
#define OPTIONS_H

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;
using namespace sf;

class options {
public:
	options(RenderWindow& window);

	void update(Vector2f mousePos, RenderWindow& window, Event event);

	void render(RenderWindow& window);

	bool doneClicked(Vector2f mousePos);
	bool bubblepressed[8] = { 1,1,1,1,0,0,1,0 };

private:
	Font font;
	Font font2;
	Font font3;

	Text done;
	Text sound;
	Text music;
	Text fullscreen;
	Text helpbubles;
	Text mousespeed;
	Text slow;
	Text fast;

	Texture optionsbackg;
	Texture cersor_;
	Texture cersor3_;
	Texture fishferenzy;
	Texture bubb1[8];

	Sprite Optionsbackg;
	Sprite cersor;
	Sprite cersor3;
	Sprite fishFerenzy;
	Sprite Bubb1[8];
};

#endif


