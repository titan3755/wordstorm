#pragma once

#ifndef ANIMATION_H
#define ANIMATION_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // for rand() and srand()
#include <SDL.h>
#undef main
#include <SDL_ttf.h>
#include <SDL_image.h>

// this class will be used to setup and render the animations
// the animation will take a word, initial font size and its position as an argument
// the animation will be a simple one, where the word will shrink in size and disappear
// initially, the color of the word will be set to green
// the animation will simply consist of smaller and smaller font sizes of the word
// along with smaller and smaller font sizes, the word will also have a smaller and smaller opacity
class Animation {
public:
	Animation(SDL_Renderer* renderer, TTF_Font* font, SDL_Color init_clr, std::string word, int font_size, SDL_Rect position);
	~Animation();

	// setup will be done in the constructor
	void setupAnimation();
	// update functions for the animation
	void updateAnimation(float dt);
	// render functions for the animation
	void renderAnimation();
	// cleanup functions for the animation
	void cleanupAnimation();
	// getter functions for the animation
	SDL_Color* getInitColor();
	int getOpacity();
	std::string getWord();
	SDL_Rect* getWordPosition();
	int getWordFontSize();
	SDL_Texture* getWordTexture();
private:
	// the animation will have a texture, position, font size, opacity
	// the texture will be generated as needed and the position will be the same as the initial position
	// the font size will be smaller and smaller
	// the opacity will be smaller and smaller
	SDL_Color init_color;
	std::string word;
	SDL_Texture* texture;
	SDL_Rect position;
	int font_size;
	int opacity;
	SDL_Renderer* renderer;
	TTF_Font* font;
};

#endif // ANIMATION_H
