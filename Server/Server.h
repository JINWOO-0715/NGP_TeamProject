#pragma once

class Server : public Game
{
public:
	Server();

	virtual bool Init() override;
	virtual void Shutdown() override;

	virtual void Run() override;

private:
	vector<TCPSocketPtr> mSockets;
	TCPSocketPtr mListenSocket;
};