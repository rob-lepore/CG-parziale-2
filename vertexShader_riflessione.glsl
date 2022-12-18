#version 330 core
layout (location = 0) in vec3 aPos;   // Attributo Posizione
layout (location =2 ) in vec3  aNormal; // Attributo Normale
 
 uniform mat4 Model;
 uniform mat4 Projection;
 uniform mat4 View;

out vec3 Normal;
out vec3 Position;
 
void main()
{
Normal = mat3(transpose(inverse(Model))) * aNormal;  //Normali in coordinate del mondo
Position = vec3(Model * vec4(aPos, 1.0)); //in WCS
gl_Position = Projection * View * Model * vec4(aPos,1.0);
} 