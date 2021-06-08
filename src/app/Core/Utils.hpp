#pragma once

#include "pch.hpp"

#define ASSERT(x) if (!(x)) __debugbreak();

std::string readFile(const std::string &filepath);
