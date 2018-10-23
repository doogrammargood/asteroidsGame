#include "common.h"
Background::Background(){
	
	int width = 4014; //pixels
	int height = 3981;
	int n = 3;
	stbi_set_flip_vertically_on_load(1);
	localBuffer = stbi_load("nebula.jpg", &width, &height, &n, 4);
	vec2 positions[4]={
		vec2(-1.0, -1.0),
		vec2(1.0, -1.0),
		vec2(1.0, 1.0),
		vec2(-1.0, 1.0)
		};
	vec2 tex_positions[4]={
		vec2(0.0, 0.0),
		vec2(1.0, 0.0),
		vec2(1.0, 1.0),
		vec2(0.0, 1.0)
		};
	std::string vshader = shader_path + "vtexture_Shader.glsl";
	std::string fshader = shader_path + "ftexture_Shader.glsl";
	GLchar* vertex_shader_source = readShaderSource(vshader.c_str());
	GLchar* fragment_shader_source = readShaderSource(fshader.c_str());
	
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, (const GLchar**) &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	check_shader_compilation(vshader, vertex_shader);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const GLchar**) &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	check_shader_compilation(fshader, fragment_shader);
	
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
    
	glLinkProgram(program);
	check_program_link(program);
	
	glGenTextures( 1, &textureBuffer );
	glBindTexture(GL_TEXTURE_2D, textureBuffer);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glGenBuffers(1, &buffer);
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	//glBindBuffer( GL_ARRAY_BUFFER, buffer );
	
	glBufferData( GL_ARRAY_BUFFER, 8*sizeof(vec2), NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, 4*sizeof(vec2), positions );
	glBufferSubData( GL_ARRAY_BUFFER, 4*sizeof(vec2), 4*sizeof(vec2), tex_positions);
	
	
	vpos_location = glGetAttribLocation(program, "pos");
	vtex_location = glGetAttribLocation(program, "texCoord");
	glEnableVertexAttribArray(vpos_location);
	glEnableVertexAttribArray(vtex_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
	glVertexAttribPointer(vtex_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(4*sizeof(vec2)) );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
	glBindTexture(GL_TEXTURE_2D, buffer);
	if (localBuffer){
		stbi_image_free(localBuffer);
		}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureBuffer);
	glBindFragDataLocation(program, 0, "color");
	uniform_color = glGetUniformLocation(program, "u_Color");
	uniform_texture = glGetUniformLocation(program, "u_Texture");
	glBindVertexArray(0);
	}
	
void Background::draw(){
  glUseProgram( program );
  glBindVertexArray( vao );
  glUniform1i(uniform_texture, 0);
  glUniform4fv(uniform_color, 1, vec4(1,1,1,1));
  glDrawArrays(GL_TRIANGLE_FAN,0,4);
	}