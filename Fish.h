#pragma once  
#include <SFML/Graphics.hpp> 
#include <cstdint>

class Fish {
	// ===== [SECTION 1: PRIVATE MEMBERS] =====
private:
	float angle = 0;
	// textures of fish
	sf::Texture fish_texture;
	sf::Sprite* Background;
	bool isOutscreen = 1;
	// the speed that fish moves
	uint8_t speed = 65.0f;

	// directions of moving
	sf::Vector2i X_Y_direction;

	float change_direction_timer = 0;
	//Animation variables groups
	int8_t current_frame = 0;
	float timer = 0, deltatime = 0, returning_timer = 0;
	bool escaping = 0;

	//scale eating factor
	sf::Vector2f eat_scale = { 1,1 };

	//the are dimensions that AI fish will swim in
	sf::Vector2f work_area;



	//fish floating rectangle
	float fish_widthSize = 0, fish_heightSize = 0;
	int8_t fish_column = 0, fish_row = 0;


	// testttttt
	//animation check 
	int8_t returning_frame = 0;


	void swimming();
	void eating();
	void eated();
	void returning();

	// ===== [SECTION 2: making fish creature] ===== 
public:
	sf::Sprite sprite;
	sf::RectangleShape mouth;

	int8_t Eated_frame = 0, Eating_frame = 0;

	//determine the type of the fish 
	enum class type {
		MINNOW = 2,
		SURGEON_FISH = 3,
		LION_FISH = 4,
	};
	//creates new fish by the constructor
	Fish();

	//fish type to determine from the type constants
	Fish::type current_type = type::MINNOW;


	void  init(Fish::type type, sf::Texture& background, sf::Sprite& background_sprite);

	// ===== [SECTION 3: GAMEPLAY METHODS] =====
		/*
		 * update handling the fish logic
		 * @param deltaTime Time since last frame (seconds)
		 * @param background x size for the window x size
		 * @param background_y_size for the window y size
		 *
		 * swimming makes the swimming animation
		 * returning makes the changing direction animation
		 * eating is for if the fish eates smaller fish
		 * eated is for if the fish has been eated from a bigger fish
		 */

	void update(float deltaTime);
	void idle();


	// ===== [SECTION 5: ACCESSORS] =====
		/**
		 * Gets current position
		 * @return Vector2f(x,y) position
		 */


	void setPosition(bool isstart);
	void setDirection();
	int randPosition(int modulo);
	void isEating();
	void isReturning();
	void isEscaping();
	void isEated();
	bool isinBackground();
	sf::FloatRect getGlobalBounds();

};