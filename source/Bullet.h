#ifndef __Blaster__
#define __Blaster__

#include "common.h"

class Bullet{ //
  struct{
		vec3 color;
		vec2 velocity;
		vec2 position;
		bool left_laser; //true when the left laser is present.
		bool right_laser;
		vec2 pointing;
		float angle;
		int time_to_live;
		}state;
  struct {
    GLuint vao_left;
	GLuint vao_right;
    GLuint program;       //shader program
    GLuint buffer_left;
	GLuint buffer_right;
    GLuint vertex_shader, fragment_shader;  //Shaders
    GLint vpos_location, vcolor_location;   //reference to pos and color in shaders
	GLint M_location;
  } GLvars;
  
public:
  Bullet(vec2 pos, vec2 vel, vec2 pointing, vec3 color); //constructor. Takes the position and velocity for the ship.
  void gl_init();
  void draw(mat4 proj);
  bool update_state();//returns false when the laser should be deleted
  vec2 locate();
  vec2* leftBoundingBox(); //returns the polygon in world space of the left bounding box
  vec2* rightBoundingBox();
  inline vec3 get_color(){
	  return vec3(state.color.x, state.color.y, state.color.z);
	  }
  inline void remove_right_laser(){
	  state.right_laser=false;
	  }
  inline void remove_left_laser(){
	  state.left_laser=false;
	  }
  inline bool is_gone(){
	  if (!state.left_laser and !state.right_laser)
		  return true;
	  return false;
	  }
  inline bool has_left(){
	  return state.left_laser;
	  }
  inline bool has_right(){
	  return state.right_laser;
	  }
};
#endif