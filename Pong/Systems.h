#pragma once

class Systems
{
public:
	static void DrawRect(SDL_Renderer* renderer, float w, float h, const Vector2& position);
	static void UpdateDirection(const uint8_t* keystates, float& outDirection);
	static void UpdatePosition(float speed, const Vector2& direction, Vector2& outPosition, float deltaTime);
	static bool Intersects(const SDL_Rect& a, const SDL_Rect& b);
};