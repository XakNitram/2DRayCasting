#pragma once
#include "pch.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Buffer.hpp"
#include "Math/Geometrics.hpp"

struct Boundary {
	LineSegment line;
	lwvl::VertexArray vao;
	lwvl::ArrayBuffer vbo;

	// TODO: If needed, add a default constructor and an 'update' method
	Boundary(float x1, float y1, float x2, float y2);
	Boundary(Boundary&& other) noexcept;

	void draw();
};
