#version 330 core

in vec2 v_TexCoords;
layout(location = 0) out vec4 final;

uniform vec2 u_MouseCoords;
uniform vec2 u_Resolution;
uniform vec2 u_Offset;
uniform sampler2D u_Texture;

void main() {
	vec2 correctedMouseCoord = (u_MouseCoords - u_Offset) / u_Resolution.y;
	vec2 correctedFragCoord = (gl_FragCoord.xy - u_Offset) / u_Resolution.y;
	
	float d = distance(correctedMouseCoord, correctedFragCoord);
	float cd = clamp(pow(d, 0.5), 0.0, 1.0);
	
	vec4 direct = texture(u_Texture, v_TexCoords);
	float luma = 0.25 * ((0.2627 * direct.r) + (0.6780 * direct.g) + (0.0593 * direct.b));
	vec3 gray = vec3(luma, luma, luma);

	final = vec4(mix(direct.rgb, gray, cd), 1.0);
}
