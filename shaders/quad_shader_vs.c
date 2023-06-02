#version 330 core
layout (location = 0) in vec3 vert;
layout (location = 1) in vec2 uv;

out vec2 InUv;

uniform vec2 ScreenDimensions;
uniform vec2 Position;
uniform vec2 Scale;
uniform float Angle;

vec2
NDCPlayerCoords(vec2 Position)
{
	float Nx = (Position.x / ScreenDimensions.x) * 2.0f - 1.0f;
	float Ny = (Position.y / ScreenDimensions.y) * 2.0f - 1.0f;
	return vec2(Nx, Ny);
}
void main()
{
	float SinAngle = sin(Angle);
	float CosAngle = cos(Angle);
	mat2 M = mat2(CosAngle, SinAngle, -SinAngle, CosAngle);
	vec3 VertScaled = vert *  vec3(Scale, 1.0f);
	VertScaled.x -= Scale.x * .5f;
	vec3 VertRotated = vec3(M * VertScaled.xy, VertScaled.z);
	VertScaled.x += Scale.x * .5f;
	vec3 VertTranslated  = VertRotated + vec3(Position.xy,0.0f);
	vec3 VertMappedToNDC = vec3(NDCPlayerCoords(VertTranslated.xy), vert.z);
	gl_Position = vec4(VertMappedToNDC, 1.0f);
    InUv = uv;
}