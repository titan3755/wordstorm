#include <render.h>

// constructor
Render::Render(SDL_Window* window, SDL_Renderer* renderer, int scrn_width, int scrn_height) {
	this->window = window;
	this->renderer = renderer;
	this->scrn_width = scrn_width;
	this->scrn_height = scrn_height;
	this->horizontal_divider = nullptr; // Initialize horizontal_divider
	this->upper_screen_background = nullptr;
	this->upper_screen_leftmost_vertical_divider = nullptr;
	this->upper_screen_leftmost_vertical_divider_color = nullptr;
	this->lower_screen_background = nullptr;
	this->horizontal_divider_color = nullptr;
	this->upper_screen_background_color = nullptr;
	this->lower_screen_background_color = nullptr;
	this->score_text_font = nullptr;
	this->score_text_texture = nullptr;
	this->score_text = "";
	this->score_text_rect.x = 0;
	this->score_text_rect.y = 0;
	this->score_text_rect.w = 0;
	this->score_text_rect.h = 0;
	this->score_text_color.r = 255;
	this->score_text_color.g = 255;
	this->score_text_color.b = 255;
	this->score_text_color.a = 255;
	this->timer_text = "";
	this->timer_text_color.r = 255; // Initialize timer_text_color
	this->timer_text_color.g = 255;
	this->timer_text_color.b = 255;
	this->timer_text_color.a = 255;
	this->timer_text_font = nullptr; // Initialize timer_text_font
	this->timer_text_texture = nullptr; // Initialize timer_text_texture
	this->timer_text_rect.x = 0;
	this->timer_text_rect.y = 0;
	this->timer_text_rect.w = 0;
	this->timer_text_rect.h = 0;
	this->alphabet_positions = std::map<char, SDL_Rect*>();
	this->alphabet_textures = std::map<char, std::vector<SDL_Texture*>>();
	this->alphabet_states = std::map<char, bool>(); // true if the alphabet is pressed, false otherwise
	this->normal_alphabet_color = { 255, 255, 255, 255 };
	this->pressed_alphabet_color = { 0, 255, 0, 255 };
	this->words = std::vector<Word*>();
	this->animations = std::vector<Animation*>();
	this->title_screen_background_image_texture = nullptr;
	this->title_screen_title_text_texture = nullptr;
	this->title_screen_subtitle_text_texture = nullptr;
	this->title_screen_title_text = "";
	this->title_screen_subtitle_text = "";
	this->title_screen_title_text_rect.x = 0;
	this->title_screen_title_text_rect.y = 0;
	this->title_screen_title_text_rect.w = 0;
	this->title_screen_title_text_rect.h = 0;
	this->title_screen_title_text_color.r = 255;
	this->title_screen_title_text_color.g = 255;
	this->title_screen_title_text_color.b = 255;
	this->title_screen_subtitle_text_rect.x = 0;
	this->title_screen_subtitle_text_rect.y = 0;
	this->title_screen_subtitle_text_rect.w = 0;
	this->title_screen_subtitle_text_rect.h = 0;
	this->title_screen_subtitle_text_color.r = 255;
	this->title_screen_subtitle_text_color.g = 255;
	this->title_screen_subtitle_text_color.b = 255;
}

// destructor
Render::~Render() {
	// cleanup the components
	// cleanup the title screen
	
	// cleanup the horizontal divider
	cleanupScreenHorizontalDivider();
	// cleanup the upper screen background
	cleanupUpperScreenBackground();
	// cleanup the lower screen background
	cleanupLowerScreenBackground();
	// cleanup the score text
	cleanupScoreText();
	// cleanup the timer text
	cleanupTimerText();
	// cleanup the textures of the alphabet letters
	cleanupAlphabetTextures();
	// cleanup the positions of the alphabet letters
	cleanupAlphabetPositions();
	// cleanup the states of the alphabet letters
	cleanupAlphabetStates();
	// cleanup the renderer and window (no need to cleanup the renderer and window here, as they will be cleaned up in the application class)
	// SDL_DestroyRenderer(renderer);
	// SDL_DestroyWindow(window);
}

