#version 400

layout(location = 0) out vec4 f4FragColor;
in vec2 f2Texcoord;
flat in int  iTextureIdx;

layout(binding = 0)uniform sampler2D g_Texture0;
layout(binding = 1)uniform sampler2D g_Texture1;
layout(binding = 2)uniform sampler2D g_Texture2;

vec4 fetchTexel()
{
//	return texture(g_Texture0,f2Texcoord);
	if(iTextureIdx == 0) return texture(g_Texture0,f2Texcoord);
	if(iTextureIdx == 1) return texture(g_Texture1,f2Texcoord);
	/*if(iTextureIdx == 2)*/ return texture(g_Texture2,f2Texcoord);
/*
if(iTextureIdx == 0) return vec4(1,0,0,1);
if(iTextureIdx == 1) return vec4(1,1,0,1);
return vec4(1);
	*/
}

void main()
{
	f4FragColor = max(fetchTexel(), vec4(0));
}