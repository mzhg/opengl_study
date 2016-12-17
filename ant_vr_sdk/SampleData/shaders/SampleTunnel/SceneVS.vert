#version 400

layout(location = 0) in vec4 aPosition;

uniform mat4 g_ViewProjection;
// uniform bool g_Horizontal;
out vec2 f2Texcoord;
flat out int  iTextureIdx;

void main()
{
	gl_Position = g_ViewProjection * aPosition;
	
	
	bool g_Horizontal = true;
	int face_id = gl_VertexID /6;
	float scaler;
	if(face_id == 0)
	{
		iTextureIdx = 0;
		scaler = 0.02;
	}
	else if(face_id == 1||face_id == 2)
	{
		iTextureIdx = 1;
		g_Horizontal = false;
		scaler = 0.1;
	}
	else
	{
		iTextureIdx = 2;
		scaler = 0.02;
	}
	
	 
	f2Texcoord.x = (g_Horizontal?aPosition.x:aPosition.y + 1.0)/2.0; 
	f2Texcoord.y = abs(aPosition.z) * scaler;
}