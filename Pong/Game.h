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

private:
	vector<Entity*> mEntities;
};
