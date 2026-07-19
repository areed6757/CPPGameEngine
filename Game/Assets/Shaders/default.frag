#version 440 core

out vec4 FragColor;
in vec3 color;
in vec2 texCoord;

uniform bool useTexture;
uniform sampler2D tex0;

void main()
{
	FragColor = useTexture ? texture(tex0, texCoord) : vec4(color, 1.0);
}