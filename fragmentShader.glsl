#version 330 core
in vec4 ourColor;
out vec4 FragColor;  

in vec3 N, L,R,V,H;
in vec3 FragPos;

 in vec2  frag_coord_st;


  uniform float time;
 uniform vec2 resolution;
 uniform int sceltaFS;
 float strenght=0.1;
struct Light{
    vec4 vector;
	vec3 color;
	float power;
 };

 //definizione di una varialie uniform che ha la struttura PointLight
uniform Light light;
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

 #define pi 3.14159235659

 uniform sampler2D id_tex;

 
void main()
{
    if(sceltaFS == 0) {
        FragColor=ourColor;
    }
    else if(sceltaFS==1) //OK
    {
        //Shading interpolativo
        FragColor=mix(ourColor,texture(id_tex,vec2(frag_coord_st.x,frag_coord_st.y)),0.3);
    }         
    else if (sceltaFS==2) //OK
	{
        //Shading di Phong per modello di illuminazione di Phong
            
        vec3 ambient = strenght * light.color * material.ambient * light.power;

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = diff * light.color * material.diffuse * light.power;

        float spec = pow(max(dot(V, R), 0.0), material.shininess);
        vec3 specular = light.power * spec * light.color * material.specular ; 

        vec3 result = (ambient + diffuse + specular) * ourColor.rgb;
        FragColor = vec4(result, 1.0);
            
        FragColor=mix(FragColor,texture(id_tex,vec2(frag_coord_st.x,frag_coord_st.y)),0.3);
    } 
    else if (sceltaFS == 3) //OK
    {
        //Shading di Blinn-Phong per modello di illuminazione di Blinn-Phong
        vec3 ambient = strenght * light.color * material.ambient * light.power;

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = diff * light.color * material.diffuse * light.power;

        float spec = pow(max(dot(N, H), 0.0), 4*material.shininess);
        vec3 specular = light.power * spec * light.color * material.specular ; 

        vec3 result = (ambient + diffuse + specular) * ourColor.rgb;
        FragColor = vec4(result, 1.0);
            
        FragColor=mix(FragColor,texture(id_tex,vec2(frag_coord_st.x,frag_coord_st.y)),0.3);
    
    }
    else if (sceltaFS==4) //OK
	{
        //Shading di Phong senza texturing da usare per mesh obj
        vec3 ambient = strenght * light.color * material.ambient * light.power;

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = diff * light.color * material.diffuse * light.power;

        float spec = pow(max(dot(V, R), 0.0), material.shininess);
        vec3 specular = light.power * spec * light.color * material.specular ; 

        vec3 result = (ambient + diffuse + specular);
        FragColor = vec4(result, 1.0);
    } 
    else if(sceltaFS == 5) { //OK
        FragColor = texture(id_tex, frag_coord_st);
    }
    else if(sceltaFS == 6) { // OK
        //Toon shader
        float intensity = dot(L,N);
        vec4 color = ourColor;
	    if (intensity > 0.95)
		    FragColor = vec4(color.rgb,1);
	    else if (intensity > 0.5)
		    FragColor = vec4((color-vec4(0.2)).rgb,1);
	    else if (intensity > 0.25)
		    FragColor = vec4((color-vec4(0.4)).rgb,1);
	    else
		    FragColor = vec4((color-vec4(0.5)).rgb,1);

    }
}