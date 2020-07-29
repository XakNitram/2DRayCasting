#pragma once
#include <string>
#include <fstream>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();

std::string readFile(const std::string & filepath);
