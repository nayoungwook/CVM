#include "builtin_function.h"

bool BUILTIN::BUILTIN_FUNCTION::is_infinite_parameters() const {
	return this->infinite_parameters;
}

void BUILTIN::PRINT::run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) {
	for (int i = 0; i < frame->local_memories.size(); i++) {
		std::cout << get_memory_data_print_value(frame->local_memories[std::to_string(i)], machine, caller);
	}
}

void BUILTIN::INPUT::run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) {
	std::string _in;
	std::cin >> _in;

	Memory* result = new Memory(_string, "string");
	result->data = new StringLiteralData(_in);

	caller->stack->push(result);
}

void BUILTIN::INITIALIZE_ENGINE::run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) {
	engine::initialize_engine(command);
}

Memory* create_empty_class_memory(const std::string& class_name) {
	Memory* result = new Memory(_class, class_name);
	ClassData* data = new ClassData(class_name);
	result->data = data;
	return result;
}

Memory* create_empty_texture_memory(Machine* machine, const std::string& path) {
	Memory* result = new Memory(_texture, "texture");
	TextureData* data = new TextureData(machine, path);
	result->data = data;
	return result;
}

Memory* create_empty_number_memory() {
	Memory* result = new Memory(_number, "number");
	NumberData* data = new NumberData(0);
	result->data = data;
	return result;
}

Memory* create_empty_integer_memory() {
	Memory* result = new Memory(_integer, "int");
	IntegerData* data = new IntegerData(0);
	result->data = data;
	return result;
}

Data* get_parameter_data(FunctionFrame* frame, const std::string& name) {
	if (frame->local_memories.find(name) != frame->local_memories.end()) {
		return frame->local_memories[name]->data;
	}
	else {
		std::cout << "Error! cannot find parameter : " << name << std::endl;
		exit(1);
	}

	return nullptr;
}

void BUILTIN::CREATE_WINDOW::run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) {
	Memory* result = create_empty_class_memory("Window");

	BUILTIN::WINDOW* _window = new BUILTIN::WINDOW();

	static_cast<BUILTIN::WINDOW*>(_window)->window = engine::create_window(machine,
		dynamic_cast<StringLiteralData*>(get_parameter_data(frame, "title"))->extracted_value,
		dynamic_cast<IntegerData*>(get_parameter_data(frame, "width"))->data_value,
		dynamic_cast<IntegerData*>(get_parameter_data(frame, "height"))->data_value);

	dynamic_cast<ClassData*>(result->data)->code_memory = _window;

	caller->stack->push(result);
}

void BUILTIN::CREATE_TEXTURE::run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) {
	Memory* result = create_empty_texture_memory(machine, dynamic_cast<StringLiteralData*>(get_parameter_data(frame, "path"))->extracted_value);

	caller->stack->push(result);
}

void BUILTIN::USE_SHADER::run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) {
	ShaderData* result = dynamic_cast<ShaderData*>(get_parameter_data(frame, "shader"));
	machine->current_shader = result;
	glLinkProgram(result->shader_program);
	glUseProgram(result->shader_program);
}


void BUILTIN::RUN_ENGINE::run_builtin(Machine* machine, FunctionFrame* frame, FunctionFrame* caller, Command* command) {
	BUILTIN::WINDOW* window = dynamic_cast<BUILTIN::WINDOW*>(dynamic_cast<ClassData*>(frame->local_memories["window"]->data)->code_memory);
	engine::game_loop(window->window, machine, command);
}
