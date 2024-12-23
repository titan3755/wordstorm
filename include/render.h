#pragma once

#ifndef RENDER_H
#define RENDER_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <word.h>
#include <SDL.h>
#undef main
#include <SDL_ttf.h>
#include <SDL_image.h>

// this class will be used to setup and render different components of the game
class Render {
public:
	Render(SDL_Window* window, SDL_Renderer* renderer, int scrn_width, int scrn_height);
	~Render();

	// setup functions for different components
	void setupScreenHorizontalDivider();
	void setupUpperScreenLeftmostVerticalDivider();
	void setupUpperScreenBackground();
	void setupLowerScreenBackground();
	void setupScoreText(TTF_Font* font, std::string text, int x, int y);
	void setupTimerText(TTF_Font* font, std::string text, int x, int y);
	// a setup function which creates the textures for every english alphabet letter and there will be two sets of
	// textures, one for the normal state and one for the moment when that specific key is pressed
	// the sets of textures will be stored in a map and each value will be an array of SDL_Texture pointers
	// the map will be stored in the private section of the class
	// when in normal state, the color of the alphabet letter will be white and when pressed, the color will be green
	// the function will take the font, the normal color and the pressed color as arguments
	void setupAlphabetTextures(TTF_Font* font);
	// a setup function which will create the positions of the alphabet letters on the screen
	// the function will take the width and height of the screen and the number of columns and rows
	// the function will create the positions of the alphabet letters on the screen and store them in a map
	// the map will be stored in the private section of the class
	// the key will be the alphabet letter and the value will be an SDL_Rect pointer
	void setupAlphabetPositions();
	// a setup function which will create the states of the alphabet letters
	// the function will take the alphabet letters as an argument and create the states of the alphabet letters
	// the states will be stored in a map and the key will be the alphabet letter and the value will be a boolean
	// the map will be stored in the private section of the class
	// the state will be false if the key is not pressed and true if the key is pressed
	void setupAlphabetStates(char keys[], int size);
	// a setup function which will create the words shooting from the top portion of the screen and across it
	// the function will take a word as an argument and create the textures and positions of the word
	// the word will have a velocity, position, texture
	// all of the above except the texture and position will be random within a certain range
	// the texture will be generated as needed and the position will be somewhere outside the screen
	// the angle will be such that the word will move across the viewable screen
	void setupWord(std::string word);

	// update functions for different components
	void updateScoreText(std::string text);
	void updateScoreColor(SDL_Color color);
	void updateScorePosition(int x, int y);
	void updateTimerText(std::string text);
	void updateTimerColor(SDL_Color color);
	void updateTimerPosition(int x, int y);
	// an update function which will update the state of the alphabet letters
	// the argument will be the key which is pressed and the state of that key will be updated
	// the rest of the keys will be updated to the normal state
	// note: if more than one key is being pressed, change the state accordingly
	// note: if more than one letter, then take an array of characters as an argument
	// note: the array of characters refers to the keys which are pressed
	void updateAlphabetStates(char key);
	// overload the above function to take an array of characters as an argument
	void updateAlphabetStates(char keys[], int size);
	// another overload which takes a map as an argument
	// the map will have the key as the alphabet letter and the value as a boolean
	void updateAlphabetStates(std::map<char, bool> states);
	// a function to update the positions of the alphabet letters on the screen
	// the argument will be a map which will have the key as the alphabet letter and the value as an SDL_Rect pointer
	// the function will update the positions of the alphabet letters on the screen
	void updateAlphabetPositions(std::map<char, SDL_Rect*> positions);
	// a function to update the words shooting from the top portion of the screen and across it
	// the word will have a velocity, position, texture
	// the velocity and position will be updated in this function
	void updateWords(float dt);

	// cleanup functions for different components
	void cleanupScreenHorizontalDivider();
	void cleanupUpperScreenBackground();
	void cleanupLowerScreenBackground();
	void cleanupScoreText();
	void cleanupTimerText();
	// a cleanup function which will cleanup the textures of the alphabet letters
	// the function will take the map of the textures and cleanup all the textures
	void cleanupAlphabetTextures();
	// a cleanup function which will cleanup the positions of the alphabet letters
	// the function will take the map of the positions and cleanup all the positions
	void cleanupAlphabetPositions();
	// a cleanup function which will cleanup the states of the alphabet letters
	// the function will take the map of the states and cleanup
	void cleanupAlphabetStates();

	// render functions for different components (will be used in the game loop)
	void renderScreenHorizontalDivider();
	void renderUpperScreenLeftmostVerticalDivider();
	void renderUpperScreenBackground();
	void renderLowerScreenBackground();
	void renderScoreText();
	void renderTimerText();
	// a render function which will render the alphabet letters on the screen
	// the function will take the current state of the alphabet letters and render them accordingly
	// if the state is normal, the normal texture will be rendered and if the state is pressed, the pressed texture will be rendered
	void renderAlphabetLetters();
	// a render function which will render the words shooting from the top portion of the screen and across it
	// the function will take the word and render it on the screen
	void renderWords();

	// alphabet properties getters
	std::map<char, std::vector<SDL_Texture*>> getAlphabetTextures() const;
	std::map<char, SDL_Rect*> getAlphabetPositions() const;
	std::map<char, bool> getAlphabetStates() const;

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	int scrn_width;
	int scrn_height;
	SDL_Rect* horizontal_divider;
	SDL_Rect* upper_screen_background;
	SDL_Rect* lower_screen_background;
	SDL_Color* horizontal_divider_color;
	SDL_Rect* upper_screen_leftmost_vertical_divider;
	SDL_Color* upper_screen_leftmost_vertical_divider_color;
	SDL_Color* upper_screen_background_color;
	SDL_Color* lower_screen_background_color;
	TTF_Font* score_text_font;
	SDL_Texture* score_text_texture;
	SDL_Rect score_text_rect;
	SDL_Color score_text_color;
	std::string score_text;
	TTF_Font* timer_text_font;
	SDL_Texture* timer_text_texture;
	SDL_Rect timer_text_rect;
	SDL_Color timer_text_color;
	std::string timer_text;
	// a map to store the textures of the alphabet letters
	// the key will be the alphabet letter and the value will be an array of SDL_Texture pointers
	// the array will have two elements, one for the normal state and one for the pressed state
	std::map<char, std::vector<SDL_Texture*>> alphabet_textures;
	// two colors for the alphabet letters, one for the normal state and one for the pressed state
	SDL_Color normal_alphabet_color;
	SDL_Color pressed_alphabet_color;
	// a map to store the positions of the alphabet letters on the screen
	// the key will be the alphabet letter and the value will be an SDL_Rect pointer
	std::map<char, SDL_Rect*> alphabet_positions;
	// a map to store the state of the alphabet letters, whether they are pressed or not
	// the key will be the alphabet letter and the value will be a boolean
	std::map<char, bool> alphabet_states;
	// a map to store the words shooting from the top portion of the screen and across it
	// the words will be an object of a class stored in a vector
	std::vector<Word*> words;
};

#endif // RENDER_H