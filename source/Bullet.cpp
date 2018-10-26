#include "common.h"
//#include "utils/geometryhelpers.h"
using namespace Angel;
mat2 rotaion(float angle){ //Returns a rotation matrix by a given angle. The angle is in radians
  mat2 toReturn(vec2(cos(angle),-sin(angle)),vec2(sin(angle), cos(angle)));
  return toReturn;
}
Bullet::Bullet(vec2 pos, vec2 vel, vec2 pointing, vec3 color){
	state.color = color;
	state.velocity = vel + 0.1*normalize(pointing);
	state.position = pos;
    state.left_laser = true;
    state.right_laser = true;
	state.pointing = pointing;
	state.angle = atan2(pointing.x,pointing.y);
	state.time_to_live = 100;
}

bool Bullet::update_state(){
	state.position += state.velocity;
	state.time_to_live -= 1;
	if (state.position.x>1)
		state.position.x-=2;
	if (state.position.y>1)
		state.position.y-=2;
	if (state.position.x<-1)
		state.position.x+=2;
	if (state.position.y<-1)
		state.position.y+=2;
	if (state.time_to_live == 0)
		return false;
	return true;
	}
	
void Bullet::gl_init(){
  vec2 elipse_outer[361]; //The outer edge of the elipse.
  vec2 elipse_inner[361]; //The inner edge
  vec3 bullet_color_left[361*2];
  vec3 bullet_color_right[361*2];
  vec2 left_bullet[2*361];
  vec2 right_bullet[2*361];
  for (int i=0; i<361; i++){
	  elipse_outer[i] = rotaion((i-1)*3.141592654/180.0)*vec2(0.0,1.0);
	  elipse_inner[i] = elipse_outer[i];
	  elipse_outer[i].y *= 25.0; //scale outer more than inner
	  elipse_inner[i].y *= 20.0;
	  if (elipse_outer[i].y>=0)
		elipse_outer[i].y = pow(elipse_outer[i].y,0.5); //flattens the elipse
	  else
		elipse_outer[i].y = -pow(-elipse_outer[i].y,0.5);
	  if (elipse_inner[i].y>=0)
		elipse_inner[i].y = pow(elipse_inner[i].y,0.5);
	  else
		elipse_inner[i].y = -pow(-elipse_inner[i].y,0.5);
	  elipse_inner[i].x *= 0.99;
	  elipse_outer[i].x -= 3.0; //Shift left
	  elipse_inner[i].x -= 3.0;
	  elipse_outer[i] = elipse_outer[i]*(1/7.0);
	  elipse_inner[i] = elipse_inner[i]*(1/7.0);
	  }
  for (int i=0; i<361*2; i++){
	  bullet_color_left[i] = state.color;
	  bullet_color_right[i] = state.color;
  }
  for (int i = 0; i<2*361; i+=2){ //We interleave the outer and inner elipses.
	  left_bullet[i]=elipse_outer[i/2];
	  left_bullet[i+1]=elipse_inner[i/2];
	  right_bullet[i]=elipse_outer[i/2]+vec2(6.0/7.0,0);//shifts the bullet by 2/7ths
	  right_bullet[i+1]=elipse_inner[i/2]+vec2(6.0/7.0,0);
  }
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
  GLvars.vcolor_location =  glGetUniformLocation(GLvars.program,  "vColor" );
  GLvars.M_location = glGetUniformLocation(GLvars.program, "M" );
  
  
  glGenVertexArrays(1,&GLvars.vao_right);
  glBindVertexArray(GLvars.vao_right);
  glGenBuffers(1,&GLvars.buffer_right);
  glBindBuffer(GL_ARRAY_BUFFER, GLvars.buffer_right);
  glBufferData(GL_ARRAY_BUFFER, sizeof(right_bullet), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(right_bullet), right_bullet);
  glEnableVertexAttribArray(GLvars.vpos_location);
  glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glBindVertexArray(0);
  
  glGenVertexArrays( 1, &GLvars.vao_left );
  glBindVertexArray( GLvars.vao_left );
  glGenBuffers( 1, &GLvars.buffer_left );
  glBindBuffer( GL_ARRAY_BUFFER, GLvars.buffer_left );
  glBufferData( GL_ARRAY_BUFFER, sizeof(left_bullet), NULL, GL_STATIC_DRAW );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(left_bullet), left_bullet );
  glEnableVertexAttribArray(GLvars.vpos_location);
  glVertexAttribPointer( GLvars.vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glBindVertexArray(0);
	
}
void Bullet::draw(mat4 proj){
	
  glUseProgram( GLvars.program );
  proj = proj*Translate(state.position.x,state.position.y,0)*Scale(0.15,0.15,0.15)*RotateZ(-360/(2*3.141592654)*state.angle);
  //If you have a modelview matrix, pass it with proj
  glUniformMatrix4fv( GLvars.M_location, 1, GL_TRUE, proj );
  glUniform4fv(GLvars.vcolor_location, 1, vec4(state.color.x, state.color.y, state.color.z, 1));
  glUseProgram( GLvars.program );
  if (state.right_laser){
    glBindVertexArray(GLvars.vao_right);//draw the right bullet
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 2*361);
  }
  if (state.left_laser){
    glBindVertexArray( GLvars.vao_left );//draw the left bullet
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 2*361);
  }
    
  glBindVertexArray(0);
  glUseProgram(0);
}

vec2 Bullet::locate(){
	return state.position;
	}
	
vec2* Bullet::leftBoundingBox(){
	vec2 *boundingBox=new vec2[5]{vec2(4/7.0, 5/7.0), vec2(2/7.0, 5/7.0), vec2(2/7.0,-5/7.0),vec2(4/7.0,-5/7.0),vec2(4/7.0, 5/7.0)};
	for (int i = 0; i<5; i++){
		boundingBox[i]= rotation(state.angle)*boundingBox[i];
		boundingBox[i]= 0.15*boundingBox[i];
		boundingBox[i]= boundingBox[i]+state.position;
		}
	return boundingBox;
	}
vec2* Bullet::rightBoundingBox(){
	vec2 *boundingBox=new vec2[5]{vec2(-4/7.0, 5/7.0),vec2(-4/7.0,-5/7.0),vec2(-2/7.0, -5/7.0),vec2(-2/7.0,5/7.0),vec2(-4/7.0, 5/7.0)};
	for (int i = 0; i<5; i++){
		boundingBox[i]= rotation(state.angle)*boundingBox[i];
		boundingBox[i]= 0.15*boundingBox[i];
		boundingBox[i]= boundingBox[i]+state.position;
		}
	return boundingBox;
	}