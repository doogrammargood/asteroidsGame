#include "common.h"
#include <time.h>
#include <SFML/Audio.hpp>
#include <algorithm>
using namespace Angel;
int MAX_ASTEROIDS_SIZE=100;
Ship ship;
//Asteroid asteroid;
//Bullet bullet(vec2(0,0),vec2(0,0), vec2(0.0,1.0),vec3(1.0,0.0,0.0));
std::vector<Bullet*> bullets;
std::vector<Asteroid*> asteroids;
std::vector<Explosion*> explosions;
Bullet* bang;
Asteroid* rock;
Explosion* powt;
sf::SoundBuffer lasers_a;
sf::SoundBuffer lasers_e;
sf::SoundBuffer lasers_g;
sf::SoundBuffer b_chr;
sf::SoundBuffer c_chr;
sf::SoundBuffer fs_chr;
sf::SoundBuffer thruster_sound;
sf::SoundBuffer flippty;
sf::SoundBuffer collideb;
sf::SoundBuffer collidec;
sf::SoundBuffer collideea;
sf::SoundBuffer ea_chord;
sf::SoundBuffer b_harm;
sf::SoundBuffer e_harm;
sf::SoundBuffer g_harm;

sf::Sound noise_lasers_a;
sf::Sound noise_lasers_e;
sf::Sound noise_lasers_g;
sf::Sound noise_b_chr;
sf::Sound noise_c_chr;
sf::Sound noise_fs_chr;
sf::Sound noise_thruster_sound;
sf::Sound noise_flippty;
sf::Sound noise_collideb;
sf::Sound noise_collidec;
sf::Sound noise_collideea;
sf::Sound noise_ea_chord;
sf::Sound noise_b_harm;
sf::Sound noise_e_harm;
sf::Sound noise_g_harm;
sf::Music music;

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{	
	
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  if (!ship.living()){
	 if (key == GLFW_KEY_R && action == GLFW_PRESS){
		   for (auto it = asteroids.begin() ; it != asteroids.end(); ++it)
		{
		delete (*it);
		}	 
		asteroids.clear();
		ship.revive();
	 }
	return;
		}
  if (key == GLFW_KEY_LEFT){
	  if (action == GLFW_PRESS)
		  ship.rotate_left();
	  else if(action == GLFW_RELEASE)
		  ship.stop_rotate_left();
  }
  if (key == GLFW_KEY_RIGHT){
	  if (action == GLFW_PRESS)
		  ship.rotate_right();
	  else if(action == GLFW_RELEASE)
		  ship.stop_rotate_right();
  }
  if (key == GLFW_KEY_SPACE){
    if(action == GLFW_PRESS)
	  noise_thruster_sound.play();
      ship.start_thruster();
    if(action == GLFW_RELEASE)
      ship.stop_thruster();
  }
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
	  ship.flip();
	  noise_flippty.play();
  }
	  
  if (key == GLFW_KEY_S && action == GLFW_PRESS){
	  bang = ship.shoot('r');
	  noise_lasers_a.play();
	  bang->gl_init();
	  bullets.push_back(bang);
  }
  if (key == GLFW_KEY_D && action == GLFW_PRESS){
	  bang = ship.shoot('g');
	  noise_lasers_e.play();
	  bang->gl_init();
	  bullets.push_back(bang);
  }
  if (key == GLFW_KEY_F && action == GLFW_PRESS){
	  bang = ship.shoot('b');
	  noise_lasers_g.play();
	  bang->gl_init();
	  bullets.push_back(bang);
  }
  if (key == GLFW_KEY_M && action == GLFW_PRESS){
	  if (music.getVolume()>0)
		music.setVolume(0);
	  else
		music.setVolume(90);
	  }
  if (key == GLFW_KEY_N && action == GLFW_PRESS){
	  if (noise_lasers_a.getVolume()==0){
	    noise_flippty.setVolume(100);
		noise_lasers_a.setVolume(40);
		noise_lasers_e.setVolume(20);
		noise_lasers_g.setVolume(20);
		noise_b_chr.setVolume(40);
		noise_c_chr.setVolume(40);
		noise_fs_chr.setVolume(40);
		noise_thruster_sound.setVolume(70);
		noise_collideb.setVolume(60);
		noise_collidec.setVolume(60);
		noise_collideea.setVolume(60);
		noise_ea_chord.setVolume(60);
		noise_b_harm.setVolume(100);
		noise_e_harm.setVolume(100);
		noise_g_harm.setVolume(100);
	  }
	  else{
		noise_flippty.setVolume(0);
		noise_lasers_a.setVolume(0);
		noise_lasers_e.setVolume(0);
		noise_lasers_g.setVolume(0);
		noise_b_chr.setVolume(0);
		noise_c_chr.setVolume(0);
		noise_fs_chr.setVolume(0);
		noise_thruster_sound.setVolume(0);
		noise_collideb.setVolume(0);
		noise_collidec.setVolume(0);
		noise_collideea.setVolume(0);
		noise_ea_chord.setVolume(0);
		noise_b_harm.setVolume(0);
		noise_e_harm.setVolume(0);
		noise_g_harm.setVolume(0);
		  }
	  
	  }
}

