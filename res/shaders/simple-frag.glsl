#version 430

uniform vec4 uColor = vec4(1.0, 1.0, 1.0, 1.0);
out vec4 color;
in vec2 fragPosition;

//texture coordinate
in vec2 tc;
layout(binding = 0) uniform sampler2D samp;

void main()
{
	color = uColor;		

	color = texture(samp, tc);

	float originalAlpha = color.a;
	if(fragPosition.x < -0.95 || fragPosition.x > 0.95 ||
		fragPosition.y < -0.95 || fragPosition.y > 0.95)
		color *= 0.5;
	color.a = originalAlpha;
}
