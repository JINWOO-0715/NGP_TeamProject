#pragma once

struct TransformComponent
{
	TransformComponent()
		: Position(Vector2::Zero) {}

	TransformComponent(const Vector2& position)
		: Position(position) {}

	TransformComponent(float x, float y)
		: Position(Vector2(x, y)) {}

	Vector2 Position;
};

struct PaddleComponent
{
	PaddleComponent()
		: Width(0.0f)
		, Height(0.0f) {}

	PaddleComponent(const Vector2& vec)
		: Width(vec.x)
		, Height(vec.y) {}

	PaddleComponent(float w, float h)
		: Width(w)
		, Height(h) {}

	float Width;
	float Height;
};