#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1
layout (location = 2 ) in vec3 vertexNormal; // Attributo Normale 2
 layout (location =3 ) in vec2 coord_st; // Attributo texture

 uniform mat4 Model;
 uniform mat4 Projection;
 uniform mat4 View;
uniform int sceltaVS;
uniform float time;
uniform vec3 viewPos;

//Struttura per la gestione di un punto luce
struct Light{
    vec4 vector;
	vec3 color;
	float power;
 };
 float strenght=0.1;
 //definizione di una variabile uniform che hala struttura PointLight
uniform Light light;

//Struttura per la gestione di un materiale

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

 

out vec2  frag_coord_st;
out vec3 FragPos;
out vec4 ourColor; // output a color to the fragment shader
out vec3 N,V,L,R,H;
 
void main()
{
    ourColor = aColor; // set ourColor to the input color we got from the vertex data
   
    if (sceltaVS==0)
    {
        frag_coord_st = coord_st;
        gl_Position = Projection*View*Model*vec4(aPos, 1.0); 
    }

    else if (sceltaVS==1) // OK
    {
        //Shading di Gouraud

        gl_Position = Projection*View*Model*vec4(aPos, 1.0);
        vec4 eyePosition= View*Model*vec4(aPos,1.0);

        mat3 G= mat3(transpose(inverse(View*Model)));
        N= normalize(G*vertexNormal);     
        V= normalize(viewPos - eyePosition.xyz);
        if(light.vector.w == 0.0)
        {
          L = normalize((View*-light.vector).xyz);
        }
        else if(light.vector.w == 1.0){
          vec4 eyeLightPos= View * light.vector;
          L = normalize((eyeLightPos- eyePosition).xyz);
        }
        R = reflect(-L,N);  //Costruisce la direzione riflessa di L rispesso alla normale

        vec3 ambient = strenght * light.color * material.ambient * light.power;

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = diff * light.color * material.diffuse * light.power;

        float spec = pow(max(dot(V, R), 0.0), material.shininess);
        vec3 specular = light.power * spec * light.color * material.specular ; 

        vec3 result = (ambient + diffuse + specular) * ourColor.rgb;
        ourColor = vec4(result, 1.0);

        frag_coord_st=coord_st;
    }

    else if (sceltaVS==2) //OK
    {
        //Modello di illuminazione di Blinn-Phong

        gl_Position = Projection*View*Model*vec4(aPos, 1.0);
        vec4 eyePosition= View*Model*vec4(aPos,1.0);
        mat3 G= mat3(transpose(inverse(View*Model)));
        N= normalize(G*vertexNormal);
                     
        //Calcoliamo la direzione della luce L, la direzione riflessione R e di vista
        V = normalize(viewPos - eyePosition.xyz);
        if(light.vector.w == 0.0)
        {
            L = normalize((View*-light.vector).xyz);
        }
        else if(light.vector.w == 1.0){
            vec4 eyeLightPos= View * light.vector;
            L = normalize((eyeLightPos- eyePosition).xyz);
        }        
        H = normalize(L+V); //Costruisce il vettore H a meta' tra direzione di luce e di vista

        frag_coord_st=coord_st;
    }
    else if (sceltaVS==3) //OK
    {
        //Modello di illuminazione di Phong
        gl_Position = Projection*View*Model*vec4(aPos, 1.0);
        vec4 eyePosition= View*Model*vec4(aPos,1.0);

        mat3 G= mat3(transpose(inverse(View*Model)));
        N= normalize(G*vertexNormal);     
        V= normalize(viewPos - eyePosition.xyz);
        if(light.vector.w == 0.0)
        {
          L = normalize((View*-light.vector).xyz);
        }
        else if(light.vector.w == 1.0){
          vec4 eyeLightPos= View * light.vector;
          L = normalize((eyeLightPos- eyePosition).xyz);
        }
        R = reflect(-L,N);  //Costruisce la direzione riflessa di L rispesso alla normale

        frag_coord_st=coord_st;
    }
    
}  

