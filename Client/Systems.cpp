#include "PCH.h"
#include "Systems.h"

#include "Game.h"

void Systems::DrawPaddle(float w, float h, const Vector2& position)
{
	SDL_Rect r{
		static_cast<int>(position.x),
		static_cast<int>(position.y),
		static_cast<int>(w),
		static_cast<int>(h)
	};

	SDL_SetRenderDrawColor(Game::Instance()->GetRenderer(), 255, 255, 255, 255);
	SDL_RenderFillRect(Game::Instance()->GetRenderer(), &r);
}
