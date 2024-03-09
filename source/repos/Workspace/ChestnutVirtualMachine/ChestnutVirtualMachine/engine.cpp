#include "engine.h"

namespace engine {
	void initialize_engine(Command* command) {
		int _sdl_init_result = SDL_Init(SDL_INIT_VIDEO);

		if (_sdl_init_result != 0) {
			SDL_Log("Unable to initizlie SDL : %s", SDL_GetError());
			exit(1);
		}

		IMG_Init(IMG_INIT_PNG);
		IMG_Init(IMG_INIT_JPG);

		std::cout << "           =@%%@=        " << std::endl;
		std::cout << "         =@%%%%%@@=      " << std::endl;
		std::cout << "       =@%*%%%%%%%@@=    " << std::endl;
		std::cout << "     =@@@@@@@@@@@@@@@@=  " << std::endl;
		std::cout << "    @@@@@@@@@@@@@@@@%@@@=" << std::endl;
		std::cout << "   @@@@@@@@@@@@@@@@@@@@@@" << std::endl;
		std::cout << "   @%%%%%@@@@@@@@@@@@@@@@" << std::endl;
		std::cout << "  ==                  #@@" << std::endl;
		std::cout << " = **  *.*           %%@ " << std::endl;
		std::cout << "=  *.. .=:%        %++   " << std::endl;
		std::cout << "=..=   = .-==+++=-::.    " << std::endl;

		std::cout << "CHESTNUT - BY NAYOUNGWOOK. (2024) Engine initialized." << std::endl;

		initialized = true;
	}

	void run_scene_function(Memory* scene_memory, const std::string& name, Machine* machine, Command* command) {
		FunctionFrame* function = new FunctionFrame(name);
		SceneData* scene_data = (static_cast<SceneData*>(scene_memory->data));

		if (((SceneCodeMemory*)scene_data->code_memory)->member_functions[name]->code_memories.find("") == ((SceneCodeMemory*)scene_data->code_memory)->member_functions[name]->code_memories.end()) {
			std::cout << "Error, cannot find scene function " << name << std::endl;
			exit(1);
		}

		FunctionCodeMemory* function_code_memory = ((SceneCodeMemory*)scene_data->code_memory)->member_functions[name]->code_memories[""];

		function->code_memory = function_code_memory;
		function->caller_class = scene_memory;

		function->run(machine, command);
	}

	SDL_Window* create_window(Machine* machine, const std::string& title, int width, int height) {
		SDL_Window* result = SDL_CreateWindow(title.c_str(), 100, 100, width, height, SDL_WINDOW_OPENGL);
		machine->context = SDL_GL_CreateContext(result);

		glewExperimental = GL_TRUE;

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

		SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		
		int init_status = glewInit();
		if (init_status != GLEW_OK) {
			SDL_Log("Failed to inintialize OpenGL : %s", glewGetErrorString(init_status));
			exit(1);
		}

		glGetError();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// create default shader
		machine->default_shader = create_shader("./fragment.glsl", "./transform.glsl");

		machine->default_shader->register_uniform_data("uWorldTransform");
		machine->default_shader->register_uniform_data("uViewProj");

		Memory* default_shader_memory = new Memory(_shader, "shader");
		default_shader_memory->data = machine->default_shader;
		machine->global_memories.insert(std::make_pair("default_shader", default_shader_memory));

		machine->current_shader = machine->default_shader;

		return result;
	}

	void game_loop(SDL_Window* window, Machine* machine, Command* command) {

		if (machine->global_memories.find("current_scene") == machine->global_memories.end()
			||
			machine->global_memories.find("current_scene")->second->data->root_type == _null) {
			std::cout << "Error! current_scene is null. to start engine please declare it." << std::endl;
			exit(1);
		}

		Memory* current_scene = machine->global_memories.find("current_scene")->second;

		engine::run_scene_function(current_scene, "init", machine, command);

		bool _running = true;
		int tick_count = 0;
		int backup_ticks = 0, current_ticks = 0;

		SDL_Event event;

		while (_running) {
			if (current_ticks - backup_ticks != 0)
				std::cout << 1000 / (current_ticks - backup_ticks) << std::endl;

			backup_ticks = current_ticks;
			tick_count = SDL_GetTicks();
			current_ticks = tick_count;

			Uint32 start_time = SDL_GetTicks();

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_QUIT:
					_running = false;
					break;
				}
			}

			engine::run_scene_function(current_scene, "tick", machine, command);

			glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			engine::run_scene_function(current_scene, "render", machine, command);

			SDL_GL_SwapWindow(window);
			if ((1000 / 60) > (SDL_GetTicks() - start_time))
			{
				SDL_Delay((1000 / 60) - (SDL_GetTicks() - start_time)); //Yay stable framerate!
			}
		}

		shutdown(machine, window);
	}

	void shutdown(Machine* machine, SDL_Window* window) {
		SDL_DestroyWindow(window);
		SDL_GL_DeleteContext(machine->context);

		std::cout << "SDL Quitting...." << std::endl;
		SDL_Quit();
	}
}