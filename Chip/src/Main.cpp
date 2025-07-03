#include <iostream>
#include <algorithm>
#include <string>
#include <cmath>

#include <SDL.h>
#undef main

#include "Screen.h"
#include "Timer.h"
#include "Emulator.h"

#define SCREEN_WIDTH    64
#define SCREEN_HEIGHT   32
#define TARGET_FPS		60

const int SCREEN_TICKS_PER_FRAME = 1000 / TARGET_FPS;

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
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
	Chip::Emulator emulator("ibm");

	bool loop = true;
	SDL_Event e;

	Chip::Timer fpsTimer;
	Chip::Timer capTimer;

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
		}

		float avgFPS = countedFrames / (fpsTimer.GetTicks() / 1000.0f);
		if (avgFPS > 2000000)
		{
			avgFPS = 0;
		}

		deltaTime = (fpsTimer.GetTicks() / 1000.0f) - previousTime;
		previousTime = fpsTimer.GetTicks() / 1000.0f;

		emulator.Tick(deltaTime);
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