#pragma once

class Client : public Game
{
public:
	Client();

	virtual bool Init() override;
	virtual void Shutdown() override;

	virtual void Run() override;

	SDL_Renderer* GetRenderer() { return mRenderer; }

	int GetWindowWidth() const { return mWindowWidth; }
	int GetWindowHeight() const { return mWindowHeight; }

private:
	bool NetworkInit();
	void ProcessInput();
	void Update();
	void Render();
	void LoadData();

private:
	int mWindowWidth;
	int mWindowHeight;

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	uint32_t mTicksCount;

	TCPSocketPtr mClientSocket;
};