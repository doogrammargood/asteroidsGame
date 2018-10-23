
#ifndef __Background__
#define __Background__

#include "common.h"
#include "stb/stb_image.h"
class Background{
	/*This class draws the background, an image from the Hubble Telescope:
	 * "the central region of the Tarantula Nebula in the Large Magellanic Cloud."
	 *The Cherno has a good description of how these work:
	 * https://www.youtube.com/watch?v=n4k7ANAFsIQ
	 * */
	
	GLuint textures;
	GLuint vao;           //Vertex array object
	GLuint program;       //shader program
	GLuint textureBuffer; //The texture buffer
	GLuint buffer;		  //The actual buffer
	GLuint vertex_shader, fragment_shader;  //Shaders
	GLint vpos_location, vtex_location;   //reference to pos and texture coords in shaders
	GLint uniform_color;
	GLint uniform_texture;
	unsigned char* localBuffer;
	
public:
	
	Background();
	void draw();
	};
#endif