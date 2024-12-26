#include <word.h>

Word::Word(SDL_Renderer* renderer, TTF_Font* font, std::string word, int scrn_width, int scrn_height) {
	this->renderer = renderer;
	this->font = font;
	this->word = word;
	this->scrn_width = scrn_width;
	this->scrn_height = scrn_height;
	this->texture = nullptr;
	this->position = { 0, 0, 0, 0 };
	this->velocity = 0;

	// setup the word
	setupWord();
}

Word::~Word() {
	cleanupWord();
}

void Word::setupWord() {
	// create a surface from the word
	// create random color
	SDL_Color rand_color = { 0 + rand() % (255 - 0 + 1), 0 + rand() % (255 - 0 + 1), 0 + rand() % (255 - 0 + 1), 255 };
	SDL_Surface* surface = TTF_RenderText_Solid(font, word.c_str(), rand_color);
	if (surface == nullptr) {
		std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
		return;
	}

	// create a texture from the surface
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == nullptr) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}

	// set the position of the word (randomly set the y have and set the x value as just outside the screen at right side)
	position.x = scrn_width;
	position.y = 30 + rand() % ((((scrn_height * 75) / 100) - 45) - 30 + 1);
	position.w = surface->w;
	position.h = surface->h;

	// set the velocity of the word
	velocity = 3;

	// free the surface
	SDL_FreeSurface(surface);
}

void Word::updateWord(float dt) {
	// update the position of the word
	if (position.x == 0) {
		position.x -= 100;
	}
	position.x -= velocity * dt;
}

void Word::renderWord() {
	// render the word
	SDL_RenderCopy(renderer, texture, nullptr, &position);
}

void Word::cleanupWord() {
	// cleanup the texture
	SDL_DestroyTexture(texture);
}

std::string Word::getWord() {
	return word;
}

SDL_Rect* Word::getWordPosition() {
	return &position;
}

int Word::getWordVelocity() {
	return velocity;
}

SDL_Texture* Word::getWordTexture() {
	return texture;
}

void Word::setVelocity(int velocity) {
	this->velocity = velocity;
}