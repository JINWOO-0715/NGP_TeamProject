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
	
	void Run();

	Entity* CreateEntity();
	Entity* CreatePaddle();
	Entity* CreateBall();

protected:
	virtual void ProcessInput() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void LoadData() = 0;

	bool mIsRunning;
	entt::registry mRegistry;

private:
	vector<Entity*> mEntities;
};