// setup functions for different components

// setup the title screen
void Render::setupTitleScreen(TTF_Font* font) {
	// use bg.jpg as the background image
	SDL_Surface* surface = IMG_Load("assets/bg.jpg");
	if (surface == nullptr) {
		std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
		return;
	}
	title_screen_background_image_texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (title_screen_background_image_texture == nullptr) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}
	SDL_FreeSurface(surface);
	// set the title screen title text as wordstorm and subtitle text as press any key to start
	// store the text as texture
	title_screen_title_text = "WordStorm";
	title_screen_subtitle_text = "Press any key to start";
	SDL_Surface* text_surface = TTF_RenderText_Solid(font, title_screen_title_text.c_str(), title_screen_title_text_color);
	if (text_surface == nullptr) {
		std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
		return;
	}
	title_screen_title_text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (title_screen_title_text_texture == nullptr) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}
	title_screen_title_text_rect.w = text_surface->w;
	title_screen_title_text_rect.h = text_surface->h;
	SDL_FreeSurface(text_surface);
	text_surface = TTF_RenderText_Solid(font, title_screen_subtitle_text.c_str(), title_screen_subtitle_text_color);
	if (text_surface == nullptr) {
		std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
		return;
	}
	title_screen_subtitle_text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (title_screen_subtitle_text_texture == nullptr) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}
	title_screen_subtitle_text_rect.w = text_surface->w;
	title_screen_subtitle_text_rect.h = text_surface->h;
	SDL_FreeSurface(text_surface);
	// change the x and y coordinates of the rects to slightly above the center of the window
	// and slightly below the center of the window
	title_screen_title_text_rect.x = (scrn_width - title_screen_title_text_rect.w) / 2;
	title_screen_title_text_rect.y = (scrn_height - title_screen_title_text_rect.h) / 2 - 50;
	title_screen_subtitle_text_rect.x = (scrn_width - title_screen_subtitle_text_rect.w) / 2;
	title_screen_subtitle_text_rect.y = (scrn_height - title_screen_subtitle_text_rect.h) / 2 + 50;
}

// the horizontal divider between the upper and lower screens (upper screen will have greater area than the lower screen)
void Render::setupScreenHorizontalDivider() {
	horizontal_divider = new SDL_Rect();
	horizontal_divider->x = 0;
	horizontal_divider->y = (scrn_height * 75) / 100;
	horizontal_divider->w = scrn_width;
	horizontal_divider->h = 20;
	horizontal_divider_color = new SDL_Color();
	horizontal_divider_color->r = 127;
	horizontal_divider_color->g = 0;
	horizontal_divider_color->b = 255;
	horizontal_divider_color->a = 255;
}

// the leftmost upper screen divider (this is the place where the words will disappear)
// (note: remember to relocate the score and timer text to the upper screen)
void Render::setupUpperScreenLeftmostVerticalDivider() {
	upper_screen_leftmost_vertical_divider = new SDL_Rect();
	upper_screen_leftmost_vertical_divider->x = 0;
	upper_screen_leftmost_vertical_divider->y = 0;
	upper_screen_leftmost_vertical_divider->w = 20;
	upper_screen_leftmost_vertical_divider->h = scrn_height * 75 / 100;
	upper_screen_leftmost_vertical_divider_color = new SDL_Color();
	upper_screen_leftmost_vertical_divider_color->r = 255;
	upper_screen_leftmost_vertical_divider_color->g = 0;
	upper_screen_leftmost_vertical_divider_color->b = 0;
	upper_screen_leftmost_vertical_divider_color->a = 255;
}

// the background of the upper screen
void Render::setupUpperScreenBackground() {
	upper_screen_background = new SDL_Rect();
	upper_screen_background->x = 0;
	upper_screen_background->y = 0;
	upper_screen_background->w = scrn_width;
	upper_screen_background->h = (scrn_height * 75) / 100;
	upper_screen_background_color = new SDL_Color();
	upper_screen_background_color->r = 255;
	upper_screen_background_color->g = 255;
	upper_screen_background_color->b = 255;
	upper_screen_background_color->a = 255;
}

