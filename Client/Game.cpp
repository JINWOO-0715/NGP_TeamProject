#include "PCH.h"
#include "Game.h"

#include "Entity.h"

Game::Game()
	: mWindowWidth(0)
	, mWindowHeight(0)
	, mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
	, mIsRunning(true)
{

}

Game* Game::Instance()
{
	static Game instance;
	return &instance;
}

bool Game::Init(int w, int h)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindowWidth = w;
	mWindowHeight = h;

	mWindow = SDL_CreateWindow("Untitled", 100, 100, w, h, 0);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
		return false;
	}

	mTicksCount = SDL_GetTicks();

	LoadData();

	return true;
}

void Game::Run()
{
	while (mIsRunning)
	{
		ProcessInput();
		Update();
		Render();
	}
}

void Game::Shutdown()
{
	for (auto e : mEntities)
	{
		delete e;
	}
	mEntities.clear();

	IMG_Quit();
	//SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

Entity* Game::CreateEntity()
{
	Entity* e = new Entity{ mRegistry.create(), this };
	e->AddComponent<TransformComponent>();
	mEntities.push_back(e);
	return e;
}

Entity* Game::CreatePaddle(float w, float h, float speed)
{
	Entity* e = CreateEntity();
	e->AddTag<Paddle>();
	e->AddComponent<PaddleComponent>(w, h);
	e->AddComponent<MovementComponent>(speed);
	return e;
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const uint8_t* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	auto view = mRegistry.view<Paddle>();
	for (auto entity : view)
	{
		Entity e = { entity, this };

		MovementComponent& movement = e.GetComponent<MovementComponent>();

		Systems::UpdateDirection(state, movement.Direction);
	}
}

void Game::Update()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	auto view = mRegistry.view<Paddle>();
	for (auto entity : view)
	{
		Entity e = { entity, this };

		MovementComponent& movement = e.GetComponent<MovementComponent>();
		TransformComponent& transform = e.GetComponent<TransformComponent>();

		Systems::UpdatePosition(movement.Speed, movement.Direction, transform.Position, deltaTime);
	}
}

void Game::Render()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mRenderer);

	auto view = mRegistry.view<PaddleComponent, TransformComponent>();
	for (auto entity : view)
	{
		auto [paddle, transform] = view.get<PaddleComponent, TransformComponent>(entity);

		Systems::DrawPaddle(paddle.Width, paddle.Height, transform.Position);
	}

	SDL_RenderPresent(mRenderer);
}

void Game::LoadData()
{
	CreatePaddle();
}
