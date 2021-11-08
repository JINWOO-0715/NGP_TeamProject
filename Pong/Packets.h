#pragma once

constexpr uint8_t LEFT_PADDLE_ID = 0;
constexpr uint8_t RIGHT_PADDLE_ID = 1;
constexpr uint8_t BALL_ONE_ID = 2;
constexpr uint8_t BALL_TWO_ID = 3;

enum class PacketType
{
	Hello,
	Replicate,
	GameOver
};

struct ServerToClient
{
	PacketType PType;

	// Left paddle
	uint8_t LeftPaddleID;
	Vector2 LeftPaddlePosition;

	// Right paddle
	uint8_t RightPaddleID;
	Vector2 RightPaddlePosition;

	// Ball One
	uint8_t BallOneID;
	Vector2 BallOnePosition;

	// Ball Two
	uint8_t BallTwoID;
	Vector2 BallTwoPosition;
};

struct ClientToServer
{
	float YDirection;
};