// the background of the lower screen
void Render::setupLowerScreenBackground() {
	lower_screen_background = new SDL_Rect();
	lower_screen_background->x = 0;
	lower_screen_background->y = (scrn_height * 75) / 100;
	lower_screen_background->w = scrn_width;
	lower_screen_background->h = (scrn_height * 25) / 100;
	lower_screen_background_color = new SDL_Color();
	lower_screen_background_color->r = 0;
	lower_screen_background_color->g = 0;
	lower_screen_background_color->b = 0;
	lower_screen_background_color->a = 255;
}

// the score text
void Render::setupScoreText(TTF_Font* font, std::string text, int x, int y) {
	score_text_font = font;
	score_text = text;
	score_text_color.r = 0;
	score_text_color.g = 0;
	score_text_color.b = 0;
	score_text_color.a = 255;
	score_text_texture = nullptr;
	score_text_rect.x = x;
	score_text_rect.y = y;
	score_text_rect.w = 0;
	score_text_rect.h = 0;

	// render the text to a texture
	SDL_Surface* text_surface = TTF_RenderText_Solid(score_text_font, score_text.c_str(), score_text_color);
	if (text_surface == nullptr) {
		std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "TTF_RenderText_Solid Success!" << std::endl;
	}
	score_text_rect.w = text_surface->w;
	score_text_rect.h = text_surface->h;
	score_text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (score_text_texture == nullptr) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "SDL_CreateTextureFromSurface Success!" << std::endl;
	}

	// cleanup the surface
	SDL_FreeSurface(text_surface);
}

// the timer text
void Render::setupTimerText(TTF_Font* font, std::string text, int x, int y) {
	timer_text_font = font;
	timer_text = text;
	timer_text_color.r = 0;
	timer_text_color.g = 0;
	timer_text_color.b = 0;
	timer_text_color.a = 255;
	timer_text_texture = nullptr;
	timer_text_rect.x = x;
	timer_text_rect.y = y;
	timer_text_rect.w = 0;
	timer_text_rect.h = 0;

	// render the text to a texture
	SDL_Surface* text_surface = TTF_RenderText_Solid(timer_text_font, timer_text.c_str(), timer_text_color);
	if (text_surface == nullptr) {
		std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "TTF_RenderText_Solid Success!" << std::endl;
	}
	timer_text_rect.w = text_surface->w;
	timer_text_rect.h = text_surface->h;
	timer_text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (timer_text_texture == nullptr) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "SDL_CreateTextureFromSurface Success!" << std::endl;
	}

	// cleanup the surface
	SDL_FreeSurface(text_surface);
}

// create the alphabet textures
void Render::setupAlphabetTextures(TTF_Font* font) {
	// create the unpressed alphabet textures
	for (char c = 'A'; c <= 'Z'; c++) {
		// create the alphabet texture
		SDL_Surface* text_surface = TTF_RenderText_Solid(font, std::string(1, c).c_str(), normal_alphabet_color);
		if (text_surface == nullptr) {
			std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
			return;
		}
		else {
			std::cout << "TTF_RenderText_Solid Success!" << std::endl;
		}
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		if (text_texture == nullptr) {
			std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
			return;
		}
		else {
			std::cout << "SDL_CreateTextureFromSurface Success!" << std::endl;
		}

		// add the texture to the map by accessing the vector inside the map
		alphabet_textures[c].push_back(text_texture);

		// cleanup the surface
		SDL_FreeSurface(text_surface);
	}
	// create the pressed alphabet textures
	for (char c = 'A'; c <= 'Z'; c++) {
		// create the alphabet texture
		SDL_Surface* text_surface = TTF_RenderText_Solid(font, std::string(1, c).c_str(), pressed_alphabet_color);
		if (text_surface == nullptr) {
			std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
			return;
		}
		else {
			std::cout << "TTF_RenderText_Solid Success!" << std::endl;
		}
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		if (text_texture == nullptr) {
			std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
			return;
		}
		else {
			std::cout << "SDL_CreateTextureFromSurface Success!" << std::endl;
		}

		// add the texture to the map
		alphabet_textures[c].push_back(text_texture);

		// cleanup the surface
		SDL_FreeSurface(text_surface);
	}
}

