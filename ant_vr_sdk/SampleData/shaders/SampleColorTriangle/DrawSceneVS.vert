in vec2 Position;
in vec4 Color;

//in vec2 TexCoord;
//out vec2 vTexCoord;
out vec4 vColor;

void main()
{
	gl_Position = vec4(Position.x, Position.y, 0.0, 1.0);
	//vTexCoord = vec2( Position.x * 0.5 + 0.5, -Position.y * 0.5 + 0.5 );
	vColor = Color;
}