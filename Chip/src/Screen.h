#pragma once

#include <SDL.h>

namespace Chip
{
	class Screen
	{
	public:
		Screen(SDL_Window* window, int screenWidth, int screenHeight);
		~Screen();

		void Render();
		void UpdateScreen(uint32_t* data);

	private:
		SDL_Window* m_Window;

		SDL_Renderer* m_Renderer;
		SDL_Texture* m_ScreenTexture;

		int m_ScreenWidth;
		int m_ScreenHeight;
	};
}