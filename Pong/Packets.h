#pragma once

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
};

struct ClientToServer
{

};