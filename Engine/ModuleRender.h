#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"
#include <queue>
#include <ios>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	struct RenderData
	{
		SDL_Texture* texture;
		SDL_Rect* section;
		SDL_Rect* rect;
	};

	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	bool BlitBackground(SDL_Texture* texture, int x, int y, SDL_Rect* section, float speed = 1.0f);
	bool Blit(SDL_Texture* texture, int x, int y, int z, SDL_Rect* section, float speed = 1.0f);
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera = true);

public:
	SDL_Renderer* renderer = nullptr;
	SDL_Rect camera;
	
	std::queue<RenderData*> _background;
	std::priority_queue<std::pair<int, RenderData*>> _foreground;
};

#endif // __MODULERENDER_H__