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

void Systems::UpdateDirection(const uint8_t* keystates, float& outDirection)
{
	outDirection = 0.0f;

	if (keystates[SDL_SCANCODE_W])
	{
		outDirection -= 1.0f;
	}

	if (keystates[SDL_SCANCODE_S])
	{
		outDirection += 1.0f;
	}
}

void Systems::UpdatePosition(float speed, float direction, Vector2& outPosition, float deltaTime)
{
	outPosition.y += speed * direction * deltaTime;
}
