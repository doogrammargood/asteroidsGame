//
//  Ship.cpp
//  Asteroids
//
//  Created by Brian Summa on 6/5/15.
//
//

#include "common.h"
#include <functional>
#include <cmath>
#include <typeinfo>
#include "Bullet.h"
//Ship constructor
Ship::Ship(){
	state.cur_location = vec2(0,0);
	state.pointing = vec2(0,-1.0);
	state.velocity = vec2(0.0,0.0);
	state.angle = 0;
	state.turning_left = false;
	state.turning_right = false;
	state.thruster_on = false;
	state.sheild = 10;
};

//Called everytime an animation tick happens
void Ship::update_state(){
	
	float acceleration = 0;
	if (state.turning_left && !state.turning_right){
		state.angle -= 10.0;
		state.pointing = state.RotateZ2D(state.angle)*vec2(0,-1.0);
	}else if(!state.turning_left && state.turning_right){
		state.angle += 10.0;
		state.pointing = state.RotateZ2D(state.angle)*vec2(0,-1.0);
		}
    if (state.thruster_on){
	    acceleration=1/(20*state.mass);
	  }
    state.velocity += acceleration*state.pointing;
	state.velocity *= 0.98;
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
Bullet* Ship::shoot(char color){
	Bullet* zapper;
	if (color == 'r')
		zapper = new Bullet(state.cur_location,state.velocity,state.pointing,vec3(1.0, 0.0, 0.0));
	else if (color == 'g')
		zapper = new Bullet(state.cur_location,state.velocity,state.pointing,vec3(0.0, 1.0, 0.0));
	else if (color == 'b')
		zapper = new Bullet(state.cur_location,state.velocity,state.pointing,vec3(0.0, 0.0, 1.0));
	return zapper;
	}
void Ship::get_hit(vec2 other_velocity, float other_mass){
	vec2 midline=((state.mass - other_mass) / (state.mass + other_mass) )*state.velocity + (2*other_mass/(state.mass + other_mass))*other_velocity;
	state.velocity = midline;
	state.sheild -= length(other_velocity)*other_mass;
	}
void Ship::prepare_thruster(){
	//The thruster will have this shape http://mathworld.wolfram.com/TeardropCurve.html with m=7.
	
  GLvars.vpos_location   = glGetAttribLocation(GLvars.program, "vPos");
  GLvars.vcolor_location = glGetAttribLocation(GLvars.program, "vColor" );
  //M_location = glGetUniformLocation(glShaderProgram, "M" );
  vec2 *thruster_curve;
  vec2 thruster[50];
  vec3 thruster_colors[50];
  auto f = [](float x){float t = acos(x); return sin(t)*pow(sin(0.5*t),4); };
  thruster_curve = drawCurve(-1.0, 1.0, f, 25);
  for (int i=0; i<25; i++){//Rotate the curve 90 degrees, move it down and scale it.
	  thruster_curve[i] = rotation(3.1415926535/2.0)*thruster_curve[i];
	  thruster_curve[i].y = thruster_curve[i].y+1.625;
	  thruster_curve[i] = 1/7.0*5.75/2.0*thruster_curve[i];
	  thruster_curve[i].x = 0.75*thruster_curve[i].x;//make it thinner
	  }
  for (int i=0; i<25; i++){ //Copy the curve into 'thruster'
	  thruster[i]=thruster_curve[i];
	  }
  for (int i=0; i<25; i++){ //Store the reverse of the curve backwards to complete the teardrop.
	  thruster[25+i] = vec2(-thruster_curve[24-i].x,thruster_curve[24-i].y);
	  }
  for (int i=0; i<50; i++){
	  thruster_colors[i]=vec3(216/255.0,60/255.0,63/255.0);
	 
	  }
  int thruster_size = sizeof(thruster);
  int color_size = sizeof(thruster_colors);
  //GLvars.vpos_location   = glGetAttribLocation(GLvars.program, "vPos");
  //GLvars.vcolor_location = glGetAttribLocation(GLvars.program, "vColor" );

  glGenVertexArrays(1,&GLvars.thrusterVAO);
  glBindVertexArray(GLvars.thrusterVAO);
  glGenBuffers(1,&GLvars.thruster_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, GLvars.thruster_buffer);
  glBufferData(GL_ARRAY_BUFFER, thruster_size+color_size, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, thruster_size, thruster);
  glBufferSubData(GL_ARRAY_BUFFER,thruster_size,color_size,thruster_colors);
  glEnableVertexAttribArray(GLvars.vpos_location);
  glEnableVertexAttribArray(GLvars.vcolor_location);
  glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(thruster_size) );		
  glBindVertexArray(0);
	
	}
