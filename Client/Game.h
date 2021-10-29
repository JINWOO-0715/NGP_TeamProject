#pragma once

class Entity;

class Game
{
	friend class Entity;

public:
	static Game* Instance();

	bool Init(int w = 1024, int h = 768);
	void Run();
	void Shutdown();

	Entity* CreateEntity();

	SDL_Renderer* GetRenderer() { return mRenderer; }

	int GetWindowWidth() const { return mWindowWidth; }
	int GetWindowHeight() const { return mWindowHeight; }

private:
	Game();

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
	bool mIsRunning;

	entt::registry mRegistry;

	vector<Entity*> mEntities;
};
