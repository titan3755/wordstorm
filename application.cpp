#include <application.h>

const int TARGET_FRAME_RATE = 144;
const int FRAME_TIME = 1000 / TARGET_FRAME_RATE;

enum class State {
	START,
	PLAY,
	END
};

Application::Application(int scrn_width, int scrn_height, std::string base_title) {
	this->scrn_width = scrn_width;
	this->scrn_height = scrn_height;
	this->base_title = base_title;
	this->window = nullptr;
	this->renderer = nullptr;
	this->running = false;
	this->high_score = 0;
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

	int flags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(flags) & flags)) {
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

	// read the high score from the file
	std::ifstream file("stats.txt");
	if (file.is_open()) {
		// check if the file is empty
		if (file.peek() == std::ifstream::traits_type::eof()) {
			high_score = 0;
		}
		else {
			// check if the data obtained is a number
			if (file >> high_score) {
				std::cout << "High Score: " << high_score << std::endl;
			}
			else {
				std::cerr << "Invalid data in file!" << std::endl;
			}
		}
		file.close();
	}
	else {
		std::cerr << "Unable to open file!" << std::endl;
		// create the file
		std::ofstream file("stats.txt");
		if (file.is_open()) {
			file << high_score;
			file.close();
		}
		else {
			std::cerr << "Unable to create file!" << std::endl;
		}
	}

	// read the words from the file
	std::ifstream file_words("words.txt");
	if (file_words.is_open()) {
		// check if the file is empty
		if (file_words.peek() == std::ifstream::traits_type::eof()) {
			std::cerr << "File is empty!" << std::endl;
		}
		else {
			// read the entire file and separate the words by space
			std::string word;
			while (file_words >> word) {
				words_file.push_back(word);
			}
			// make every word in the vector capital
			for (int i = 0; i < words_file.size(); i++) {
				std::transform(words_file[i].begin(), words_file[i].end(), words_file[i].begin(), ::toupper);
			}
		}
		file_words.close();
	}
	else {
		std::cerr << "Unable to open file!" << std::endl;
		// create the file
		std::ofstream file_words("words.txt");
		std::string big_string = "MOUNTAIN RIVER OCEAN CASTLE VILLAGE FOREST BRIDGE WINDOW SHADOW WHISPER JOURNEY GALAXY HORIZON ADVENTURE PUZZLE CLOCK MIRROR LANTERN FEATHER TREASURE GARDEN LIGHTHOUSE STORM FIREPLACE ARMOR NOTEBOOK MELODY DRAGON PARACHUTE DESERT PLANET COMPASS FOUNTAIN TELESCOPE SYMPHONY BUTTERFLY LIBRARY QUARTZ SAPPHIRE EMERALD PAINTING VOYAGE ISLAND MEMORY UMBRELLA PENDULUM STARLIGHT CRESCENT CLOUD LABYRINTH ANCHOR BREEZE CANYON";
		if (file_words.is_open()) {
			file_words << big_string;
			// push_back to words_file by separating the words by space
			// slice the big_string by space
			std::istringstream stream(big_string);
			std::string word;
			while (stream >> word) {
				words_file.push_back(word);
			}
			file_words.close();
		}
		else {
			std::cerr << "Unable to create file!" << std::endl;
		}
	}

	running = true;
}

