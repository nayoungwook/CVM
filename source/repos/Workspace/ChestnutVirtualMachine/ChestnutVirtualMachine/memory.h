#pragma once

#include "code_memory.h"
#include <unordered_map>
#include "SDL.h"
#include "SDL_image.h"
#include "Math.h"

#include "machine.h"
#include "frame.h"
#include "basic_command.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>

#include "gl/glew.h"
#include "SDL_opengl.h"

#include "stb_image.h"

class Machine;

enum data_type {
	_unknown = -1,
	_number = 0, _bool = 1, _class = 2, _string = 3, _null = 4,
	_character = 5, _array = 6, _scene = 7, _integer = 8, _float = 9, _object = 10, _vector = 11,
	_texture = 13, _shader = 14,
};

static std::unordered_map<std::string, data_type> data_type_from_string = {
	{"unknown", _unknown},
	{"number", _number},
	{"bool",_bool},
	{"class",_class},
	{"string", _string},
	{"null", _null},
	{"char", _character},
	{"array", _array},
	{"scene", _scene},
	{"int", _integer},
	{"float", _float},
	{"object", _object},
	{"vector", _vector},
	{"texture", _texture},
	{"shader", _shader},
};

static std::unordered_map<data_type, std::string> string_from_data_type = {
	{_unknown, "unknown"},
	{_number, "number"},
	{_bool, "bool"},
	{_class, "class"},
	{_string, "string"},
	{_null, "null"},
	{_character, "char"},
	{_array, "array"},
	{_scene, "scene"},
	{_integer, "int"},
	{_float, "float"},
	{_object, "object"},
	{_vector, "vector"},
	{_texture, "texture"},
	{_shader, "shader"},
};

static std::unordered_map<data_type, data_type> root_types = {
	{_unknown, _unknown},
	{_number, _number},
	{_bool, _bool},
	{_class, _class},
	{_string, _string},
	{_null, _null},
	{_character, _character},
	{_array, _array},
	{_scene, _class},
	{_integer, _number},
	{_float, _number},
	{_object, _class},
	{_vector, _vector},
	{_texture, _texture},
	{_shader, _shader},
};

class Data {
private:
public:
	data_type type = _unknown;
	data_type root_type = root_types[type];
	virtual ~Data() {};
	Data() {	};
};

class Memory {
private:
public:
	virtual ~Memory() {
		if (this->data != nullptr)
			delete this->data;
	};

	data_type type = _unknown;
	std::string declaration_name = "unknown";

	Memory(data_type type, const std::string& declaration_name) : type(type), declaration_name(declaration_name) {

	};
	Data* data = nullptr;
};

class NumberData : public Data {
private:
public:
	long double data_value = 0;

	NumberData(double data_value) : data_value(data_value) {
		type = _number;
		root_type = root_types[type];
	};
};

class IntegerData : public Data {
private:
public:
	int data_value = 0;

	IntegerData(int data_value) : data_value(data_value) {
		type = _integer;
		root_type = root_types[type];
	};
};

class FloatData : public Data {
private:
public:
	float data_value = 0;

	FloatData(float data_value) : data_value(data_value) {
		type = _float;
		root_type = root_types[type];
	};
};

class StringLiteralData : public Data {
private:
public:
	std::string data_value = "";
	std::string extracted_value = "";

	StringLiteralData(std::string data_value) : data_value(data_value), extracted_value(data_value.substr(1, data_value.size() - 2)) {
		type = _string;
		root_type = root_types[type];
	}
};

class ClassData : public Data {
private:
public:
	std::string name;
	ClassCodeMemory* code_memory = nullptr;
	Memory* parent_memory = nullptr;

	std::map<std::string, Memory*> private_memories;
	std::map<std::string, Memory*> protected_memories;
	std::map<std::string, Memory*> public_memories;
	std::map<std::string, Memory*> default_memories;

	Memory* find_full_access_memory(const std::string& name);
	Memory* find_public_access_memory(const std::string& name);
	Memory* find_parent_access_memory(const std::string& name);

	FunctionBundle* find_full_access_function(const std::string& name);
	FunctionBundle* find_public_access_function(const std::string& name);
	FunctionBundle* find_parent_access_function(const std::string& name);

	ClassData(const std::string& name) : name(name) {
		type = _class;
		root_type = root_types[type];
	}
};

class SceneData : public ClassData {
private:
public:

	SceneData(const std::string& name) : ClassData(name) {
		type = _scene;
		root_type = root_types[type];
	}
};

class NullData : public Data {
private:
public:
	NullData() {
		type = _null;
		root_type = root_types[type];
	}
};

class BoolData : public Data {
private:
public:
	bool data_value = false;
	BoolData(bool data_value) : data_value(data_value) {
		type = _bool;
		root_type = root_types[type];
	}
};

class CharacterData : public Data {
private:
public:
	char data_value = false;
	CharacterData(char data_value) : data_value(data_value) {
		type = _character;
		root_type = root_types[type];
	}
};

class ArrayData : public Data {
private:
public:
	std::vector<Memory*> data_value;

	ArrayData(std::vector<Memory*>& data_value) : data_value(data_value) {
		type = _array;
		root_type = root_types[type];
	}
};

class VectorData : public Data {
private:
public:
	std::vector<Memory*> data_value;

	VectorData(std::vector<Memory*>& data_value) : data_value(data_value) {
		type = _vector;
		root_type = root_types[type];
	}
};

class ShaderData : public Data {
private:
	std::string path;
public:

	unsigned int vertex_shader = GL_NONE;
	unsigned int fragment_shader = GL_NONE;
	unsigned int shader_program = GL_NONE;

	std::map<std::string, unsigned int> uniform_data;

	bool compile_shader(unsigned int& _shader, const std::string& path, GLenum type);
	bool compile_succeeded(unsigned int shader);
	void set_matrix_uniform(const char* name, Matrix4 mat);
	void register_uniform_data(const char* name);

	ShaderData() {
		type = _shader;
		root_type = root_types[type];
	}
};

class TextureData : public Data {
private:
	std::string path = "";
public:
	void create_texture(Machine* machine);

	bool recompute_world_transform = true;
	Matrix4 world_transform;

	unsigned int texture;

	int width, height, channel;

	unsigned char* bytes;
	unsigned int vertex_count;
	unsigned int indices_count;
	unsigned int vbo;
	unsigned int ebo;
	unsigned int vao;

	~TextureData() {
		glDeleteTextures(1, &texture);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteVertexArrays(1, &vao);
	}
	TextureData(Machine* machine, const std::string& path) : path(path) {
		type = _texture;
		root_type = root_types[type];
		create_texture(machine);
	}
};

class ObjectData : public ClassData {
private:
public:
	Memory* primitive_texture = nullptr, * primitive_position = nullptr,
		* primitive_width = nullptr, * primitive_height = nullptr;
	void initialize_object_data(ObjectData* object);

	ObjectData(const std::string& name) : ClassData(name) {
		type = _object;
		root_type = root_types[type];

		initialize_object_data(this);
	}
};

class StackArea {
private:
public:
	std::map<std::string, Memory*> memories;
};


ShaderData* create_shader(const std::string& fragment_path, const std::string& vertex_path);
std::string types_to_string_format(Machine* machine, std::vector<Data*>& parameter_data);
std::string types_to_string_format(Machine* machine, std::vector<Memory*>& parameter_data);
std::string types_to_string_format(Machine* machine, FunctionCodeMemory* code_memory);
