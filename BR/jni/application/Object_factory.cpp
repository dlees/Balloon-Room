#include "Object_factory.h"

#include <zenilib.h>

#include "Game_Object.h"
#include "Balloon.h"
#include "Player.h"
#include "Utility.h"

#include "Game_Model.h"

#include <vector>

using namespace Zeni;


Game_Object *create_ob(const String& type, const Point2f &init)
{
	int lowest_area = 1;

	if (Game_Model::get().difficulty < 3)
		lowest_area = 3;	

	else if (Game_Model::get().difficulty == 3)
		lowest_area = 2;


	if (type == "Small")
		return new Balloon(init, Vector2f(32,32), lowest_area, 30, "BL_balloon");

	else if (type == "Medium")
		return new Balloon(init, Vector2f(48,48), lowest_area+1, 20, "red");

	else if (type == "Large")
		return new Balloon(init, Vector2f(64,64), lowest_area+2, 10, "green");
	
	else if (type == "Player")
			return new Player(init, Vector2f(player_size_c, player_size_c));
	
	else
		throw Error("Trying to create ship of unknown type!");
}

Game_Object *get_rand_balloon(const Point2f &init)
{
	Random r;

	
	int num_balloons_c = 3;

	if (Game_Model::get().difficulty < 2 || Game_Model::get().difficulty == 4)
		num_balloons_c = 2;
	


	switch (r.rand_lte(num_balloons_c-1))
	{
		case 2:
			return create_ob("Small", init);
			break;
		case 1:
			return create_ob("Medium", init);
			break;
		case 0:
			return create_ob("Large", init);
			break;
	}
}
