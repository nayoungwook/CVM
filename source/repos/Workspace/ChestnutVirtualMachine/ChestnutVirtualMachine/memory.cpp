#include "memory.h"
#include "code_memory.h"
#include "machine.h"
#include "frame.h"
#include "basic_command.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>

Memory* ClassData::find_parent_access_memory(const std::string& name) {
	if (dynamic_cast<ClassData*>(this->parent_memory->data)->public_memories.find(name) != dynamic_cast<ClassData*>(this->parent_memory->data)->public_memories.end()) {
		return dynamic_cast<ClassData*>(this->parent_memory->data)->public_memories.find(name)->second;
	}
	else if (dynamic_cast<ClassData*>(this->parent_memory->data)->protected_memories.find(name) != dynamic_cast<ClassData*>(this->parent_memory->data)->protected_memories.end()) {
		return dynamic_cast<ClassData*>(this->parent_memory->data)->protected_memories.find(name)->second;
	}

	if (dynamic_cast<ClassData*>(this->parent_memory->data)->parent_memory != nullptr) {
		return dynamic_cast<ClassData*>(this->parent_memory->data)->find_parent_access_memory(name);
	}

	return nullptr;
}

Memory* ClassData::find_full_access_memory(const std::string& name) {
	if (this->public_memories.find(name) != this->public_memories.end()) {
		return this->public_memories.find(name)->second;
	}
	else if (this->private_memories.find(name) != this->private_memories.end()) {
		return this->private_memories.find(name)->second;
	}
	else if (this->protected_memories.find(name) != this->protected_memories.end()) {
		return this->protected_memories.find(name)->second;
	}
	else if (this->default_memories.find(name) != this->default_memories.end()) {
		return this->default_memories.find(name)->second;
	}

	if (this->parent_memory != nullptr) {
		return find_parent_access_memory(name);
	}

	return nullptr;
}

Memory* ClassData::find_public_access_memory(const std::string& name) {
	if (this->public_memories.find(name) != this->public_memories.end()) {
		return this->public_memories.find(name)->second;
	}
	else if (this->default_memories.find(name) != this->default_memories.end()) {
		return this->default_memories.find(name)->second;
	}

	if (this->parent_memory != nullptr) {
		return dynamic_cast<ClassData*>(this->parent_memory->data)->find_public_access_memory(name);
	}

	return nullptr;
}

FunctionBundle* ClassData::find_parent_access_function(const std::string& name) {
	if (this->code_memory->member_functions.find(name) != this->code_memory->member_functions.end()) {
		std::string access_modifier = this->code_memory->member_functions[name]->access_modifier;
		if (access_modifier == "public" || access_modifier == "protected") {
			return this->code_memory->member_functions[name];
		}
	}

	if (this->parent_memory != nullptr) {
		return dynamic_cast<ClassData*>(this->parent_memory->data)->find_parent_access_function(name);
	}

	return nullptr;
}

FunctionBundle* ClassData::find_full_access_function(const std::string& name) {
	if (this->code_memory->member_functions.find(name) != this->code_memory->member_functions.end()) {
		return this->code_memory->member_functions[name];
	}

	if (this->parent_memory != nullptr) {
		return find_parent_access_function(name);
	}

	return nullptr;
}

FunctionBundle* ClassData::find_public_access_function(const std::string& name) {
	if (this->code_memory->member_functions.find(name) != this->code_memory->member_functions.end()) {
		std::string access_modifier = this->code_memory->member_functions[name]->access_modifier;
		if (access_modifier == "public") {
			return this->code_memory->member_functions[name];
		}
	}
	if (this->parent_memory != nullptr) {
		return dynamic_cast<ClassData*>(this->parent_memory->data)->find_public_access_function(name);
	}

	return nullptr;
}

std::string types_to_string_format(Machine* machine, std::vector<Data*>& parameter_data) {
	std::string result = "";
	for (Data* data : parameter_data) {
		if (data->type == _class || data->type == _object) { // function can be called with parent class.
			ClassData* class_data = dynamic_cast<ClassData*>(data);
			result += get_parent_data(machine, class_data->code_memory)->name;
		}
		else {
			result += string_from_data_type[data->root_type];
		}
	}

	return result;
}

std::string types_to_string_format(Machine* machine, std::vector<Memory*>& parameter_data) {
	std::vector<Data*> refined_data;

	for (Memory* memory : parameter_data) {
		refined_data.push_back(memory->data);
	}

	return types_to_string_format(machine, refined_data);
}

