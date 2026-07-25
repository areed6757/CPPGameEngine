#version 440 core
in float alpha;
out vec4 FragColor;
uniform vec3 particleColor;
void main() {
	FragColor = vec4(particleColor, alpha);
}