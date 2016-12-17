
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
extern float4 textureLod(sampler2D sampler, float2 location, float lod);
#endif

#if ENABLE_IN_OUT_FEATURE
in vec4 ScreenSpaceUV;
LAYOUT_LOC(0) out vec4 f4FragColor;
#else
varying vec4 ScreenSpaceUV;
#define f4FragColor gl_FragColor
#define texture  texture2D
#endif

uniform sampler2D g_SceneColor;
#ifndef ENABLE_DEPTH_ALPHA 
uniform sampler2D g_SceneDepth;
#endif 

uniform mat4 g_ViewProjectionInverseMatrix;
uniform mat4 g_previousViewProjectionMatrix;
uniform int  g_iNumSamples = 6;
uniform float g_VelocityFactor = 0.5;

void main()
{
	vec2 f2TexCoord = ScreenSpaceUV.xy;
	
	// Get the initial color at this pixel.
	vec4 f4Color = textureLod(g_SceneColor, f2TexCoord, 0.0);
#if ENABLE_DEPTH_ALPHA
	float fZOverW = f4Color.a;
#else
	// Get the depth buffer value at this pixel
	float fZOverW = textureLod(g_SceneDepth, f2TexCoord, 0.0).x;
#endif
	if(fZOverW == 1.0)
	{
		//Here nothing at background. so just ignore it.
		f4FragColor = f4Color;
		return;
	}
	
	vec4 f4ProjPos = vec4(ScreenSpaceUV.zw, 2.0 * fZOverW - 1.0, 1);
	// Transform by the view-projection inverse.
	vec4 f4WorldPos = g_ViewProjectionInverseMatrix * f4ProjPos;
	f4WorldPos /= f4WorldPos.w;
	
	vec4 f4CurrentPos = f4ProjPos;
	// Use the world position, and transform by the previous view-projection matrix.
	vec4 f4PreviousPos = g_previousViewProjectionMatrix * f4WorldPos;
	f4PreviousPos.xyz /= f4PreviousPos.w;
	
	// Use this frame's position and last frame's to compute the pixel
   	// velocity.
	vec2 velocity = (f4CurrentPos.xy - f4PreviousPos.xy) / 2.0 * g_VelocityFactor;
   	
   	f2TexCoord +=  velocity;
   	for(int i = 1; i < g_iNumSamples; i++, f2TexCoord +=  velocity)
   	{
		// TODO Need fix the issou of the f2TexCoord beyond the texture scop. 
   		// Sample the color buffer along the velocity vector.
   		vec4 f4CurrentColor = textureLod(g_SceneColor, f2TexCoord, 0.0);
		// Add the current color to our color sum.
		f4Color += f4CurrentColor;
   	}
   	
   	// Average all of the samples to get the final blur color.
   	f4FragColor = f4Color/float(g_iNumSamples);
}