void Application::run() {
	// setup the state
	State state = State::START;
	// setup the render
	Render render(window, renderer, scrn_width, scrn_height);
	// setup the components
	render.setupTitleScreen(TTF_OpenFont("assets/LEMONMILK-Regular.otf", 50));
	render.setupScreenHorizontalDivider();
	render.setupUpperScreenLeftmostVerticalDivider();
	render.setupUpperScreenBackground();
	render.setupLowerScreenBackground();
	render.setupHighScoreText(TTF_OpenFont("assets/KOMIKAX.ttf", 24), "High Score: " + std::to_string(high_score), 30, 10);
	render.setupScoreText(TTF_OpenFont("assets/KOMIKAX.ttf", 24), "Score: 0", 280, 10);
	render.setupTimerText(TTF_OpenFont("assets/KOMIKAX.ttf", 24), "Time: 0", 430, 10);
	// setup the status bar box at right side of the screen
	render.setupStatusBarBox(scrn_width - 200, 0 + 12, 100, 40, "assets/iconone.png", "assets/icontwo.png", "assets/iconthree.png");
	render.setupAlphabetTextures(TTF_OpenFont("assets/Nexa-Heavy.ttf", 24));
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
	std::vector<std::string> big_words_rand = {
		"ELECTROCARDIOGRAPHY", "ANTIAUTHORITARIAN", "COMPARTMENTALIZATION", "DISCOMBOBULATE", "CONCEPTUALIZATION",
		"OVERCOMPENSATE", "MATERIALISTICALLY", "VALETUDINARIANISM", "INTERNATIONALIZATION", "PARTICULARIZATION",
		"TINTINNABULATION", "DEPERSONALIZATION", "PROFESSIONALIZATION", "HYDROELECTRICITY", "ANACHRONISTICALLY",
		"INDIVIDUALIZATION", "MULTIFARIOUSNESS", "INTERCHANGEABILITY", "NEUROTRANSMITTER", "ELECTROMECHANICAL",
		"DECRIMINALIZATION", "COUNTERARGUMENT", "INCONSEQUENTIALLY", "HYPERSENSITIVITY", "PHANTASMAGORICAL",
		"CONTEMPTUOUSNESS", "TRANSMOGRIFICATION", "DENATIONALIZATION", "UNDERDEVELOPMENT", "DISENFRANCHISE",
		"TELECONFERENCING", "PRESUMPTUOUSNESS", "UNDERACHIEVEMENT", "CONSUBSTANTIATION", "SUPERCONDUCTIVITY"
	};
	std::vector<const char*> font_paths = {
		"assets/LEMONMILK-Regular.otf",
		"assets/KOMIKAX.ttf",
		"assets/Nexa-Heavy.ttf",
		"assets/HeyComic.ttf"
	};
	// append to the words_rand vector the words from the words_file vector
	for (int i = 0; i < words_file.size(); i++) {
		words_rand.push_back(words_file[i]);
	}
	bool velocity_changed = false;
	int vel_val = 3;
	bool spawn_rate_changed = false;
	int velocity_rand_interval = (rand() % 60 + 30);
	int spawn_rate_rand_interval = (rand() % 60 + 30);
	int timer_play = 60; // 60 seconds (on every word typed, timer will increase by 5 seconds) (on every word missed, timer will decrease by 2 seconds)
	render.updateTimer(timer_play);
	Uint32 startTimez = SDL_GetTicks(); // Start time

	const Uint32 intervalz = 1000; // 1 second
	Uint32 nextTriggerTimez = startTimez + intervalz;
	while (running) {
		Uint32 frameStart = SDL_GetTicks();
		Uint32 currentTime = SDL_GetTicks();
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			if (event.type == SDL_KEYDOWN) {
				if (state == State::START) {
					state = State::PLAY;
					timer_play = 60;
				}
				if (state == State::PLAY) {
					char key = static_cast<char>(event.key.keysym.sym);
					if (key >= 'a' && key <= 'z') { // Ensure it's a lowercase letter
						// convert the key to uppercase
						key = toupper(key);
						keyStates[key] = true;
					}
					key_pressed_str += key;
				}
				if (state == State::END) {
					state = State::START;
					// cleanup the gameover screen
					timer_play = 60;
					render.cleanupGameOverScreen();
				}
			}
			if (event.type == SDL_KEYUP) {
				if (state == State::START) {
					// ...
				}
				if (state == State::PLAY) {
					char key = static_cast<char>(event.key.keysym.sym);
					if (key >= 'a' && key <= 'z') { // Ensure it's a lowercase letter
						// convert the key to uppercase
						key = toupper(key);
						keyStates[key] = false;
					}
				}
				if (state == State::END) {
					// ...
				}
			}
		}

		if (currentTime >= nextTriggerTimez) {
			timer_play -= 1;
			nextTriggerTimez += intervalz;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		if (state == State::START) {
			render.renderTitleScreen();
		}
		if (state == State::PLAY) {
			// spawn a word every 2 seconds
			static float timer = 0.0f;
			timer += 0.013333f; // 75 fps (1/75 = 0.01333...f)
			if (spawn_rate_changed) {
				if (timer >= 1.0f) {
					timer = 0.0f;
					// select random word from the words_rand vector
					std::string word_sel = words_rand[rand() % words_rand.size()];
					// randomly select font path
					const char* font_path = font_paths[rand() % font_paths.size()];
					render.setupWord(word_sel, font_path);
				}
			}
			else if (timer >= 2.0f && !spawn_rate_changed) {
				timer = 0.0f;
				// select random word from the words_rand vector
				std::string word_sel = words_rand[rand() % words_rand.size()];
				// randomly select font path
				const char* font_path = font_paths[rand() % font_paths.size()];
				render.setupWord(word_sel, font_path);
			}

			// spawn a big word every 5 seconds
			static float timer_big = 0.0f;
			timer_big += 0.013333f; // 75 fps (1/75 = 0.01333...f)
			if (timer_big >= 25.0f) {
				timer_big = 0.0f;
				// select random big word from the big_words_rand vector
				std::string word_sel = big_words_rand[rand() % big_words_rand.size()];
				// randomly select font path
				const char* font_path = font_paths[rand() % font_paths.size()];
				render.setupWord(word_sel, font_path);
			}

			// change vel_val and velocity_changed variables to normal every 3 seconds
			static float timer_vel_reset = 0.0f;
			timer_vel_reset += 0.013333f; // 75 fps (1/75 = 0.01333...f)
			if (timer_vel_reset >= 6.0f && velocity_changed) {
				timer_vel_reset = 0.0f;
				velocity_changed = false;
				vel_val = 3;
				std::cout << "reset" << std::endl;
			}

			// randomly change velocity of words
			static float timer_vel = 0.0f;
			timer_vel += 0.013333f; // 75 fps (1/75 = 0.01333...f)
			// change the velocity of the waords based on a random time interval bnetween 10 and 40
			if (timer_vel >= velocity_rand_interval) {
				timer_vel = 0.0f;
				velocity_changed = true;
				std::vector<Word*> words = render.getWords();
				int rand_vel = rand() % 480 + 3;
				vel_val = rand_vel;
				for (int i = 0; i < words.size(); i++) {
					// set velocity randomly between 3 and 20
					words[i]->setVelocity(rand_vel);
					std::cout << "Velocity: " << words[i]->getWordVelocity() << std::endl;
				}
				render.setWords(words);
				timer_vel_reset = 0.0f;
				velocity_rand_interval = (rand() % 60 + 30);
				std::cout << "Velocity Changed!" << std::endl;
			}

			// reset the spawn rate every 3 seconds
			static float timer_spawn_rate_reset = 0.0f;
			timer_spawn_rate_reset += 0.013333f; // 75 fps (1/75 = 0.01333...f)
			if (timer_spawn_rate_reset >= 6.0f && spawn_rate_changed) {
				timer_spawn_rate_reset = 0.0f;
				spawn_rate_changed = false;
			}

			// randomly change spawn rate of words
			static float timer_spawn_rate = 0.0f;
			timer_spawn_rate += 0.013333f; // 75 fps (1/75 = 0.01333...f)
			// change the spawn rate of the words based on a random time interval between 10 and 40
			if (timer_spawn_rate >= spawn_rate_rand_interval) {
				timer_spawn_rate = 0.0f;
				spawn_rate_changed = true;
				timer_spawn_rate_reset = 0.0f;
				spawn_rate_rand_interval = (rand() % 60 + 30);
				std::cout << "Spawn Rate Changed!" << std::endl;
			}

			// update the timer every second
			// accurately calculate the time passes independent of the frame rate


			// detect if one of the words in the word vector is a substring of the key_pressed_str
			// if multiple words are substrings of the key_pressed_str, the word which is the first substring will be removed
			// if one substring corresponds to multiple words, the first word which is a substring will be removed
			std::vector<Word*> words = render.getWords();
			// check if any of the words have reached the left side of the screen
			for (int i = 0; i < words.size(); i++) {
				if (words[i]->getWordPosition()->x <= 0) {
					// remove 2 seconds from the timer
					timer_play -= 5;
					// end animation (red)
					render.setupAnimation(words[i]->getWord(), 24, { 255, 0, 0 }, *words[i]->getWordPosition());
					// remove the word
					words.erase(words.begin() + i);
					// update the words
					render.setWords(words);
					// update the score
					score -= 2;
					render.updateScore(score);
					break;
				}
			}
			// set the words (check and give timer/score)
			for (int i = 0; i < words.size(); i++) {
				std::string word = words[i]->getWord();
				if (key_pressed_str.find(word) != std::string::npos) {
					// add 5 seconds to the timer
					// check if word is in big_words_rand
					auto it = std::find(big_words_rand.begin(), big_words_rand.end(), word);
					if (it != big_words_rand.end()) {
						timer_play += 60;
						score += 80;
					}
					else if (velocity_changed) {
						if (vel_val >= 50 && vel_val < 70) {
							timer_play += 25;
							score += 25;
						}
						else if (vel_val >= 70 && vel_val < 90) {
							timer_play += 37;
							score += 37;
						}
						else if (vel_val >= 90 && vel_val < 110) {
							timer_play += 49;
							score += 49;
						}
						else if (vel_val >= 110 && vel_val < 130) {
							timer_play += 51;
							score += 51;
						}
						else {
							timer_play += 63;
							score += 63;
						}
					}
					else {
						if (word.length() <= 5) {
							timer_play += 5;
							score += 5;
						}
						else if (word.length() <= 10 && word.length() > 5) {
							timer_play += 7;
							score += 7;
						}
						else if (word.length() <= 15 && word.length() > 10) {
							timer_play += 9;
							score += 9;
						}
						else if (word.length() <= 20 && word.length() > 15) {
							timer_play += 11;
							score += 11;
						}
						else {
							timer_play += 13;
							score += 13;
						}
					}
					// end animation (green)
					render.setupAnimation(word, 24, { 0, 255, 0 }, *words[i]->getWordPosition());
					// remove the word
					words.erase(words.begin() + i);
					// update the words
					render.setWords(words);
					key_pressed_str.erase(key_pressed_str.find(word), word.length());
					render.updateScore(score);
					break;
				}
			}

			// check if score is greater than high score
			if (score > high_score) {
				high_score = score;
				// update the high score file
				std::ofstream file("stats.txt", std::ios::out);
				file << high_score;
				file.close();
				// update the high score text
				render.updateHighScore(high_score);
			}

			// update status
			if (velocity_changed && spawn_rate_changed) {
				render.updateStatusBarBoxStatus(true, true, true);
			}
			else if (velocity_changed && !spawn_rate_changed) {
				render.updateStatusBarBoxStatus(true, false, true);
			}
			else if (!velocity_changed && spawn_rate_changed) {
				render.updateStatusBarBoxStatus(true, true, false);
			}
			else {
				render.updateStatusBarBoxStatus(true, false, false);
			}

			// update the components
			render.updateAlphabetStates(keyStates);
			render.updateWords(0.013333f); // 75 fps (1/75 = 0.01333...f)
			render.updateAnimations(0.000333f); // 75 fps (1/75 = 0.01333...f)
			// update the timer only when the timer_play variable has changed
			static int timer_play_prev = timer_play;
			if (timer_play != timer_play_prev) {
				render.updateTimer(timer_play);
				timer_play_prev = timer_play;
			}

			// check if the timer has reached 0
			if (timer_play <= 0) {
				state = State::END;

				// setup the gameover screen
				render.setupGameOverScreen(TTF_OpenFont("assets/KOMIKAX.ttf", 48), score, high_score);

				// set the high score in the file
				std::ofstream file("stats.txt", std::ios::out);
				file << high_score;
				file.close();

				// clear the key pressed string
				key_pressed_str.clear();

				// clear the words
				words.clear();
				render.setWords(words);

				// clear the animations
				render.setAnimations(std::vector<Animation*>());

				// reset the score
				score = 0;
				render.updateScore(score);

				// reset the timer
				timer_play = 60;
				render.updateTimer(timer_play);

				// reset other timer variable
				timer_play_prev = timer_play;

				// reset the key states
				for (int i = 0; i < 26; i++) {
					keyStates[alphabet[i]] = false;
				}

				// reset the alphabet states
				render.setupAlphabetStates(alphabet, 26);
			}

			// render the components
			render.renderLowerScreenBackground();
			render.renderScreenHorizontalDivider();
			render.renderUpperScreenBackground();
			render.renderUpperScreenLeftmostVerticalDivider();
			render.renderHighScoreText();
			render.renderScoreText();
			render.renderTimerText();
			render.renderStatusBarBox();
			render.renderAlphabetLetters();
			render.renderWords();
			render.renderAnimations();

			Uint32 frameTime = SDL_GetTicks() - frameStart;
			if (frameTime < FRAME_TIME) {
				SDL_Delay(FRAME_TIME - frameTime);
			}
		}
		if (state == State::END) {
			// render gameover screen
			render.renderGameOverScreen();
		}
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

