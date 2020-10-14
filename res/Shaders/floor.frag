#version 330 core

in vec2 v_TexCoords;
layout(location = 0) out vec4 final;

uniform sampler2D u_Texture;

void main() {
	vec4 direct = texture(u_Texture, v_TexCoords);

	//rec601 NTSC luma
	//float luma = (0.299 * direct.r) + (0.587 * direct.g) + (0.114 * direct.b);

	//ITU-R BT.709 HDTV luma
	//float luma = (0.2126 * direct.r) + (0.7152 * direct.g) + (0.0722 * direct.b);

	//ITU-R BT.2100 HDR luma
	float luma = (0.2627 * direct.r) + (0.6780 * direct.g) + (0.0593 * direct.b);

	float finalBrightness = 0.25 * luma;

	final = vec4(finalBrightness, finalBrightness, finalBrightness, 1.0);
}
