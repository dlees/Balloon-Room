#include "Collision_Table.h"

#include "Game_Model.h"
#include "Game_Object.h"
#include "Player.h"
#include "Balloon.h"
#include "Wall.h"
#include "Utility.h"

#include <vector>
#include <zenilib.h>

using namespace std;
using namespace Zeni;

const int num_Obs_c = 4;

Collision_Table::Collision_Table()
	: table(num_Obs_c, vector<fxn_ptr_t>())
{
	table.at(Balloon::balloon_ID_c).push_back(NULL);

	table.at(Balloon::balloon_ID_c).push_back(
		(fxn_ptr_t)&Collision_Table::collideBB);

	table.at(Balloon::balloon_ID_c).push_back(
		(fxn_ptr_t)&Collision_Table::collideBP);

	table.at(Balloon::balloon_ID_c).push_back(
		(fxn_ptr_t)&Collision_Table::collideBW);



	
	table.at(Player::player_ID_c).push_back(NULL);

	table.at(Player::player_ID_c).push_back(
		(fxn_ptr_t)&Collision_Table::collidePB);

	table.at(Player::player_ID_c).push_back(
		(fxn_ptr_t)&Collision_Table::collidePP);

	table.at(Player::player_ID_c).push_back(
		(fxn_ptr_t)&Collision_Table::collidePW);



	
	table.at(Wall::wall_ID_c).push_back(NULL);

	table.at(Wall::wall_ID_c).push_back(
		(fxn_ptr_t)&Collision_Table::collideWB);

	table.at(Wall::wall_ID_c).push_back(
		(fxn_ptr_t)&Collision_Table::collideWP);

	table.at(Wall::wall_ID_c).push_back(
		(fxn_ptr_t)&Collision_Table::collideWW);
}


void Collision_Table::collideBB(Balloon* b1, Balloon* b2)
{
	if (!circle_collide(b1, b2))
		return;

	float time = set_pos_of_collide(b1, b2);
	
	Point2f collide = b1->m_pos + b2->m_pos;
	collide.x/=2;
	collide.y/=2;
	
	if (!Game_Model::get().helium)
		b1->pass(*b2, collide);

	set_new_velo(b1, b2, time);
}

void Collision_Table::collidePB(Player* p1, Balloon* b1)
{	
	if (!circle_collide(p1, b1))
		return;
		
	float time = set_pos_of_collide(b1, p1);
	Point2f collide = b1->m_pos + p1->m_pos;
	collide.x/=2;
	collide.y/=2;

	set_new_velo(b1,p1, time);

	if (p1->push_state && p1->push > .01 && b1->multipliers.empty())
	{
		b1->accelerate(p1->push, up_c);
		p1->score += b1->worth * Game_Model::get().get_active_balloons();

		Game_Model::get().static_texts.push_back(
			Game_Model::Static_text(collide, 40
			, itoa(b1->worth * Game_Model::get().get_active_balloons()), Color()));

		b1->get_hit(p1);
	}
	else
	{
		play_sound("bad_bounce");
	}

}


void Collision_Table::collideBP(Balloon* b1, Player* p1)
{
	collidePB(p1,b1);
}

void Collision_Table::collidePP(Player* , Player* )
{

}

void Collision_Table::collideWB(Wall* w1, Balloon* ob2)
{
	
	collideBW(ob2, w1);
}

void Collision_Table::collidePW(Player *ob2, Wall *w1)
{
	 if (ob2->m_pos.y + ob2->m_size.y > screen_y_c)
	 {
		ob2->m_pos.y = screen_y_c - ob2->m_size.y;
		ob2->add_friction();
		
		Vector2f velo = ob2->get_velo();
		velo.j = 0;
		ob2->set_velo(velo);
	 }

	if (ob2->m_pos.x < 0)
	{
		ob2->m_pos.x = 0;
		Vector2f velo = ob2->get_velo();
		velo.i = abs(velo.i / 2);
		ob2->set_velo(velo);
	}

	else if (ob2->m_pos.x + ob2->m_size.x > screen_x_c)
	{
		ob2->m_pos.x = screen_x_c-ob2->m_size.x;
		Vector2f velo = ob2->get_velo();
		velo.i = -abs(velo.i / 2);
		ob2->set_velo(velo);
	}
}

void Collision_Table::collideWP(Wall* w1, Player* ob2)
{
	collidePW(ob2, w1);
}

void Collision_Table::collideBW(Balloon *ob2, Wall *w1)
{
	if (ob2->m_pos.y + ob2->m_size.y > screen_y_c)
	{
		Game_Model::get().pop_balloon(ob2);
	}
	else
	{
		collideWG(w1, ob2);
	}
}

void Collision_Table::collideWW(Wall*, Wall*)
{

}

void Collision_Table::collideWG(Wall *, Game_Object *ob2)
{
	 if (ob2->m_pos.y + ob2->m_size.y > screen_y_c)
	 {
		ob2->m_pos.y = screen_y_c-ob2->m_size.y;
		ob2->add_friction();
	 }

	 else if (ob2->m_pos.y < -50)
	 {
		ob2->m_pos.y = 0;
		ob2->bounce(.3f);
	 }

	if (ob2->m_pos.x < 0)
	{
		ob2->m_pos.x = 0;
		Vector2f velo = ob2->get_velo();
		velo.i = abs(velo.i);
		ob2->set_velo(velo);
	}

	else if (ob2->m_pos.x + ob2->m_size.x > screen_x_c)
	{
		ob2->m_pos.x = screen_x_c-ob2->m_size.x;
		Vector2f velo = ob2->get_velo();
		velo.i = -abs(velo.i);
		ob2->set_velo(velo);
	}
}

float Collision_Table::set_pos_of_collide(Game_Object *b1, Game_Object *b2)
{
	float time = (b1->get_radius() + b2->get_radius() - (b1->center() - b2->center()).magnitude());
	time /= abs((b2->get_velo() - b1->get_velo()).magnitude());

	b2->m_pos -= b2->get_velo() * time;
	b1->m_pos -= b1->get_velo() * time;


	return time;
}

void Collision_Table::set_new_velo(Game_Object *ob1, Game_Object *ob2, float time)
{
	Vector2f v1 = ob1->get_velo();
	Vector2f v2 = ob2->get_velo();

	float m1(ob1->mass);
	float m2(ob2->mass);
	
	ob1->set_velo((2*m2*v2 + v1*(m1-m2))/(m1+m2));
	ob2->set_velo((2*m1*v1 + v2*(m1-m2))/(m1+m2));

	ob1->m_pos += ob1->get_velo() * time;
	ob2->m_pos += ob2->get_velo() * time;
}

bool Collision_Table::circle_collide(Game_Object *ob1, Game_Object *ob2)
{
	if (ob1 == ob2)
		return false;

	const Vector2f dist_vec = ob1->m_pos - ob2->m_pos +
                              0.5f * (ob1->m_size - ob2->m_size);

    const float dist = sqrt(dist_vec * dist_vec);
 
    return dist < ob1->get_radius() + ob2->get_radius();
}

bool Collision_Table::rect_collide(Game_Object *ob1, Game_Object *ob2)
{
	if (ob1->m_pos.x > (ob2->m_pos.x + ob2->m_size.x))
	{
		return false;
	}
	if (ob2->m_pos.x > (ob1->m_pos.x + ob1->m_size.x))
	{
		return false;
	}
	if (ob1->m_pos.y > (ob2->m_pos.y + ob2->m_size.y))
	{
		return false;
	}
	if (ob2->m_pos.y > (ob1->m_pos.y + ob1->m_size.y))
	{
		return false;
	}

	return true;
}