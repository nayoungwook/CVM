#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

#include "command.h"
#include "token.h"
#include "frame.h"
#include "builtin_function.h"
#include "code_memory.h"
#include "SDL.h"

namespace BUILTIN {

	class BUILTIN_CLASS : public ClassCodeMemory {
	public:
		BUILTIN_CLASS(const std::string& name) : ClassCodeMemory(name) {
			type = class_code_memory;
		};
	};

	class WINDOW : public BUILTIN_CLASS {
	private:
	public:
		SDL_Window* window = nullptr;
		WINDOW() : BUILTIN_CLASS("Window") {};
	};
}