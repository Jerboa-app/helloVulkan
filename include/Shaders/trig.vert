#version 450

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec3 a_colour;

layout(location = 0) out vec3 fragColour;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(a_position, 0.0, 1.0);
    fragColour = a_colour;
}