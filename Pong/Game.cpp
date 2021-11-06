#include "PongPCH.h"
#include "Game.h"

#include "Tags.h"
#include "Components.h"
#include "Entity.h"

Game::Game()
	: mIsRunning(true)
{

}

bool Game::Init()
{
	return true;
}

void Game::Shutdown()
{

}

Entity* Game::CreateEntity()
{
	Entity* e = new Entity{ mRegistry.create(), this };
	e->AddComponent<TransformComponent>();

	return e;
}

Entity* Game::CreatePaddle()
{
	auto e = CreateEntity();
	e->AddComponent<RectComponent>(PADDLE_WIDTH, PADDLE_HEIGHT);
	e->AddComponent<MovementComponent>(PADDLE_SPEED);
	e->AddTag<Paddle>();

	return e;
}

Entity* Game::CreateBall()
{
	auto e = CreateEntity();
	e->AddComponent<RectComponent>(BALL_WIDTH, BALL_WIDTH);
	e->AddComponent<MovementComponent>(BALL_SPEED);
	e->AddTag<Ball>();
	
	return e;
}
