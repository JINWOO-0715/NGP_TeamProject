#pragma once

class Game
{
	friend class Entity;

public:
	Game();
	~Game();

	bool Init();
	void Run();
	void Shutdown();

	Entity CreateEntity();

private:
	void ProcessInput();
	void Update();
	void Render();

	void LoadData();

private:
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	uint32_t mTicksCount;
	bool mIsRunning;

	entt::registry mRegistry;
};