std::string types_to_string_format(Machine* machine, FunctionCodeMemory* code_memory) {
	std::string result = "";

	for (std::pair<std::string, std::string> data : code_memory->parameters) {
		result += string_from_data_type[root_types[data_type_from_string[data.second]]];
	}

	return result;
}

bool ShaderData::compile_shader(unsigned int& _shader, const std::string& path, GLenum type) {
	std::ifstream shader_file(path);

	if (shader_file.is_open()) {
		std::stringstream str_stream;

		str_stream << shader_file.rdbuf();

		std::string content = str_stream.str();
		const char* content_char = content.c_str();

		unsigned int result = 0;
		result = glCreateShader(type);

		glShaderSource(result, 1, &(content_char), nullptr);
		glCompileShader(result);

		if (!compile_succeeded(result)) {
			SDL_Log("Failed to compile shader : %s", path.c_str());
			return false;
		}

		_shader = result;
	}
	else {
		SDL_Log("Failed to find shader : %s", path.c_str());
		return false;
	}

	return true;
}

void ShaderData::register_uniform_data(const char* name) {
	unsigned int loc = glGetUniformLocation(this->shader_program, name);
	this->uniform_data.insert(std::make_pair(name, loc));
}

void ShaderData::set_matrix_uniform(const char* name, Matrix4 matrix) {
	if (this->uniform_data.find(name) == this->uniform_data.end()) {
		std::cout << "Error! uniform " << name << " not found." << std::endl;
		exit(1);
	}

	glUniformMatrix4fv(
		this->uniform_data[name],
		1,
		GL_TRUE,
		matrix.GetAsFloatPtr()
	);
}

bool ShaderData::compile_succeeded(unsigned int shader) {
	int status;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		fprintf(stderr, "Compilation error in shader : %s\n", strInfoLog);
		delete[] strInfoLog;

		return false;
	}

	return true;
}

ShaderData* create_shader(const std::string& fragment_path, const std::string& vertex_path) {
	ShaderData* shader_data = new ShaderData();

	if (!(
		shader_data->compile_shader(shader_data->fragment_shader, fragment_path, GL_FRAGMENT_SHADER) &&
		shader_data->compile_shader(shader_data->vertex_shader, vertex_path, GL_VERTEX_SHADER))) {
		return nullptr;
	}

	shader_data->shader_program = glCreateProgram();

	glAttachShader(shader_data->shader_program, shader_data->vertex_shader);
	glAttachShader(shader_data->shader_program, shader_data->fragment_shader);

	glLinkProgram(shader_data->shader_program);
	glUseProgram(shader_data->shader_program);

	return shader_data;
}

void TextureData::create_texture(Machine* machine) {
	std::cout << "load texture" << std::endl;
	unsigned char* bytes = stbi_load(this->path.c_str(), &this->width, &this->height, &this->channel, 4);

	if (!bytes) {
		std::cout << "Error! stb_image failed to load image : " << this->path << std::endl;
		exit(1);
	}
	this->bytes = bytes;

	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	float vertices[] = {
		-0.5f, 0.5f, 0.0f,   0.f, 0.f,
		0.5f, 0.5f, 0.0f,    1.f, 0.f,
		0.5f, -0.5f, 0.0f,   1.f, 1.f,
		-0.5f, -0.5f, 0.0f,  0.f, 1.f,
	};

	int position_vertex_count = 3, tex_coord_count = 2;
	vertex_count = position_vertex_count + tex_coord_count;

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	indices_count = sizeof(indices) / sizeof(unsigned int);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ARRAY_BUFFER, vertex_count * 4 * sizeof(float), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, position_vertex_count, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_count, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, tex_coord_count, GL_FLOAT, GL_FALSE, sizeof(float) * vertex_count, (void*)(sizeof(float) * position_vertex_count));
}

void ObjectData::initialize_object_data(ObjectData* object) {
	std::vector<Memory*> position_memories;

	for (int i = 0; i < 3; i++) {
		Memory* _single_memory = new Memory(_float, "float");
		_single_memory->data = new FloatData(.0f);

		position_memories.push_back(_single_memory);
	}

	object->primitive_position = new Memory(_vector, "vector");
	object->primitive_position->data = new VectorData(position_memories);

	object->primitive_texture = new Memory(_texture, "texture");
	object->primitive_texture->data = new NullData();

	object->primitive_width = new Memory(_float, "float");
	object->primitive_width->data = new FloatData(100);

	object->primitive_height = new Memory(_float, "float");
	object->primitive_height->data = new FloatData(100);
}