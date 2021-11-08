#include "ServerPCH.h"
#include "Server.h"

Server::Server()
	: Game()
	, mNumPlayers(0)
{
	mIsRecvPacket.fill(false);
}

bool Server::Init()
{
	SocketUtil::StaticInit();

	CreateGameWorld();

	mListenThread = { std::thread(&Server::ListenThreadFunc, this) };

	return true;
}

void Server::Shutdown()
{
	SocketUtil::StaticShutdown();
}

void Server::Run()
{
	while (mIsRunning)
	{
		std::lock_guard<std::mutex> guard(m);
		bool isAllDataRecv = std::all_of(mIsRecvPacket.begin(), mIsRecvPacket.end(), [](bool value) { return value; });
		if (isAllDataRecv)
		{
			UpdatePaddlesPosition();
			UpdateBallsPosition();
			CheckBallAndPaddle();
			CheckBallAndWall();

			SendPacketToClient(PacketType::Replicate);

			mIsRecvPacket.fill(false);
		}
	}
}

void Server::ListenThreadFunc()
{
	TCPSocketPtr listenSock = SocketUtil::CreateTCPSocket();
	SocketAddress addr(SERVER_IP, SERVER_PORT);

	if (listenSock->Bind(addr) == SOCKET_ERROR)
	{
		exit(EXIT_FAILURE);
	}

	if (listenSock->Listen() == SOCKET_ERROR)
	{
		exit(EXIT_FAILURE);
	}

	SocketAddress clientAddr;

	while (true)
	{
		TCPSocketPtr clientSock = listenSock->Accept(clientAddr);
		LOG("Hello, client! {0}", clientAddr.ToString());

		if (mNumPlayers >= MAXIMUM_PLAYER_NUM)
		{
			LOG("Pong does not support more than {0} players.", MAXIMUM_PLAYER_NUM);
			clientSock.reset();
			continue;
		}

		mClientThreads[mNumPlayers] = { std::thread(&Server::ClientThreadFunc, this, clientSock, mNumPlayers) };
		mNumPlayers++;

		mClientSockets.push_back(clientSock);
	}
}

void Server::ClientThreadFunc(const TCPSocketPtr& clientSock, int clientNum)
{
	LOG("My Client number : {0}", clientNum);

	SendPacketToClient(PacketType::Hello, clientSock);

	ClientToServer packet;
	while (true)
	{
		ZeroMemory(&packet, sizeof(packet));

		int err = clientSock->Recv(&packet, sizeof(packet));

		if (err > 0)
		{
			std::lock_guard<std::mutex> guard(m);
			mPacketsFromClientThread[clientNum] = packet;
			mIsRecvPacket[clientNum] = true;
		}
	}
}

void Server::CreateGameWorld()
{
	// Create left paddle
	{
		auto paddle = CreatePaddle();
		auto& id = paddle->AddComponent<IdComponent>(LEFT_PADDLE_ID);
		auto& transform = paddle->GetComponent<TransformComponent>();
		transform.Position = Vector2(0.0f, (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2));
		mEntities[id.ID] = paddle;
	}

	// Create right paddle
	{
		auto paddle = CreatePaddle();
		auto& id = paddle->AddComponent<IdComponent>(RIGHT_PADDLE_ID);
		auto& transform = paddle->GetComponent<TransformComponent>();
		transform.Position = Vector2((WINDOW_WIDTH - PADDLE_WIDTH), (WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2));
		mEntities[id.ID] = paddle;
	}

	// Create Ball one
	{
		auto ball = CreateBall();
		auto& id = ball->AddComponent<IdComponent>(BALL_ONE_ID);
		auto& transform = ball->GetComponent<TransformComponent>();
		auto& movement = ball->GetComponent<MovementComponent>();
		transform.Position = Vector2((WINDOW_WIDTH / 2) - (BALL_WIDTH / 2), (WINDOW_HEIGHT / 2) - (BALL_WIDTH / 2));  // Center of screen
		movement.Direction = Vector2(-1.0f, -1.0f); // Up-Left
		mEntities[id.ID] = ball;
	}

	// Create Ball two
	{
		auto ball = CreateBall();
		auto& id = ball->AddComponent<IdComponent>(BALL_TWO_ID);
		auto& transform = ball->GetComponent<TransformComponent>();
		auto& movement = ball->GetComponent<MovementComponent>();
		transform.Position = Vector2((WINDOW_WIDTH / 2) - (BALL_WIDTH / 2), (WINDOW_HEIGHT / 2) - (BALL_WIDTH / 2));  // Center of screen
		movement.Direction = Vector2(1.0f, 1.0f); // Down-Right
		movement.Speed = 200.0f;
		mEntities[id.ID] = ball;
	}
}

