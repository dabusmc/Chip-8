#include <iostream>
#include <algorithm>

#include <SDL.h>
#undef main

#define SCREEN_WIDTH    64
#define SCREEN_HEIGHT   32

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

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == nullptr)
	{
		SDL_Log("Unable to create SDL renderer: %s", SDL_GetError());
		return 1;
	}

	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_Texture* texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH,
		SCREEN_HEIGHT
	);

	if (texture == nullptr)
	{
		SDL_Log("Unable to create texture: %s", SDL_GetError());
	}

	uint8_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT * 4] = { 0 };
	for (auto i = 0; i < SCREEN_WIDTH * 4; i += 4)
	{
		pixels[12 * SCREEN_WIDTH + i + 0] = 255;
		pixels[12 * SCREEN_WIDTH + i + 1] = 255;
		pixels[12 * SCREEN_WIDTH + i + 2] = 255;
		pixels[12 * SCREEN_WIDTH + i + 3] = 255;
	}

	int texturePitch = 0;
	void* texturePixels = nullptr;
	if (SDL_LockTexture(texture, nullptr, &texturePixels, &texturePitch) != 0)
	{
		SDL_Log("Unable to lock texture: %s", SDL_GetError());
	}
	else
	{
		memcpy(texturePixels, pixels, texturePitch * SCREEN_HEIGHT);
	}
	SDL_UnlockTexture(texture);

	bool loop = true;
	SDL_Event e;
	while (loop)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				loop = false;
			}
		}

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}