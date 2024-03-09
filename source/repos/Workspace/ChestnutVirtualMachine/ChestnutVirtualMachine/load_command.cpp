#include "load_command.h"

void run_command_load_identifier(Command* command, Machine* machine, FunctionFrame* frame) {
	Memory* peek_memory = frame->stack->peek();

	std::string name = command->operands[0]->identifier;

	frame->stack->pop();

	if (peek_memory->data->type == _class || peek_memory->data->type == _scene || peek_memory->data->type == _object) {
		Memory* loaded_memory = object_load_identifier(peek_memory, command, machine, frame);

		if (loaded_memory == nullptr)
			loaded_memory = class_load_identifier(peek_memory, name, command, machine, frame);

		if (loaded_memory == nullptr)
			new VariableNotFoundError(name, command->line_number);

		frame->stack->push(loaded_memory);
	}
	else if (peek_memory->data->type == _vector) {
		vector_load_identifier(peek_memory, command, machine, frame);
	}
	else {
		new FailedToLoadFromIdentifierError(name, command->line_number);
	}
}

Memory* class_load_identifier(Memory* peek_memory, const std::string& name, Command* command, Machine* machine, FunctionFrame* frame) {
	if (frame->caller_class != nullptr && dynamic_cast<ClassData*>(peek_memory->data) == frame->caller_class->data) {
		return dynamic_cast<ClassData*>(peek_memory->data)->find_full_access_memory(name);
	}
	else {
		return dynamic_cast<ClassData*>(peek_memory->data)->find_public_access_memory(name);
	}
}

Memory* object_load_identifier(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame) {
	std::string name = command->operands[0]->identifier;

	ObjectData* object_data = dynamic_cast<ObjectData*>(peek_memory->data);

	std::set<std::string> object_system_data = {
		{"position"},
		{"texture"},
		{"width"},
		{"height"},
	};

	if (object_system_data.find(name) == object_system_data.end()) {
		return nullptr;
	}

	if (name == "position") {
		return object_data->primitive_position;
	}
	else if (name == "texture") {
		return object_data->primitive_texture;
	}
	else if (name == "width") {
		return object_data->primitive_width;
	}
	else if (name == "height") {
		return object_data->primitive_height;
	}

	return nullptr;
}

void object_load_call(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame, std::vector<Memory*>& parameters) {
	int parameter_count = parameters.size();
	std::string name = command->operands[0]->identifier;

	std::map<std::string, int> require_parameter_counts = {
		{"render", 0},
	};

	if (require_parameter_counts.find(name) == require_parameter_counts.end() ||
		parameter_count != require_parameter_counts[name]) {
		class_load_call(peek_memory, command, machine, frame, parameters);
	}

	ObjectData* object_data = dynamic_cast<ObjectData*>(peek_memory->data);
	TextureData* texture_data = dynamic_cast<TextureData*>(object_data->primitive_texture->data);

	if (name == "render") {
		std::vector<Memory*> _position_data = dynamic_cast<VectorData*>(object_data->primitive_position->data)->data_value;

		float _width_data = (float)get_number_data(object_data->primitive_width->data);
		float _height_data = (float)get_number_data(object_data->primitive_height->data);

		SDL_Rect position = {
			get_number_data(_position_data[0]->data),
			get_number_data(_position_data[1]->data),
			_width_data, _height_data,
		};

		if (texture_data == nullptr) {
			std::cout << "Error! texture of this object is null" << std::endl;
			exit(1);
		}

		//SDL_RenderCopy(machine->renderer, texture_data->texture, nullptr, &position);
	}
}

