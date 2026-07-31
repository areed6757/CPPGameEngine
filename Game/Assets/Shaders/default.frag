#version 440 core

out vec4 FragColor;
in vec3 color;
in vec2 texCoord;
in float healthFraction;

uniform bool useTexture;
uniform sampler2D tex0;

void main()
{
	vec4 baseColor  = useTexture ? texture(tex0, texCoord) : vec4(color, 1.0);
	vec3 brokenTint = vec3(0.3, 0.05, 0.05);
	FragColor = vec4(mix(brokenTint, baseColor.rgb, healthFraction), baseColor.a);
}