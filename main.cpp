#include<SDL.h>
#include<iostream>
#include"chip8.h"


int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	SDL_CreateWindowAndRenderer(960, 480, SDL_WINDOW_OPENGL, &window, &renderer);

	SDL_Event e;
	bool quit = false;

	InitChip8();
	if (argc == 2) {
		LoadROMFromFile(argv[1]);
	}
	else {
		std::cout << "Enter file path: ";
		std::string fpath;
		std::cin >> fpath;
		LoadROMFromFile(fpath);
	}
	
	//LoadTestROM();

	int delaytime = 1000/DEFAULT_IPS;
	int sound_counter = 0;
	int delay_counter = 0;
	while (!quit) {
		if (ST) {
			sound_counter += delaytime;
			if (sound_counter > 16) {
				ST--;
			}
		}
		if (DT) {
			delay_counter += delaytime;
			if (delay_counter > 16) {
				DT--;
			}
		}
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = 1;
			}
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_1:
					gKEY = 1;
					gKeyStatus[1] = 1;
					break;
				case SDLK_2:
					gKEY = 2;
					gKeyStatus[2] = 1;
					break;
				case SDLK_3:
					gKEY = 3;
					gKeyStatus[3] = 1;
					break;
				case SDLK_4:
					gKEY = 0xC;
					gKeyStatus[0xC] = 1;
					break;
				case SDLK_q:
					gKEY = 4;
					gKeyStatus[4] = 1;
					break;
				case SDLK_w:
					gKEY = 5;
					gKeyStatus[5] = 1;
					break;
				case SDLK_e:
					gKEY = 6;
					gKeyStatus[6] = 1;
					break;
				case SDLK_r:
					gKEY = 0xD;
					gKeyStatus[0xD] = 1;
					break;
				case SDLK_a:
					gKEY = 7;
					gKeyStatus[7] = 1;
					break;
				case SDLK_s:
					gKEY = 8;
					gKeyStatus[8] = 1;
					break;
				case SDLK_d:
					gKEY = 9;
					gKeyStatus[9] = 1;
					break;
				case SDLK_f:
					gKEY = 0xE;
					gKeyStatus[0xE] = 1;
					break;
				case SDLK_z:
					gKEY = 0xA;
					gKeyStatus[0xA] = 1;
					break;
				case SDLK_x:
					gKEY = 0;
					gKeyStatus[0] = 1;
					break;
				case SDLK_c:
					gKEY = 0xB;
					gKeyStatus[0xB] = 1;
					break;
				case SDLK_v:
					gKEY = 0xF;
					gKeyStatus[0xF] = 1;
					break;
				}
				//std::cout << "Key down event: " << gKEY << std::endl;
			}
			if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
				case SDLK_1:
					gKEY = 1;
					gKeyStatus[1] = 0;
					break;
				case SDLK_2:
					gKEY = 2;
					gKeyStatus[2] = 0;
					break;
				case SDLK_3:
					gKEY = 3;
					gKeyStatus[3] = 0;
					break;
				case SDLK_4:
					gKEY = 0xC;
					gKeyStatus[0xC] = 0;
					break;
				case SDLK_q:
					gKEY = 4;
					gKeyStatus[4] = 0;
					break;
				case SDLK_w:
					gKEY = 5;
					gKeyStatus[5] = 0;
					break;
				case SDLK_e:
					gKEY = 6;
					gKeyStatus[6] = 0;
					break;
				case SDLK_r:
					gKEY = 0xD;
					gKeyStatus[0xD] = 0;
					break;
				case SDLK_a:
					gKEY = 7;
					gKeyStatus[7] = 0;
					break;
				case SDLK_s:
					gKEY = 8;
					gKeyStatus[8] = 0;
					break;
				case SDLK_d:
					gKEY = 9;
					gKeyStatus[9] = 0;
					break;
				case SDLK_f:
					gKEY = 0xE;
					gKeyStatus[0xE] = 0;
					break;
				case SDLK_z:
					gKEY = 0xA;
					gKeyStatus[0xA] = 0;
					break;
				case SDLK_x:
					gKEY = 0;
					gKeyStatus[0] = 0;
					break;
				case SDLK_c:
					gKEY = 0xB;
					gKeyStatus[0xB] = 0;
					break;
				case SDLK_v:
					gKEY = 0xF;
					gKeyStatus[0xF] = 0;
					break;
				}
				// std::cout << "Key up event: " << gKEY << std::endl;
				gKEY = 0;
			}
			
		}
		if (chip8_exec()) {
			//Render
			SDL_SetRenderDrawColor(renderer, 252, 110, 2, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(renderer);
			SDL_SetRenderDrawColor(renderer, 252, 231, 2, SDL_ALPHA_OPAQUE);
			for (int i = 0; i < 64; i++) {
				for (int j = 0; j < 32; j++) {
					if (DISPLAY[i][j] == 1) {
						SDL_Rect r;
						r.x = i * 15;
						r.y = j * 15;
						r.h = 15;
						r.w = 15;
						SDL_RenderFillRect(renderer, &r);
					}
				}
			}
			SDL_RenderPresent(renderer);
		}
		SDL_Delay(delaytime);
	}

	return 0;
}