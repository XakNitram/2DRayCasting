#pragma once
#include <memory>
#include <vector>
#include <array>
#include "Caster.h"
#include "Geometrics.h"
#include "VertexArray.h"

// TODO: Find some way to populate an array of angle slices.
constexpr unsigned int numRays = 64;


class AngleCaster : public Caster {
	Point pos;
	VertexArray vao;
	std::vector<std::unique_ptr<Point>> intersections;

public:
	AngleCaster(float x, float y);

	void update(const float x, const float y);

	void look(const std::vector<Boundary>& bounds);

	void draw() const;
};


class FilledAngleCaster : public Caster {
	Point pos;
	VertexArray vao;
	std::vector<std::unique_ptr<Point>> intersections;

public:
	FilledAngleCaster(float x, float y);

	void update(const float x, const float y);

	void look(const std::vector<Boundary>& bounds);

	void draw() const override;
};
