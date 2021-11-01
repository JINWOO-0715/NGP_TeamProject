#pragma once

class Client : public Game
{
public:
	Client();

	virtual bool Init() override;
	virtual void Shutdown() override;

	SDL_Renderer* GetRenderer() { return mRenderer; }

	int GetWindowWidth() const { return mWindowWidth; }
	int GetWindowHeight() const { return mWindowHeight; }

protected:
	virtual void ProcessInput() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void LoadData() override;

private:
	int mWindowWidth;
	int mWindowHeight;

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	uint32_t mTicksCount;
};