

#if GL_ES
precision highp float;
#if __VERSION__  >= 300
#define ENABLE_VERTEX_ID 1
#define ENABLE_IN_OUT_FEATURE 1
#endif

#else

// The Desktop Platform, Almost all of the video drivers support the gl_VertexID, so just to enable it simply.
#define ENABLE_VERTEX_ID 1

#if __VERSION__ >= 130
#define ENABLE_IN_OUT_FEATURE 1
#endif

#endif

#if  ENABLE_IN_OUT_FEATURE
in vec4 vColor;
out vec4 FragColor;
#else
varying vec4 vColor;
#define FragColor gl_FragColor
#define texture  texture2D
#endif 


uniform vec3 uDiffuseMaterial;
uniform vec3 uAmbientMaterial;

uniform sampler2D uTex0;

void main()
{
	//float k = uDiffuseMaterial.x + uAmbientMaterial.x;
	//vec3 col = texture(uTex0, vTexCoord).xyz;
	FragColor = vColor;
}