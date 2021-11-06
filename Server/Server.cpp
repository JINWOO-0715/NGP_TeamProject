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
			// Update each paddle's position
			for (int i = 0; i < mPacketsFromClientThread.size(); i++)
			{
				const ClientToServer& packet = mPacketsFromClientThread[i];

				auto paddle = mEntities[LEFT_PADDLE_ID + i];

				auto& transform = paddle->GetComponent<TransformComponent>();
				auto& movement = paddle->GetComponent<MovementComponent>();

				Systems::UpdatePosition(movement.Speed, Vector2(0.0f, packet.YDirection), transform.Position, 0.016f);
			}

			// TODO :: Update ball's position
			{
				auto ballOne = mEntities[BALL_ONE_ID];

				auto& transform = ballOne->GetComponent<TransformComponent>();
				auto& movement = ballOne->GetComponent<MovementComponent>();

				Systems::UpdatePosition(movement.Speed, movement.Direction, transform.Position, 0.016f);
			}

			// Send packet to all clients
			ServerToClient packet;
			packet.LeftPaddleID = LEFT_PADDLE_ID;
			packet.LeftPaddleBType = BehaviorType::Update;
			packet.LeftPaddlePosition = mEntities[LEFT_PADDLE_ID]->GetComponent<TransformComponent>().Position;
			packet.RightPaddleID = RIGHT_PADDLE_ID;
			packet.RightPaddleBType = BehaviorType::Update;
			packet.RightPaddlePosition = mEntities[RIGHT_PADDLE_ID]->GetComponent<TransformComponent>().Position;
			packet.BallOneID = BALL_ONE_ID;
			packet.BallOneBType = BehaviorType::Update;
			packet.BallOnePosition = mEntities[BALL_ONE_ID]->GetComponent<TransformComponent>().Position;

			for (const auto& clientSock : mClientSockets)
			{
				clientSock->Send(&packet, sizeof(packet));
			}

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

	// Send HelloPacket to client
	{
		ServerToClient packet;

		auto leftPaddle = mEntities[LEFT_PADDLE_ID];
		auto rightPaddle = mEntities[RIGHT_PADDLE_ID];
		auto ballOne = mEntities[BALL_ONE_ID];

		packet.LeftPaddleID = LEFT_PADDLE_ID;
		packet.LeftPaddleBType = BehaviorType::Create;
		packet.LeftPaddlePosition = leftPaddle->GetComponent<TransformComponent>().Position;

		packet.RightPaddleID = RIGHT_PADDLE_ID;
		packet.RightPaddleBType = BehaviorType::Create;
		packet.RightPaddlePosition = rightPaddle->GetComponent<TransformComponent>().Position;

		packet.BallOneID = BALL_ONE_ID;
		packet.BallOneBType = BehaviorType::Create;
		packet.BallOnePosition = ballOne->GetComponent<TransformComponent>().Position;

		clientSock->Send(&packet, sizeof(packet));
	}

	{
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
}
