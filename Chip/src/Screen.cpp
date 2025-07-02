#include "Screen.h"

#include <assert.h>
#include <cstring>

namespace Chip
{
	Screen::Screen(SDL_Window* window, int screenWidth, int screenHeight)
		: m_Window(window), m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight)
	{
		m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);
		assert(m_Renderer && "Failed to create SDL Renderer");
		SDL_RenderSetLogicalSize(m_Renderer, screenWidth, screenHeight);

		m_ScreenTexture = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
		assert(m_ScreenTexture && "Failed to create SDL Texture");
	}

	Screen::~Screen()
	{
		SDL_DestroyRenderer(m_Renderer);
	}

	void Screen::Render()
	{
		SDL_RenderClear(m_Renderer);
		SDL_RenderCopy(m_Renderer, m_ScreenTexture, nullptr, nullptr);
		SDL_RenderPresent(m_Renderer);
	}

	void Screen::UpdateScreen(uint32_t* data)
	{
		int texturePitch = 0;
		void* texturePixels = nullptr;

		if (SDL_LockTexture(m_ScreenTexture, nullptr, &texturePixels, &texturePitch) != 0)
		{
			SDL_Log("Unable to lock texture: %s", SDL_GetError());
		}
		else
		{
			std::memcpy(texturePixels, data, texturePitch * m_ScreenHeight);
		}
		SDL_UnlockTexture(m_ScreenTexture);
	}
}