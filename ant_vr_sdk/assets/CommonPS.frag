layout(location = 0) out vec4 Out_f4FragColor;

in vec4 m_f4Color;
in vec2 m_f2Texcoord;

uniform sampler2D g_Texture0;

void main()
{
#if 0
	Out_f4FragColor = m_f4Color;
#else
	Out_f4FragColor = texture(g_Texture0, m_f2Texcoord);
#endif
}