void Ship::prepare_sheild(){
	//Draws the sheild for the ship.
	vec2 *shpt1, *shpt2, *shpt3, *shpt4;
	vec2 sheild[100];
	//sheild = drawCurve(-0.5, .5, [](float x){return x;}, 40);
	float *params1 = findParabola(vec2(-4.5/7.0, 0),vec2(0, -7/7.0),vec2(4.5/7.0, 0));
	float *params2 = findParabola(vec2(4.5/7.0, 0/7.0),vec2(3/7.0, -2/7.0),vec2(1.5/7.0, 1/7.0));
	float *params3 = findParabola(vec2(1.5/7.0, 1/7.0),vec2(0/7.0, -3.5/7.0),vec2(-1.5/7.0, 1/7.0));
	float *params4 = findParabola(vec2(-1.5/7.0, 1/7.0),vec2(-3/7.0,-2/7.0),vec2(-4.5/7.0, 0/7.0));
	auto f1 = [params1](float x){return params1[0]*pow(x,2)+params1[1]*x+params1[2];};
	auto f2 = [params2](float x){return params2[0]*pow(x,2)+params2[1]*x+params2[2];};
	auto f3 = [params3](float x){return params3[0]*pow(x,2)+params3[1]*x+params3[2];};
	auto f4 = [params4](float x){return params4[0]*pow(x,2)+params4[1]*x+params4[2];};
	
	shpt1 = drawCurve(-4.5/7.0, 4.5/7.0, f1, 40);
	shpt2 = drawCurve(4.5/7.0, 1.5/7.0, f2, 20);
	shpt3 = drawCurve(1.5/7.0, -1.5/7.0, f3, 20);
	shpt4 = drawCurve(-1.5/7.0, -4.5/7.0, f4, 20);
	
	for (int i=0; i<100; i++){
		if (i<40){sheild[i]=shpt1[i];}
		else if(i<60){sheild[i]=shpt2[i-40];}
		else if(i<80){sheild[i]=shpt3[i-60];}
		else {sheild[i]=shpt4[i-80];}
		}
	glBindFragDataLocation(GLvars.program, 0, "fragColor");
	
	vec3 sheild_colors[100];
	for (int i=0; i<100; i++){
		sheild_colors[i]=vec3(1.0,1.0,1.0);
		}
	  
  
  GLvars.vpos_location   = glGetAttribLocation(GLvars.program, "vPos");
  GLvars.vcolor_location = glGetAttribLocation(GLvars.program, "vColor" );
  //M_location = glGetUniformLocation(glShaderProgram, "M" );
  
  int sheild_size = sizeof(sheild);
  int color_size = sizeof(sheild_colors);

  glGenVertexArrays(1,&GLvars.sheildVAO);
  glBindVertexArray(GLvars.sheildVAO);
  glGenBuffers(1,&GLvars.sheild_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, GLvars.sheild_buffer);
  glBufferData(GL_ARRAY_BUFFER, sheild_size+color_size, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sheild_size, sheild);
  glBufferSubData(GL_ARRAY_BUFFER,sheild_size,color_size,sheild_colors);
  glEnableVertexAttribArray(GLvars.vpos_location);
  glEnableVertexAttribArray(GLvars.vcolor_location);
  glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sheild_size) );		
  glBindVertexArray(0);
  delete[] shpt1; //Delete the sheild parts
  delete[] shpt2;
  delete[] shpt3;
  delete[] shpt4;
  delete[] params1;
  delete[] params2;
  delete[] params3;
  delete[] params4;
	}
	
