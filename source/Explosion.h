
#ifndef __Explosion__
#define __Explosion__

#include "common.h"
class Explosion{

  //Record of the explosion's state
  struct {
	  int time_to_live;
	  vec4 color;
	  float size;
	  vec2 location;
	  vec4 color_edge;
	  mat4 color_change;
  } state;
  
  //OpenGL variables for a ship
  struct {
    GLuint vao;           //Vertex array object
    GLuint program;       //shader program
    GLuint buffer;        //Vertex buffer objects
    GLuint vertex_shader, fragment_shader;  //Shaders
    GLint vpos_location, vcolor_location;   //reference to pos and color in shaders
    GLint M_location;     //Reference to matrix in shader
	GLint C_location;
  } GLvars;

  
public:
  

  Explosion(vec2 location, vec3 color);
  
  bool update_state();
  void gl_init();  
  void draw(mat4 proj);
};


#endif /* defined(__Asteroids__Ship__) */
