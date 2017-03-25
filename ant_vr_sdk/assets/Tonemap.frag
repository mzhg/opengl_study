
in vec4 ScreenSpaceUV;
uniform sampler2D   sceneTex;
uniform sampler2D   blurTex;
uniform sampler2D   lumTex;
/*
uniform float blurAmount;
uniform float exposure;
uniform float gamma;
*/

uniform vec4 g_ToneParams;

out vec4 f4FragColor;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 filmicTonemapping(vec3 x)
{
	return ((x*(A*x + C*B) + D*E) / (x*(A*x + B) + D*F)) - E / F;
}
float vignette(vec2 pos, float inner, float outer)
{
	float r = length(pos);
	r = 1.0 - smoothstep(inner, outer, r);
	return r;
}
void main()
{
	float blurAmount = g_ToneParams.x;
	float exposure = g_ToneParams.y;
	float gamma = g_ToneParams.z;

	vec4 scene = texture(sceneTex, a_texCoord);
	vec4 blurred = texture(blurTex, a_texCoord);
	float lum = texture(lumTex, vec2(0.0, 0.0)).r;
	vec3 c = mix(scene.rgb, blurred.rgb, blurAmount);
	c = c * exposure / lum;
	c = c * vignette(a_texCoord*2.0 - 1.0, 0.55, 1.5);
	float ExposureBias = 1.0;
	c = filmicTonemapping(ExposureBias*c);
	vec3 whiteScale = 1.0 / filmicTonemapping(vec3(W, W, W));
	c = c*whiteScale;
	c.r = pow(c.r, gamma);
	c.g = pow(c.g, gamma);
	c.b = pow(c.b, gamma);
	f4FragColor = vec4(c, 1.0);
}