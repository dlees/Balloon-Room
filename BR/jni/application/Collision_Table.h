#pragma once

#include <vector>

class Game_Object;
class Balloon;
class Player;
class Wall;


class Collision_Table
{
public:
	Collision_Table();

	void handle_collision(int i, int j, Game_Object *A, Game_Object *B)
	{
		(this->*table.at(i).at(j))(A,B);
	}

private:
	typedef void (Collision_Table::*fxn_ptr_t)(Game_Object*, Game_Object*);
	std::vector<std::vector<fxn_ptr_t> > table;

	void collideBB(Balloon*, Balloon*);
	void collidePB(Player*, Balloon*);
	void collideBP(Balloon*, Player*);
	void collidePP(Player*, Player*);
	void collideWB(Wall*, Balloon*);
	void collidePW(Player*, Wall*);
	void collideWP(Wall*, Player*);
	void collideBW(Balloon*, Wall*);
	void collideWW(Wall*, Wall*);
	void collideWG(Wall*, Game_Object*);

	float set_pos_of_collide(Game_Object *ob1, Game_Object *ob2);
	void set_new_velo(Game_Object *ob1, Game_Object *ob2, float time);
	bool circle_collide(Game_Object *ob1, Game_Object *ob2);
	bool rect_collide(Game_Object *ob1, Game_Object *ob2);
};
