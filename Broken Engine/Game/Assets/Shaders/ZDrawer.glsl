#version 440 core 
#define VERTEX_SHADER 
#ifdef VERTEX_SHADER 

layout (location = 0) in vec3 a_Position; 

uniform vec2 nearfar; 
uniform mat4 u_Model; 
uniform mat4 u_View; 
uniform mat4 u_Proj; 

out mat4 v_Projection; 
out vec2 v_NearFarPlanes; 

void main()
{ 
	v_NearFarPlanes = nearfar; 
	v_Projection = u_Proj; 
	gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0); 
}
#endif //VERTEX_SHADER
#define FRAGMENT_SHADER 
#ifdef FRAGMENT_SHADER

in vec2 v_NearFarPlanes;
in mat4 v_Projection;

out vec4 color;

float LinearizeDepth(float depth)
{
	float z = 2.0 * depth - 1.0; // back to NDC 
	return 2.0 * v_NearFarPlanes.x * v_NearFarPlanes.y / (v_NearFarPlanes.y + v_NearFarPlanes.x - z * (v_NearFarPlanes.y - v_NearFarPlanes.x));
}

void main()
{
	float depth = LinearizeDepth(gl_FragCoord.z) / v_NearFarPlanes.y;
	color = vec4(vec3(gl_FragCoord.z * v_NearFarPlanes.y * v_NearFarPlanes.x), 1.0);
}

#endif //FRAGMENT_SHADER