void class_load_call(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame, std::vector<Memory*>& parameters) {
	size_t parameter_count = parameters.size();
	std::string name = command->operands[0]->identifier;
	FunctionFrame* function_frame = new FunctionFrame(name);

	function_frame->caller_class = peek_memory;
	function_frame->caller_function = frame;
	FunctionCodeMemory* found_code_memory = nullptr;

	FunctionBundle* found_function_bundle = nullptr;

	if (frame->caller_class == peek_memory)
		found_function_bundle = dynamic_cast<ClassData*>(peek_memory->data)->find_full_access_function(name);
	else
		found_function_bundle = dynamic_cast<ClassData*>(peek_memory->data)->find_parent_access_function(name);

	bool exist_with_name = found_function_bundle != nullptr;

	if (!exist_with_name) {
		new FunctionNotFoundError(name, types_to_string_format(machine, frame->code_memory), command->line_number);
	}
	else {
		std::string type_string = types_to_string_format(machine, parameters);
		bool exist_with_type = (found_function_bundle->code_memories.find(type_string) != found_function_bundle->code_memories.end());

		if (exist_with_type) {
			found_code_memory = found_function_bundle->code_memories.find(type_string)->second;
		}
		else {
			new AccessModifierError(name, command->line_number);
		}
	}

	function_frame->code_memory = found_code_memory;

	for (int i = 0; i < parameter_count; i++) {
		function_frame->local_memories.insert(std::make_pair(function_frame->code_memory->parameters[i].first, parameters[i]));
	}

	if (function_frame != nullptr) {
		if (function_frame->code_memory->type == built_in_function_code_memory) {
			function_frame = exchange_bulitin_function_frame(function_frame);
			dynamic_cast<BuiltInFunctionFrame*>(function_frame)->built_in_code_memory = (BUILTIN::BUILTIN_FUNCTION*)found_code_memory;
			dynamic_cast<BuiltInFunctionFrame*>(function_frame)->run(machine, frame, command);
		}
		else {
			function_frame->run(machine, command);
		}
	}
}

void array_load_call(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame, std::vector<Memory*>& parameters) {
	int parameter_count = parameters.size();
	std::string name = command->operands[0]->identifier;

	std::map<std::string, int> require_parameter_counts = {
		{"length", 0},
		{"clear", 0},
		{"add", 1},
		{"remove", 0},
	};

	if (require_parameter_counts.find(name) == require_parameter_counts.end() ||
		parameter_count != require_parameter_counts[name]) {
		new FunctionNotFoundError(name, types_to_string_format(machine, frame->code_memory), command->line_number);
	}

	ArrayData* array_data = dynamic_cast<ArrayData*>(peek_memory->data);

	if (name == "length") {
		Memory* result = create_empty_integer_memory();

		dynamic_cast<IntegerData*>(result->data)->data_value = array_data->data_value.size();

		frame->stack->push(result);
	}
	else if (name == "add") {
		Memory* add_memory = new Memory(parameters[0]->type, parameters[0]->declaration_name);
		add_memory->data = parameters[0]->data;
		array_data->data_value.push_back(add_memory);
	}
	else if (name == "clear") {
		array_data->data_value.clear();
	}
}

Data* expect_and_get_parameter_data(Machine* machine, Command* command, FunctionFrame* frame, std::vector<Memory*>& parameters, int index, data_type expected_type) {
	if (parameters[index] == nullptr || parameters[index]->data == nullptr) {
		new VariableNotFoundError(frame->code_memory->parameters[index].first, command->line_number);
	}

	if (parameters[index]->data->root_type != expected_type) {
		std::cout << "Error! unexpected parameter " << string_from_data_type[parameters[index]->data->type] << " expected : " << string_from_data_type[expected_type] << std::endl;
		exit(1);
	}

	if (parameters[index]->data->root_type == _class) {
		std::string _rhs, _lhs;

		if (parameters[index]->data->type == _class)
			_rhs = get_parent_data(machine, machine->get_class_memory(parameters[index]->declaration_name))->name,
			_lhs = get_parent_data(machine, machine->get_class_memory(frame->code_memory->parameters[index].first))->name;

		if (parameters[index]->data->type == _object)
			_rhs = get_parent_data(machine, machine->get_object_memory(parameters[index]->declaration_name))->name,
			_lhs = get_parent_data(machine, machine->get_object_memory(frame->code_memory->parameters[index].first))->name;

		if (_lhs != _rhs) {
			std::cout << "Error! class type of lhs does not match with rhs | " << _lhs << " : " << _rhs << std::endl;
			exit(1);
		}
	}

	return parameters[index]->data;
}

