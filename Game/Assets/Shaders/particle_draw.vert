#version 440 core
struct Particle { vec2 position; vec2 velocity; float age; float maxLifetime; };
layout(std430, binding = 0) buffer ParticleBuffer { Particle particles[]; };

uniform mat4 projection;
uniform vec2 cameraOffset;
uniform float particleSize;

out float alpha;

const vec2 corners[6] = vec2[] (
	vec2(-0.5, -0.5), vec2(0.5, -0.5), vec2(0.5, 0.5),
	vec2(-0.5, -0.5), vec2(0.5, 0.5), vec2(-0.5, 0.5)
);

void main() {
	Particle p = particles[gl_InstanceID];
	vec2 corner = corners[gl_vertexID];
	vec2 relPos = p.position - cameraOffset;
	vec2 worldPos = relPos + corner * particleSize;
	gl_Position = projection * vec4(worldPos, 0.0, 1.0);
	alpha = (p.age < p.maxLifetime) ? (1.0 - p.age / p.maxLifetime) : 0.0;
}