#pragma once
#pragma once

#ifndef WORD_H
#define WORD_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // for rand() and srand()
#include <SDL.h>
#undef main
#include <SDL_ttf.h>
#include <SDL_image.h>

// this class will be used to setup and render the words
class Word {
public:
	Word(SDL_Renderer* renderer, TTF_Font* font, std::string word, int scrn_width, int scrn_height);
	~Word();

	// setup will be done in the constructor
	void setupWord();
	// update functions for the word
	void updateWord(float dt);
	// render functions for the word
	void renderWord();
	// cleanup functions for the word
	void cleanupWord();
	// getter functions for the word
	std::string getWord();
	SDL_Rect* getWordPosition();
	int getWordVelocity();
	SDL_Texture* getWordTexture();
private:
	// the word will have a texture, position, velocity
	// the texture will be generated as needed and the position will be somewhere outside the screen
	// the velocity will be random within a certain range
	std::string word;
	SDL_Texture* texture;
	SDL_Rect position;
	int velocity;
	int scrn_width;
	int scrn_height;
	SDL_Renderer* renderer;
	TTF_Font* font;
};


#endif // WORD_H