void init(){
  
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);
  ship.gl_init();
}

//Call update function 30 times a second
void animate(){
  if(glfwGetTime() > 0.033){
    glfwSetTime(0.0);
    ship.update_state();
	bool remove_roid = false;
	bool hit = false; //set to true if a laser hit an asteroid.
	bool ship_alive = true;
	Bullet *to_remove = NULL;
	std::tuple<Asteroid*,Asteroid*> shards;
	std::vector<Asteroid*> newAsteroids;
	vec3 temp_color;
	vec3 roid_color;
	vec2 *left_laser_boundary;
	vec2 *right_laser_boundary;
	vec2 *asteroid_boundary;
	vec2 *ship_boundary;
	int numVerts;
	if (ship.get_sheild()>0)
		numVerts = 16;
	else
		numVerts = 23;
	for (auto it = bullets.begin(); it != bullets.end();){ // check if any bullets have reach their end of life
		//https://stackoverflow.com/questions/9927163/erase-element-in-vector-while-iterating-the-same-vector
		if(!(*it)->update_state()){
		  bullets.erase(it);
		}
		else
		  ++it;
	}
	for (auto it = explosions.begin(); it != explosions.end();){
		if(!(*it)->update_state()){
			explosions.erase(it);
		}
		else
		  ++it;
		}
	ship_boundary = ship.getBoundary();
	for (auto it = asteroids.begin(); it != asteroids.end();){
		remove_roid = false;
		(*it)->update_state();
		if (ship.living() and length((*it)->locate() - ship.locate())<1 and polygonsIntersect((*it)->getBoundary(),ship_boundary,13,numVerts)){ //Check to see if the ship hit an asteroid.
			roid_color = (*it)->get_color();
			shards=(*it)->shatter(ship.velocity(), ship.get_mass());
			ship_alive = ship.get_hit((*it)->get_velocity(),(*it)->get_mass(), (*it)->get_color());
			remove_roid = true;
			if (length((*it)->get_color()) < 1.2){
				if ((*it)->get_color().x == 1)
					noise_collideb.play();
				if ((*it)->get_color().y == 1)
					noise_collidec.play();
				if ((*it)->get_color().z == 1)
					noise_collideea.play();
			}
			else{
				if ((*it)->get_color().x == 0)
					noise_collideb.play();
				if ((*it)->get_color().y == 0)
					noise_collidec.play();
				if ((*it)->get_color().z == 0)
					noise_collideea.play();
				}
			newAsteroids.push_back(std::get<0>(shards));
			newAsteroids.push_back(std::get<1>(shards));
		}
		
		for (auto las = bullets.begin(); las != bullets.end();){ //Check to see if any bullets hit an asteroid.
			if(length((*it)->locate() - (*las)->locate() )< 0.3){
				asteroid_boundary = (*it)->getBoundary();
				left_laser_boundary =(*las)->leftBoundingBox();
				right_laser_boundary =(*las)->rightBoundingBox();
				if((*las)->has_left() and polygonsIntersect(asteroid_boundary, left_laser_boundary, 13, 5) ){
					(*las)->remove_left_laser();
					hit = true;
				}
				if((*las)->has_right() and polygonsIntersect(asteroid_boundary, right_laser_boundary, 13, 5) ){
					(*las)->remove_right_laser();
					hit = true;
				}
				if (hit){
					temp_color = (*las)->get_color() + (*it)->get_color();
					temp_color = vec3(std::min(1,(int)temp_color.x), std::min(1,(int)temp_color.y), std::min(1,(int)temp_color.z));
					if(length(temp_color - (*it)->get_color())>0.1){//If it has changed colors..
						roid_color = (*it)->get_color();
						(*it)->set_color(temp_color);
						if(length(temp_color - vec3(1,1,1))<0.01){
							remove_roid = true;
							noise_ea_chord.play();
						}
						if((*it)->get_color().x==0)
							noise_b_chr.play();
						if((*it)->get_color().y==0)
							noise_c_chr.play();
						if((*it)->get_color().z==0)
							noise_fs_chr.play();						
					}
					hit = false;
					delete[] asteroid_boundary;
					delete[] left_laser_boundary;
					delete[] right_laser_boundary;
				}

			}
			if ((*las)->is_gone()){
				bullets.erase(las);
			}
			else
				++las;
		}
		if(remove_roid){
			powt = new Explosion((*it)->locate(),roid_color);
			powt->gl_init();
			explosions.push_back(powt);
			asteroids.erase(it);
		}
		else
			++it;
	}
	if (!ship_alive){
		//powt = new Explosion(ship.locate(),vec3(0.25,0.5,1));
		powt = new Explosion(ship.locate(),vec3(1,1,1));
		powt->gl_init();
		explosions.push_back(powt);
		temp_color = ship.sheild_color();
		if (length(temp_color) < 1.1){
			if (temp_color.x >0)
				noise_b_harm.play();
			else if (temp_color.y>0)
				noise_e_harm.play();
			else if (temp_color.z>0)
				noise_g_harm.play();
			}
		else{
			if (temp_color.x < 1)
				noise_b_harm.play();
			else if (temp_color.y<1)
				noise_e_harm.play();
			else if (temp_color.z<1)
				noise_g_harm.play();
			}
		
		}
	delete[] ship_boundary;
	for(auto ast = newAsteroids.begin(); ast != newAsteroids.end();){
		if(asteroids.size()<MAX_ASTEROIDS_SIZE){
			(*ast)->gl_init();
			asteroids.push_back(*ast);
		}
		newAsteroids.erase(ast);
		}
	}
  }


