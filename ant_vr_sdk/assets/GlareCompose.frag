#if GL_ES
precision highp float;
#if __VERSION__  >= 300
#define ENABLE_VERTEX_ID 1
#define ENABLE_IN_OUT_FEATURE 1
#endif

#if __VERSION__ >= 310
#define LAYOUT_LOC(x)  layout(location = x) out 
#else
#define LAYOUT_LOC(x)
#endif

#else

// The Desktop Platform, Almost all of the video drivers support the gl_VertexID, so just to enable it simply.
#define ENABLE_VERTEX_ID 1

#if __VERSION__ >= 130
#define ENABLE_IN_OUT_FEATURE 1
#endif

#if __VERSION__ >= 400
#define LAYOUT_LOC(x)  layout(location = x) 
#else
#define LAYOUT_LOC(x)
#endif

#endif

#ifdef __cplusplus
#define uniform
#define varying
#define texture(x, y) 
typedef int sampler2D;
struct float2
{
	float x, y;
};
struct float4
{
	float z, w;
	union
	{
		float2 xy;
		struct
		{
			float x;
		};
		struct
		{
			float y;
		};
	};
};

float4 _f4OutColor;
#define gl_FragColor _f4OutColor
typedef float4 vec4;
typedef float2 vec2;

extern float4 texture2D(sampler2D sampler, float2 location);
#endif

#if ENABLE_IN_OUT_FEATURE
in vec4 ScreenSpaceUV;
in vec2 TexCoord2;
in vec2 TexCoord3;
in vec2 TexCoord4;
LAYOUT_LOC(0) out vec4 f4FragColor;
#else
varying vec4 ScreenSpaceUV;
varying vec2 TexCoord2;
varying vec2 TexCoord3;
varying vec2 TexCoord4;

#define f4FragColor gl_FragColor
#define texture  texture2D
#endif

uniform sampler2D g_Texture1;
uniform sampler2D g_Texture2;
uniform sampler2D g_Texture3;
uniform sampler2D g_Texture4;

uniform vec4 g_MixCoeff;

void main()
{
	f4FragColor = texture(g_Texture1, ScreenSpaceUV.xy)*g_MixCoeff.x
				+ texture(g_Texture2, ScreenSpaceUV.xy)*g_MixCoeff.y
				+ texture(g_Texture3, ScreenSpaceUV.xy)*g_MixCoeff.z
				+ texture(g_Texture4, ScreenSpaceUV.xy)*g_MixCoeff.w;
}
