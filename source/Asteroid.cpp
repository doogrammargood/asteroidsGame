
#include "common.h"
#include <cmath>
#include <typeinfo>
#include <stdlib.h>
#include <time.h>
#include <tuple>
mat2 rotatin(float angle){ //Returns a rotation matrix by a given angle. The angle is in radians.
  mat2 toReturn(vec2(cos(angle),-sin(angle)),vec2(sin(angle), cos(angle)));
  return toReturn;
}

Asteroid::Asteroid(){
  state.flashNumber=0;
  state.cur_location = vec2(rand()/(float)RAND_MAX, rand()/(float)RAND_MAX);
  float color_picker = rand()/(float)RAND_MAX;
  if (color_picker < 1/3.0)
	state.color = vec3(1, 0, 0);
  else if (color_picker < 2/3.0)
	state.color = vec3(0, 1, 0);
  else
	state.color = vec3(0,0,1);
  state.velocity = vec2(0.03*rand()/(float)RAND_MAX, 0.03*rand()/(float)RAND_MAX);
  if(rand()/(float)RAND_MAX<0.5)
	  state.velocity.x = -1*state.velocity.x;
  if(rand()/(float)RAND_MAX<0.5)
	  state.velocity.y *= -1*state.velocity.y;
  state.pointing = vec2(0,-1.0);
  state.angle = 0;
  state.angular_velocity = 0.1*rand()/(float)RAND_MAX;//radians per frame.
  if(rand()/(float)RAND_MAX<0.5)
	  state.angular_velocity = -1*state.angular_velocity; //Sometimes the asteroid should rotate the other way.
  state.mass = 0; //will be updated in gl_init().
  state.center = vec2(0,0); //will be updated in gl_init().
};


//Called everytime an animation tick happens
void Asteroid::update_state(){
  if (state.flashNumber >0)
	state.flashNumber -= 0.1;
  state.angle += state.angular_velocity;
  state.pointing = rotation(state.angle)*vec2(0,-1.0);
  state.cur_location += state.velocity;
	if (state.cur_location.x>1){
		state.cur_location.x-=2;
		}
	if (state.cur_location.y>1){
		state.cur_location.y-=2;
		}
	if (state.cur_location.x<-1){
		state.cur_location.x+=2;
		}
	if (state.cur_location.y<-1){
		state.cur_location.y+=2;
		}
}
std::tuple<Asteroid*, Asteroid*> Asteroid::shatter(vec2 other_velocity, float other_mass){
	//The asteroid breaks apart.
	//https://www.khanacademy.org/science/physics/linear-momentum/elastic-and-inelastic-collisions/a/what-are-elastic-and-inelastic-collisions
	//The two pieces will be bisected by vector that would result if the collision had been elastic.
	vec2 midline;
	vec2 asteroidVector = state.mass * state.velocity;
	float spread = 0.1+rand()/(float)RAND_MAX; //radian angle of separation between two new asteroids.
	midline = ((state.mass - other_mass) / (state.mass + other_mass) )*state.velocity + (2*other_mass/(state.mass + other_mass))*other_velocity;
	Asteroid* minus_asteroid;
	Asteroid* plus_asteroid;
	std::tuple<vec3,vec3> colors;
	if (length(state.color) == 1.0){ //if we have a primary color
		if(state.color.x==1)
			colors = std::tuple<vec3,vec3>(vec3(0,1,0),vec3(0,0,1));
		if(state.color.y==1)
			colors = std::tuple<vec3,vec3>(vec3(1,0,0),vec3(0,0,1));
		if(state.color.z==1)
			colors = std::tuple<vec3,vec3>(vec3(1,0,0),vec3(0,1,0));
		}
	else{
		if(state.color.x==0)
			colors = std::tuple<vec3,vec3>(vec3(0,1,0),vec3(0,0,1));
		if(state.color.y==0)
			colors = std::tuple<vec3,vec3>(vec3(1,0,0),vec3(0,0,1));
		if(state.color.z==0)
			colors = std::tuple<vec3,vec3>(vec3(1,0,0),vec3(0,1,0));
		}
	minus_asteroid = new Asteroid(std::get<0>(colors), rotation(-spread)*midline, state.cur_location+5*rotation(-spread)*midline);
	plus_asteroid = new Asteroid(std::get<1>(colors), rotation(spread)*midline, state.cur_location+5*rotation(spread)*midline);
	std::tuple<Asteroid*, Asteroid*> toReturn(minus_asteroid, plus_asteroid);
	return toReturn;
	}

