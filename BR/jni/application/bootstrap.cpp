/* This file is part of the Zenipex Library (zenilib).
 * Copyleft (C) 2011 Mitchell Keith Bloch (bazald).
 *
 * This source file is simply under the public domain.
 */

#include <zenilib.h>
#include "Game_Model.h"
#include "Player.h"
#include "Utility.h"

#include "high_scores.h"

#if defined(_DEBUG) && defined(_WINDOWS)
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

using namespace std;
using namespace Zeni;

int Players = 1;
int difficulty = 1;
float screen_x_c = 512;

void print_scores()
{
	Font &fr = get_Fonts()["medium"];

	multiset<Score_t> scores;
	get_HS(scores,( "hs" + itoa(difficulty + 100*Players) + ".txt").data());
		
		if (Game_Model::get().get_score() == *scores.rbegin())
			get_Fonts()["title"].render_text("NEW\nHIGH SCORE!", Point2f(screen_x_c/2, 0),
				Color(1,1,0,1), ZENI_CENTER);

	fr.render_text("High Scores", Point2f(screen_x_c+ sidebar_c/2, 0),
		get_Colors()["title_text"],
		ZENI_CENTER);
	
	int place = 1;
	Color col;
	for (multiset<Score_t>::reverse_iterator i = scores.rbegin() ;
		i != scores.rend() ; i++, place++)	
	{
		if (Game_Model::get().get_score() == *i)
			col = Color(1,1,0,1);
		else 
			col = get_Colors()["title_text"];

		fr.render_text(itoa(*i),Point2f(screen_x_c + sidebar_c/2, place*40),
				col,
				ZENI_CENTER);

	}
}

class Play_State : public Gamestate_Base {
  Play_State(const Play_State &);
  Play_State operator=(const Play_State &);
 
public:
  Play_State() 
    : m_time_passed(0.0f),
    m_left(false),
    m_hands_up(false),
    m_right(false)
	, m_hands_down(true)
	, m_release(true)
	, m_release_down(false),
	    n_left(false),
    n_hands_up(false),
    n_right(false)
	, n_hands_down(true)
	, n_release(true)
	, n_release_down(false)
	, escape(false)
	, m_next(false)
	, helium(false)
	, m_prev_clear_color(get_Video().get_clear_Color())
  {
    set_pausable(true);

	Color back_col = get_Colors()["black"];

	Vertex2f_Color v0(Point2f(0,0), back_col);
	Vertex2f_Color v1(Point2f(screen_x_c,0.0f), back_col);
	Vertex2f_Color v4(Point2f(screen_x_c, screen_y_c),back_col);
	Vertex2f_Color v2(Point2f(0.0f,screen_y_c), back_col);

	back = Quadrilateral<Vertex2f_Color> (v0, v2,v4, v1);
  }

private:
  bool m_left;
  bool m_right;
  bool m_hands_up;
  bool m_hands_down;
  bool m_release;
  bool m_release_down;
  bool n_left;
  bool n_right;
  bool n_hands_up;
  bool n_hands_down;
  bool n_release;
  bool n_release_down;
  bool m_next;
  bool helium;
  Quadrilateral<Vertex2f_Color> back;

  bool escape;
  
  Chronometer<Time> m_chrono;
  float m_time_passed;
	 Color m_prev_clear_color;
	
	void on_push() {
    //get_Window().mouse_grab(true);
    get_Window().mouse_hide(true);
	m_chrono.start(); 
	
	Game_Model::get().restart(Players, difficulty);

	get_Video().set_clear_Color(Color(0,.1,.1,.1));

    //get_Game().joy_mouse.enabled = false;
  }

  void on_pop() {
    //get_Window().mouse_grab(false);
    get_Window().mouse_hide(false);
	get_Video().set_clear_Color(m_prev_clear_color);
    //get_Game().joy_mouse.enabled = true;
  }

