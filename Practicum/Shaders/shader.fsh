#version 330
precision highp float;

uniform sampler2D tex;
uniform mat4x4 projection;
uniform mat4x4 view;
uniform vec2 objPosition;
uniform bool isVisible;
uniform bool isOwned;
uniform vec4 sideColour;


in vec2 texcoord;

out vec4 gl_FragColor;

void main(void)
{
	 vec4 texColor = texture2D(tex, texcoord);
	 gl_FragColor = texColor;

	 if(isOwned)
	 {
		gl_FragColor *= sideColour;
	 }

	 if(!isVisible)
	 {
		gl_FragColor *= vec4(0.3f, 0.3f, 0.3f, 1.0f);
	 }
}