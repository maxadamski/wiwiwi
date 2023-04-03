#pragma once

#include <cstdio>
#include <cstdint>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <string>
#include <vector>
#include <map>

#include <memory>
#include <algorithm>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <IL/il.h>

#define DEBUG
#define PI 3.14159f
#define TAU (2*PI)

#define sizeof_val(X) (sizeof(X[0]))
#define sizeof_vec(X) (sizeof(X[0]) * X.size())

// Removing commas with '##' only works in GCC!
#define eprintf(f_, ...) fprintf(stderr, (f_), ##__VA_ARGS__)

#ifdef DEBUG
#define dprintf(f_, ...) fprintf(stderr, (f_), ##__VA_ARGS__)
#else
#define dprintf(f_, ...)
#endif

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;
using std::shared_ptr;
using std::weak_ptr;
using glm::mat4;
using glm::vec4;
using glm::vec3;
using glm::vec2;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef GLFWwindow Window;

inline void fatalError(const char *message) {
	fprintf(stderr, "fatal error: %s\n", message);
	exit(1);
}

inline f64 clip(f64 value, f64 min, f64 max) {
	value = value < min ? min : value;
	value = value > max ? max : value;
	return value;
}

inline f64 rad(f64 deg) {
	return deg * PI / 180.0;
}

bool readFile(const string &path, string &data);
bool fileExists(const string &path);
