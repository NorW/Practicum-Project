#version 330
precision highp float;

uniform sampler2D tex;
uniform mat4x4 projection;
uniform mat4x4 model;
uniform bool isEnabled;
uniform bool isText;
uniform bool isSelected;
uniform vec4 colour;

in vec2 texcoord;

out vec4 gl_FragColor;

void main(void)
{
	if(!isText)
	{
		vec4 texColor = texture2D(tex, texcoord);
		gl_FragColor = texColor;
	}
	else
	{
		gl_FragColor = colour;
	}

	if(!isEnabled)
	{
		gl_FragColor = gl_FragColor * vec4(0.3f, 0.3f, 0.3f, 1.0f);	//Grey out if not enabled.
	}

	gl_FragColor = gl_FragColor * colour;
	
}
