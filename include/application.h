#pragma once

#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <fstream>
#include <render.h>
#include <word.h>
#include <string>
#include <SDL.h>
#undef main
#include <SDL_ttf.h>
#include <SDL_image.h>

class Application {
public:
	Application(int scrn_width, int scrn_height, std::string base_title);
	~Application();

	void init();
	void run();
	void cleanup();
	
private:
	int scrn_width;
	int scrn_height;
	std::string base_title;
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool running;
	int high_score;
};

#endif // APPLICATION_H