// create the alphabet positions (the alphabets will be arranged in such a way that they will be in the bottom portion of the screen and will be evenly spaced and it will not overlap or go outside the screen)
void Render::setupAlphabetPositions() {
	// calculate the width of each alphabet
	int alphabet_width = scrn_width / 26;

	// calculate the height of each alphabet
	int alphabet_height = scrn_height / 4;

	// calculate the x position of the first alphabet
	int x = 0;

	// calculate the y position of the first alphabet
	int y = scrn_height / 2 + alphabet_height;

	// create the alphabet positions
	for (char c = 'A'; c <= 'Z'; c++) {
		// create the alphabet position
		SDL_Rect* rect = new SDL_Rect();
		rect->x = x;
		rect->y = y;
		rect->w = alphabet_width;
		rect->h = alphabet_height;

		// add the position to the map
		alphabet_positions[c] = rect;

		// update the x position for the next alphabet
		x += alphabet_width;
	}
}

// setup the alphabet states (all the alphabets will be in the normal state initially)
void Render::setupAlphabetStates(char keys[], int size) {
	for (int i = 0; i < size; i++) {
		alphabet_states[keys[i]] = false;
	}
}

// setup the words
void Render::setupWord(std::string word) {
	Word* w = new Word(renderer, TTF_OpenFont("assets/KOMIKAX.ttf", 30), word, scrn_width, scrn_height);
	w->setupWord();
	words.push_back(w);
}

// setup the animation
void Render::setupAnimation(std::string word, int font_size, SDL_Color init_clr, SDL_Rect position) {
	Animation* a = new Animation(renderer, TTF_OpenFont("assets/KOMIKAX.ttf", font_size), init_clr, word, font_size, position);
	a->setupAnimation();
	animations.push_back(a);
}

// update functions for different components

// update the score text
void Render::updateScoreText(std::string text) {
	score_text = text;

	// render the text to a texture
	SDL_Surface* text_surface = TTF_RenderText_Solid(score_text_font, score_text.c_str(), score_text_color);
	if (text_surface == nullptr) {
		std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "TTF_RenderText_Solid Success!" << std::endl;
	}
	score_text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (score_text_texture == nullptr) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "SDL_CreateTextureFromSurface Success!" << std::endl;
	}

	// cleanup the surface
	SDL_FreeSurface(text_surface);
}

// update the score by incrementing
void Render::updateScore(int score) {
	std::string text = "Score: " + std::to_string(score);
	updateScoreText(text);
}

// update the color of the score text
void Render::updateScoreColor(SDL_Color color) {
	score_text_color = color;

	// render the text to a texture
	SDL_Surface* text_surface = TTF_RenderText_Solid(score_text_font, score_text.c_str(), score_text_color);
	if (text_surface == nullptr) {
		std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "TTF_RenderText_Solid Success!" << std::endl;
	}
	score_text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (score_text_texture == nullptr) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "SDL_CreateTextureFromSurface Success!" << std::endl;
	}

	// cleanup the surface
	SDL_FreeSurface(text_surface);
}

// update the position of the score text
void Render::updateScorePosition(int x, int y) {
	score_text_rect.x = x;
	score_text_rect.y = y;
}

// update the timer
void Render::updateTimer(int timer) {
	std::string text = "Timer: " + std::to_string(timer);
	updateTimerText(text);
}

// update the timer text
void Render::updateTimerText(std::string text) {
	timer_text = text;

	// render the text to a texture
	SDL_Surface* text_surface = TTF_RenderText_Solid(timer_text_font, timer_text.c_str(), timer_text_color);
	if (text_surface == nullptr) {
		std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "TTF_RenderText_Solid Success!" << std::endl;
	}
	timer_text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (timer_text_texture == nullptr) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "SDL_CreateTextureFromSurface Success!" << std::endl;
	}

	// cleanup the surface
	SDL_FreeSurface(text_surface);
}

