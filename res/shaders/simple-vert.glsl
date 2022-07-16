#version 430

layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 texCoord;
layout(binding = 0) uniform sampler2D samp;

out vec2 tc;

//scale
uniform float uScale = 1.0;
//Position
uniform vec2 uPos = vec2(0.0, 0.0);

out vec2 fragPosition;

void main()
{
	vec2 p = (pos.xy * uScale + uPos);	
	gl_Position = vec4(p.xy, 0.0, 1.0); 

	fragPosition = pos.xy;

	tc = texCoord;
}
