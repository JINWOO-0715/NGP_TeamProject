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
	LOG("Im on my way!");
}
