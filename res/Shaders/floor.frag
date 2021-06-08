#version 330 core

in vec2 v_TexCoords;
layout(location = 0) out vec4 final;

uniform sampler2D u_Texture;

void main() {
	vec3 ambientColor = vec3(1.0, 1.0, 1.0);
	vec4 floorColor = texture(u_Texture, v_TexCoords);
	vec3 gamma = vec3(1.0 / 2.2);

	//rec601 NTSC luma
	//float luma = (0.299 * direct.r) + (0.587 * direct.g) + (0.114 * direct.b);

	//ITU-R BT.709 HDTV luma
	//float luma = (0.2126 * direct.r) + (0.7152 * direct.g) + (0.0722 * direct.b);

	//ITU-R BT.2100 HDR luma
//	float luma = (0.2627 * floorColor.r) + (0.6780 * floorColor.g) + (0.0593 * floorColor.b);
//	vec3 gray = vec3(luma);

	float ambientStrength = 0.00625;
	vec3 ambient = ambientStrength * ambientColor;
    vec3 color = ambient * floorColor.rgb;

    // Reinhard tone mapping.
    vec3 ldr = color / (color + vec3(1.0));
	final = vec4(pow(ldr, gamma), 1.0);
}