int main(void)
{
if (!lasers_a.loadFromFile("sounds/e_shoot_session.wav"))
  std::cout<<"can't find a"<<std::endl;
if (!lasers_e.loadFromFile("sounds/b_shoot_session.wav"))
  std::cout<<"can't find e"<<std::endl;
if (!lasers_g.loadFromFile("sounds/g_shoot_session.wav"))
  std::cout<<"can't find g"<<std::endl;
if (!b_chr.loadFromFile("sounds/bm_chord_session.wav"))
  std::cout<<"can't find b chord"<<std::endl;
if (!c_chr.loadFromFile("sounds/c_chord_session.wav"))
  std::cout<<"can't find c chord"<<std::endl;
if (!fs_chr.loadFromFile("sounds/fsharpchord_session.wav"))
  std::cout<<"can't find fs chord"<<std::endl;
if (!thruster_sound.loadFromFile("sounds/thruster_session.wav"))
  std::cout<<"can't find thruster_sound"<<std::endl;
if (!flippty.loadFromFile("sounds/flippity_a_session.wav"))
  std::cout<<"can't load flippity sound."<<std::endl;
if (!collideb.loadFromFile("sounds/bouncy_a_session.wav") )
  std::cout<<"can't load a chord."<<std::endl;
if (!collidec.loadFromFile("sounds/bouncy_g_session.wav") )
  std::cout<<"can't load g chord."<<std::endl;
if (!collideea.loadFromFile("sounds/bouncy_g9_session.wav") )
  std::cout<<"can't load g9 chord."<<std::endl;
if (!ea_chord.loadFromFile("sounds/ea_chord_session.wav") )
  std::cout<<"can't load ea chord."<<std::endl;
if (!b_harm.loadFromFile("sounds/b_harm_session.wav"))
  std::cout<<"can't load b harm"<<std::endl;
if (!e_harm.loadFromFile("sounds/e_harm_session.wav"))
  std::cout<<"can't load e harm"<<std::endl;
if (!g_harm.loadFromFile("sounds/g_harm_session.wav"))
  std::cout<<"can't load g harm"<<std::endl;
  
  noise_lasers_a.setBuffer(lasers_a);
  noise_lasers_e.setBuffer(lasers_e);
  noise_lasers_g.setBuffer(lasers_g);
  noise_b_chr.setBuffer(b_chr);
  noise_c_chr.setBuffer(c_chr);
  noise_fs_chr.setBuffer(fs_chr);
  noise_thruster_sound.setBuffer(thruster_sound);
  noise_flippty.setBuffer(flippty);
  noise_collideb.setBuffer(collideb);
  noise_collidec.setBuffer(collidec);
  noise_collideea.setBuffer(collideea); 
  noise_ea_chord.setBuffer(ea_chord);
  noise_b_harm.setBuffer(b_harm);
  noise_e_harm.setBuffer(e_harm);
  noise_g_harm.setBuffer(g_harm);
  
  
  noise_flippty.setVolume(100);
  noise_lasers_a.setVolume(40);
  noise_lasers_e.setVolume(20);
  noise_lasers_g.setVolume(20);
  noise_b_chr.setVolume(40);
  noise_c_chr.setVolume(40);
  noise_fs_chr.setVolume(40);
  noise_thruster_sound.setVolume(70);
  noise_collideb.setVolume(60);
  noise_collidec.setVolume(60);
  noise_collideea.setVolume(60);
  noise_ea_chord.setVolume(60);
  noise_b_harm.setVolume(100);
  noise_e_harm.setVolume(100);
  noise_g_harm.setVolume(100);

  /*noise_flippty.setVolume(0);
  noise_lasers_a.setVolume(0);
  noise_lasers_e.setVolume(0);
  noise_lasers_g.setVolume(0);
  noise_b_chr.setVolume(0);
  noise_c_chr.setVolume(0);
  noise_fs_chr.setVolume(0);
  noise_thruster_sound.setVolume(0);
  noise_collideb.setVolume(0);
  noise_collidec.setVolume(0);
  noise_collideea.setVolume(0);
  noise_ea_chord.setVolume(0);*/
  if (!music.openFromFile("sounds/game_music_session.ogg"))
	std::cout<<"can't find music."<<std::endl;
	//return -1; // error
  music.setVolume(90);
  //music.setVolume(0);
  music.play();
  srand (time(NULL));
  GLFWwindow* window;
  mat4 proj;
  glfwSetErrorCallback(error_callback);
  
  if (!glfwInit())
    exit(EXIT_FAILURE);
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  glfwWindowHint(GLFW_SAMPLES, 10);
  
  
  window = glfwCreateWindow(1024, 768, "Kaliedonaughts", NULL, NULL);
  if (!window){
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwSetKeyCallback(window, key_callback);
  
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);
  init();
  Background background;
  while (!glfwWindowShouldClose(window)){
	if(rand()/(float)RAND_MAX<0.1/(10*(float)asteroids.size() +1.0)and asteroids.size()<MAX_ASTEROIDS_SIZE){
		rock = new Asteroid();
		rock->gl_init();
		asteroids.push_back(rock);
		}
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    //Pick a coordinate system that makes the most sense to you
    //(left, right, top, bottom)
    proj = Ortho2D(-1.0, 1.0, 1.0, -1.0);
    
    animate();
	if (!ship.living())
		background.setEndscreen();
	else
		background.unsetEndscreen();
    
    glClear(GL_COLOR_BUFFER_BIT);
	
	background.draw();
	ship.draw(proj);
	//asteroid.draw(proj);
	for (auto it = bullets.begin(); it != bullets.end(); ++it)
		(*it)->draw(proj);
	for (auto it = asteroids.begin(); it != asteroids.end(); ++it)
		(*it)->draw(proj);
	for (auto it = explosions.begin(); it != explosions.end(); ++it)
		(*it)->draw(proj);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
    
  }
  
  glfwDestroyWindow(window);
  
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
