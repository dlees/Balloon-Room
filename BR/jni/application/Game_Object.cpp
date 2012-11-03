#include "Game_Object.h"
#include <zenilib.h>

#include "Utility.h"
#include "Game_Model.h"

using namespace Zeni;
using namespace std;


const int Game_Object::object_ID_c = 0;
const float friction_c = 1.0f/5.0f; 

void Game_Object::render(const String &texture, const Color &filter) const {

    // Use a helper defined in Zeni/EZ2D.h
    render_image(
      texture, // which texture to use
      m_pos, // upper-left corner
      m_pos + m_size, // lower-right corner
      m_theta, // rotation in radians
      1.0f, // scaling factor
      m_pos + 0.5f *m_size, // point to rotate & scale about
      false, // whether or not to horizontally flip the texture
      filter); // what Color to "paint" the texture
  }

void Game_Object::turn_left(const float &theta_) {
    m_theta += theta_;
  }
 
void Game_Object::move_forward(const float &move_) {
    m_pos.x += move_ * sin(m_theta);
    m_pos.y += move_ * -cos(m_theta);
  }

void Game_Object::move_left(const float &move_)
{
    m_pos.x += move_ * -cos(m_theta);
    m_pos.y += move_ * sin(m_theta);	
}

void Game_Object::accelerate(const float &change, const float &dir)
{

	Vector2f change_v(change * cos(dir), change * sin(dir));
	Vector2f velo = get_velo();

	velo += change_v * Game_Model::get().time_step() * 120;

	set_velo(velo);
}

void Game_Object::update()
{
	float speed_limit = 15.0f;
	if (m_speed > speed_limit)
		m_speed = speed_limit;

	m_pos.x += m_speed * cos(m_dir) * Game_Model::get().time_step() * 120;
	m_pos.y += m_speed * sin(m_dir) * Game_Model::get().time_step() * 120;
	
	add_gravity();
	
	// wind
	accelerate(Game_Model::get().get_wind().magnitude() / mass
		, Game_Model::get().get_wind().theta());
}


 void Game_Object::add_friction()
 {
	 accelerate(m_speed * friction_c, m_dir - Global::pi);
 }
 
 void Game_Object::add_gravity()
 {
	accelerate(gravity_c, down_c);
 }

