#include "NPC.h"
#include <cassert>
#include "ModuleSceneManager.h"
#include "Scene.h"
#include "State.h"
#include <fstream>
#include "ModuleTextures.h"

using json = nlohmann::json;

NPC::NPC()
{
}

NPC::~NPC()
{
}

void NPC::PreUpdate()
{
	prepare_input(_last_input);
	State* newState = _state->HandleInput(*this, _last_input);

	if (newState)
		SwitchState(newState);
}

void NPC::Update()
{
	State* newState = _state->Update(*this, _last_input);

	if (newState)
		SwitchState(newState);

	correct_position();

	Sprite* rect = &current_animation.GetCurrentFrame();

	int positionX = Position.x;
	int colliderX = Position.x;
	int attackX = Position.x;

	if (flip)
	{
		positionX += rect->Pivot.x - rect->Rect.w + center;
		colliderX = positionX;
		if (attack_collider)
			attackX = positionX;
	}
	else
	{
		positionX -= rect->Pivot.x;
		if (attack_collider)
			attackX += rect->Rect.w - attack_collider->rect.w;
	}

	int positionY = Position.y + rect->Pivot.y;

	if (FeetCollider)
		FeetCollider->SetPos(colliderX, Position.y + FeetCollider->rect.h, Position.z);

	if (attack_collider) // TODO: Correct collider position
		attack_collider->SetPos(attackX, Position.y + FeetCollider->rect.h, Position.z);

	if (_revive_timer)
	{
		if (_revive_timer->finished)
		{
			_draw = 0;
			RELEASE(_revive_timer);
		}
		else
			_draw = (_draw + 1) % 15;
	}

	if (_draw < 7)
		App->renderer->Blit(graphics, positionX, positionY, Position.z, &rect->Rect, 1.f, flip);
}

void NPC::PostUpdate()
{
}

bool NPC::CleanUp()
{
	_animations.clear();
	//RELEASE(_state);
	RELEASE(Idle);
	RELEASE(Walking);
	RELEASE(Jump);
	RELEASE(Attack);
	RELEASE(Damaged);
	RELEASE(Dead);
	RELEASE(_revive_timer);

	if (FeetCollider)
		FeetCollider->to_delete = true;
	if (attack_collider)
		attack_collider->to_delete = true;

	return true;
}

bool NPC::OnCollision(Collider& origin, Collider& other)
{
	return true;
}

void NPC::LoadFromJson(const char* file)
{
	ifstream in(file);
	json js;
	in >> js;

	LoadFromJson(js);
}

void NPC::LoadFromJson(nlohmann::json& js)
{
	json color_key = js["color_key"];
	SDL_Color key = { color_key[0], color_key[1], color_key[2], color_key[3] };

	string sheet = js["sheet"];
	graphics = App->textures->Load((string("Simpsons/") + sheet).c_str(), &key);

	life = js["life"];
	center = js["center"];
	Speed = js["speed"];

	json animations = js["animations"];
	for (json::iterator it = animations.begin(); it != animations.end(); ++it)
	{
		json animation = (*it);
		SpriteAnimation anim;

		for (auto frame : animation["frames"])
		{
			auto jrect = frame["rect"];
			SDL_Rect rect = { jrect[0], jrect[1], jrect[2], jrect[3] };
			auto jpivot = frame["pivot"];
			iPoint pivot(jpivot[0], jpivot[1]);
			anim.frames.push_back({ rect, pivot });
		}

		anim.speed = animation["speed"];
		anim.loop = animation["loop"];

		AddAnimation(it.key(), anim);
	}
}

void NPC::AddAnimation(const string& name, const SpriteAnimation& animation)
{
	_animations[name] = animation;
}

bool NPC::SetAnimation(const string& name)
{
	auto it = _animations.find(name);

	bool ret = it != _animations.end();

	if (ret)
		current_animation = it->second;
	else
		LOG("Animation %s does not exist!", name.c_str());

	return ret;
}

SpriteAnimation* NPC::CurrentAnimation()
{
	return &current_animation;
}

void NPC::SetDirection(int x)
{
	if (x)
		flip = x < 0;
}

bool NPC::LooksRight() const
{
	return !flip;
}

void NPC::SwitchState(State* newState)
{
	assert(newState != nullptr);

	_state = newState;
	_state->Enter(*this, _last_input);
}

void NPC::BeginAttack()
{
	assert(attack_collider == nullptr);

	Damage = INVINCIBLE;
	attack_collider = App->collision->AddCollider(FeetCollider->rect, this);
	attack_collider->type = PLAYER_ATTACK;
}

void NPC::FinishAttack()
{
	assert(attack_collider != nullptr);

	Damage = NONE;
	attack_collider->to_delete = true;
	attack_collider = nullptr;
}

void NPC::ReceiveAttack(int damage)
{
	if (Damage == NONE && Damage != INVINCIBLE && !_revive_timer)
	{
		life -= damage;
		Damage = damage == 1 && (rand() % 3 <= 2) ? SLIGHT : STRONG;

		if (Damaged)
			SwitchState(Damaged);
	}
}

void NPC::Die()
{
	if (FeetCollider)
		FeetCollider->to_delete = true;
}

bool NPC::IsAlive() const
{
	return life > 0 && Damage != REVIVING;
}

int NPC::GetLife() const
{
	return life;
}

void NPC::Revive()
{
	_revive_timer = App->timer->AddTimer(2 * 1000);
}

void NPC::correct_position()
{
}

