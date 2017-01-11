#include "Player.h"
#include "Engine.h"
#include "ModuleRender.h"
#include <cassert>
#include "State.h"
#include "ModuleInput.h"
#include "Scene.h"
#include "ModuleSceneManager.h"
#include "ModuleAudio.h"

Player::Player()
{
}

Player::~Player()
{
}

bool Player::Start()
{
	FxDizzy = App->audio->LoadFx("Simpsons/audio/26-player-dead.wav");
	return FxDizzy != -1;
}

void Player::Update()
{
	if (App->scene_manager->CurrentScene()->Finished() && !_win)
	{
		Position.y = 0;
		_win = true;
		SwitchState(Win);
	}

	NPC::Update();
}

bool Player::CleanUp()
{
	RELEASE(Revive);
	RELEASE(Win);

	return NPC::CleanUp();
}

bool Player::OnCollision(Collider& origin, Collider& other)
{
	if (origin.type == PLAYER_ATTACK && other.type == ENEMY)
	{
		NPC* enemy = static_cast<NPC*>(other.attached);
		enemy->ReceiveAttack(calculate_damage());
		return false;
	}

	return true;
}

void Player::Die()
{
	--Lifes;

	if (Lifes < 0)
	{
		Disable();
		NPC::Die();
	}
	else
	{
		life = 10;
		SwitchState(Revive);
	}
}

void Player::correct_position()
{
	pair<int, int> x, z;

	App->scene_manager->CurrentScene()->SceneLimits(x, z);

	if (Position.x < x.first)
		Position.x = x.first;
	else if (Position.x > x.second)
		Position.x = x.second;

	if (Position.z < z.first)
		Position.z = z.first;
	else if (Position.z > z.second)
		Position.z = z.second;
}

void Player::prepare_input(Input& input)
{
	input.x = App->input->GetAxis(PlayerNumber, X);
	input.y = App->input->GetAxis(PlayerNumber, Y);

	input.attack = App->input->GetPlayerKey(PlayerNumber, ATTACK);
	input.jump = App->input->GetPlayerKey(PlayerNumber, JUMP);
}

int Player::calculate_damage()
{
	int r = rand() % 2 + 1;

	if (r == 2)
		r = rand() % 2 + 1;

	return r;
}
