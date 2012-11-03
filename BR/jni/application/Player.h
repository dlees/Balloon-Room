#pragma once

#include "Game_Object.h"

class Collision_Table;

class Player :
	public Game_Object
{
public:
	Player(const Zeni::Point2f &position_,
			const Zeni::Vector2f &size_
			) 
			:  Game_Object(position_, size_, 1000000),
			score(0), num(++num_players), image("girl"+Zeni::itoa(num)), push_state(false)
	{radius_factor = .9f;

	}

	~Player(void);

	void walk_left(float time_step);
	void walk_right(float time_step);

	void hands_up();
	void hands_down();

	void update();

	void render() const;

	virtual int get_ID() const 
		{return player_ID_c;}

	int get_score()
		{return score;}

	void add_points(int points)
		{score += points;}
	

protected:
	static int num_players;
	int num;

	virtual int get_parent_ID() const
		{return Game_Object::get_ID();}

private:
	const static int player_ID_c;

	int score;
	Zeni::String image;

	float push; // push force
	bool push_state; // are we pushing?

	friend Collision_Table;
};


