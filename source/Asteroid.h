
#ifndef __Asteroids__
#define __Asteroids__

#include "common.h"
class Asteroid{

  vec2 asteroid[6];
  vec3 asteroid_color[6];
  
  //Record of the asteroid's state
  struct {
    vec2 cur_location;   //Current position of the center
    float angle;         //Rotation angle
    vec2 pointing;       //Vector pointing to the front of the asteroid
    mat2 RotateZ2D( const GLfloat theta ){
      GLfloat angle = DegreesToRadians * theta;
      mat2 c;
      c[0][0] = c[1][1] = cos(angle);
      c[1][0] = sin(angle);
      c[0][1] = -c[1][0];
      return c;
    }
    vec2 velocity;       //Velocity
	float angular_velocity;
	float mass;
	vec2 center; //the center of mass.
    bool thruster_on;    //Boolean if a thruster is on
	vec3 color;
	float flashNumber;// between 0 and 1. 0 When not flashing.
	vec2 boundary[13]; //The polygon which is the boundary of the asteroid.
  } state;
  
  //OpenGL variables for a ship
  struct {
    GLuint vao;           //Vertex array object
    GLuint program;       //shader program
    GLuint buffer;        //Vertex buffer objects
    GLuint vertex_shader, fragment_shader;  //Shaders
    GLint vpos_location, vcolor_location;   //reference to pos and color in shaders
    GLint M_location;     //Reference to matrix in shader
  } GLvars;

  
public:
  

  Asteroid();
  inline Asteroid(vec3 color, vec2 velocity, vec2 position) : Asteroid(){
		state.color = color;
		state.velocity = velocity;
		state.cur_location = position;
		state.flashNumber = 1.0;
	}
  
  void update_state();
  void gl_init();  
  void draw(mat4 proj);
  vec2* getBoundary();
  inline vec2 locate(){return state.cur_location;}
  inline vec3 get_color(){return state.color;}
  inline float get_mass(){return state.mass;}
  inline vec2  get_velocity(){return state.velocity;}
  inline void set_color(vec3 newColor){
	state.color = newColor;
	state.flashNumber = 1.0;
	  }
  inline void flash(){state.flashNumber = 1.0;}//turns white when hit. Not currently used.
  std::tuple<Asteroid*, Asteroid*> shatter(vec2 other_velocity, float other_mass);
  
};


#endif /* defined(__Asteroids__Ship__) */
