#pragma once

class Entity;

class Game
{
	friend class Entity;

public:
	Game();
	virtual ~Game() = default;

	virtual bool Init();
	virtual void Shutdown();

	virtual void Run() = 0;
	
	Entity* CreateEntity();
	Entity* CreatePaddle();
	Entity* CreateBall();

protected:
	bool mIsRunning;
	entt::registry mRegistry;

	const int SERVER_PORT = 9000;
	const char* SERVER_IP = "127.0.0.1";

	const int WINDOW_WIDTH = 1024;
	const int WINDOW_HEIGHT = 768;

private:
	vector<Entity*> mEntities;
};
