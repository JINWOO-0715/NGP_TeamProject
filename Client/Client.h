#pragma once

class Client : public Game
{
public:
	Client();

	virtual bool Init() override;
	virtual void Shutdown() override;

	virtual void Run() override;

	SDL_Renderer* GetRenderer() { return mRenderer; }

private:
	bool NetworkInit();
	void ProcessInput();
	void Update();
	void Render();

	void RecvPacketFromServer();
	void SendPacketToServer(const uint8_t* keystate);

	void ProcessHelloPacket(const ServerToClient& packet);
	void ProcessReplicatePacket(const ServerToClient& packet);
	void ProcessGameOverPacket(const ServerToClient& packet);

private:
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	uint32_t mTicksCount;

	TCPSocketPtr mClientSocket;

	bool mIsGameStart;
};