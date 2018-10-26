
#include "common.h"
#include <cmath>
#include <typeinfo>
#include <stdlib.h>
#include <time.h>
#include <tuple>

Explosion::Explosion(vec2 location, vec3 color){
	//state.location = vec2(rand()/(float)RAND_MAX, rand()/(float)RAND_MAX);
	state.location = location;
	state.color = vec4(color.x, color.y, color.z, 1);
	state.time_to_live=16;
	//state.color_edge = state.color+vec4(0.1, 0.2, 0.2, 0) - vec4(0,0,0,1);
	state.color_edge = vec4(0.0,0.0,0.0,0.0);
	state.size = 0.1;
};


//Called everytime an animation tick happens
bool Explosion::update_state(){
	state.time_to_live -= 1;
	int age = 15 - state.time_to_live;
	float portion = (age%5)/(float)5;
	//std::cout<< portion<<std::endl;
	mat4 m0 = mat4(vec4(1,0,0,0),vec4(0,1,0,0),vec4(0,0,1,0),vec4(0,0,0,1)); //the identity
	//mat4 m2 = mat4(vec4(1,0.8,0.5,0),vec4(0.3,1,0.5,0),vec4(0,0.3,1,0),vec4(0,0,0,1)); //the makes more yellow
	mat4 m2 = mat4(vec4(1,0.35,0.35,0),vec4(0.35,1,0.35,0),vec4(0.35,0.35,1,0),vec4(0,0,0,1)); //the makes more yellow
	mat4 m1 = mat4(vec4(1,0.25,0.25,0),vec4(0.25,1,0.25,0),vec4(0.25,0.25,1,0),vec4(0,0,0,1));
	mat4 m3 = mat4(vec4(1,0.35,0.35,0),vec4(0.35,1,0.35,0),vec4(0.35,0.35,1,0),vec4(0,0,0,0));
	if (age <5)
		state.color_change = portion*m0 + (1.0-portion)*m1;
	else if (age <2*5)
		state.color_change = portion*m1 + (1.0-portion)*m2;
	else
		state.color_change = portion*m2 + (1.0-portion)*m3;
	state.size *=1.1;
	if (state.time_to_live ==0)
		return false;
	return true;
	
}
void Explosion::gl_init(){

  vec2 triangulation[14];
  triangulation[0]=vec2(0,0);
  for (int i=1; i<14; i++){
	  triangulation[i]=rotation(i*2*pi/12.0)*vec2(0,0.2+0.6*rand()/(float)RAND_MAX);
	  }
  triangulation[13]=vec2(triangulation[1].x, triangulation[1].y);
  vec4 color_points[14];
  color_points[0]=state.color;
  for (int i=1; i<14; i++){ //Make the points on the boundary clear.
	color_points[i]=state.color_edge; 
  }
  
  std::string vshader = shader_path + "vshader_Ship.glsl";
  std::string fshader = shader_path + "fshader_Ship.glsl";
  
  GLchar* vertex_shader_source = readShaderSource(vshader.c_str());
  GLchar* fragment_shader_source = readShaderSource(fshader.c_str());
  
  GLvars.vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(GLvars.vertex_shader, 1, (const GLchar**) &vertex_shader_source, NULL);
  glCompileShader(GLvars.vertex_shader);
  check_shader_compilation(vshader, GLvars.vertex_shader);
  
  GLvars.fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(GLvars.fragment_shader, 1, (const GLchar**) &fragment_shader_source, NULL);
  glCompileShader(GLvars.fragment_shader);
  check_shader_compilation(fshader, GLvars.fragment_shader);
  
  GLvars.program = glCreateProgram();
  glAttachShader(GLvars.program, GLvars.vertex_shader);
  glAttachShader(GLvars.program, GLvars.fragment_shader);
  
  glLinkProgram(GLvars.program);
  check_program_link(GLvars.program);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindFragDataLocation(GLvars.program, 0, "fragColor");
  
  GLvars.vpos_location   = glGetAttribLocation(GLvars.program, "vPos");
  GLvars.vcolor_location = glGetAttribLocation(GLvars.program, "vColor" );
  GLvars.M_location = glGetUniformLocation(GLvars.program, "M" );
  GLvars.C_location = glGetUniformLocation(GLvars.program, "C" );
  // Create a vertex array object
  glGenVertexArrays( 1, &GLvars.vao );
  //Set GL state to use vertex array object
  glBindVertexArray( GLvars.vao );
  
  //Generate buffer to hold our vertex data
  glGenBuffers( 1, &GLvars.buffer );
  //Set GL state to use this buffer
  glBindBuffer( GL_ARRAY_BUFFER, GLvars.buffer );
  
  //Create GPU buffer to hold vertices
  glBufferData( GL_ARRAY_BUFFER, sizeof(triangulation)+sizeof(color_points), NULL, GL_STATIC_DRAW );
  //First part of array holds vertices
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(triangulation), triangulation );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(triangulation), sizeof(color_points), color_points );
  
  
  glEnableVertexAttribArray(GLvars.vpos_location);
  glEnableVertexAttribArray(GLvars.vcolor_location);
  glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  //glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glVertexAttribPointer( GLvars.vcolor_location, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(triangulation)) );
  
  glBindVertexArray(0);

}

//Draw a ship
void Explosion::draw(mat4 proj){
  proj = proj*Translate(state.location.x,state.location.y,0)*Scale(state.size,state.size,state.size);
  glUseProgram( GLvars.program );
  glBindVertexArray( GLvars.vao );
  
  glUniformMatrix4fv( GLvars.M_location, 1, GL_TRUE, proj );
  glUniformMatrix4fv( GLvars.C_location, 1, GL_TRUE, state.color_change);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 14);
  
  glBindVertexArray(0);
  glUseProgram(0);
}