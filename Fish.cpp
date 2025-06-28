#include "Fish.h"
#include <cmath>
#include <iostream>

//==initialization==//
Fish::Fish() {};

void Fish::init(Fish::type type, sf::Texture background_texture, sf::Sprite& background_sprite)
{
	//setting working area
	work_area.x = background_texture.getSize().x * 3 + 100;
	work_area.y = background_texture.getSize().y * 2;
	Background = &background_sprite;
	// initialize the first position 
	setPosition((rand() % 10) > 5);

	// Set up sprite 
	current_type = type;

	switch (current_type) {
	case Fish::type::MINNOW:
		fish_texture.loadFromFile("./assets/fish_spritesheets/Minow.png");
		fish_widthSize = 2395 / 14.0f;
		fish_heightSize = 319 / 3.0f;
		sprite.setTextureRect(sf::IntRect(2, 1 * fish_heightSize + 1,
			fish_widthSize - 3, fish_heightSize - 1));
		break;

	case Fish::type::SURGEON_FISH:
		fish_texture.loadFromFile("./assets/fish_spritesheets/surgeon_fish.png");
		fish_widthSize = 2395 / 14.0f;
		fish_heightSize = 319 / 3.0f;
		sprite.setTextureRect(sf::IntRect(2, 1 * fish_heightSize + 1,
			fish_widthSize - 3, fish_heightSize - 1));
		break;

	case Fish::type::LION_FISH:
		fish_texture.loadFromFile("./assets/fish_spritesheets/lionfish.png");
		fish_row = 0;
		fish_widthSize = 992 / 15.0f;
		fish_heightSize = 99 / 2.0f;
		sprite.setTextureRect(sf::IntRect(2, fish_row * fish_heightSize + 1,
			fish_widthSize - 2, fish_heightSize - 1));
		break;
	}

	sprite.setTexture(fish_texture);
	sprite.setOrigin(sprite.getGlobalBounds().width / 2, sprite.getGlobalBounds().height / 2);
	idle();
}

// === [2. UPDATE METHOD] === //
void Fish::update(float dt) {
	deltatime = dt;

	if (sprite.getPosition().x >= work_area.x && isOutscreen) {
		setDirection();
		if (X_Y_direction.x == 1) setPosition(true);
		escaping = 0;
	}

	if (sprite.getPosition().x <= -100 && isOutscreen) {
		setDirection();
		if (X_Y_direction.x == -1) setPosition(false);
		escaping = 0;
	}

	if ((sprite.getPosition().x > 0 && sprite.getPosition().x < work_area.x - 100) || X_Y_direction.x == 0) {
		isOutscreen = true;
	}

	if (sprite.getPosition().y >= work_area.y) X_Y_direction.y = -1;
	if (sprite.getPosition().y <= work_area.y / 3) {
		X_Y_direction.y = 1;
	}

	if (!X_Y_direction.x) {
		setDirection();
	}

	if (!(X_Y_direction.x * int(current_type) * std::cos(angle) * speed * deltatime)) {
		std::cout << X_Y_direction.x << " " << '\n';
	}

	if (returning_frame > 0)returning();
	else if (Eating_frame > 0)eating();
	else if (Eated_frame > 0)eated();
	else swimming();

	if (!escaping) {

		change_direction_timer += deltatime;
		if (change_direction_timer >= rand() % 10 + 10 && isinBackground()) {
			if (rand() % 10 > 5) {
				isReturning();
				speed = 65.0f;
			}
			change_direction_timer = 0;
		}
	}
	sprite.move(
		X_Y_direction.x * int(current_type) * int(!(returning_frame > 0)) * std::cos(angle) * speed * deltatime,
		X_Y_direction.x * std::sin(angle) * X_Y_direction.y * speed * deltatime * int(current_type) * int(!(returning_frame > 0))
	);

	sprite.setScale(X_Y_direction.x * -0.45 * eat_scale.x * int(current_type),
		0.45 * int(current_type) * eat_scale.y);
}

// ===== [SECTION 3: GAMEPLAY METHODS] =====

void Fish::swimming() {
	timer += deltatime;
	fish_row = 1;

	if (timer >= 0.07f) {
		switch (current_type) {
		case Fish::type::MINNOW:
			fish_row = 0;
			fish_widthSize = 991 / 15.0f;
			fish_heightSize = 99 / 2.0f;
			sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
				fish_row * fish_heightSize + 1, fish_widthSize - 3, fish_heightSize - 1));
			fish_column = (fish_column + 1) % 15;
			break;

		case Fish::type::SURGEON_FISH:
			fish_widthSize = 2395 / 14.0f;
			fish_heightSize = 319 / 3.0f;
			sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
				fish_row * fish_heightSize + 1, fish_widthSize - 3, fish_heightSize - 1));
			fish_column = (fish_column + 1) % 14;
			break;

		case Fish::type::LION_FISH:
			fish_row = 2;
			fish_widthSize = 2535 / 14.0f;
			fish_heightSize = 645 / 4.0f;
			sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
				fish_row * fish_heightSize + 1, fish_widthSize - 2, fish_heightSize - 1));
			fish_column = (fish_column + 1) % 14;
			break;
		}
		timer = 0;
	}
}

