#version 150

in vec4 pos;
in vec2 texCoord;
out vec2 v_TexCoord;
void
main()
{
	gl_Position = pos;
	v_TexCoord = texCoord;	
}