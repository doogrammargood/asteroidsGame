#version 150

in vec4 vPos;
in vec4 vColor;
out vec4 color;

uniform mat4 M;
uniform mat4 C;

void
main()
{
  gl_Position = M*vPos;
  color = C*vColor;
}
