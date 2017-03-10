layout(location = 0) in vec4 In_f4Position;
layout(location = 1) in vec3 In_f3Normal;
layout(location = 2) in vec2 In_f2Texcoord;

// cube vertex, just for debug
uniform vec4 g_VertexColors[24];
uniform mat4 g_MVP;
uniform mat4 g_TexcoordMat;

out vec4 m_f4Color;
out vec2 m_f2Texcoord;

void main()
{
	gl_Position = g_MVP * In_f4Position;
	m_f4Color = g_VertexColors[gl_VertexID];

	m_f2Texcoord = (g_TexcoordMat * vec4(In_f2Texcoord, 0, 1)).xy;
}