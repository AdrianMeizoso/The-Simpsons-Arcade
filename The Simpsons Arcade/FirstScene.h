#ifndef __FIRSTSCENE_H__
#define __FIRSTSCENE_H__

#include "Scene.h"
#include "ModuleTimer.h"

struct SDL_Texture;

typedef std::pair<int, std::list<Entity*>*> PILE;

class FirstScene :
	public Scene
{
public:
	FirstScene(bool active);
	~FirstScene();

	bool Start() override;
	update_status Update() override;
	bool CleanUp() override;

	bool Finished() const override;
	bool GameOver() const override;
private:
	void initialize_scene();
	bool can_advance();

	SDL_Texture* _background;
	SDL_Rect _floor, _walls;
	std::queue<PILE> _stages;
	ModuleTimer::Timer* _end_timer = nullptr;
};

#endif // __FIRSTSCENE_H__
