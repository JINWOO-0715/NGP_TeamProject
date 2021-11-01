#include "ClientPCH.h"
#include "Client.h"

int main()
{
	unique_ptr<Client> c = std::make_unique<Client>();

	bool res = c->Init();

	if (res)
	{
		c->Run();
	}

	c->Shutdown();

	return 0;
}