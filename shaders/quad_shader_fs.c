#version 330 core

out vec4 FragColor;
uniform vec3 Color;
in vec2 InUv;

void 
main()
{
	FragColor = vec4(1.0f, 1.0f, .80f, 1.0f);
}