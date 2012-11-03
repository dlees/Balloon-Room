#pragma once

#include <zenilib.h>

class Collision_Table;


class Game_Object {
public:
  Game_Object(const Zeni::Point2f &position_,
              const Zeni::Vector2f &size_,
			  int mass_,
              const float &theta_ = 0.0f,
              const float &speed_ = 0.0f,
			  const float &dir_ = 0.0f)
  : m_pos(position_),
    m_size(size_),
    m_theta(theta_),
    m_speed(speed_),
	m_dir(dir_),
	mass(mass_)
	, radius_factor(1)
  {
  }
 
  // If you might delete base class pointers, you need a virtual destructor.
  virtual ~Game_Object() {}
  virtual void update();
  virtual void render() const = 0; // pure virtual function call

  void turn_left(const float &theta_);
 
  void move_forward(const float &move_);

  void move_left(const float &move_);

  void accelerate(const float &change_, const float &dir_);
	
  const Zeni::Point2f & get_position() const {return m_pos;}
  const Zeni::Vector2f & get_size() const {return m_size;}
  const float & get_theta() const {return m_theta;}
 
  const float get_radius() const 
	{return 0.5f * m_size.x * radius_factor;}
	
  virtual int get_ID() const
		{return object_ID_c;}

protected:
  void render(const Zeni::String &texture, const Zeni::Color &filter = Zeni::Color()) const;
  
	virtual int get_parent_ID() const = 0;
	
	float get_speed() {return m_speed;}
	float get_dir() {return m_dir;}
		
	Zeni::Vector2f get_velo() {
		return Zeni::Vector2f(m_speed * cos(m_dir), m_speed * sin(m_dir));
	}

	float radius_factor;

private:
  float m_theta; // angle left from North 
  Zeni::Vector2f m_size; // (width, height)	
  Zeni::Point2f m_pos; // Upper left corner

  float m_speed;
  float m_dir; // angle up from East

  int mass;

  void add_gravity();
  void add_friction();

	void set_velo(Zeni::Vector2f velo) {	
		m_speed = velo.magnitude();
		m_dir = velo.theta();
	}

	void set_velo(float speed, float dir) {
		m_speed = speed;
		m_dir = dir;
	}

	Zeni::Point2f center() {
		return m_pos + m_size/2;
	}

	void bounce(float factor) {
		Zeni::Vector2f velo = get_velo();
		velo.y *= -1 * factor;
		set_velo(velo);
	}

  const static int object_ID_c;

  friend Collision_Table;
};
