#version 330 core

in vec2 v_TexCoords;
layout(location = 0) out vec4 final;

uniform vec2 u_MouseCoords;
uniform vec2 u_Resolution;
uniform vec2 u_Offset;
uniform vec3 u_LightColor;
uniform sampler2D u_Texture;

void main() {
	vec3 ambientColor = vec3(1.0, 1.0, 1.0);
	vec3 floorColor = texture(u_Texture, v_TexCoords).rgb;
	vec3 gamma = vec3(1.0 / 2.2);

	vec3 lightNormal = vec3(0.0, 0.0, 1.0);
	vec2 correctedMouseCoord = (u_MouseCoords - u_Offset) / u_Resolution.y;
	vec2 correctedFragCoord = (gl_FragCoord.xy - u_Offset) / u_Resolution.y;
	vec3 lightDir = normalize(vec3(correctedMouseCoord - correctedFragCoord, 10.0));

	float spaceCorrectionFactor = 100.0;
	float d = spaceCorrectionFactor * length(correctedMouseCoord - correctedFragCoord);
	float attenuation = 1.0/(1.0 + 0.045 * d + 0.0075 * d * d);

	float ambientStrength = 0.0125;
	vec3 ambient = ambientStrength * ambientColor;

	float diff = max(dot(lightNormal, lightDir), 0.0);
	vec3 diffuse = diff * u_LightColor;

	vec3 phongLight = (ambient + attenuation * diffuse) * floorColor;

	final = vec4(pow(phongLight, gamma), 1.0);
}
