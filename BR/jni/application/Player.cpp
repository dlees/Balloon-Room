#include "Player.h"

#include "Utility.h"
#include "Game_Model.h"

#include <zenilib.h>

using namespace Zeni;

const int Player::player_ID_c = 2;
int Player::num_players = 0;
Player::~Player(void)
{
	num_players--;
}

void Player::render() const
{
	Game_Object::render(image);
}

void Player::update()
{
	Game_Object::update();
	
	if (push > 0)
		push -= Game_Model::get().time_step() *4* start_push_c;
}


void Player::walk_left(float time_step)
{
	accelerate(time_step * 40.0f, left_c);
}

void Player::walk_right(float time_step)
{
	accelerate(time_step * 40.0f, right_c);
}


void Player::hands_up()
{
	if (!push_state)
	{
		image = "hands" + itoa(num);
		push_state = true;
		push = start_push_c;
		radius_factor = 1.1f;
	}
}

void Player::hands_down()
{
	image = "girl" + itoa(num);
	push_state = false;
	radius_factor = .9f;
}
