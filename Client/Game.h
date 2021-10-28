#pragma once

class Game
{
public:
	Game();
	~Game();

	bool Init();
	void Run();
	void Shutdown();

private:
	void ProcessInput();
	void Update();
	void Render();

private:
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	uint32_t mTicksCount;
	bool mIsRunning;
};
