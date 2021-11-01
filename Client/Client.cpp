#include "ClientPCH.h"
#include "Client.h"

Client::Client()
	: Game()
	, mWindowWidth(0)
	, mWindowHeight(0)
	, mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
{

}

bool Client::Init()
{
	Game::Init();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindowWidth = 1024;
	mWindowHeight = 768;

	mWindow = SDL_CreateWindow("Pong", 100, 100, mWindowWidth, mWindowHeight, 0);
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

void Client::Shutdown()
{
	Game::Shutdown();

	IMG_Quit();
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Client::ProcessInput()
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

void Client::Update()
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

void Client::Render()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mRenderer);

	auto view = mRegistry.view<PaddleComponent, TransformComponent>();
	for (auto entity : view)
	{
		auto [paddle, transform] = view.get<PaddleComponent, TransformComponent>(entity);

		Systems::DrawRect(mRenderer, paddle.Width, paddle.Height, transform.Position);
	}

	SDL_RenderPresent(mRenderer);
}

void Client::LoadData()
{
	auto e = CreateEntity();
	e->AddComponent<PaddleComponent>(15.0f, 100.0f);
	e->AddComponent<MovementComponent>(100.0f);
	e->AddTag<Paddle>();
}
