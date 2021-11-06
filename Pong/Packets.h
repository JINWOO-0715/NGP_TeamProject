#pragma once

constexpr uint8_t LEFT_PADDLE_ID = 0;
constexpr uint8_t RIGHT_PADDLE_ID = 1;
constexpr uint8_t BALL_ONE_ID = 2;
constexpr uint8_t BALL_TWO_ID = 2;

enum class BehaviorType
{
	Create,
	Update,
	Delete,
};

struct ServerToClient
{
	// Left paddle
	uint8_t LeftPaddleID;
	BehaviorType LeftPaddleBType;
	Vector2 LeftPaddlePosition;

	// Right paddle
	uint8_t RightPaddleID;
	BehaviorType RightPaddleBType;
	Vector2 RightPaddlePosition;

	// Ball One
	uint8_t BallOneID;
	BehaviorType BallOneBType;
	Vector2 BallOnePosition;
};

struct ClientToServer
{
	float YDirection;
};