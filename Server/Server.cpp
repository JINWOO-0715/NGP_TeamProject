#include "ServerPCH.h"
#include "Server.h"

Server::Server()
	: Game()
	, mNumPlayers(0)
{

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

		mClientThreads[mNumPlayers] = { std::thread(&Server::ClientThreadFunc, this, clientSock) };
		mNumPlayers++;
	}
}

void Server::ClientThreadFunc(const TCPSocketPtr& clientSock)
{
	// Send HelloPacket to client
	ServerToClient packet;

	auto leftPaddle = mEntities[LEFT_PADDLE_ID];
	auto rightPaddle = mEntities[RIGHT_PADDLE_ID];

	packet.LeftPaddleID = LEFT_PADDLE_ID;
	packet.LeftPaddleBType = BehaviorType::Create;
	packet.LeftPaddlePosition = leftPaddle->GetComponent<TransformComponent>().Position;

	packet.RightPaddleID = RIGHT_PADDLE_ID;
	packet.RightPaddleBType = BehaviorType::Create;
	packet.RightPaddlePosition = rightPaddle->GetComponent<TransformComponent>().Position;

	clientSock->Send(&packet, sizeof(packet));

	while (true)
	{

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
}
