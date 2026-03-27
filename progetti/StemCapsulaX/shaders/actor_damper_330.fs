#version 330

in vec4 fragColor;

uniform vec4 colDiffuse;
uniform vec4 partColor;

out vec4 finalColor;

void main()
{
  finalColor = colDiffuse * partColor * fragColor;
}