	 void on_key(const SDL_KeyboardEvent &event) {
		switch(event.keysym.sym) {
		  case SDLK_w:
			n_hands_up = event.type == SDL_KEYDOWN;
			break;

		  case SDLK_UP:
			m_hands_up = event.type == SDL_KEYDOWN;
			break;

		  case SDLK_a:
			n_left = event.type == SDL_KEYDOWN;
			break;
		  case SDLK_LEFT:
			m_left = event.type == SDL_KEYDOWN;
			break;
 
		  case SDLK_d:
			n_right = event.type == SDL_KEYDOWN;
			break;
		  case SDLK_RIGHT:
			m_right = event.type == SDL_KEYDOWN;
			break;

		  case SDLK_s:
			  n_release_down = event.type == SDL_KEYDOWN;
			  break;
		  case SDLK_DOWN:
			  m_release_down = event.type == SDL_KEYDOWN;
			  break;

		  case SDLK_RETURN:
			  m_next = event.type == SDL_KEYDOWN;
			  break;

		  case SDLK_SPACE:
			  if (Game_Model::get().game_over())
				  break;
			  helium = event.type == SDL_KEYDOWN;
			  break;
			  
		  case SDLK_ESCAPE:
			  escape = event.type == SDL_KEYDOWN;
			  if (Game_Model::get().game_over())
				  break;

		  default:
			Gamestate_Base::on_key(event); // Let Gamestate_Base handle it
			break;
		}
	}

	void perform_logic()
	{
		const float time_passed = m_chrono.seconds();
		const float time_step = time_passed - m_time_passed;
		m_time_passed = time_passed;

		if (m_left)
			Game_Model::get().get_player(0)->walk_left(time_step);
		if (m_right)
			Game_Model::get().get_player(0)->walk_right(time_step);
		if (m_hands_up && m_hands_down)
		{
			Game_Model::get().get_player(0)->hands_up();
			m_hands_down = false;
		}
		else if (!m_hands_up && !m_hands_down)
		{
			Game_Model::get().get_player(0)->hands_down();
			m_hands_down = true;
		}
		if (m_release && m_release_down)
		{
			Game_Model::get().release_balloon();
			m_release = false;
		}	
		else if (!m_release && !m_release_down)
			m_release = true;

		if (Game_Model::get().num_players > 1)
		{		
			if (n_left)
				Game_Model::get().get_player(1)->walk_left(time_step);
			if (n_right)
				Game_Model::get().get_player(1)->walk_right(time_step);
			if (n_hands_up && n_hands_down)
			{
				Game_Model::get().get_player(1)->hands_up();
				n_hands_down = false;
			}
			else if (!n_hands_up && !n_hands_down)
			{
				Game_Model::get().get_player(1)->hands_down();
				n_hands_down = true;
			}
			if (n_release && n_release_down)
			{
				Game_Model::get().release_balloon();
				n_release = false;
			}	
			else if (!n_release && !n_release_down)
				n_release = true;

		}

		if (helium)
			Game_Model::get().activate_helium();

		Game_Model::get().update();

		if (Game_Model::get().game_over())
		{
			Game_Model::get().finish_game();
			
			if (escape)
				get_Game().pop_state();
			if (m_next)
			{
				get_Game().pop_state();
				get_Game().push_state(new Play_State);
			}
		}
	}

	void render()
	{
		get_Video().set_2d(make_pair(Point2f(0.0f, 0.0f)
		, Point2f(screen_x_c + sidebar_c, screen_y_c)), true);
		

		get_Video().render(back);

		Game_Model::get().render();
		
	
		if (Game_Model::get().game_over())
		{	
			String win;
			if (Game_Model::get().win())
				win = "You Win!!!";
			else
				win = "All Balloons\nPopped!\nGame Over!";

			get_Fonts()["title"].render_text(
			win +"\nENTER = Retry\nESC = Menu"
			, Point2f(screen_x_c/2, screen_y_c/4 )
			, Color(), ZENI_CENTER);
		
			print_scores();

		}
	}
};



