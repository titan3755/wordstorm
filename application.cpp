#include <application.h>

Application::Application(int scrn_width, int scrn_height, std::string base_title) {
	this->scrn_width = scrn_width;
	this->scrn_height = scrn_height;
	this->base_title = base_title;
	this->window = nullptr;
	this->renderer = nullptr;
	this->running = false;
}

Application::~Application() {
	cleanup();
}

void Application::init() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "SDL_Init Success!" << std::endl;
	}

	if (TTF_Init() != 0) {
		std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "TTF_Init Success!" << std::endl;
	}

	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "IMG_Init Success!" << std::endl;
	}

	window = SDL_CreateWindow(base_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scrn_width, scrn_height, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "SDL_CreateWindow Success!" << std::endl;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return;
	}
	else {
		std::cout << "SDL_CreateRenderer Success!" << std::endl;
	}

	running = true;
}

void Application::run() {
	Render render(window, renderer, scrn_width, scrn_height);
	render.setupScreenHorizontalDivider();
	render.setupUpperScreenLeftmostVerticalDivider();
	render.setupUpperScreenBackground();
	render.setupLowerScreenBackground();
	render.setupScoreText(TTF_OpenFont("assets/KOMIKAX.ttf", 24), "Score: 0", 30, 10);
	render.setupTimerText(TTF_OpenFont("assets/KOMIKAX.ttf", 24), "Time: 0", 200, 10);
	render.setupAlphabetTextures(TTF_OpenFont("assets/HeyComic.ttf", 24));
	render.setupAlphabetPositions();
    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    render.setupAlphabetStates(alphabet, 26);
	SDL_Event event;
	// key pressed
	std::map<char, bool> keyStates;
	for (int i = 0; i < 26; i++) {
		keyStates[alphabet[i]] = false;
	}
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			if (event.type == SDL_KEYDOWN) {
				char key = static_cast<char>(event.key.keysym.sym);
				if (key >= 'a' && key <= 'z') { // Ensure it's a lowercase letter
					// convert the key to uppercase
					key = toupper(key);
					keyStates[key] = true;
				}
			}
			if (event.type == SDL_KEYUP) {
				char key = static_cast<char>(event.key.keysym.sym);
				if (key >= 'a' && key <= 'z') { // Ensure it's a lowercase letter
					// convert the key to uppercase
					key = toupper(key);
					keyStates[key] = false;
				}
			}
		}

		// spawn a word every 2 seconds
		static float timer = 0.0f;
		timer += 0.013333f; // 75 fps (1/75 = 0.01333...f)
		if (timer >= 2.0f) {
			timer = 0.0f;
			render.setupWord("HELLO");
		}

		// update the components
		render.updateAlphabetStates(keyStates);
		render.updateWords(0.013333f); // 75 fps (1/75 = 0.01333...f)

		// render the components

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// render the components
		render.renderLowerScreenBackground();
		render.renderScreenHorizontalDivider();
		render.renderUpperScreenBackground();
		render.renderUpperScreenLeftmostVerticalDivider();
		render.renderScoreText();
		render.renderTimerText();
		render.renderAlphabetLetters();
		render.renderWords();

		SDL_RenderPresent(renderer);
	}
}

void Application::cleanup() {
	if (renderer != nullptr) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}

	if (window != nullptr) {
		SDL_DestroyWindow(window);
		window = nullptr;
	}

	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}