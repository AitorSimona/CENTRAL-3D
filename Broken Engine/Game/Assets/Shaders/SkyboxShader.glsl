#version 460 core 
#define VERTEX_SHADER 
#ifdef VERTEX_SHADER 

layout (location = 0) in vec3 a_Position; 

uniform mat4 u_View; 
uniform mat4 u_Proj; 

out vec3 TexCoords; 

void main()
{ 
	TexCoords = a_Position * vec3(1.0, -1.0, 1.0); 
	gl_Position = u_Proj * u_View * vec4(a_Position, 1.0); 
}
#endif //VERTEX_SHADER

#define FRAGMENT_SHADER 
#ifdef FRAGMENT_SHADER 

in vec3 TexCoords; 

uniform float u_GammaCorrection = 1.0;
uniform samplerCube skybox;

out vec4 color; 

void main()
{
	vec4 tex_color = texture(skybox, TexCoords); //u_GammaCorrection 
	color = pow(tex_color, vec4(vec3(1.0/u_GammaCorrection), 1.0));
} 
#endif //FRAGMENT_SHADER 
