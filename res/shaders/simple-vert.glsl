#version 430

layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 texCoord;
layout(binding = 0) uniform sampler2D samp;

out vec2 tc;

//scale
uniform float uScale = 1.0;
//Position
uniform vec2 uPos = vec2(0.0, 0.0);

uniform float uRot = 0.0f;

out vec2 fragPosition;

void main()
{	
	vec2 pos2 = mat2x2(cos(uRot * 3.14 / 180.0), -sin(uRot * 3.14 / 180.0),
					   sin(uRot * 3.14 / 180.0), cos(uRot * 3.14 / 180.0)) * pos.xy;	
	vec2 p = (pos2.xy * uScale + uPos);	

	gl_Position = vec4(p.xy, 0.0, 1.0); 

	fragPosition = pos.xy;

	tc = texCoord;
}