void texture_load_call(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame, std::vector<Memory*>& parameters) {
	int parameter_count = parameters.size();
	std::string name = command->operands[0]->identifier;

	std::map<std::string, int> require_parameter_counts = {
		{"render", 3},
		{"render_rotation", 4},
	};

	if (require_parameter_counts.find(name) == require_parameter_counts.end() ||
		parameter_count != require_parameter_counts[name]) {
		new FunctionNotFoundError(name, types_to_string_format(machine, frame->code_memory), command->line_number);
	}

	TextureData* texture_data = dynamic_cast<TextureData*>(peek_memory->data);

	if (name == "render" || name == "render_rotation") {
		std::vector<Memory*> _position_data = dynamic_cast<VectorData*>(expect_and_get_parameter_data(machine, command, frame, parameters, 0, _vector))->data_value;

		float _width_data = (float)get_number_data(expect_and_get_parameter_data(machine, command, frame, parameters, 1, _number));
		float _height_data = (float)get_number_data(expect_and_get_parameter_data(machine, command, frame, parameters, 2, _number));


		float x = dynamic_cast<FloatData*>(_position_data[0]->data)->data_value;
		float y = dynamic_cast<FloatData*>(_position_data[1]->data)->data_value;

		SDL_Point rotation_center = {
			_width_data / 2, _height_data / 2
		};

		float rotation = .0f;
		if (name == "render_rotation") {
			rotation = (float)get_number_data(expect_and_get_parameter_data(machine, command, frame, parameters, 3, _number));
		}

		if (texture_data == nullptr) {
			std::cout << "Error! texture of this object is null" << std::endl;
			exit(1);
		}

		Matrix4 world_transform = Matrix4::CreateScale(_width_data, _height_data, 1.0f) * Matrix4::CreateTranslation(Vector3(x, y, 0)) * Matrix4::CreateRotationZ(rotation);

		if (machine->current_shader == nullptr) {
			std::cout << "Error! current shader is null. please declare it." << std::endl;
			exit(1);
		}

		glBindTexture(GL_TEXTURE_2D, texture_data->texture);

		machine->current_shader->set_matrix_uniform("uWorldTransform", world_transform);
		machine->current_shader->set_matrix_uniform("uViewProj", Matrix4::CreateSimpleViewProj(800, 600));

		glBindVertexArray(texture_data->vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}
}

void vector_load_identifier(Memory* peek_memory, Command* command, Machine* machine, FunctionFrame* frame) {
	std::string name = command->operands[0]->identifier;

	VectorData* vector_data = dynamic_cast<VectorData*>(peek_memory->data);

	std::map<std::string, int> vector_indexes = {
		{"x", 0},
		{"y", 1},
		{"z", 2},
	};

	if (vector_indexes.find(name) == vector_indexes.end()) {
		std::cout << "Error! " << name << " is not element of vector." << std::endl;
		exit(1);
	}

	if (name == "x" || name == "y" || name == "z") {

		if (vector_indexes[name] >= vector_data->data_value.size()) {
			std::cout << "Error! this vector does not contain element named : " << name << std::endl;
		}

		Memory* result = vector_data->data_value[vector_indexes[name]];
		frame->stack->push(result);
	}
}

void run_command_load_call(Command* command, Machine* machine, FunctionFrame* frame) {
	int parameter_count = std::stoi(command->operands[1]->identifier);
	std::vector<Memory*> parameters;

	for (int i = 0; i < parameter_count; i++) {
		Memory* memory = frame->stack->peek();

		parameters.push_back(memory);

		frame->stack->pop();
	}

	Memory* peek_memory = frame->stack->peek();

	frame->stack->pop();

	if (peek_memory->data->type == _class || peek_memory->data->type == _scene) {
		class_load_call(peek_memory, command, machine, frame, parameters);
	}
	else if (peek_memory->data->type == _object) {
		object_load_call(peek_memory, command, machine, frame, parameters);
	}
	else if (peek_memory->data->type == _array) {
		array_load_call(peek_memory, command, machine, frame, parameters);
	}
	else if (peek_memory->data->type == _texture) {
		texture_load_call(peek_memory, command, machine, frame, parameters);
	}
	else {
		std::string _type = "unknown";
		if (string_from_data_type.find(peek_memory->data->type) != string_from_data_type.end())
			_type = string_from_data_type[peek_memory->data->type];

		new FailedToLoadFromIdentifierError(_type, command->line_number);
	}

}