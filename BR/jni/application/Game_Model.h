#pragma once

#include <vector>
#include <zenilib.h>

#include "Collision_Table.h"
class Game_Object;
class Player;

class Game_Model
{
public:
	static Game_Model &get()
	{
		static Game_Model m;
		return m;
	}

	~Game_Model(void);

	void update();
	
	void render() const;
	Player *get_player(int num) const;

	float time_step()
		{return m_time_step;}

	void release_balloon();
	void pop_balloon(Game_Object *balloon);

	bool game_over();
	void restart(int players, int diff);

	void finish_game();
	int get_score() const;

	int get_active_balloons()
		{return active_balloons;}

	bool win()
		{return queued_balloons + active_balloons > 0 && helium;}

	void stop();
	float get_time_left();

	void activate_helium();

	Zeni::Vector2f get_wind() {return wind;}

	struct Static
	{
		Static (Zeni::Point2f pos_, int time_):
			pos(pos_), time(time_){}
		Zeni::Point2f pos;
		int time;
	};
	std::vector<Static> statics;

	struct Static_text
	{
		Static_text (Zeni::Point2f pos_, int time_
				, const Zeni::String &msg_, Zeni::Color Col_, Zeni::String size_ = "small"):
			pos(pos_), time(time_), msg(msg_), col(Col_), size(size_)
			{if (pos.x < 30) pos.x = 30;}

		Zeni::Point2f pos;
		int time;
		Zeni::String msg;
		Zeni::Color col;
		Zeni::String size;
	};

	std::vector<Static_text> static_texts;

	bool helium;
	int num_players;
	int difficulty;
	int score_needed;

private:
	float helium_time;
	bool finished;
	int active_balloons;
	int queued_balloons;

	std::vector<Game_Object*> objs;
	std::vector<Player*> players;
	std::vector<Game_Object*> to_delete;

	Zeni::Chronometer<Zeni::Time> m_chrono;
	float m_time_passed;
	float m_time_step;
	float time_started;

	Zeni::Vector2f wind;

	float cur_time() const
		{return m_time_passed - time_started;}
	
	void start(int players);
	void remove_ob(Game_Object *ob);

	void check_collisions();
	bool is_colliding(Game_Object *ob1, Game_Object *ob2);

	Collision_Table table;

	Game_Model();
	Game_Model(const Game_Model &);
	Game_Model &operator=(const Game_Model &);
 };

