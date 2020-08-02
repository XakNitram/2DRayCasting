#version 330 core

layout(location = 0) out vec4 color;
uniform vec3 u_Color = vec3(0.2, 0.4, 0.8);

void main() {
    color = vec4(u_Color, 1.0);
}