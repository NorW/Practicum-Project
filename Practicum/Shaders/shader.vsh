#version 330
precision highp float;

uniform sampler2D tex;
uniform mat4x4 projection;
uniform mat4x4 view;
uniform vec2 objPosition;
uniform bool isVisible;
uniform bool isOwned;
uniform vec4 sideColour;

in vec2 in_position;
in vec2 in_texcoord;

out vec2 texcoord;

void main(void)
{
	texcoord = in_texcoord;
	gl_Position = projection * view * vec4(in_position, 0, 1);
}
