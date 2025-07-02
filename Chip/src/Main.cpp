#include <iostream>
#include <algorithm>

#include <SDL.h>
#undef main

#include "Screen.h"

#define SCREEN_WIDTH    64
#define SCREEN_HEIGHT   32

inline uint32_t RGBAToInt(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return (r << 0) | (g << 8) | (b << 16) | (a << 24);
}

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

	uint32_t* pixels = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];
	for (auto i = 0; i < SCREEN_WIDTH; i++)
	{
		for (auto j = 0; j < SCREEN_HEIGHT; j++)
		{
			pixels[j * SCREEN_WIDTH + i] = RGBAToInt(0, 0, 0, 255);
		}
	}

	screen->UpdateScreen(pixels);

	bool loop = true;
	SDL_Event e;
	
	int g = 0;
	int adder = 1;
	float timer = 0.0f;
	float timeBetween = 5.0f;

	uint64_t NOW = SDL_GetPerformanceCounter();
	uint64_t LAST = 0;
	double deltaTime = 0;

	while (loop)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				loop = false;
			}
		}

		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

		bool swap = false;
		for (auto i = 0; i < SCREEN_HEIGHT; i++)
		{
			for (auto j = 0; j < SCREEN_WIDTH; j++)
			{
				if (swap)
				{
					if ((i * SCREEN_WIDTH + j) % 2 != 0)
					{
						pixels[i * SCREEN_WIDTH + j] = RGBAToInt(g, g, g, 255);
					}
				}
				else
				{
					if ((i * SCREEN_WIDTH + j) % 2 == 0)
					{
						pixels[i * SCREEN_WIDTH + j] = RGBAToInt(g, g, g, 255);
					}
				}
			}
			swap = !swap;
		}
		screen->UpdateScreen(pixels);

		if (timer <= 0.0f)
		{
			g += adder;
			if (g >= 255)
			{
				adder = -1;
			}
			else if (g <= 0)
			{
				adder = 1;
			}
			timer = timeBetween;
		}
		else
		{
			timer -= deltaTime;
		}

		screen->Render();
	}

	delete[] pixels;
	delete screen;
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}