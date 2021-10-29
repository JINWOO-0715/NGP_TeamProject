#include "PCH.h"

int main()
{
	auto game = Game::Instance();

	bool res = game->Init();
	if (!res)
	{
		LOG("Could not initialize game.");
	}
	else
	{
		game->Run();
	}

	game->Shutdown();

	return 0;
}