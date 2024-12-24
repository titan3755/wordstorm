#include <application.h>

const int TARGET_FRAME_RATE = 144;
const int FRAME_TIME = 1000 / TARGET_FRAME_RATE;

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
	// this string will store the keys which are pressed
	std::string key_pressed_str;
	for (int i = 0; i < 26; i++) {
		keyStates[alphabet[i]] = false;
	}
	int score = 0;
	std::vector<std::string> words_rand = {
	"MOUNTAIN", "RIVER", "OCEAN", "CASTLE", "VILLAGE", "FOREST", "BRIDGE", "WINDOW", "SHADOW", "WHISPER",
	"JOURNEY", "GALAXY", "HORIZON", "ADVENTURE", "PUZZLE", "CLOCK", "MIRROR", "LANTERN", "FEATHER", "TREASURE",
	"GARDEN", "LIGHTHOUSE", "STORM", "FIREPLACE", "ARMOR", "NOTEBOOK", "MELODY", "DRAGON", "PARACHUTE", "DESERT",
	"PLANET", "COMPASS", "FOUNTAIN", "TELESCOPE", "SYMPHONY", "BUTTERFLY", "LIBRARY", "QUARTZ", "SAPPHIRE", "EMERALD",
	"PAINTING", "VOYAGE", "ISLAND", "MEMORY", "UMBRELLA", "PENDULUM", "STARLIGHT", "CRESCENT", "CLOUD", "LABYRINTH",
	"ANCHOR", "BREEZE", "CANYON", "DESIRE", "EMBER", "FABRIC", "GLACIER", "HARVEST", "ILLUSION", "JEWEL",
	"KINGDOM", "LANTERN", "MOMENT", "NEBULA", "OASIS", "PEBBLE", "QUIVER", "REFUGE", "SCEPTER", "TAPESTRY",
	"UMBRELLA", "VOYAGER", "WILLOW", "XYLOPHONE", "YONDER", "ZEPHYR", "BOULDER", "CIPHER", "DAGGER", "ECLIPSE",
	"FABLE", "GROTTO", "HORIZON", "ISLAND", "JIGSAW", "KEYSTONE", "LABYRINTH", "MIST", "NECTAR", "ORACLE",
	"PARADOX", "QUAGMIRE", "RAVEN", "SILHOUETTE", "THRONE", "UTOPIA", "VALLEY", "WALTZ", "XENON", "ZENITH"
	};
	while (running) {
		Uint32 frameStart = SDL_GetTicks();
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
				key_pressed_str += key;
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
			// select random word from the words_rand vector
			std::string word_sel = words_rand[rand() % words_rand.size()];
			render.setupWord(word_sel);
		}

		// detect if one of the words in the word vector is a substring of the key_pressed_str
		// if multiple words are substrings of the key_pressed_str, the word which is the first substring will be removed
		// if one substring corresponds to multiple words, the first word which is a substring will be removed
		std::vector<Word*> words = render.getWords();
		// check if any of the words have reached the left side of the screen
		for (int i = 0; i < words.size(); i++) {
			if (words[i]->getWordPosition()->x <= 0) {
				// remove the word
				words.erase(words.begin() + i);
				// update the words
				render.setWords(words);
				// update the score
				score -= 5;
				render.updateScore(score);
				break;
			}
		}
		// set the words
		for (int i = 0; i < words.size(); i++) {
			std::string word = words[i]->getWord();
			if (key_pressed_str.find(word) != std::string::npos) {
				render.setupAnimation(word, 24, *words[i]->getWordPosition());
				// remove the word
				words.erase(words.begin() + i);
				// update the words
				render.setWords(words);
				key_pressed_str.erase(key_pressed_str.find(word), word.length());
				// update the score
				score += 10;
				render.updateScore(score);
				break;
			}
		}

		// update the components
		render.updateAlphabetStates(keyStates);
		render.updateWords(0.013333f); // 75 fps (1/75 = 0.01333...f)
		render.updateAnimations(0.000333f); // 75 fps (1/75 = 0.01333...f)

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
		render.renderAnimations();

		SDL_RenderPresent(renderer);

		Uint32 frameTime = SDL_GetTicks() - frameStart;
		if (frameTime < FRAME_TIME) {
			SDL_Delay(FRAME_TIME - frameTime);
		}
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