#pragma once

#include "engine.h"
#include "machine.h"
#include "error.h"
#include <fstream>
#include <iostream>
#include "cstdlib"

#define NO_SDL_GLEXT
#include "gl/glew.h"
#include "SDL_opengl.h"

#include "SDL.h"
#include "SDL_image.h"

namespace engine {
	static bool initialized = false;

	void initialize_engine(Command* command);
	void run_scene_function(Memory* scene_memory, const std::string& name, Machine* machine, Command* command);
	void game_loop(SDL_Window* window, Machine* machine, Command* command);
	SDL_Window* create_window(Machine* machine, const std::string& title, int width, int height);
	void shutdown(Machine* machine, SDL_Window* window);
}