#pragma once

#include <thread>
#include <mutex>

class Server : public Game
{
public:
	Server();

	virtual bool Init() override;
	virtual void Shutdown() override;

	virtual void Run() override;

private:
	void ListenThreadFunc();
	void ClientThreadFunc(const TCPSocketPtr& clientSock, int clientNum);

	void CreateGameWorld();

private:
	static const int MAXIMUM_PLAYER_NUM = 2;

	std::thread mListenThread;
	std::thread mClientThreads[MAXIMUM_PLAYER_NUM];

	uint8_t mNumPlayers;
	
	array<ClientToServer, MAXIMUM_PLAYER_NUM> mPacketsFromClientThread;
	array<bool, MAXIMUM_PLAYER_NUM> mIsRecvPacket;

	vector<TCPSocketPtr> mClientSockets;

	std::mutex m;
};