#version 330 core

/* 
Translated from "Shader Coding: Truchet Tiling Explained"
 -> https://www.youtube.com/watch?v=2R7h76GoIJM
*/

layout(location = 0) out vec4 final;  // layout 0 so we can render to GL_COLOR_ATTACHMENT0
uniform vec2 u_Resolution;
uniform vec3 u_Color = vec3(1.0, 0.0, 0.0);
//uniform float u_Time;

float Hash21(vec2 p) {
    p = fract(p * vec2(234.34, 435.345));
    p += dot(p, p+34.23);
    return fract(p.x * p.y);
}

void main() {
    vec2 uv = (gl_FragCoord.xy - (0.5 * u_Resolution.xy)) / u_Resolution.y;
    vec3 col = vec3(0);  // black base
    //vec3 col = vec3(0.5, 0.0, 0.0);  // maroon base
    //vec3 col = vec3(0.5, 1.0, 0.80);  // aquamarine base
    //vec3 col = vec3(0.25, 1.00, 0.80);  // turquoise base
    //vec3 col = vec3(1.0, 1.0, 1.0);

    //uv += u_Time * 0.2;
    uv *= 5.0;
    vec2 gv = fract(uv) - 0.5;
    vec2 id = floor(uv);

    float n = Hash21(id);

    float thickness = 0.25;

    if (n < 0.5) { gv.x *= -1.0; }
    float dto = abs(abs(gv.x + gv.y) - 0.5);
    float mask = smoothstep(0.01, -0.01, dto - thickness);

    col += mask;
    //col += n;
    col *= u_Color;

    //if (gv.x > 0.48 || gv.y > 0.48) { col = vec3(1, 0, 0); }
    final = vec4(col, 1.0);
}
