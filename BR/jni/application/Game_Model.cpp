#include "Game_Model.h"

#include "Game_Object.h"
#include "Player.h"
#include "Object_factory.h"
#include "zenilib.h"
#include "Utility.h"
#include "high_scores.h"
#include <zeni/Sound_Buffer.h>

#include "Wall.h"

#include <vector>
#include <fstream>

using namespace Zeni;
using namespace std;

const int game_length_c = 60;


static int high_score;
static int low_score;
const int scores_saved_c = 8;


Game_Model::Game_Model(void)
{
}

void Game_Model::restart(int num, int diff)
{
	for (int i = 0 ; i < objs.size() ; i++)
	{
		delete objs[i];
	}
	objs.clear();
	players.clear();
	statics.clear();
	static_texts.clear();
	
	difficulty = diff;

	start(num);
}

void Game_Model::start(int num_players_)
{
	int max_ball_c = difficulty * 3;


	num_players = num_players_;

	Player *p;
	objs.push_back(p = dynamic_cast<Player*>(create_ob("Player",
		Point2f(screen_x_c/2, screen_y_c- player_size_c))));
	players.push_back(p);

	if (num_players > 1)
	{
		objs.push_back(p = dynamic_cast<Player*>(create_ob("Player",
			Point2f(screen_x_c/2, screen_y_c- player_size_c))));
		players.push_back(p);
	}

	objs.push_back(new Wall(Point2f(0,screen_x_c), Vector2f(screen_y_c, 10)));
	objs.push_back(new Wall(Point2f(screen_y_c,0), Vector2f(10, screen_x_c)));
	objs.push_back(new Wall(Point2f(0,0), Vector2f(10, screen_y_c)));

	active_balloons = 0;
	queued_balloons = max_ball_c;

	m_chrono.start();
	time_started = m_chrono.seconds();

	
	multiset<Score_t> scores;
	get_HS(scores,("hs" + itoa(difficulty + 100*num_players) + ".txt").data());
	
	if (scores.size() > 1)
		low_score = *scores.begin();	
	if (scores.size() > 0)	
		high_score = *scores.rbegin();

	finished = false;
	helium = false;

	
	Game_Model::get().static_texts.push_back(
			Game_Model::Static_text(Point2f(screen_x_c/2, screen_y_c/2), 120
			, "Press down to release a balloon!", Color(1,0,1,0), "small"));

}

Game_Model::~Game_Model(void)
{
}

void Game_Model::update()
{	
	static Random r;
	static float wind_time = 0;
	
	if (game_over())
		return;


	if (difficulty >= 6 && r.rand_lte(500) == 0)
	{
		wind = Vector2f(r.frand_lte() - .5f, 0);

		Game_Model::get().static_texts.push_back(
			Game_Model::Static_text(Point2f(screen_x_c/2, screen_y_c/2), 60
			, "WIND!", Color(1,0,1,0), "title"));

		wind_time = m_time_passed;
	}
	else if (wind_time + 0.5f < m_time_passed)
		wind = Vector2f();


	for (int i = 0 ;  i < statics.size() ; i++)
		statics[i].time-- ;
			
	for (int i = 0 ;  i < static_texts.size() ; i++)
		static_texts[i].time-- ;
	
	for_each(objs.begin(), objs.end(), mem_fun(&Game_Object::update));

	check_collisions();


	for_each(to_delete.begin(), to_delete.end(), bind1st(mem_fun(&Game_Model::remove_ob),this));
	to_delete.clear();

	float time_passed = 0;
	m_time_step = 0;

	while (m_time_step < 1.0f/frame_rate_c)
	{	
		time_passed = m_chrono.seconds();	
		m_time_step = time_passed - m_time_passed;
	}
	m_time_passed = time_passed;

}

float Game_Model::get_time_left()
{return (game_length_c - cur_time());
}

bool Game_Model::game_over()
{
	if (active_balloons == 0 && queued_balloons == 0)
		return true;

	if (cur_time() > game_length_c && !helium && active_balloons == 0)
		return true;

	if (helium && helium_time + .5 < m_time_passed)
		return true;

	return false;
}

Player *Game_Model::get_player(int num) const
{
	return players.at(num);
}

void render_time(int time_left, int y)
{	
	Color col ;

	if (time_left > 30)
		col = Color(1.0,0.0,1.0,0);
	else if (time_left > 10)
	{
		col = Color(1.0,1.0,1.0,0);
	}	
	else
	{		
		col = Color(1.0,1.0,0,0);
		get_Fonts()["title"].render_text(
		itoa(time_left)
			, Point2f(screen_x_c/2, 200), col, ZENI_CENTER);
	
	}
	get_Fonts()["medium"].render_text(
		"Time Left:"
			, Point2f(screen_x_c + 10, y), col);

	get_Fonts()["medium"].render_text(
		itoa(time_left)
			, Point2f(screen_x_c + 20, y+50), col);
	

}

