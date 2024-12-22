#include <iostream>
#include <application.h>
#include <SDL.h>
#undef main // SDL defines main, so we need to undefine it
#include <SDL_ttf.h>
#include <SDL_image.h>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr char BASE_TITLE[] = "WordStorm";

int main(int argc, char* argv[]) {
	Application app(SCREEN_WIDTH, SCREEN_HEIGHT, BASE_TITLE);
	app.init();
	app.run();
	/*app.cleanup();*/
	// cleanup will be done automatically when the application object goes out of scope
	return 0;
}