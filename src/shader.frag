#version 330 core
uniform vec3 faceColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(faceColor, 1.0);
}