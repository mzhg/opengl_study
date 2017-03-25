#if GL_ES

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

#if ENABLE_VERTEX_ID
const vec2 QuadVertices[4] = vec2[4]
(
vec2(-1.0, -1.0),
vec2(1.0, -1.0),
vec2(-1.0, 1.0),
vec2(1.0, 1.0)
);

const vec2 QuadTexCoordinates[4] = vec2[4]
(
vec2(0.0, 1.0),
vec2(1.0, 1.0),
vec2(0.0, 0.0),
vec2(1.0, 0.0)
);

#else
attribute vec4 aPosition;
#endif

#if ENABLE_IN_OUT_FEATURE
out vec2 TexCoord1;
out vec2 TexCoord2;
out vec2 TexCoord3;
out vec2 TexCoord4;

out gl_PerVertex
{
	vec4 gl_Position;
};
#else
varying vec2 TexCoord1;
varying vec2 TexCoord2;
varying vec2 TexCoord3;
varying vec2 TexCoord4;
#endif

/*
uniform vec2 g_StepSize;
uniform vec2 g_Stride;
*/

uniform vec4 g_TexCoordScale;


void main()
{
#if ENABLE_VERTEX_ID
	gl_Position = vec4(QuadVertices[gl_VertexID], 0.0, 1.0);
#else
	gl_Position = aPosition;
#endif
	//	ScreenSpaceUV = vec4(0.5 * gl_Position.xy + 0.5, gl_Position.xy);
	vec2 TexAttribute = 0.5 * gl_Position.xy + 0.5;
	TexCoord1 = (TexAttribute - 0.5) * g_TexCoordScale[0] + 0.5;
	TexCoord2 = (TexAttribute - 0.5) * g_TexCoordScale[1] + 0.5;
	TexCoord3 = (TexAttribute - 0.5) * g_TexCoordScale[2] + 0.5;
	TexCoord4 = (TexAttribute - 0.5) * g_TexCoordScale[3] + 0.5;
}