void Ship::prepare_cockpit(){
  vec2 cockpit[361]; //will be an elipse.
  
  for (int i=0; i<361; i++){
	  cockpit[i] = rotation((i-1)*pi/180.0)*vec2(0.0,1.0);
	  }
  vec3 color_cockpit[361];
  color_cockpit[0]=vec3(1.0,1.0,1.0);
  for (int i=0; i<361; i++){
	  color_cockpit[i] = vec3(1.0,1.0,1.0);
	  }
  mat2 scale(vec2(0.8/7.0,0.0),vec2(0.0,2.0/7.0));
  for (int i=0; i<361; i++){ //translate vertically by 2
      cockpit[i] = scale*cockpit[i]; //stretch the cockpit into an elipse
	  cockpit[i] = cockpit[i] + vec2(0.0, -2.75/7.0);
	  }
	  
	  //String with absolute paths to our shaders, shader_path set by CMAKE hack
  std::string vshader = shader_path + "vshader_Ship.glsl";
  std::string fshader = shader_path + "fshader_Ship.glsl";
  
  GLchar* vertex_shader_source = readShaderSource(vshader.c_str());
  GLchar* fragment_shader_source = readShaderSource(fshader.c_str());
  
  
  glBindFragDataLocation(GLvars.program, 0, "fragColor");
  
  GLvars.vpos_location   = glGetAttribLocation(GLvars.program, "vPos");
  GLvars.vcolor_location = glGetAttribLocation(GLvars.program, "vColor" );
  //M_location = glGetUniformLocation(glShaderProgram, "M" );
	  
  glGenVertexArrays(1,&GLvars.cockpitVAO);
  glBindVertexArray(GLvars.cockpitVAO);
  glGenBuffers(1,&GLvars.cockpit_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, GLvars.cockpit_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cockpit)+sizeof(color_cockpit), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cockpit), cockpit);
  glBufferSubData(GL_ARRAY_BUFFER,sizeof(cockpit),sizeof(color_cockpit),color_cockpit);
  glEnableVertexAttribArray(GLvars.vpos_location);
  glEnableVertexAttribArray(GLvars.vcolor_location);
  glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(cockpit)) );		
  glBindVertexArray(0);

	}
void Ship::gl_init(){
  vec2 ship_vert[23]= { vec2( 0.5/7.0, 1.0/7.0),
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
						
  vec3 ship_color[230];
  
  for (int i =0; i<230; i++){
	  ship_color[i] = vec3(1.0, 1.0, 1.0);
	  }
  vec2 *smooth_ship;
  //vec2 new_smooth_ship[9];
  smooth_ship = smoothCorners(ship_vert, 23);
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
  
  glBindFragDataLocation(GLvars.program, 0, "fragColor");
  
  GLvars.vpos_location   = glGetAttribLocation(GLvars.program, "vPos");
  GLvars.vcolor_location = glGetAttribLocation(GLvars.program, "vColor" );
  GLvars.M_location = glGetUniformLocation(GLvars.program, "M" );
  
  // Create a vertex array object
  glGenVertexArrays( 1, &GLvars.vao );
  //Set GL state to use vertex array object
  glBindVertexArray( GLvars.vao );
  
  //Generate buffer to hold our vertex data
  glGenBuffers( 1, &GLvars.buffer );
  //Set GL state to use this buffer
  glBindBuffer( GL_ARRAY_BUFFER, GLvars.buffer );
  
  //Create GPU buffer to hold vertices and color
  glBufferData( GL_ARRAY_BUFFER, 230*sizeof(*smooth_ship) + sizeof(ship_color), NULL, GL_STATIC_DRAW );
  //First part of array holds vertices
  glBufferSubData( GL_ARRAY_BUFFER, 0, 230*sizeof(*smooth_ship), *smooth_ship );
  //Second part of array hold colors (offset by sizeof(triangle))
  glBufferSubData( GL_ARRAY_BUFFER, 230*sizeof(*smooth_ship), sizeof(ship_color), ship_color );
  
  
  glEnableVertexAttribArray(GLvars.vpos_location);
  glEnableVertexAttribArray(GLvars.vcolor_location );
  
  glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glVertexAttribPointer( GLvars.vcolor_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(230*sizeof(*smooth_ship)) );
  
  glBindVertexArray(0);
  
  prepare_sheild();
  prepare_cockpit();
  prepare_thruster();

}

//Draw a ship
void Ship::draw(mat4 proj){
	
  proj = proj*Translate(state.cur_location.x,state.cur_location.y,0)*Scale(0.15,0.15,0.15)*RotateZ(state.angle)*Translate(0,0.25,0);
  //GLuint glShaderProgram;
  
  //GLuint cockpitVAO;
  glUseProgram( GLvars.program );
  glBindVertexArray( GLvars.vao );
  
  //If you have a modelview matrix, pass it with proj
  glUniformMatrix4fv( GLvars.M_location, 1, GL_TRUE, proj );
  
  //Draw something
  glDrawArrays(GL_LINE_LOOP, 0, 230);
  glBindVertexArray(GLvars.cockpitVAO);
  glDrawArrays(GL_LINE_LOOP,0,361);
  if (state.sheild>0){
	glBindVertexArray(GLvars.sheildVAO);
	glDrawArrays(GL_LINE_LOOP,0,100);
  }
  if(state.thruster_on){
    glBindVertexArray(GLvars.thrusterVAO);
	glDrawArrays(GL_LINE_LOOP,0,100);
  }
  
  glBindVertexArray(0);
  glUseProgram(0);

}
