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

	PaddleComponent(float w, float h)
		: Width(w)
		, Height(h) {}

	float Width;
	float Height;
};

struct MovementComponent
{
	MovementComponent()
		: Speed(0.0f)
		, Direction(0.0f) {}

	MovementComponent(float speed)
		: Speed(speed)
		, Direction(0.0f) {}

	float Speed;
	float Direction;
};