void Fish::idle() {
	timer += deltatime;

	switch (current_type) {
	case Fish::type::MINNOW:
		fish_widthSize = 2395 / 14.0f;
		fish_heightSize = 319 / 3.0f;
		sprite.setTextureRect(sf::IntRect(0 * fish_widthSize + 2,
			1 * fish_heightSize + 1, fish_widthSize - 3, fish_heightSize - 1));
		break;

	case Fish::type::SURGEON_FISH:
		fish_widthSize = 2395 / 14.0f;
		fish_heightSize = 319 / 3.0f;
		sprite.setTextureRect(sf::IntRect(0 * fish_widthSize + 2,
			1 * fish_heightSize + 1, fish_widthSize - 3, fish_heightSize - 1));
		break;

	case Fish::type::LION_FISH:
		fish_row = 0;
		fish_widthSize = 991 / 15.0f;
		fish_heightSize = 99 / 2.0f;
		if (timer >= 0.1f) {
			sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
				fish_row * fish_heightSize + 1, fish_widthSize - 2, fish_heightSize - 1));
			fish_column = (fish_column + 1) % 6;
			timer = 0;
		}
		break;
	}
}

void Fish::returning() {
	timer += deltatime;


	if (timer >= 0.06f) {
		switch (current_type) {
		case Fish::type::MINNOW:
			fish_row = 1;
			fish_column = 7 - returning_frame;
			fish_widthSize = 991 / 15.0f;
			fish_heightSize = 99 / 2.0f;
			sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
				fish_row * fish_heightSize + 1, fish_widthSize - 3, fish_heightSize - 2));
			break;

		case Fish::type::SURGEON_FISH:
			fish_column = 5 - returning_frame;
			fish_row = 2;
			fish_widthSize = 2395 / 14.0f;
			fish_heightSize = 319 / 3.0f;
			sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
				fish_row * fish_heightSize + 1, fish_widthSize - 3, fish_heightSize - 1));
			break;

		case Fish::type::LION_FISH:
			fish_column = 5 - returning_frame;
			fish_row = 3;
			fish_widthSize = 2535 / 14.0f;
			fish_heightSize = 645 / 4.0f;
			sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
				fish_row * fish_heightSize + 1, fish_widthSize - 2, fish_heightSize - 1));
			break;

		}

		returning_frame--;
		if (returning_frame == 0)X_Y_direction.x *= -1;
		timer = 0;
	}
}

void Fish::eating() {
	timer += deltatime;
	fish_row = 0;

	if (timer >= 0.05f) {
		switch (current_type) {
		case Fish::type::SURGEON_FISH:
			fish_column = 5 - Eating_frame;
			fish_widthSize = 2395 / 14.0f;
			fish_heightSize = 319 / 3.0f;
			sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
				fish_row * fish_heightSize + 1, fish_widthSize - 3, fish_heightSize - 1));
			Eating_frame--;
			break;

		case Fish::type::LION_FISH:
			fish_column = 6 - Eating_frame;
			fish_widthSize = 2535 / 14.0f;
			fish_heightSize = 645 / 4.0f;
			sprite.setTextureRect(sf::IntRect(fish_column * fish_widthSize + 2,
				fish_row * fish_heightSize + 1, fish_widthSize - 2, fish_heightSize - 1));
			Eating_frame--;
			break;

		default:
			break;
		}
		timer = 0;
	}
}

void Fish::eated() {
	timer += deltatime;

	if (timer >= 0.08f) {
		timer = 0.0f;
		if (Eated_frame > 1) {
			eat_scale.x *= 0.5f;
			eat_scale.y *= 0.5f;

			sf::Color color = sprite.getColor();
			color.a = (color.a * 0.7f);
			sprite.setColor(color);
		}
		else {
			sprite.setColor(sf::Color::White);
			setPosition(rand() % 2);
			eat_scale = { 1, 1 };
		}
		Eated_frame--;
	}
	/*sprite.setColor(sf::Color::White);
	setPosition(rand() % 2);
	eat_scale = { 1, 1 };*/
}

// ===== [SECTION 5: ACCESSORS] =====

void Fish::setPosition(bool isstart) {
	if (isstart) {
		sprite.setPosition(-100, randPosition(int(work_area.y)) + work_area.y / 3);
	}
	else {
		sprite.setPosition(work_area.x, randPosition(int(work_area.y)) + work_area.y / 3);
	}

	speed = 65.0f;
	angle = randPosition(30) * 3.14159265f / 180.0f;
}

void Fish::setDirection() {
	X_Y_direction.x = (rand() % 3) - 1;
	X_Y_direction.y = (rand() % 3) - 1;
	speed = 65.0f;
	if (!X_Y_direction.x) {
		setDirection();
	}
}

int Fish::randPosition(int modulo) {
	return ((rand() % modulo) * (rand() % modulo) +
		(rand() % modulo) * (rand() % modulo) -
		(rand() % modulo) * (rand() % modulo)) % modulo;
}

void Fish::isEating() {
	switch (current_type) {
	case Fish::type::SURGEON_FISH: Eating_frame = 5; break;
	case Fish::type::LION_FISH: Eating_frame = 6; break;
	default: break;
	}
	returning_frame = 0;
}

void Fish::isReturning() {
	switch (current_type) {
	case Fish::type::MINNOW: returning_frame = 7; break;
	case Fish::type::SURGEON_FISH:
	case Fish::type::LION_FISH: returning_frame = 5; break;
	}
	Eating_frame = 0;
}

void Fish::isEscaping() {
	isReturning();
	speed = 200;
	escaping = 1;
}

void Fish::isEated() {
	Eating_frame = 0;
	Eated_frame = 4;
	returning_frame = 0;
}

bool Fish::isinBackground() {
	return sprite.getGlobalBounds().intersects(Background->getGlobalBounds());
}

sf::FloatRect Fish::getGlobalBounds() {
	if (current_type == Fish::type::MINNOW)
		return sprite.getGlobalBounds();
	sf::FloatRect globalBounds = sprite.getGlobalBounds();
	globalBounds.left += globalBounds.left / 12;
	globalBounds.top += globalBounds.top / 12;
	globalBounds.height -= globalBounds.height / 2.5;
	globalBounds.width -= globalBounds.width / 2.5;
	return globalBounds;
}