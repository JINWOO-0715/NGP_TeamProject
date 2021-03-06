#include "ClientPCH.h"
#include "Client.h"

Client::Client()
	: Game()
	, mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
	, mClientSocket(nullptr)
	, mIsGameStart(false)
{

}

bool Client::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("Pong", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
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

	if (NetworkInit() == false)
	{
		LOG("Unable to initialize Network");
		return false;
	}

	mTicksCount = SDL_GetTicks();

	return true;
}

void Client::Shutdown()
{
	SocketUtil::StaticShutdown();

	IMG_Quit();
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Client::Run()
{
	while (mIsRunning)
	{
		ProcessInput();
		Update();
		Render();
	}
}

bool Client::NetworkInit()
{
	SocketUtil::StaticInit();

	mClientSocket = SocketUtil::CreateTCPSocket();

	SocketAddress serveraddr(SERVER_IP, SERVER_PORT);
	if (mClientSocket->Connect(serveraddr) != SOCKET_ERROR)
	{
		RecvPacketFromServer();
		return true;
	}
	else
	{
		return false;
	}
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

	SendPacketToServer(state);
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

	RecvPacketFromServer();
}

void Client::Render()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mRenderer);

	auto view = mRegistry.view<RectComponent, TransformComponent>();
	for (auto entity : view)
	{
		auto [paddle, transform] = view.get<RectComponent, TransformComponent>(entity);

		Systems::DrawRect(mRenderer, paddle.Width, paddle.Height, transform.Position);
	}

	SDL_RenderPresent(mRenderer);
}

void Client::RecvPacketFromServer()
{
	ServerToClient packet;

	int err = mClientSocket->Recv(&packet, sizeof(packet), MSG_WAITALL);

	if (err <= 0)
	{
		exit(EXIT_FAILURE);
	}

	switch (packet.PType)
	{
	case PacketType::Hello:
		ProcessHelloPacket(packet);
		break;

	case PacketType::Replicate:
		ProcessReplicatePacket(packet);
		break;

	case PacketType::GameOver:
		ProcessGameOverPacket(packet);
		break;

	default:
		break;
	}
}

void Client::SendPacketToServer(const uint8_t* keystate)
{
	ClientToServer packet{ 0.0f };

	if (keystate[SDL_SCANCODE_W])
	{
		packet.YDirection -= 1.0f;
	}

	if (keystate[SDL_SCANCODE_S])
	{
		packet.YDirection += 1.0f;
	}

	int err = mClientSocket->Send(&packet, sizeof(packet));

	if (err == SOCKET_ERROR)
	{
		exit(EXIT_FAILURE);
	}
}

void Client::ProcessHelloPacket(const ServerToClient& packet)
{
	// Create left paddle
	{
		auto paddle = CreatePaddle();
		auto& id = paddle->AddComponent<IdComponent>(packet.LeftPaddleID);
		auto& transform = paddle->GetComponent<TransformComponent>();
		transform.Position = packet.LeftPaddlePosition;
		mEntities[id.ID] = paddle;
	}

	// Create right paddle
	{
		auto paddle = CreatePaddle();
		auto& id = paddle->AddComponent<IdComponent>(packet.RightPaddleID);
		auto& transform = paddle->GetComponent<TransformComponent>();
		transform.Position = packet.RightPaddlePosition;
		mEntities[id.ID] = paddle;
	}

	// Create ball one
	{
		auto ball = CreateBall();
		auto& id = ball->AddComponent<IdComponent>(packet.BallOneID);
		auto& transform = ball->GetComponent<TransformComponent>();
		transform.Position = packet.BallOnePosition;
		mEntities[id.ID] = ball;
	}

	// Create ball two
	{
		auto ball = CreateBall();
		auto& id = ball->AddComponent<IdComponent>(packet.BallTwoID);
		auto& transform = ball->GetComponent<TransformComponent>();
		transform.Position = packet.BallTwoPosition;
		mEntities[id.ID] = ball;
	}
}

void Client::ProcessReplicatePacket(const ServerToClient& packet)
{
	// Update all entities' position
	auto leftPaddle = mEntities[packet.LeftPaddleID];
	leftPaddle->GetComponent<TransformComponent>().Position = packet.LeftPaddlePosition;

	auto rightPaddle = mEntities[packet.RightPaddleID];
	rightPaddle->GetComponent<TransformComponent>().Position = packet.RightPaddlePosition;

	auto ballOne = mEntities[packet.BallOneID];
	ballOne->GetComponent<TransformComponent>().Position = packet.BallOnePosition;

	auto ballTwo = mEntities[packet.BallTwoID];
	ballTwo->GetComponent<TransformComponent>().Position = packet.BallTwoPosition;
}

void Client::ProcessGameOverPacket(const ServerToClient& packet)
{

}

