
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

#if 0
half4 PS_Downsample(DownsampleVertexOutput IN,
	uniform sampler2D OrigSampler,
	uniform float HighlightThreshold
	) : COLOR{
	half4 c;
#if 0
	// sub sampling
	c = tex2D(OrigSampler, IN.TexCoord[0]);
#else
	// box filter
	c = tex2D(OrigSampler, IN.TexCoord[0]) * 0.25;
	c += tex2D(OrigSampler, IN.TexCoord[1]) * 0.25;
	c += tex2D(OrigSampler, IN.TexCoord[2]) * 0.25;
	c += tex2D(OrigSampler, IN.TexCoord[3]) * 0.25;
#endif

	// store hilights in alpha
	c.a = highlights(c.rgb, HighlightThreshold);

	return c;
}
#endif

#ifdef __cplusplus
#define uniform
#define texture(x, y)  
#endif

uniform float g_fHighlightThreshold;



void main()
{

}