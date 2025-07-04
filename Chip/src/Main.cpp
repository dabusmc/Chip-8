#include <iostream>
#include <algorithm>
#include <string>
#include <cmath>
#include <sstream>
#include <fstream>

#include <SDL.h>
#undef main

#include "Screen.h"
#include "Timer.h"
#include "Emulator.h"

#define SCREEN_WIDTH    64
#define SCREEN_HEIGHT   32
#define TARGET_FPS		60

const int SCREEN_TICKS_PER_FRAME = 1000 / TARGET_FPS;

bool isValidFilepath(const std::string& filepath)
{
	std::ifstream f(filepath);
	if (!f.good())
	{
		std::cout << "Please enter a valid file path" << std::endl;
		return false;
	}

	size_t extensionDot = filepath.find_last_of('.');

	if (extensionDot == std::string::npos)
	{
		std::cout << "Please enter a valid .ch8 file" << std::endl;
		return false;
	}

	std::string extension = filepath.substr(extensionDot);
	if (extension != ".ch8")
	{
		std::cout << "Please enter a valid .ch8 file" << std::endl;
		return false;
	}

	return true;
}

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Please enter a file to run" << std::endl;
		return 1;
	}

	std::string filePath = std::string(argv[1]);
	if (!isValidFilepath(filePath))
	{
		return 1;
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to intialize SDL: %s", SDL_GetError());
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("CHIP-8",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH * 20,
		SCREEN_HEIGHT * 20,
		SDL_WINDOW_SHOWN);

	if (window == nullptr)
	{
		SDL_Log("Unable to create SDL window: %s", SDL_GetError());
		return 1;
	}

	Chip::Screen* screen = new Chip::Screen(window, SCREEN_WIDTH, SCREEN_HEIGHT);
	Chip::Emulator emulator(filePath);
	//emulator.Pause();

	bool loop = true;
	SDL_Event e;

	Chip::Timer fpsTimer;
	Chip::Timer capTimer;
	std::stringstream timeText;

	float deltaTime = 0.0f;
	float previousTime = 0.0f;

	int countedFrames = 0;
	fpsTimer.Start();

	while (loop)
	{
		capTimer.Start();

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				loop = false;
			}
			else if (e.type == SDL_KEYDOWN && emulator.IsPaused())
			{
				if (e.key.keysym.sym == SDLK_RETURN)
				{
					emulator.Step();
				}
				else if (e.key.keysym.sym == SDLK_r)
				{
					std::cout << std::endl;
					emulator.DumpRegisters();
					std::cout << std::endl;
				}
				else if (e.key.keysym.sym == SDLK_i)
				{
					std::cout << std::endl;
					emulator.DumpIndex();
					std::cout << std::endl;
				}
				else if (e.key.keysym.sym == SDLK_g)
				{
					std::cout << std::endl;
					emulator.DumpGP();
					std::cout << std::endl;
				}
				else if (e.key.keysym.sym == SDLK_m)
				{
					std::cout << "Start: ";
					uint16_t start;
					std::cin >> start;

					std::cout << "End: ";
					uint16_t end;
					std::cin >> end;

					emulator.DumpRam(start, end);
				}
			}
		}

		float avgFPS = countedFrames / (fpsTimer.GetTicks() / 1000.0f);
		if (avgFPS > 2000000)
		{
			avgFPS = 0;
		}

		timeText.str("");
		timeText << "CHIP-8 ::: ";
		timeText << avgFPS;
		timeText << " FPS";

		SDL_SetWindowTitle(window, timeText.str().c_str());

		deltaTime = (fpsTimer.GetTicks() / 1000.0f) - previousTime;
		previousTime = fpsTimer.GetTicks() / 1000.0f;

		emulator.Tick();
		screen->UpdateScreen(emulator.GetPixelBuffer());
		screen->Render();
		++countedFrames;

		int frameTicks = capTimer.GetTicks();
		if (frameTicks < SCREEN_TICKS_PER_FRAME)
		{
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
		}
	}

	delete screen;
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}