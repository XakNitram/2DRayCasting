#pragma once
#include <vector>
#include "Boundary.h"

class __declspec(novtable) Caster {
public:
	virtual void update(const float x, const float y) = 0;
	virtual void look(const std::vector<Boundary>& bounds) = 0;
	virtual void draw() const = 0;
};
