#version 330 core

 uniform vec3 ViewPos;
  


in vec3 Normal;
in vec3 Position;

 
uniform samplerCube cubemap;
out vec4 FragColor;

void main()
{
     vec3 E = normalize(Position - ViewPos);
     vec3 R = reflect(E, normalize(Normal));
     
    FragColor =  vec4(texture(cubemap, R).rgb,1.0);
}