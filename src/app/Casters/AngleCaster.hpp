#pragma once
#include "pch.hpp"
#include "Caster.hpp"
#include "Math/Geometrics.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"

// TODO: Find some way to populate an array of angle slices.
constexpr unsigned int numRays = 64;


class LineAngleCaster : public Caster {
	Point pos;
	lwvl::VertexArray vao;
	lwvl::ArrayBuffer vbo;
	lwvl::ElementBuffer ebo;
	std::vector<Point> intersections;

public:
	LineAngleCaster();

	void update(float x, float y) final;

	void look(const std::vector<LineSegment>& bounds) final;

	void draw() final;
};


class FilledAngleCaster : public Caster {
	Point pos;
	lwvl::VertexArray vao;
	lwvl::ArrayBuffer vbo;
	std::vector<Point> intersections;

public:
	FilledAngleCaster();

	void update(float x, float y) final;

	void look(const std::vector<LineSegment>& bounds) final;

	void draw() final;
};
