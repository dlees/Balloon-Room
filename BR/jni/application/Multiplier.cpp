#include "Multiplier.h"

#include "Player.h"

#include "Game_Model.h"
#include <zenilib.h>

using namespace Zeni;


const int points_worth_c = 10;


Multiplier::~Multiplier(void)
{
}

Multiplier::Multiplier(Player *p) : 
	in_a_row(1), player(p)
{
	
}

Multiplier::Multiplier(const Multiplier &m, const Point2f &pos) 
	: in_a_row(m.in_a_row + 1), player(m.player)
{

	Game_Model::get().statics.push_back(
		Game_Model::Static(pos, 10+2));

	Game_Model::get().static_texts.push_back(
		Game_Model::Static_text(pos, 40
		, itoa(give_points()), Color()));
}

int Multiplier::give_points()
{
	int points = Game_Model::get().get_active_balloons() * points_worth_c * in_a_row;
	player->add_points(points);
	
	return points;
}