// update the color of the timer text
void Render::updateTimerColor(SDL_Color color) {
	timer_text_color = color;

	// render the text to a texture
	SDL_Surface* text_surface = TTF_RenderText_Solid(timer_text_font, timer_text.c_str(), timer_text_color);
	if (text_surface == nullptr) {
		std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "TTF_RenderText_Solid Success!" << std::endl;
	}
	timer_text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (timer_text_texture == nullptr) {
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "SDL_CreateTextureFromSurface Success!" << std::endl;
	}

	// cleanup the surface
	SDL_FreeSurface(text_surface);
}

// update the position of the timer text
void Render::updateTimerPosition(int x, int y) {
	timer_text_rect.x = x;
	timer_text_rect.y = y;
}

// an update function which will update the state of the alphabet letters
// each time the alphabets are updated, the previous state will be reset
// also new textures will be created for the updated alphabets
// discarding the old ones
// the argument is an array of characters
void Render::updateAlphabetStates(char keys[], int size) {
	// efficiently update the state of the alphabet letters
	for (auto it = alphabet_states.begin(); it != alphabet_states.end(); it++) {
		it->second = false;
	}

	// update the state of the alphabet letters
	for (int i = 0; i < size; i++) {
		alphabet_states[keys[i]] = true;
	}

}

// an update function which will update the state of the alphabet letters
// the argument will be the key which is pressed and the state of that key will be updated
// the rest of the keys will be updated to the normal state
void Render::updateAlphabetStates(char key) {
	// update the state of the alphabet letters
	for (auto it = alphabet_states.begin(); it != alphabet_states.end(); it++) {
		if (it->first == key) {
			it->second = true;
		}
		else {
			it->second = false;
		}
	}
}

// an update function which will update the state of the alphabet letters
// the argument will be a map which will have the key as the alphabet letter and the value as a boolean
void Render::updateAlphabetStates(std::map<char, bool> states) {
	// update the state of the alphabet letters
	for (auto it = alphabet_states.begin(); it != alphabet_states.end(); it++) {
		it->second = states[it->first];
	}
}

// update the words
void Render::updateWords(float dt) {
	for (int i = 0; i < words.size(); i++) {
		words[i]->updateWord(dt);
	}
}

// a function to update the positions of the alphabet letters on the screen
// the argument will be a map which will have the key as the alphabet letter and the value as an SDL_Rect pointer
// the function will update the positions of the alphabet letters on the screen
void Render::updateAlphabetPositions(std::map<char, SDL_Rect*> positions) {
	// cleanup the old positions
	cleanupAlphabetPositions();

	// update the new positions
	for (auto it = positions.begin(); it != positions.end(); it++) {
		alphabet_positions[it->first] = it->second;
	}
}

// update function for animation
void Render::updateAnimations(float dt) {
	for (int i = 0; i < animations.size(); i++) {
		animations[i]->updateAnimation(dt);
	}
}

// cleanup functions for different components
void Render::cleanupScreenHorizontalDivider() {
	delete horizontal_divider;
	delete horizontal_divider_color;
}

void Render::cleanupUpperScreenBackground() {
	delete upper_screen_background;
	delete upper_screen_background_color;
}

void Render::cleanupLowerScreenBackground() {
	delete lower_screen_background;
	delete lower_screen_background_color;
}

void Render::cleanupScoreText() {
	SDL_DestroyTexture(score_text_texture);
}

void Render::cleanupTimerText() {
	SDL_DestroyTexture(timer_text_texture);
}

void Render::cleanupAlphabetTextures() {
	for (auto it = alphabet_textures.begin(); it != alphabet_textures.end(); it++) {
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			SDL_DestroyTexture(*it2);
		}
	}
}

void Render::cleanupAlphabetPositions() {
	for (auto it = alphabet_positions.begin(); it != alphabet_positions.end(); it++) {
		delete it->second;
	}
}

void Render::cleanupAlphabetStates() {
	alphabet_states.clear();
}

void Render::cleanupTitleScreen() {
	SDL_DestroyTexture(title_screen_background_image_texture);
	SDL_DestroyTexture(title_screen_title_text_texture);
	SDL_DestroyTexture(title_screen_subtitle_text_texture);
}

