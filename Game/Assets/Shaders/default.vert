#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in mat4 instanceModel; // GLSL views mat4 location as consuming 4 locations total (this is loc 3, 4, 5, 6)
layout (location = 7) in float instanceHealthFraction;

out vec3 color;

out vec2 texCoord;

out float healthFraction;

uniform mat4 projection;

void main()
{
	gl_Position = projection * instanceModel * vec4(aPos, 1.0);
	color = aColor;
	texCoord = aTex;
	healthFraction = instanceHealthFraction;
}