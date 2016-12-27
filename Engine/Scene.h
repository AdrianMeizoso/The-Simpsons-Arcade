#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Entity.h"

class Scene : public Module
{
public:
	Scene(bool active) : Module(active) {}
	virtual ~Scene() {}

	void AddEntity(Entity* entity)
	{
		_entities.push_back(entity);
	}

	void RemoveEntity(Entity* entity)
	{
		_entities.remove(entity);
	}


	update_status PreUpdate() override
	{
		for (std::list<Entity*>::iterator it = _entities.begin(); it != _entities.end(); ++it)
			if ((*it)->IsEnabled())
				(*it)->PreUpdate();
		return UPDATE_CONTINUE;
	}

	update_status Update() override
	{
		for (std::list<Entity*>::iterator it = _entities.begin(); it != _entities.end(); ++it)
			if ((*it)->IsEnabled())
				(*it)->Update();
		return UPDATE_CONTINUE;
	}

	update_status PostUpdate() override
	{
		for (std::list<Entity*>::iterator it = _entities.begin(); it != _entities.end(); ++it)
			if ((*it)->IsEnabled())
				(*it)->PostUpdate();
		return UPDATE_CONTINUE;
	}

	bool CleanUp() override
	{
		for (std::list<Entity*>::iterator it = _entities.begin(); it != _entities.end(); ++it)
		{
			(*it)->CleanUp();
			RELEASE((*it));
		}
		_entities.clear();

		return true;
	}

protected:
	std::list<Entity*> _entities;

};

#endif // __SCENE_H__