// render functions for different components (will be used in the game loop)

// render the horizontal divider
void Render::renderScreenHorizontalDivider() {
	SDL_SetRenderDrawColor(renderer, horizontal_divider_color->r, horizontal_divider_color->g, horizontal_divider_color->b, horizontal_divider_color->a);
	SDL_RenderFillRect(renderer, horizontal_divider);
}

// render the leftmost vertical divider of the upper screen
void Render::renderUpperScreenLeftmostVerticalDivider() {
	SDL_SetRenderDrawColor(renderer, upper_screen_leftmost_vertical_divider_color->r, upper_screen_leftmost_vertical_divider_color->g, upper_screen_leftmost_vertical_divider_color->b, upper_screen_leftmost_vertical_divider_color->a);
	SDL_RenderFillRect(renderer, upper_screen_leftmost_vertical_divider);
}

// render the background of the upper screen
void Render::renderUpperScreenBackground() {
	SDL_SetRenderDrawColor(renderer, upper_screen_background_color->r, upper_screen_background_color->g, upper_screen_background_color->b, upper_screen_background_color->a);
	SDL_RenderFillRect(renderer, upper_screen_background);
}

// render the background of the lower screen
void Render::renderLowerScreenBackground() {
	SDL_SetRenderDrawColor(renderer, lower_screen_background_color->r, lower_screen_background_color->g, lower_screen_background_color->b, lower_screen_background_color->a);
	SDL_RenderFillRect(renderer, lower_screen_background);
}

// render the score text
void Render::renderScoreText() {
	SDL_RenderCopy(renderer, score_text_texture, nullptr, &score_text_rect);
}

// render the timer text
void Render::renderTimerText() {
	SDL_RenderCopy(renderer, timer_text_texture, nullptr, &timer_text_rect);
}

// a render function which will render the alphabet letters on the screen
// the function will take the current state of the alphabet letters and render them accordingly
// if the state is normal, the normal texture will be rendered and if the state is pressed, the pressed texture will be rendered
void Render::renderAlphabetLetters() {
	// loop through the alphabet positions and states and render the alphabet letters
	for (int i = 0; i < 26; i++) {
		char c = 'A' + i;
		SDL_Rect* rect = this->alphabet_positions[c];
		SDL_Texture* texture = nullptr;
		// print the states of the alphabet letters
		if (this->alphabet_states[c]) {
			texture = this->alphabet_textures[c][1];
		}
		else {
			texture = this->alphabet_textures[c][0];
		}
		SDL_RenderCopy(this->renderer, texture, nullptr, rect);
	}
}

// a render function which will render the words shooting from the top portion of the screen and across it
// the function will take the word and render it on the screen
void Render::renderWords() {
	for (int i = 0; i < words.size(); i++) {
		words[i]->renderWord();
	}
}

// render function for animation
void Render::renderAnimations() {
	for (int i = 0; i < animations.size(); i++) {
		animations[i]->renderAnimation();
	}
}

// render the title screen
void Render::renderTitleScreen() {
	// render the background image
	SDL_RenderCopy(renderer, title_screen_background_image_texture, nullptr, nullptr);
	// render the title text
	SDL_RenderCopy(renderer, title_screen_title_text_texture, nullptr, &title_screen_title_text_rect);
	// render the subtitle text
	SDL_RenderCopy(renderer, title_screen_subtitle_text_texture, nullptr, &title_screen_subtitle_text_rect);
}

// alphabet properties getters
std::map<char, std::vector<SDL_Texture*>> Render::getAlphabetTextures() const {
	return alphabet_textures;
}

std::map<char, SDL_Rect*> Render::getAlphabetPositions() const {
	return alphabet_positions;
}

std::map<char, bool> Render::getAlphabetStates() const {
	return alphabet_states;
}

// get words

std::vector<Word*> Render::getWords() const {
	return words;
}

// get animations
std::vector<Animation*> Render::getAnimations() const {
	return animations;
}

// getters for the score text

std::string Render::getScoreText() const {
	return score_text;
}

// setters

// set words
void Render::setWords(std::vector<Word*> words) {
	this->words = words;
}