class Play_State_Top : public Widget_Gamestate {
public:
	Play_State_Top()
    : Widget_Gamestate(std::make_pair(Point2f(0.0f, 0.0f), Point2f(800.0f, 600.0f)), true)
  {	
	 m_widgets.lend_Widget(p2); 
//	 m_widgets.lend_Widget(p1);    
	 m_widgets.lend_Widget(l1);    
	 m_widgets.lend_Widget(l2);    
	 m_widgets.lend_Widget(l3);     
	 m_widgets.lend_Widget(l4);    
	 m_widgets.lend_Widget(l5);    
	 m_widgets.lend_Widget(l6);    
	 m_widgets.lend_Widget(l5_2);   
	 m_widgets.lend_Widget(pop_button);
	}


class P2_Button : public Text_Button
{
public:
    P2_Button()
      : Text_Button(Point2f(300, 500.0f)
		  , Point2f(500, 550.0f)
		  , "system_36_800x600", "2 Player Wind"){}
	  
	void on_accept() {
		Players = 2;
		difficulty = 6;
		screen_x_c = 512+300;
		Game_Model::get().score_needed = 30000;
		get_Game().push_state(new Play_State);
	}

} p2;

class P1_Button : public Text_Button
{
public:
    P1_Button()
      : Text_Button(Point2f(50.0f, 50.0f)
		  , Point2f(250.0f, 100.0f)
		  , "system_36_800x600", "1 Player")
	{}
	  
	void on_accept() {
		Players = 1;
		screen_x_c = 512;
		get_Game().push_state(new Play_State);
    }
} p1;

class Level_1 : public Text_Button
{
public:
    Level_1()
      : Text_Button(Point2f(300.0f, 50.0f)
		  , Point2f(500.0f, 100.0f)
		  , "system_36_800x600", "Level 1")
	{this->color = Color(1,0,1,1);}
	  
	void on_accept() {
		Players = 1;
		difficulty = 1;
		screen_x_c = 512;
		Game_Model::get().score_needed = 1000;
		get_Game().push_state(new Play_State);
    }
} l1;

class Level_2 : public Text_Button
{
public:
    Level_2()
      : Text_Button(Point2f(300.0f, 125.0f)
		  , Point2f(500.0f, 175.0f)
		  , "system_36_800x600", "Level 2")
	{this->color = Color(1,0,1,1);}
	  
	void on_accept() {
		Players = 1;
		difficulty = 2;
		screen_x_c = 512;
		Game_Model::get().score_needed = 3000;
		get_Game().push_state(new Play_State);
    }
} l2;

class Level_3 : public Text_Button
{
public:
    Level_3()
      : Text_Button(Point2f(300.0f, 200.0f)
		  , Point2f(500.0f, 250.0f)
		  , "system_36_800x600", "Level 3")
	{this->color = Color(1,0,1,1);}
	  
	void on_accept() {
		Players = 1;
		difficulty = 3;
		screen_x_c = 512;
		Game_Model::get().score_needed = 6000;
		get_Game().push_state(new Play_State);
    }
} l3;

class Level_4 : public Text_Button
{
public:
    Level_4()
      : Text_Button(Point2f(300.0f, 275.0f)
		  , Point2f(500.0f, 325.0f)
		  , "system_36_800x600", "Level 4")
	{this->color = Color(1,0,1,1);}
	  
	void on_accept() {
		difficulty = 4;
		Players = 1;
		screen_x_c = 512+50;
		Game_Model::get().score_needed = 10000;
		get_Game().push_state(new Play_State);
    }
} l4;

class Level_5 : public Text_Button
{
public:
    Level_5()
      : Text_Button(Point2f(300.0f, 350.0f)
		  , Point2f(500.0f, 400.0f)
		  , "system_36_800x600", "Level 5")
	{this->color = Color(1,0,1,1);}
	  
	void on_accept() {
		difficulty = 5;
		Players = 1;
		screen_x_c = 512+50;
		Game_Model::get().score_needed = 15000;
		get_Game().push_state(new Play_State);
    }
} l5;

class Level_5_2p : public Text_Button
{
public:
    Level_5_2p()
      : Text_Button(Point2f(50.0f, 350.0f)
		  , Point2f(250.0f, 400.0f)
		  , "system_36_800x600", "Level 5 - 2P")
	{this->color = Color(1,0,1,1);}
	  
