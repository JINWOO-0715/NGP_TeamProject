#include "ServerPCH.h"
#include "Server.h"

Server::Server()
	: Game()
	, mListenSocket(nullptr)
{

}

bool Server::Init()
{
	Game::Init();

	SocketUtil::StaticInit();

	mListenSocket = SocketUtil::CreateTCPSocket();
	SocketAddress addr("127.0.0.1", 9000);

	if (mListenSocket->Bind(addr) == SOCKET_ERROR)
	{
		LOG("Cannot bind listen socket");
		return false;
	}

	if (mListenSocket->Listen() == SOCKET_ERROR)
	{
		LOG("Cannot change socket to listening state");
		return false;
	}

	mSockets.push_back(mListenSocket);

	return true;
}

void Server::Shutdown()
{
	SocketUtil::StaticShutdown();
}

void Server::Run()
{
	vector<TCPSocketPtr> readSockets;

	while (mIsRunning)
	{
		if (SocketUtil::Select(&mSockets, &readSockets, nullptr, nullptr, nullptr, nullptr) <= 0)
		{
			continue;
		}

		for (const auto& sock : readSockets)
		{
			if (sock == mListenSocket)
			{
				SocketAddress clientAddr;
				TCPSocketPtr clientSock = mListenSocket->Accept(clientAddr);
				mSockets.push_back(clientSock);

				LOG("Hello, client! {0}", clientAddr.ToString());

				// TODO :: Do something when client connects
			}
			else
			{
				// TODO :: Process packet sent from client
			}
		}
	}
}
