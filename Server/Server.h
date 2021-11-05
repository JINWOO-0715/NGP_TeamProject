#pragma once

#include <thread>

class Server : public Game
{
public:
	Server();

	virtual bool Init() override;
	virtual void Shutdown() override;

	virtual void Run() override;

private:
	void ListenThreadFunc();
	void ClientThreadFunc(const TCPSocketPtr& clientSock);

private:
	static const int MAXIMUM_PLAYER_NUM = 2;

	std::thread mListenThread;
	
	std::thread mClientThreads[MAXIMUM_PLAYER_NUM];

	uint8_t mNumPlayers;
};