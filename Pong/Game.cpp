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
	mEntities.push_back(e);
	return e;
}

Entity* Game::CreatePaddle()
{
	auto e = CreateEntity();
	e->AddComponent<RectComponent>(15.0f, 100.0f);
	e->AddComponent<MovementComponent>(200.0f);
	e->AddTag<Paddle>();
	mEntities.push_back(e);
	return e;
}

Entity* Game::CreateBall()
{
	auto e = CreateEntity();
	e->AddComponent<RectComponent>(15.0f, 15.0f);
	e->AddTag<Ball>();
	mEntities.push_back(e);
	return e;
}
