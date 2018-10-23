//
//  Ship.h
//  Asteroids
//
//  Created by Brian Summa on 6/5/15.
//
//

#ifndef __Asteroids__Ship__
#define __Asteroids__Ship__

#include "common.h"
#include "Bullet.h"

#define _MAX_SPEED 10
#define _DAMPING 0.98
#define _ACC 3
#define _ROT 15

class Ship{

  
  //Record of the ship's state
  struct {
    vec2 cur_location;   //Current position of the center
    float angle;         //Rotation angle
	float sheild;  //
    vec2 pointing;       //Vector pointing to the front of the ship
	float mass = 5;
    //This function will be helpful to keep track of the direction the ship
    //is pointing
    mat2 RotateZ2D( const GLfloat theta ){
      GLfloat angle = DegreesToRadians * theta;
      mat2 c;
      c[0][0] = c[1][1] = cos(angle);
      c[1][0] = sin(angle);
      c[0][1] = -c[1][0];
      return c;
    }
    vec2 velocity;       //Velocity
    bool thruster_on;    //Boolean if a thruster is on
	bool turning_left;
	bool turning_right;
  } state;
  
  //OpenGL variables for a ship
  struct {
    GLuint vao;           //Vertex array object for the outside of the ship.
	GLuint cockpitVAO;    //VAO for the eliptical cockpit.
	GLuint sheildVAO;
	GLuint thrusterVAO;
    GLuint program;       //shader program
    GLuint buffer;        //Vertex buffer object for the outside of the ship
	GLuint cockpit_buffer;
	GLuint sheild_buffer;
	GLuint thruster_buffer;
    GLuint vertex_shader, fragment_shader;  //Shaders
    GLint vpos_location, vcolor_location;   //reference to pos and color in shaders
    GLint M_location;     //Reference to matrix in shader
  } GLvars;

	vec2 raw_boundary[23]= { vec2( 0.5/7.0, 1.0/7.0), //The boundary without the sheild
		    vec2( 2.0/7.0, 2.0/7.0),
		    vec2( 3.0/7.0, 1.0/7.0),
	    	vec2( 4.0/7.0, -1.0/7.0),
		    vec2( 3.5/7.0, -2.0/7.0),
			vec2( 3.0/7.0, -2.0/7.0),
			vec2( 3.0/7.0, -1.0/7.0),
			vec2( 2.0/7.0, -0.0/7.0),
			vec2( 2.0/7.0, -2.0/7.0),
			vec2( 1.25/7.0, -4.0/7.0),
			vec2( 0.75/7.0, -5.0/7.0),
			vec2( 0.0/7.0, -6.0/7.0), // the vectors are mirrored across y after this.
			vec2( -0.75/7.0, -5.0/7.0),
			vec2( -1.25/7.0, -4.0/7.0),
			vec2( -2.0/7.0, -2.0/7.0),
			vec2( -2.0/7.0, -0.0/7.0),
			vec2( -3.0/7.0, -1.0/7.0),
			vec2( -3.0/7.0, -2.0/7.0),
			vec2( -3.5/7.0, -2.0/7.0),
			vec2( -4.0/7.0, -1.0/7.0),
			vec2( -3.0/7.0, 1.0/7.0),
			vec2( -2.0/7.0, 2.0/7.0),
			vec2( -0.5/7.0, 1.0/7.0)};
			
	vec2 sheilded_boundary[16];
  
public:
  

  Ship();
  
  inline void start_thruster(){ state.thruster_on= true;}
  inline void stop_thruster() { state.thruster_on= false;}

  inline void rotate_right(){
	  state.turning_right = true;
	  }
  inline void rotate_left(){
	  state.turning_left = true;
	  }
  inline void stop_rotate_right(){
	  state.turning_right = false;
	  }
  inline void stop_rotate_left(){
	  state.turning_left = false;
	  }
    
  void update_state();
  
  void gl_init();  
  void prepare_cockpit();
  void prepare_sheild();
  void prepare_thruster();
  void draw(mat4 proj);
  vec2* getBoundary(); //returns the bounding box.
  inline void flip(){state.pointing *= -1; state.angle += 180; }
  inline vec2 locate(){return state.cur_location;}
  inline vec2 velocity(){return state.velocity;}
  inline float get_mass(){return state.mass;}
  inline float get_sheild(){return state.sheild;}
  Bullet* shoot(char color); //Zap!
  void get_hit(vec2 other_velocity, float other_mass);
  
};


#endif /* defined(__Asteroids__Ship__) */
