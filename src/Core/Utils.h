#pragma once
#include "rcpch.h"

#define ASSERT(x) if (!(x)) __debugbreak();

std::string readFile(const std::string & filepath);