void Asteroid::gl_init(){

  vec2 triangulation[14];
  triangulation[0]=vec2(0,0);
  for (int i=1; i<14; i++){
	  triangulation[i]=rotation(i*2*pi/12.0)*vec2(0,0.2+0.6*rand()/(float)RAND_MAX);
	  }
  triangulation[13]=vec2(triangulation[1].x, triangulation[1].y);
	  
  for (int i=1; i<13; i++){
	  state.mass += length(cross(triangulation[i],triangulation[i+1]));
	  state.center += triangulation[i];
	  state.boundary[i-1]=triangulation[i];
  }
  state.boundary[12]=vec2(state.boundary[0].x, state.boundary[0].y);
  //state.mass += length(cross(triangulation[13], triangulation[1]));//This mass is twice the area in local coordinates.
  state.center = state.center/12.0;
  
  std::string vshader = shader_path + "vshader_Asteroid.glsl";
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
  
  glBindFragDataLocation(GLvars.program, 0, "fragColor");
  
  GLvars.vpos_location   = glGetAttribLocation(GLvars.program, "vPos");
  GLvars.vcolor_location = glGetUniformLocation(GLvars.program, "vColor" );
  GLvars.M_location = glGetUniformLocation(GLvars.program, "M" );
  
  // Create a vertex array object
  glGenVertexArrays( 1, &GLvars.vao );
  //Set GL state to use vertex array object
  glBindVertexArray( GLvars.vao );
  
  //Generate buffer to hold our vertex data
  glGenBuffers( 1, &GLvars.buffer );
  //Set GL state to use this buffer
  glBindBuffer( GL_ARRAY_BUFFER, GLvars.buffer );
  
  //Create GPU buffer to hold vertices
  glBufferData( GL_ARRAY_BUFFER, sizeof(triangulation), NULL, GL_STATIC_DRAW );
  //First part of array holds vertices
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(triangulation), triangulation );
  
  
  glEnableVertexAttribArray(GLvars.vpos_location);
  glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  //glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  //glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(triangulation)) );
  
  glBindVertexArray(0);

}

//Draw a ship
void Asteroid::draw(mat4 proj){
	
  proj = proj*Translate(state.cur_location.x,state.cur_location.y,0)*Scale(0.15,0.15,0.15)*RotateZ(180/3.14159265*state.angle)*Translate(-state.center.x,-state.center.y,0);
 
  glUseProgram( GLvars.program );
  glBindVertexArray( GLvars.vao );
  
  //If you have a modelview matrix, pass it with proj
  glUniformMatrix4fv( GLvars.M_location, 1, GL_TRUE, proj );
  glUniform4fv(GLvars.vcolor_location, 1, (1.0-state.flashNumber)*state.color + state.flashNumber*vec3(1.0,1.0,1.0));
  //Draw something
  glDrawArrays(GL_TRIANGLE_FAN, 0, 14);
  
  glBindVertexArray(0);
  glUseProgram(0);
}
vec2* Asteroid::getBoundary(){//returns the boundary in worldspace
	vec2 *toReturn= new vec2[13];
	for (int i=0; i<13; i++){ //We perform the same set of transformations as proj in the draw method
		toReturn[i]=state.boundary[i]-state.center; //translate to the center
		toReturn[i]=rotation(state.angle)*toReturn[i]; //rotate about that center
		toReturn[i]= 0.15*toReturn[i];
		toReturn[i]=toReturn[i] + state.cur_location;//translate to the current location.
		}
	return toReturn;
	}