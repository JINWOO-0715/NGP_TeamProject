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
	const float PADDLE_WIDTH = 15.0f;
	const float PADDLE_HEIGHT = 100.0f;
	const float PADDLE_SPEED = 200.0f;
	const float BALL_WIDTH = 15.0f;
	const float BALL_SPEED = 150.0f;

	unordered_map<uint8_t, Entity*> mEntities;
};
