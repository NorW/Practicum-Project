#version 330
precision highp float;

uniform sampler2D tex;
uniform mat4x4 projection;
uniform mat4x4 model;
uniform bool isEnabled;
uniform bool isText;
uniform bool isSelected;
uniform vec4 colour;

in vec3 in_position;
in vec2 in_texcoord;

out vec2 texcoord;

void main(void)
{
	if (!isText)
	{
		texcoord = in_texcoord;
	}

	gl_Position = projection * model * vec4(in_position, 1.0f);
}
