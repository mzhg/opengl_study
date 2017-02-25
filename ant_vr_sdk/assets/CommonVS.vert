layout(location = 0) in vec4 In_f4Position;

// cube vertex, just for debug
uniform vec4 g_VertexColors[24];
uniform mat4 g_MVP;

out vec4 m_f4Color;

void main()
{
	gl_Position = g_MVP * In_f4Position;
	m_f4Color = g_VertexColors[gl_VertexID];
}