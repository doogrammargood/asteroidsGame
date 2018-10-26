cmake_minimum_required(VERSION 2.8)
PROJECT(ASTEROIDS)
SET(CMAKE_BUILD_TYPE "Release")
set (CMAKE_CXX_STANDARD 11)
if (!MSVC)
SET(CMAKE_CXX_FLAGS "-Wno-deprecated")
endif()

#Compile and Link GLFW
ADD_SUBDIRECTORY(glfw-3.2)
link_libraries(glfw)
include_directories(${glfw_INCLUDE_DIRS})
include_directories("${CMAKE_SOURCE_DIR}/glfw-3.2/deps")
#find_package(SFML)
#ADD_SUBDIRECTORY(SFML)
#include_directories(${SFML_INCLUDE_DIRS})
#link_libraries(SFML)
#add_library(SFML "${CMAKE-SOURCE_DIR}/main.hpp")
add_library(glad "${CMAKE_SOURCE_DIR}/glfw-3.2/deps/glad/glad.h"
         		 "${CMAKE_SOURCE_DIR}/glfw-3.2/deps/glad.c")

link_libraries(glad)


SET(MY_SHADER_PATH ${CMAKE_SOURCE_DIR}/shaders/)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/shaders/shader.cpp.in ${CMAKE_SOURCE_DIR}/shaders/shader.cpp)


include_directories(${CMAKE_SOURCE_DIR}/source
					${CMAKE_SOURCE_DIR}/source/common
					${CMAKE_SOURCE_DIR}/shaders)



add_executable(asteroids WIN32 MACOSX_BUNDLE
	source/Asteroid.h
	source/utils/geometryhelpers.h
	source/utils/geometryhelpers.cpp
	source/stb/stb_image.h
	source/stb/stb_image.cpp
	source/Asteroid.cpp
	source/Background.h
	source/Background.cpp
	source/common.h
	source/Bullet.h
  	source/Bullet.cpp
	source/utils/Angel.h
	source/utils/CheckError.h
	source/utils/mat.h
	source/utils/vec.h
	shaders/shader.cpp
	source/main.cpp
	source/Ship.cpp
	source/Ship.h
	source/Explosion.cpp
	source/Explosion.h
	shaders/fshader_Ship.glsl
	shaders/vshader_Ship.glsl
	shaders/fshader_Asteroid.glsl
	shaders/vshader_Asteroid.glsl
	shaders/vtexture_Shader.glsl
	shaders/ftexture_Shader.glsl
)

#set(SFML_DIR "${CMAKE_SOURCE_DIR}/SFML/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(SFML COMPONENTS system window graphics audio REQUIRED)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(asteroids ${SFML_LIBRARIES})

#Windows cleanup
if (MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(asteroids PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

#Apple cleanup
if (APPLE)
    set_target_properties(asteroids PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "simple")

    set_target_properties(asteroids PROPERTIES
                          MACOSX_BUNDLE_SHORT_VERSION_STRING ${GLFW_VERSION}
                          MACOSX_BUNDLE_LONG_VERSION_STRING ${GLFW_VERSION_FULL}
    					  MACOSX_BUNDLE_ICON_FILE glfw.icns)
endif()
