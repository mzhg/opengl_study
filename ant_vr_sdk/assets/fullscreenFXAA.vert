
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
out vec4 ScreenSpaceUV;
out vec4 f4FXAA_ConsolePosPos;
#else
varying vec4 ScreenSpaceUV;
varying vec4 f4FXAA_ConsolePosPos;
#endif

// 1/RenderTargetWidth, 1/RenderTargetHeight, see Fxaa.. header for more details (used for VS and PS)
uniform vec2 g_f2FrameRcpFrame;

void main()
{	
#if ENABLE_VERTEX_ID
	gl_Position = vec4(QuadVertices[gl_VertexID], 0.0, 1.0);
#else
	gl_Position = aPosition;
#endif
	ScreenSpaceUV = vec4(0.5 * gl_Position.xy + 0.5, gl_Position.xy);

	f4FXAA_ConsolePosPos = vec4(ScreenSpaceUV.xy + vec2(0.0, 1.0)*g_f2FrameRcpFrame, ScreenSpaceUV.xy + vec2(1.0, 0.0)*g_f2FrameRcpFrame);
}