	void on_accept() {
		difficulty = 5;
		Players = 2;
		screen_x_c = 512+200;
		Game_Model::get().score_needed = 25000;
		get_Game().push_state(new Play_State);
    }
} l5_2;

class Level_6 : public Text_Button
{
public:
    Level_6()
      : Text_Button(Point2f(300.0f, 425.0f)
		  , Point2f(500.0f, 475.0f)
		  , "system_36_800x600", "Level 6 - Wind")
	{this->color = Color(1,0,1,1);}
	  
	void on_accept() {
		Players = 1;
		difficulty = 6;
		screen_x_c = 512+100;
		Game_Model::get().score_needed = 15000;
		get_Game().push_state(new Play_State);
    }
} l6;


 class Pop_Button : public Text_Button {
	  public:
    Pop_Button()
      : Text_Button(Point2f(600, 550.0f), Point2f(800, 600.0f), "system_36_800x600", "Main Menu")
    {
    }
	 void on_accept() {
      get_Game().pop_state();
    }

  } pop_button;
};


void render_controls(int y)
{
	int y_incr = 45;

	int screen_x_c = 300;

	Color col = Color(1.0,1.0,.75,.75);

	get_Fonts()["medium"].render_text(
		"Player 1 - Pink"
			, Point2f(screen_x_c + 10, y), col);

	get_Fonts()["small"].render_text(
		"Move: <- and ->"
			, Point2f(screen_x_c + 20, y_incr + y), col);

	get_Fonts()["small"].render_text(
		"Bounce Balloon: UP"
			, Point2f(screen_x_c + 10, y_incr*2 + y), col);
			
	get_Fonts()["small"].render_text(
		"Drop Balloon: DOWN"
			, Point2f(screen_x_c + 10, y_incr*3 + y), col);

	col = Color(1.0,0.75,1, 1);
	
	get_Fonts()["medium"].render_text(
		"Player 2 - Blue"
			, Point2f(0 + 10, y_incr*4 +y), col);

	get_Fonts()["small"].render_text(
		"Move: 'a' and 'd'"
			, Point2f(0 + 20, y_incr*5 + y), col);

	get_Fonts()["small"].render_text(
		"Bounce Balloon: 'w'"
			, Point2f(0 + 10, y_incr*6 + y), col);
			
	get_Fonts()["small"].render_text(
		"Release Balloon: 's'"
			, Point2f(0 + 10, y_incr*7 + y), col);

	col = Color();

	get_Fonts()["small"].render_text(
		"Bounce Balloons to get points!\nTo win, Press Space when you pass the goal score.\nOr, keep playing for points!\nWhen Balloons pop, you lose 1/4 of your score.\nTry Bouncing Balloons into each other!\n"
			, Point2f(0 + 10, y_incr*8 + y), col);

	
}


class Instructions_State : public Widget_Gamestate {
  Instructions_State(const Instructions_State &);
  Instructions_State operator=(const Instructions_State &);

public:
  Instructions_State()
    : Widget_Gamestate(make_pair(Point2f(0.0f, 0.0f), Point2f(800.0f, 600.0f)))
  {
  }

private:
  void on_key(const SDL_KeyboardEvent &event) {
    if(event.keysym.sym == SDLK_ESCAPE && event.state == SDL_PRESSED)
      get_Game().pop_state();
  }

  void render() {
    Widget_Gamestate::render();

  
	render_controls(0);
  }
};

class Bootstrap {
  class Gamestate_One_Initializer : public Gamestate_Zero_Initializer {
    virtual Gamestate_Base * operator()() {
      Window::set_title("Balloon Room");

      get_Joysticks();
      get_Video();
      get_Textures();
      get_Fonts();
      get_Sounds();
      get_Game().joy_mouse.enabled = true;

      return new Title_State<Play_State_Top, Instructions_State>("Balloon Room\n");
    }
  } m_goi;

public:
  Bootstrap() {
    g_gzi = &m_goi;
  }
} g_bootstrap;

int main(int argc, char **argv) {
  return zenilib_main(argc, argv);
}
