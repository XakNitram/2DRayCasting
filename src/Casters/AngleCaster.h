#pragma once
#include "rcpch.h"
#include "Casters/Caster.h"
#include "Math/Geometrics.h"
#include "Math/Boundary.h"
#include "Renderer/VertexArray.h"

// TODO: Find some way to populate an array of angle slices.
constexpr unsigned int numRays = 64;


class LineAngleCaster : public Caster {
	Point pos;
	VertexArray vao;
	std::vector<std::unique_ptr<Point>> intersections;

public:
	LineAngleCaster();

	void update(const float x, const float y);

	void look(const std::vector<Boundary>& bounds);

	void draw() const;
};


class FilledAngleCaster : public Caster {
	Point pos;
	VertexArray vao;
	std::vector<std::unique_ptr<Point>> intersections;

public:
	FilledAngleCaster();

	void update(const float x, const float y);

	void look(const std::vector<Boundary>& bounds);

	void draw() const override;
};
