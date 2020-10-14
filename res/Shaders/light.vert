#version 330 core

layout(location = 0) in vec4 position;
out vec2 v_TexCoords;

uniform vec2 u_Offset;
uniform vec2 u_Resolution;
uniform mat4 u_Projection;

void main() {
	// Given that all points on this shape fall within the quad defining the floor,
	// the texture coordinates can be mapped directly to the normalized vertex positions.
	v_TexCoords = (position.xy - u_Offset) / u_Resolution;
	gl_Position = u_Projection * position;
}