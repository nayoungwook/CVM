#version 330 core

in vec2 fragTexCoord;
out vec4 color;

uniform sampler2D uTexture;

void main()
{
  vec4 c = texture(uTexture, fragTexCoord);
  color = c * vec4(fragTexCoord, 1, 1);
}