void render_score(int score, int y)
{		
	Color col = Color(1.0,0,1.0,0);
	
	if (score > high_score)
		col = Color(1.0,1.0,0.0,1.0);

	else if (score > low_score)
	{
		col = Color(1.0,0.0,1.0,1.0);
	}
	else
	{
		col = Color(1.0,0.0,1.0,0);
	}

	get_Fonts()["medium"].render_text(
			"Score"
			, Point2f(screen_x_c + 10, y), col);

	get_Fonts()["medium"].render_text(
			itoa(score)
			, Point2f(screen_x_c + 20, y+50), col);

	if (Game_Model::get().score_needed <= score)
		get_Fonts()["medium"].render_text(
			"Press Space for Helium to Win!"
			, Point2f(screen_x_c/2, 0), Color(1.0,0,1.0,0)
			, ZENI_CENTER);
	else
		get_Fonts()["medium"].render_text(
			"Goal: "+ itoa(Game_Model::get().score_needed)
			, Point2f(screen_x_c + 20, y+100), Color(1.0,0,1.0,0));



}


void render_ball_left(int left, int y)
{
	Color col ;

	if (left > 3)
		col = Color(1.0,0.0,1.0,0);
	else if (left > 0)
	{
		col = Color(1.0,1.0,1.0,0);
	}
	else
	{
		col = Color(1.0,1.0,0.0,0);
	}

	get_Fonts()["medium"].render_text(
		"Balloons Left:"
			, Point2f(screen_x_c + 10, y), col);

	get_Fonts()["medium"].render_text(
			itoa(left)
			, Point2f(screen_x_c + 20, y+50), col);
	

}

void render_mult(int act, int y)
{
	Color col ;

	if (act < 2)
		col = Color(1.0,0.0,1.0,0);
	else if (act < 5)
	{
		col = Color(1.0,1.0,1.0,0);
	}
	else
	{
		col = Color(1.0,1.0,0.0,0);
	}

	get_Fonts()["medium"].render_text(
		"Multiplier:"
			, Point2f(screen_x_c + 10, y), col);

	get_Fonts()["medium"].render_text(
			"x" + itoa(act)
			, Point2f(screen_x_c + 20, y+50), col);
}

void Game_Model::render() const
{   
	// Use a helper defined in Zeni/EZ2D.h

	for_each(objs.begin(), objs.end()
		, mem_fun(&Game_Object::render));
	
	int y_incr = 50;

	render_score(get_score(), y_incr * 14);
	render_time(int(game_length_c - cur_time()), y_incr*12);
	render_ball_left(queued_balloons, y_incr*10);
	render_mult(active_balloons,y_incr*8);


	for (int i = 0 ;  i < statics.size() ; i++)
		if (statics[i].time > 0)
			render_image(
			  "static_1", // which texture to use
			  statics[i].pos, // upper-left corner
			  statics[i].pos + Point2f(64,64), ZENI_CENTER);

	
	for (int i = 0 ;  i < static_texts.size() ; i++)
		if (static_texts[i].time > 0)
		{ 
			get_Fonts()[static_texts[i].size].render_text(
				static_texts[i].msg
			, static_texts[i].pos, static_texts[i].col, ZENI_CENTER);
		}


}

void Game_Model::check_collisions()
{
	for (size_t i = 0 ; i < objs.size() ; i++)
		for (size_t j = i+1 ; j < objs.size() ; j++)
		{
			table.handle_collision(objs[i]->get_ID()
				, objs[j]->get_ID(), objs[i], objs[j]);
		}
}


void Game_Model::release_balloon()
{
	static int i = 0;
	if (queued_balloons > 0)
	{
		objs.push_back(get_rand_balloon(
			Point2f((screen_x_c/int(screen_x_c/100)) * (i++ % 5), 0)));
		++active_balloons;
		--queued_balloons;
	}
}


void Game_Model::pop_balloon(Game_Object *balloon)
{
	if (find(to_delete.begin(), to_delete.end(), balloon) !=  to_delete.end())
		return; // it's already been popped

	--active_balloons;
	to_delete.push_back(balloon);

	play_sound("pop");
	for (int i = 0 ; i < players.size() ; i++)
		players[i]->add_points(-1*players[i]->get_score()/4);

}

void Game_Model::remove_ob(Game_Object *ob)
{
	objs.erase(find(objs.begin(), objs.end(), ob));
	delete ob;	
}

int Game_Model::get_score() const
{
	int score = 0;
	for (int i = 0 ; i < players.size() ; i++)
		score += players[i]->get_score();

	return score;
}


void Game_Model::finish_game()
{
	if (finished)
		return;

	multiset<Score_t> scores;

	get_HS(scores, ("hs" + itoa(difficulty + 100*num_players) + ".txt").data());

	// if the score is less than the lowest score
	if (get_score()< *scores.begin() && scores.size() >= 8)
		return;

	if (scores.size() > scores_saved_c)
		scores.erase(scores.begin());

	scores.insert(get_score());

	ofstream fout(("hs" + itoa(difficulty + 100*num_players) + ".txt").data());
	
	save_HS(scores, fout);

	fout.close();

	finished = true;
}

void Game_Model::activate_helium()
{
	if (get_score() >= score_needed)
		helium = true;

	helium_time = m_time_passed;
}