void Server::UpdatePaddlesPosition()
{
	for (int i = 0; i < mPacketsFromClientThread.size(); i++)
	{
		const ClientToServer& packet = mPacketsFromClientThread[i];

		auto paddle = mEntities[LEFT_PADDLE_ID + i];

		auto& transform = paddle->GetComponent<TransformComponent>();
		auto& movement = paddle->GetComponent<MovementComponent>();

		Systems::UpdatePosition(movement.Speed, Vector2(0.0f, packet.YDirection), transform.Position, 0.016f);
	}
}

void Server::UpdateBallsPosition()
{
	for (int i = BALL_ONE_ID; i <= BALL_TWO_ID; i++)
	{
		auto ball = mEntities[i];

		auto& transform = ball->GetComponent<TransformComponent>();
		auto& movement = ball->GetComponent<MovementComponent>();

		Systems::UpdatePosition(movement.Speed, movement.Direction, transform.Position, 0.016f);
	}
}

void Server::CheckBallAndPaddle()
{
	for (uint8_t i = LEFT_PADDLE_ID; i <= RIGHT_PADDLE_ID; i++)
	{
		auto paddle = mEntities[i];

		auto& paddleTransform = paddle->GetComponent<TransformComponent>();
		auto& paddleRect = paddle->GetComponent<RectComponent>();

		SDL_Rect pRect = {
			static_cast<int>(paddleTransform.Position.x), static_cast<int>(paddleTransform.Position.y),
			static_cast<int>(paddleRect.Width), static_cast<int>(paddleRect.Height)
		};

		for (uint8_t j = BALL_ONE_ID; j <= BALL_TWO_ID; j++)
		{
			auto ball = mEntities[j];

			auto& ballTransform = ball->GetComponent<TransformComponent>();
			auto& ballRect = ball->GetComponent<RectComponent>();

			SDL_Rect bRect = {
			static_cast<int>(ballTransform.Position.x), static_cast<int>(ballTransform.Position.y),
			static_cast<int>(ballRect.Width), static_cast<int>(ballRect.Height)
			};

			bool isCollide = Systems::Intersects(pRect, bRect);

			if (isCollide)
			{
				auto& ballMovement = ball->GetComponent<MovementComponent>();
				ballMovement.Direction *= -1;
			}
		}
	}
}

void Server::CheckBallAndWall()
{
	for (uint8_t i = BALL_ONE_ID; i <= BALL_TWO_ID; i++)
	{
		auto ball = mEntities[i];

		auto& transform = ball->GetComponent<TransformComponent>();
		auto& movement = ball->GetComponent<MovementComponent>();

		const auto& ballPos = transform.Position;
		if (ballPos.x <= 0 || ballPos.x + BALL_WIDTH > WINDOW_WIDTH)
		{
			movement.Direction.x *= -1.0f;
		}

		if (ballPos.y <= 0 || ballPos.y + BALL_WIDTH > WINDOW_HEIGHT)
		{
			movement.Direction.y *= -1.0f;
		}
	}
}

void Server::SendPacketToClient(PacketType pType, const TCPSocketPtr& clientSock /*= nullptr*/)
{
	ServerToClient packet;

	packet.PType = pType;

	packet.LeftPaddleID = LEFT_PADDLE_ID;
	packet.LeftPaddlePosition = mEntities[LEFT_PADDLE_ID]->GetComponent<TransformComponent>().Position;
	packet.RightPaddleID = RIGHT_PADDLE_ID;
	packet.RightPaddlePosition = mEntities[RIGHT_PADDLE_ID]->GetComponent<TransformComponent>().Position;
	packet.BallOneID = BALL_ONE_ID;
	packet.BallOnePosition = mEntities[BALL_ONE_ID]->GetComponent<TransformComponent>().Position;
	packet.BallTwoID = BALL_TWO_ID;
	packet.BallTwoPosition = mEntities[BALL_TWO_ID]->GetComponent<TransformComponent>().Position;

	if (clientSock == nullptr) // Send packet to all clients
	{
		for (const auto& sock : mClientSockets)
		{
			sock->Send(&packet, sizeof(packet));
		}
	}
	else
	{
		clientSock->Send(&packet, sizeof(packet));
	}
}
