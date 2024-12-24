#include <animation.h>

Animation::Animation(SDL_Renderer* renderer, TTF_Font* font, std::string word, int font_size, SDL_Rect position) {
	this->renderer = renderer;
	this->font = font;
	this->word = word;
	this->font_size = font_size;
	this->position = position;
	this->texture = nullptr;
	this->opacity = 255;
}

Animation::~Animation() {
	cleanupAnimation();
}

void Animation::setupAnimation() {
	// create a surface from the word
	// create color
	SDL_Color color = { 0, 255, 0, opacity };
	SDL_Surface* surface = TTF_RenderText_Blended(font, word.c_str(), color);
	if (surface == nullptr) {
		std::cerr << "TTF_RenderText_Blended Error: " << TTF_GetError() << std::endl;
		return;
	}

	// create a texture from the surface
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == nullptr) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}

	// free the surface
	SDL_FreeSurface(surface);
}

void Animation::updateAnimation(float dt) {
	// update the font size and opacity of the word
	font_size -= 1 * dt;
	opacity -= 1 * dt;
	position.x += 1 * dt;
	position.y -= 1 * dt;
}

void Animation::renderAnimation() {
	// render the word
	SDL_SetTextureAlphaMod(texture, opacity);
	SDL_SetTextureColorMod(texture, 0, 255, 0);
	// set scale for the word
	position.w = word.length() * font_size;
	position.h = font_size;
	// render the word
	SDL_RenderCopy(renderer, texture, NULL, &position);
}

void Animation::cleanupAnimation() {
	// cleanup the texture
	SDL_DestroyTexture(texture);
}

std::string Animation::getWord() {
	return word;
}

SDL_Rect* Animation::getWordPosition() {
	return &position;
}

int Animation::getWordFontSize() {
	return font_size;
}

SDL_Texture* Animation::getWordTexture() {
	return texture;
}

