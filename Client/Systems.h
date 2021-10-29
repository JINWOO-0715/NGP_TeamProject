#pragma once

class Systems
{
public:
	static void DrawPaddle(float w, float h, const Vector2& position);
	static void UpdateDirection(const uint8_t* keystates, float& outDirection);
	static void UpdatePosition(float speed, float direction, Vector2& outPosition, float deltaTime);
};